#include "changename.h"
#include "ui_changename.h"
#include <QWidget>
#include "home.h"
#include <QPushButton>
#include <iostream>
#include <QDebug>
#include <userinfo.h>
#include <QPushButton>
extern userinfo user;
changename::changename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changename)
{
    ui->setupUi(this);
}

changename::~changename()
{
    delete ui;
}



void changename::on_buttonBox_accepted()
{
    if(ui->newname->text()!= "")
    {
        qDebug()<<"not null";
        connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &changename::confirm);
        qDebug()<<"not null ,exit";
        //flag = 1;
        //newname = ui->newName->text();
        //将修改的信息传入数据库
    }
    else
    {
        QString str1 = "修改失败";
        QString str2 = "昵称不能为空";
        qDebug()<<"null";
    }
}

void changename::confirm()
{
        // 获取用户输入的新昵称
        QString newNickname = ui->newname->text();
        qDebug()<<"confirm"<<endl;
        // 触发一个信号，将新昵称传递给Home界面
        user.name = newNickname;
        user.hasnamechanged=true;

}
/*
void changename::namechange(const QString &nickname){
    user.name = nickname;
}

*/


void changename::on_pushButton_clicked()
{

     confirm();
     this->close();
}

void changename::on_pushButton_2_clicked()
{
    this->close();
}
