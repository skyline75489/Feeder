#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QtWidgets/QMessageBox>
#include <QStringListModel>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QSettings>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMenu>
#include <QModelIndex>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

#include <o2/src/o2.h>
#include <o2/src/o2settingsstore.h>
#include <o2/src/o2globals.h>
#include <o2/src/o2requestor.h>

#include <subscribedialog.h>

#define FEED_ID 100
#define CONTENT 101
#define CONTENT_ENTRY_ID 102
#define CATEGORY_ENTRY_ID 103
#define CONTENT_HREF 104
#define CONTENT_UNREAD 105

const char CLIENT_SECRET[] = "W60IW73DYSUIISZX4OUP";
const char CLIENT_ID[] = "sandbox";
const char TOKEN_URL[] = "https://sandbox.feedly.com/v3/auth/token";
const char REQUEST_URL[] = "http://sandbox.feedly.com/v3/auth/auth";
const char SCOPE[] = "https://cloud.feedly.com/subscriptions";
const char REFRESH_TOKEN_URL[] = "http://sandbox.feedly.com/v3/auth/token";
const QString SUBSCRIPTIONS_URL = "http://sandbox.feedly.com/v3/subscriptions";
const QString CATEGORIES_URL = "http://sandbox.feedly.com/v3/categories";
const QString STREAMS_CONTENTS_URL = "http://sandbox.feedly.com/v3/streams/contents?streamId=";
const QString MARKERS_URL = "http://sandbox.feedly.com/v3/markers";


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbout_Qt_triggered();

    //Deal with OAuth2
    void onLinkedChanged();
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onTokenChanged();
    //Deal with Various requset
    void onReqFinished(int id, QNetworkReply::NetworkError error, QByteArray data);
    bool on_subscibe();
    void handleCategoryResp(QByteArray data);
    void handleSubscriptionsResp(QByteArray data);
    void handleContentsResp(QByteArray data);
    void handleMarkersResp(QByteArray data);
    //UI signal
    void on_actionSubscribe_to_a_feed_triggered();
    void on_treeView_clicked(const QModelIndex &);
    void on_listView_clicked(const QModelIndex &index);
    void on_listView_customContextMenuRequested(const QPoint &pos);
    void on_marker_menu_triggered(QAction *action);
private:

    void reqCategories();
    void reqSubscriptions();
    void markAs(bool read, QString id,int status);
    void alterItemFont(bool bold, QStandardItem *item);

    enum ReqStatus {
        Auth,Preference,Categories,Subscriptions,Tags,Contents,Markers
    };

    Ui::MainWindow *ui;
    QSettings *setting;
    O2SettingsStore *store;
    O2 *o2 ;
    O2Requestor *o2req;
    QNetworkRequest request;
    QNetworkAccessManager* nam;
    ReqStatus reqStatus;
    QString userId;
    QString plan;
    SubscribeDialog *subDialog;

    QList<QTreeWidgetItem *> items;
    QStandardItemModel *listModel;
    QStandardItemModel *treeModel;
    QStandardItem *currentItem;
    QFont currentFont;
    QMenu *listMenu;
    QMenu *treeMenu;
    QAction *markArticleAsRead;
    QAction *markArticleAsUnread;
    QMap<QString,QStandardItem*> categorieMap;
    QVector<QString> subscription;
};

#endif // MAINWINDOW_H
