#include "successfullycreategroupchat.h"
#include "ui_successfullycreategroupchat.h"

successfullycreategroupchat::successfullycreategroupchat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::successfullycreategroupchat)
{
    ui->setupUi(this);
}

successfullycreategroupchat::~successfullycreategroupchat()
{
    delete ui;
}
