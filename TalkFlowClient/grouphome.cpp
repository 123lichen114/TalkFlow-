#include "grouphome.h"
#include "ui_grouphome.h"
#include "home.h"
#include "userinfo.h"
#include "login.h"
#include "addfriends.h"
#include "chat_1v1.h"
#include <QtNetwork>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <ui_grouphome.h>
//#include "change_name.h"
//#include "change_signature.h"
#include <QDebug>

extern QString hostip;
extern int hosthost;
extern userinfo user;
extern userinfo otheruser;

QList <QString> grouplist;
QList <QString> groupnamelist;

grouphome::grouphome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::grouphome)
{
    ui->setupUi(this);
    //qDebug()<<user.name<<' '<<user.id<<endl;
    ui->labelUserName->setText(user.name);
    ui->labelUserId->setText(QString::number(user.id));
    timer = new QTimer();
    timer->start(500);
    connect(timer,SIGNAL(timeout()),this,SLOT(getGroupList()));//登陆自动触发
}

grouphome::~grouphome()
{
    delete ui;
    delete timer;
    tcpSocket->close();
    tcpSocket->deleteLater();
    delete tcpSocket;
}

//void grouphome::closeEvent(QCloseEvent *event)
//{
//    timer->stop();
//    tcpSocket = new QTcpSocket();
//    tcpSocket->abort();//取消已有链接
//    tcpSocket->connectToHost(hostip, hosthost);//链接服务器
//    if(!tcpSocket->waitForConnected(10000))
//    {
//        user.islogin = false;
//    }
//    else
//    {//服务器连接成功
//        QString message = QString("logout##%1").arg(user.id);
//        tcpSocket->write(message.toUtf8());
//        tcpSocket->flush();
//        QMessageBox::warning(this, "下线成功", "下线成功", QMessageBox::Yes);
//    }
//    tcpSocket->close();
//    delete tcpSocket;
//}

void grouphome::getGroupList(){
    tcpSocket = new QTcpSocket();
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip,hosthost);//链接服务器
    if(!tcpSocket->waitForConnected(30000))
    {
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        this->close();
        user.islogin = false;
        login *logi = new login();
        logi->show();
    }
    else
    {//服务器连接成功
        //qDebug()<<"ENTER get_friend_list_function2"<<endl;
        QString message = QString("getgrouplist##%1").arg(user.id);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            QByteArray buffer = tcpSocket->readAll();
            if(QString(buffer).section("##",0,0)==QString("getgrouplist_ok"))
            {
                int newnum = QString(buffer).section("##",1,1).toInt();
                //int newonlinenum = QString(buffer).section("##",2,2).toInt();
                //qDebug()<<newnum<<' '<<newonlinenum<<endl;
                //qDebug()<<QString(buffer)<<endl;

                ui->listWidgetGroup->clear();
                grouplist.clear();
                groupnamelist.clear();

                for(int rownum = 0;rownum < newnum ;rownum++)
                {
                    QString friendid = QString(buffer).section("##",2+rownum*2,2+rownum*2);
                    QString friendname = QString(buffer).section("##",3+rownum*2,3+rownum*2);
                    //friendlist.append(friendname);
                    //friendstatuelist.append(friendstatue);
                    //friendiplist.append(friendip);
                    ui->listWidgetGroup->insertItem(rownum,tr((friendid + " " + friendname).toUtf8()));
                }
            }
        });
    }

}

void grouphome::on_pushButtonCreate_clicked()
{
    bool ok;
    QString addgroupname = addfriends::getText(this, tr("创建群聊"),tr("请输入群聊名称"), QLineEdit::Normal,0,&ok);
    if (ok && !addgroupname.isEmpty())
    {
        tcpSocket = new QTcpSocket();
        tcpSocket->abort();//取消已有链接
        tcpSocket->connectToHost(hostip, hosthost);//链接服务器
        if(!tcpSocket->waitForConnected(30000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            this->close();
            user.islogin = false;
            login *logi = new login();
            logi->show();
        }
        else
        {//服务器连接成功
            QString message = QString("create_group##%1##%2##%3").arg(user.id).arg(addgroupname).arg(user.name);
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();

            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                if(QString(buffer).section("##",0,0)==QString("create group successed"))
                {//注册成功
                    QMessageBox::warning(this, "注册成功!", "群聊id是"+QString(buffer).section("##",1,1), QMessageBox::Yes);
                }
            });
        }
    }
}

void grouphome::on_pushButtonAdd_clicked()
{
    bool ok;
    QString addgroupid = addfriends::getText(this, tr("加入群聊"),tr("请输入群聊id"), QLineEdit::Normal,0,&ok);
    if (ok && !addgroupid.isEmpty())
    {
        tcpSocket = new QTcpSocket();
        tcpSocket->abort();//取消已有链接
        tcpSocket->connectToHost(hostip, hosthost);//链接服务器
        if(!tcpSocket->waitForConnected(30000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            this->close();
            user.islogin = false;
            login *logi = new login();
            logi->show();
        }
        else
        {//服务器连接成功
            QString message = QString("add_group##%1##%2##%3").arg(user.id).arg(addgroupid).arg(user.name);
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();

            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                if(QString(buffer).section("##",0,0)==QString("add group successed"))
                {//注册成功
                    QMessageBox::warning(this, "加入成功!", "加入"+ QString(buffer).section("##",1,1) +"成功!", QMessageBox::Yes);
                }
            });
        }
    }
}

void grouphome::on_pushButtonDel_clicked()
{
    bool ok;
    QString addgroupid = addfriends::getText(this, tr("退出群聊"),tr("请输入群聊id"), QLineEdit::Normal,0,&ok);
    if (ok && !addgroupid.isEmpty())
    {
        tcpSocket = new QTcpSocket();
        tcpSocket->abort();//取消已有链接
        tcpSocket->connectToHost(hostip, hosthost);//链接服务器
        if(!tcpSocket->waitForConnected(30000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            this->close();
            user.islogin = false;
            login *logi = new login();
            logi->show();
        }
        else
        {//服务器连接成功
            QString message = QString("leave_group##%1##%2##%3").arg(user.id).arg(addgroupid).arg(user.name);
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();

            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                if(QString(buffer).section("##",0,0)==QString("del group successed"))
                {//注册成功
                    QMessageBox::warning(this, "退出成功!", "退出"+ QString(buffer).section("##",1,1) +"成功!", QMessageBox::Yes);
                }
            });
        }
    }
}

void grouphome::on_toolButtonFriend_clicked()
{
    this->close();
    home *hom = new home();
    hom->setWindowTitle("好友");
    hom->show();
}

void grouphome::on_listWidgetGroup_itemClicked(QListWidgetItem *item)
{
    QString data = item->text();
    otheruser.id=data.section(" ",0,0).toInt();
    otheruser.name=data.section(" ",1,1);
    qDebug()<<otheruser.id<<' '<<otheruser.name<<endl;
    //释放socket资源
    timer->stop();
    timer->start(1000);
    // 根据数据创建并打开新的页面
    chat_1v1* newPage = new chat_1v1();
    newPage->show();
}

void grouphome::on_pushButton_clicked()
{
    changeavatar a;
    a.exec();
}

void grouphome::on_pushButton_2_clicked()
{
    changename a;
    a.exec();
}
