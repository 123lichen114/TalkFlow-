#include "home.h"
#include "ui_home.h"
#include "grouphome.h"
#include "userinfo.h"
#include "login.h"
#include "addfriends.h"
#include "chat_1v1.h"
#include <QtNetwork>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <ui_home.h>
//#include "change_name.h"
//#include "change_signature.h"
#include <QDebug>
#include <QString>


extern QString hostip;
extern int hosthost;
extern userinfo user;
userinfo otheruser;
int num;//当前好友数量
int onlinenum;//当前在线好友数量

QList <QString> friendlist;
QList <QString> friendnamelist;
QList <QString> friendiplist;
QList <QString> friendstatuelist;//1在线
QString avatar_default = ":/img/touxiang1.png";

home::home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    //qDebug()<<user.name<<' '<<user.id<<endl;
    ui->labelUserName->setText(user.name);
    ui->labelUserId->setText(QString::number(user.id));
    ui->toolButtonImg->setIcon(QIcon(user.signature));
    timer = new QTimer();
    timer->start(500);
    connect(timer,SIGNAL(timeout()),this,SLOT(getFriendList()));//登陆自动触发
}

home::~home()
{
    delete ui;
    delete timer;
    tcpSocket->close();
    tcpSocket->deleteLater();
    delete tcpSocket;
}



void home::getFriendList()//修改了本函数使得主页可以显示好友头像   本函数经常刷新
{
    ui->toolButtonImg->setIcon(QIcon(user.signature));
    ui->labelUserName->setText(user.name);
    if(user.hasnamechanged){sendnamechange();}
    if(user.hasavatarchanged){sendavatarchange();}
    tcpSocket = new QTcpSocket();
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip,hosthost);//链接服务器
    if(!tcpSocket->waitForConnected(3000))
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
        QString message = QString("getfriendlist##%1").arg(user.id);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            QByteArray buffer = tcpSocket->readAll();
            if(QString(buffer).section("##",0,0)==QString("getfriendlist_ok"))
            {
                int newnum = QString(buffer).section("##",1,1).toInt();
                //int newonlinenum = QString(buffer).section("##",2,2).toInt();
                //qDebug()<<newnum<<' '<<newonlinenum<<endl;
                //qDebug()<<QString(buffer)<<endl;

                ui->listWidgetFriend->clear();
                friendlist.clear();
                friendiplist.clear();
                friendstatuelist.clear();


                for (int rownum = 0; rownum < newnum; rownum++)// server端从数据库发送的数据需要加上friend avatar type。。。。。。。。。。
                {
                    QString friendid = QString(buffer).section("##",3+rownum*5,3+rownum*5);
                    QString friendname = QString(buffer).section("##",4+rownum*5,4+rownum*5);
                    QString friendstatue = QString(buffer).section("##",5+rownum*5,5+rownum*5);
                    QString friendip = QString(buffer).section("##",6+rownum*5,6+rownum*5);
                    int friendava = QString(buffer).section("##",7+rownum*5,7+rownum*5).toInt();

                    //下面的代码需要等数据库传输信息修改之后才能使用

                    QString avatarpath="";

                    switch(friendava){
                    case 1:
                        avatarpath = ":/img/touxiang1.png";
                        break;
                    case 2:
                        avatarpath = ":/img/touxiang2.png";
                        break;
                    case 3:
                        avatarpath = ":/img/touxiang3.png";
                        break;
                    case 4:
                        avatarpath = ":/img/touxiang4.png";
                        break;
                    case 5:
                        avatarpath = ":/img/touxiang5.png";
                        break;
                    case 6:
                        avatarpath = ":/img/touxiang6.png";
                        break;
                    }
                    qDebug()<<friendava<<' '<<avatarpath<<endl;
                    if(friendid == user.id)
                    {
                        user.name = friendname;
                        user.signature = avatarpath;
                        user.ip = friendip;
                    }


                    friendlist.append(friendname);
                    friendstatuelist.append(friendstatue);
                    friendiplist.append(friendip);

                    if (friendstatue == '1')
                        {
                        // 创建一个带有图像的 QListWidgetItem
                        QListWidgetItem *item = new QListWidgetItem(QIcon(avatarpath), tr((friendid + " " + friendname + " (在线)").toUtf8()));
                        // 插入 item 到 QListWidget 中
                        ui->listWidgetFriend->insertItem(rownum, item);
                        }
                    else
                        {
                        // 创建一个带有图像的 QListWidgetItem
                        QListWidgetItem *item = new QListWidgetItem(QIcon(avatarpath), tr((friendid + " " + friendname + " (离线)").toUtf8()));
                        // 插入 item 到 QListWidget 中
                        ui->listWidgetFriend->insertItem(rownum, item);
                        }
                }
            }
            else if(QString(buffer).section("##",0,0)==QString("add_friend_request"))
            {
                //qDebug()<<"Get_friend_requset"<<endl;
                QString friendname = QString(buffer).section("##",3,3);
                QString friendid = QString(buffer).section("##",1,1);
                QString dlgTitle="提示";
                QString strInfo="您将添加id:"+friendid+",用户名:"+friendname+"为好友\n确定吗？";
                QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton;
                QMessageBox::StandardButton result;
                result=QMessageBox::question(this, dlgTitle, strInfo,QMessageBox::Cancel|QMessageBox::Yes,defaultBtn);
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
                    if (result==QMessageBox::Yes)
                    {
                        QString message = QString("add_friend_agreement##%1##%2").arg(user.id).arg(friendid);
                        tcpSocket->write(message.toUtf8());
                        tcpSocket->flush();
                    }
                    else
                    {
                        QString message = QString("add_friend_deny");
                        tcpSocket->write(message.toUtf8());
                        tcpSocket->flush();
                    }
                }
            }

        });
    }
}


