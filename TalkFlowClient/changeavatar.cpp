#include "changeavatar.h"
#include "ui_changeavatar.h"
#include <userinfo.h>
#include <QDebug>
extern userinfo user;

changeavatar::changeavatar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeavatar)
{
    ui->setupUi(this);
}

changeavatar::~changeavatar()
{
    delete ui;
}

void changeavatar::on_pushButton_clicked()
{

    user.signature = QString(":/img/touxiang1.png");//img_id=1
    user.hasavatarchanged=true;
    user.avatartype="1";
    qDebug()<<"avatarchanged ot pic 1\n"<<"user.avatartype=:"<<user.avatartype;
}

void changeavatar::on_pushButton_2_clicked()
{
    user.signature =  QString(":/img/touxiang2.png");//img_id=2
    user.hasavatarchanged=true;
    user.avatartype="2";

}

void changeavatar::on_pushButton_3_clicked()
{
    user.signature = QString(":/img/touxiang3.png");//img_id=3
    user.hasavatarchanged=true;
    user.avatartype="3";
}


void changeavatar::on_pushButton_4_clicked()
{
    user.signature = QString(":/img/touxiang4.png");//img_id=4
    user.hasavatarchanged=true;
    user.avatartype="4";
}

void changeavatar::on_pushButton_5_clicked()
{
    user.signature = QString(":/img/touxiang5.png");//img_id=5
    user.hasavatarchanged=true;
    user.avatartype="5";
}

void changeavatar::on_pushButton_6_clicked()
{
    user.signature = QString(":/img/touxiang6.png");//img_id=6
    user.hasavatarchanged=true;
    user.avatartype="6";
}



void changeavatar::on_pushButton_7_clicked()
{
    this->close();
}
