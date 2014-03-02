#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //set up ui
    ui->setupUi(this);

    listModel= new QStandardItemModel(ui->listView);
    ui->listView->setModel(listModel);

    treeModel = new QStandardItemModel(ui->treeView);
    ui->treeView->setModel(treeModel);

    setting = new QSettings("my","feeder",this);
    subDialog = new SubscribeDialog(this);

    //set up oauth2
    o2 = new O2(this);
    nam = new QNetworkAccessManager(this);
    o2req = new O2Requestor(nam,o2,this);
    o2->setClientId(CLIENT_ID);
    o2->setClientSecret(CLIENT_SECRET);
    o2->setRequestUrl(REQUEST_URL);
    o2->setTokenUrl(TOKEN_URL);
    o2->setScope(SCOPE);
    o2->setLocalPort(8080);
    o2->setRefreshTokenUrl(REFRESH_TOKEN_URL);

    store = new O2SettingsStore(setting,"myfeederkey",this);
    store->setGroupKey("OAuth");
    o2->setStore(store);

    //If we've already logged in, we can get userId and plan from
    //the local setting storage
    userId = store->value("id");
    plan = store->value("plan");
    qDebug() << userId<< plan;

    //We do the linking to log in. If we've already logged in,
    //it won't hurt any way.
    o2->link();

    //connect
    connect(o2, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(o2, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(o2, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(o2, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(o2,SIGNAL(tokenChanged()),this,SLOT(onTokenChanged()));
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onTreeViewClicked(QModelIndex)));
    connect(o2req,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),this,SLOT(onReqFinished(int,QNetworkReply::NetworkError,QByteArray)));

    connect(subDialog,SIGNAL(accepted()),this,SLOT(on_subscibe()));

    reqCategories();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::onLinkedChanged() {
    // Linking (login) state has changed.
    // Use o1->linked() to get the actual state
}

void MainWindow::onLinkingFailed() {
    // Login has failed
}

void MainWindow::onLinkingSucceeded() {
    // Login has succeeded
}


void MainWindow::onOpenBrowser(const QUrl &url) {
    // Open a web browser or a web view with the given URL.
    // The user will interact with this browser window to
    // enter login name, password, and authorize your application
    // to access the Twitter account
   //QDesktopServices::openUrl(url);
   ui->webView->load(url);

   qDebug()<<"logging in";
}

void MainWindow::onTokenChanged()
{
    userId = o2->extraTokens().value("id").toString();
    plan = o2->extraTokens().value("plan").toString();
    store->setValue("id",userId);
    store->setValue("plan",plan);
}

//This is ugly, I know. But I have to do this
//because the oauth2 library doesn't give me direct
//access to the data
void MainWindow::onReqFinished(int id, QNetworkReply::NetworkError error, QByteArray data)
{
    switch (reqStatus) {
    case Auth:

        break;
    case Categories:
        handleCategoryResp(data);
        reqSubscriptions();
        break;
    case Subscriptions:
        handleSubscriptionsResp(data);
        break;
    case Contents:
        handleContentsResp(data);
    }
    //qDebug()<<data.length();
    //qDebug("%s",data.data());
}

bool MainWindow::on_subscibe()
{
    QJsonObject *json = new QJsonObject();
    QJsonObject *j1 = new QJsonObject();
    QJsonObject *j2 = new QJsonObject();
    QJsonArray *jarr = new QJsonArray();

    j1->insert("id",QJsonValue("user/" + userId + "/category/tech"));
    j1->insert("label",QJsonValue(QString("tech")));

    //j2->insert("id",userIdjson);
    //j2->insert("label",QJsonValue(QString("weekly")));

    json->insert("id",QJsonValue("feed/" + subDialog->getFeedUrl()));
    json->insert("title",QJsonValue(subDialog->getFeedTitle()));

    jarr->insert(0,*j1);
    //jarr->insert(1,*j2);

    json->insert("categories",*jarr);

    QJsonDocument *docu =  new QJsonDocument(*json);
    //qDebug() << docu->toJson();

    request.setUrl(SUBSCRIPTIONS_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    o2req->post(request,docu->toJson());

    reqCategories();
    return true;
}

void MainWindow::reqCategories()
{
    request.setUrl(CATEGORIES_URL);
    //request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    reqStatus = Categories;
    o2req->get(request);

}

void MainWindow::reqSubscriptions()
{
    request.setUrl(SUBSCRIPTIONS_URL);
    reqStatus = Subscriptions;
    o2req->get(request);
}

void MainWindow::handleCategoryResp(QByteArray data)
{
    if(!categorieMap.contains("Uncategorized")) {
        QStandardItem *uncategorized = new QStandardItem("Uncategorized");
        uncategorized->setData(QVariant::fromValue(QString("user/" + userId + "/category/global.uncategorized")),CATEGORY_ID);
        treeModel->appendRow(uncategorized);
        categorieMap.insert("Uncategorized",uncategorized);
    }

    QJsonParseError jerror;
    QJsonDocument doc = QJsonDocument::fromJson(data,&jerror);
    qDebug()<<doc;
    if(jerror.error == QJsonParseError::NoError || ! doc.isEmpty()) {
        QJsonArray arr = doc.array();
        QString tempLabel,tempId;

        for(int row = 0;row<arr.size();row++)
        {
            QJsonObject obj = arr[row].toObject();
            tempLabel = obj.value("label").toString();
            tempId = obj.value("id").toString();
            qDebug()<<tempLabel;
            if(!categorieMap.contains(tempLabel)) {
                QStandardItem *category = new QStandardItem(tempLabel);
                category->setData(QVariant::fromValue(tempId),CATEGORY_ID);
                treeModel->appendRow(category);
                categorieMap.insert(tempLabel,category);
            }
        }

    }
    else {
        qDebug() << "error when parsing json";
    }

}

//FIXME: Can't handle subscriptions with many labels(tags, whatever)
void MainWindow::handleSubscriptionsResp(QByteArray data)
{
    QJsonParseError jerror;
    QJsonDocument doc = QJsonDocument::fromJson(data,&jerror);
    qDebug()<<doc;
    if(jerror.error == QJsonParseError::NoError || ! doc.isEmpty()) {
        QJsonArray arr = doc.array();
        QString tempTitle,templabel,tempId;
        QStringList tempList;
        QStandardItem *parent;
        for(int row = 0;row<arr.size();row++)
        {
            QJsonObject obj = arr[row].toObject();
            tempTitle = obj.value("title").toString();
            tempId = obj.value("id").toString();
            qDebug()<<tempTitle;

            QStandardItem *sub = new QStandardItem(tempTitle);
            QJsonArray ca = obj.value("categories").toArray();
            if(!ca.isEmpty()) {
                for(int row2 = 0;row2<ca.size();row2++) {
                    QJsonObject obj2 = ca[row2].toObject();
                    templabel = obj2.value("label").toString();
                    qDebug()<<templabel;

                    /*if(subscriptionMap.contains(tempTitle)) {
                        tempList = subscriptionMap.find(tempTitle).value();
                        tempList.clear();
                    }
                    tempList.append(templabel);*/
                }
                //subscriptionMap.insert(tempTitle,tempList);
            }
            else {
                templabel = "Uncategorized";
            }
            parent = categorieMap.find(templabel).value();
            sub->setData(QVariant::fromValue(tempId),FEED_ID);
            parent->appendRow(sub);
        }
    }
}

//FIXME: If we load too much contents it will be really slow
void MainWindow::handleContentsResp(QByteArray data)
{
    QJsonParseError jerror;
    QJsonDocument doc = QJsonDocument::fromJson(data,&jerror);
    QString tempTitle,tempContent,tempEntryId;
    QJsonObject tempObject;
    int i = 1;
    //qDebug()<<doc;
    if(jerror.error == QJsonParseError::NoError || ! doc.isEmpty()) {
        listModel->clear();
        QJsonObject obj = doc.object();
        QJsonArray arr = obj.value("items").toArray();
        for(int row = 0;row<arr.size();row++)
        {
            QJsonObject obj2 = arr[row].toObject();
            tempTitle = obj2.value("title").toString();
            if(!tempTitle.isEmpty()) {
                qDebug() << tempTitle;
                tempObject = obj2.value("summary").toObject();
                tempContent = tempObject.value("content").toString();

                tempEntryId = obj2.value("id").toString();

                QStandardItem *contentTitle = new QStandardItem(tempTitle);
                contentTitle->setData(QVariant::fromValue(tempContent),CONTENT);
                contentTitle->setData(QVariant::fromValue(tempEntryId),CONTENT_ENTRY_ID);

                listModel->appendRow(contentTitle);
            }
        }
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);

}

void MainWindow::on_actionSubscribe_to_a_feed_triggered()
{
    if(!subDialog) {
        subDialog = new SubscribeDialog(this);
    }
    subDialog->show();
    subDialog->raise();
    subDialog->activateWindow();
}

void MainWindow::onTreeViewClicked(const QModelIndex &)
{
    qDebug("TreeView Clicked");
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = treeModel->itemFromIndex(currentIndex);
    QString contentId = currentItem->data(FEED_ID).toString();

    qDebug()<<currentItem->text();
    //A feed item clicked
    if(!contentId.isEmpty()) {
        qDebug()<<contentId;
        QString url = STREAMS_CONTENTS_URL + contentId;
        request.setUrl(QUrl(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-gzip-compressed");
        reqStatus = Contents;
        o2req->get(request);
    }
    //A category item clicked
    else {
        QString categoryId = currentItem->data(CATEGORY_ID).toString();
        qDebug()<<categoryId;
    }
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    qDebug("ListView Clicked");
    QModelIndex currentIndex = ui->listView->currentIndex();
    QStandardItem *currentItem = listModel->itemFromIndex(currentIndex);
    QString content = currentItem->data(CONTENT).toString();
    ui->webView->setHtml(content);
}