void home::on_pushButtonLogOut_clicked()
{
    timer->stop();
    tcpSocket = new QTcpSocket();
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip, hosthost);//链接服务器
    if(!tcpSocket->waitForConnected(30000))
    {
        user.islogin = false;
    }
    else
    {//服务器连接成功
        QString message = QString("logout##%1").arg(user.id);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        QMessageBox::warning(this, "下线成功", "下线成功", QMessageBox::Yes);
    }
    tcpSocket->close();
    delete tcpSocket;
    this->close();
    login * logi=new login();
    logi->setWindowTitle("登录");
    logi->show();
}

void home::on_pushButtonAdd_clicked()
{
    bool ok;
    QString addfriendid = addfriends::getText(this, tr("增添联系人"),tr("请输入对方id"), QLineEdit::Normal,0,&ok);
    if (ok && !addfriendid.isEmpty())
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
            QString message = QString("add_friend##%1##%2##%3").arg(user.id).arg(addfriendid).arg(user.name);
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();

            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                //qDebug() << QString(buffer);
                if( QString(buffer).section("##",0,0) == QString("add_friend_ok"))
                {//加朋友
                    onlinenum = -1;
                }
                else if( QString(buffer).section("##",0,0) == QString("add_friend_error"))
                {
                    QMessageBox::warning(this, "Warning!", "查无此人", QMessageBox::Yes);
                }
                else if( QString(buffer).section("##",0,0) == QString("add_friend_deny"))
                {
                    QMessageBox::warning(this, "Warning!", "对方拒绝", QMessageBox::Yes);
                }
            });
        }
    }
}

void home::on_pushButtonDel_clicked()
{
    bool ok;
    QString delfriendid = addfriends::getText(this, tr("删除联系人"),tr("请输入对方id"), QLineEdit::Normal,0,&ok);
    if (ok && !delfriendid.isEmpty())
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
            QString message = QString("del_friend##%1##%2##%3").arg(user.id).arg(delfriendid).arg(user.name);

            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();

            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                //qDebug() << QString(buffer);
                if( QString(buffer).section("##",0,0) == QString("del_friend_ok"))
                {//加朋友
                    onlinenum = -1;
                }
                else if( QString(buffer).section("##",0,0) == QString("del_friend_error"))
                {
                    QMessageBox::warning(this, "Warning!", "查无此人", QMessageBox::Yes);
                }
                else if( QString(buffer).section("##",0,0) == QString("del_friend_not_friend") ){
                    QMessageBox::warning(this, "Warning!", "对方不是你的好友", QMessageBox::Yes);
                }
            });
        }
    }
}

void home::on_listWidgetFriend_itemClicked(QListWidgetItem *item)
{
    if (item) {
        // 获取被点击项的数据
        QString data = item->text();
        otheruser.id=data.section(" ",0,0).toInt();
        otheruser.name=data.section(" ",1,1);
        //qDebug()<<otheruser.id<<' '<<otheruser.name<<endl;
        //释放socket资源
        timer->stop();
        timer->start(1000);
        // 根据数据创建并打开新的页面
        chat_1v1* newPage = new chat_1v1();
        newPage->show();
    }
}

void home::on_pushButton_clicked()
{
    changeavatar d;
    d.exec();
}

void home::on_pushButton_2_clicked()
{
    changename d;
    d.exec();
}

void home::sendnamechange()
{
    user.hasnamechanged=false;
    tcpSocket = new QTcpSocket();
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip, hosthost);//链接服务器
    if(!tcpSocket->waitForConnected(30000))
    {

        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);

        this->close();

    }
    else
    {//服务器连接成功
        QString message = QString("name_change##%1##%2").arg(user.id).arg(user.name);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        qDebug()<<"成功发送改名信息";

//        connect(tcpSocket,&QTcpSocket::readyRead,[=](){

//            QByteArray buffer = tcpSocket->readAll();

//            //qDebug() << QString(buffer);

//            if( QString(buffer).section("##",0,0) == QString("name_change_ok"))
//            {

//            }//成功发送
//            else
//            {
//                QMessageBox::warning(this, "Warning!", "改名不成功，仅你可见新名字", QMessageBox::Yes);
//            }

//        });

    }
}


void home::sendavatarchange()
{
    user.hasavatarchanged=false;

    tcpSocket = new QTcpSocket();
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip, hosthost);//链接服务器
    if(!tcpSocket->waitForConnected(30000))
    {

        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);

        this->close();

    }
    else
    {//服务器连接成功
        QString message = QString("avatar_change##%1##%2").arg(user.id).arg(user.avatartype);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        qDebug()<<"成功发送头像修改信息";

        connect(tcpSocket,&QTcpSocket::readyRead,[=](){

            QByteArray buffer = tcpSocket->readAll();

            //qDebug() << QString(buffer);

            if( QString(buffer).section("##",0,0) == QString("ok"))
            {

            }//成功发送
            else
            {
                QMessageBox::warning(this, "Warning!", "改头像不成功，仅你自己可见新头像", QMessageBox::Yes);
            }
        });
    }
}
void home::on_toolButton_2_clicked()
{
    this->close();
    grouphome *gh = new grouphome();
    gh->setWindowTitle("群聊");
    gh->show();
}


