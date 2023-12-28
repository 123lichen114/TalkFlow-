#include "chat_1v1.h"
#include "ui_chat_1v1.h"
#include "QWidget"
#include "ui_chat_1v1.h"
#include <userinfo.h>
#include <QTimer>
#include <QMessageBox>
#include <QHostAddress>
#include <login.h>
#include <QDateTime>
#include <QEvent>
#include <QDebug>
#include "sendfiledialog.h"
#include "receivefiledialog.h"
extern userinfo user;
bool is_open_chatdialog; //对话是否打开
extern userinfo otheruser;
extern QString hostip;
extern int hosthost;
chat_1v1::chat_1v1(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::chat_1v1)
{
    ui->setupUi(this);
    ui->Title->setText("您正在与"+otheruser.name+"进行对话");
    is_open_chatdialog = true;
    tcpSocket = new QTcpSocket();
    timer = new QTimer();
    timer->start(500);
    //qDebug()<<otheruser.id<<otheruser.name;
    connect(timer,SIGNAL(timeout()),this,SLOT(getchathistory()));

}
chat_1v1::~chat_1v1()
{
    is_open_chatdialog = false;
    timer->stop();
    delete ui;
}

void chat_1v1::getchathistory()
{
    //qDebug()<<"getchathistory called";
    tcpSocket->abort();//取消已有链接
    tcpSocket->connectToHost(hostip, hosthost);//链接服务器

    if(!tcpSocket->waitForConnected(30000))
    {
        //qDebug()<<"Failed to connect";
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        this->close();
    }
    else
    {//服务器连接成功
        QString message = QString("chat_history##%1##%2").arg(user.id).arg(otheruser.id);
        //qDebug()<<"";
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            QByteArray buffer = tcpSocket->readAll();
            //qDebug()<<"readyread"<<endl;
            if(QString(buffer).section("##",0,0)==QString("chat_history_ok"))
            {
                //qDebug()<<"getchathistory OK";
                QString chatshow = "";
                int num = QString(buffer).section("##",1,1).toInt();
                for(int rownum = 0;rownum < num ;rownum++)
                {
                    QDateTime time = QDateTime::fromString( QString(buffer).section("##",rownum*4+2,rownum*4+2),"yyyy-MM-dd hh:mm:ss.zzz");
                    //qDebug()<<time.toString();
                    QString timeshow = time.toString("MM-dd hh:mm:ss");
                    //qDebug()<<timeshow;
                    QString senderid = QString(buffer).section("##",rownum*4+3,rownum*4+3);
                    QString idshow = "";
                    if(senderid.toInt() == user.id)
                    {//我自己发送的消息
                        idshow = " 我：";
                    }
                    else
                    {
                        idshow = " " + QString(buffer).section("##",rownum*4+5,rownum*4+5) + "：";
                    }
                    chatshow = "("+timeshow+")" + idshow + QString(buffer).section("##",rownum*4+4,rownum*4+4) +"\n" + chatshow;
                    qDebug()<<chatshow<<endl;
                }
                //ui->textBrowser_1v1->clear();
                ui->textBrowser_1v1->setText(chatshow);
            }
            else if(QString(buffer).section("##",0,0)==QString("chat_history_error"))
            {
                ui->textBrowser_1v1->setText("无消息记录");
            }
        });
    }
}


void chat_1v1::on_Button_sendMsg_clicked() //点击发送
{
    //发送消息
        if(ui->userinput->text()!="")
        {
            //qDebug()<<"Msg not null";
            tcpSocket->abort();//取消已有链接
            tcpSocket->connectToHost(hostip, hosthost);//链接服务器
            QString ip = tcpSocket->peerAddress().toString().section(":",3,3);
            int port = tcpSocket->peerPort();
            QString str = QString("[%1:%2]").arg(ip).arg(port);
            //qDebug() << str ;
            if(!tcpSocket->waitForConnected(30000))
            {
                QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
                this->close();
                user.islogin = false;
                login *lg = new login();
                lg->show();
            }
            else
            {//服务器连接成功
                //第一个是时间，第二个发送的ID，第三个是接受的ID，第四个是内容
                //qDebug()<<"Connect ok";
                QString nowstr = QDateTime::currentDateTime().currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
                //qDebug()<<nowstr;
                QString message = QString("chat_send##%1##%2##%3##%4").arg(user.id).arg(nowstr).arg(otheruser.id).arg(ui->userinput->text());
                //qDebug()<<message;
                tcpSocket->write(message.toUtf8());
                tcpSocket->flush();
                ui->textBrowser_1v1->insertPlainText(message);
                ui->userinput->clear();
            }
        }
        else
        {//空消息
            QMessageBox::warning(this, "Warning!", "不能发送空消息", QMessageBox::Yes);
        }
}



//李宸 加了这个函数 意思是当B在和A的聊天框中，A发出了文件后，B能得到通知是否接收 模仿了home.cpp中的好友申请，还不知道有没有起作用
void chat_1v1::getReady_for_file()
{
    //qDebug()<<"ENTER getReady_for_file"<<endl;
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
        //qDebug()<<"ENTER getReady_for_file"<<endl;
        QString message = QString("receive_file_request##%1").arg(user.id);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect
        ( tcpSocket,&QTcpSocket::readyRead,[=](){
            QByteArray buffer = tcpSocket->readAll();
           if(QString(buffer).section("##",0,0)==QString("receive_file_request"))
            {
                //qDebug()<<"receive_file_request"<<endl;
                QString friendname = QString(buffer).section("##",3,3);
                QString friendid = QString(buffer).section("##",1,1);
                QString dlgTitle="提示";
                QString strInfo="您将收到id:"+friendid+",用户名:"+friendname+"传来的文件\n确定接收吗？";
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
                        QString message = QString("recieve_file_agreement##%1##%2").arg(user.id).arg(friendid);
                        tcpSocket->write(message.toUtf8());
                        tcpSocket->flush();
                    }
                    else
                    {
                        QString message = QString("recieve_file_deny");
                        tcpSocket->write(message.toUtf8());
                        tcpSocket->flush();
                    }
                }
            }
        });
    }
}

void chat_1v1::on_Button_sendfile_clicked() //发文件
{
   sendfiledialog *sf=new sendfiledialog();
   sf->show();
}
void chat_1v1::on_Button_receivefile_clicked() //收文件
{
    receivefiledialog*se =new receivefiledialog();
    se->show();
}

void chat_1v1::on_pushButton_clicked()
{
    sendfiledialog *sf=new sendfiledialog();
    sf->show();
}
