#include "creategroupchat.h"
#include "ui_creategroupchat.h"

creategroupchat::creategroupchat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::creategroupchat)
{
    ui->setupUi(this);
}

creategroupchat::~creategroupchat()
{
    delete ui;
}
