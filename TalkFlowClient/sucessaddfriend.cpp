#include "sucessaddfriend.h"
#include "ui_sucessaddfriend.h"

sucessaddfriend::sucessaddfriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sucessaddfriend)
{
    ui->setupUi(this);
}

sucessaddfriend::~sucessaddfriend()
{
    delete ui;
}
