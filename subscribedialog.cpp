#include "subscribedialog.h"
#include "ui_subscribedialog.h"

SubscribeDialog::SubscribeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubscribeDialog)
{
    ui->setupUi(this);
    this->setFixedSize(400,300);
}

SubscribeDialog::~SubscribeDialog()
{
    delete ui;
}

QString SubscribeDialog::getFeedUrl()
{
    return ui->lineEdit->text();
}

QString SubscribeDialog::getFeedTitle()
{
    return ui->lineEdit_2->text();
}
