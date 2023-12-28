#include "ui_login.h"
#include "userinfo.h"
#include "home.h"
#include "login.h"
#include "registerdialog.h"
#include <QtNetwork>
#include <QWidget>
#include <QMessageBox>

userinfo user;
QString hostip = "192.168.245.1";
int hosthost = 8888;

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket();
}

login::~login()
{
    delete ui;
    delete tcpSocket;
}

void login::on_checkBoxShowPwd_stateChanged(int arg1)//显示用户输入的密码
{
    if(arg1){
        ui->lineEditPwd->setEchoMode(QLineEdit::EchoMode::Normal);
    }
    else{
        ui->lineEditPwd->setEchoMode(QLineEdit::EchoMode::Password);
    }
}

void login::on_pushButtonLogin_clicked()//处理点击登陆按钮操作
{
    if(ui->lineEditUserName->text()!=""&& ui->lineEditPwd->text()!= "")
    {//账号密码不能为空
        tcpSocket->abort();//取消已有链接
        tcpSocket->connectToHost(hostip, hosthost);//链接服务器
        QString ip = tcpSocket->peerAddress().toString().section(":",3,3);
        int port = tcpSocket->peerPort();
        QString str = QString("[%1:%2]").arg(ip).arg(port);
        qDebug() << str ;
        if(!tcpSocket->waitForConnected(20000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        }
        else
        {//服务器连接成功
            QString loginmessage = QString("login##%1##%2").arg(ui->lineEditUserName->text()).arg(ui->lineEditPwd->text());
            tcpSocket->write(loginmessage.toUtf8());
            tcpSocket->flush();
            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                if(QString(buffer).section("##",0,0)==QString("login successed"))
                {//登陆成功
                    user.id=QString(buffer).section("##",1,1).toInt();
                    user.name = QString(buffer).section("##",2,2);
                    user.islogin = true;
                    this->close();
                    home *hom = new home();
                    hom->setWindowTitle("主页");
                    hom->show();
                    //qDebug()<<"login success"<<endl;
                }
                else if(QString(buffer).section("##",0,0)==QString("login error"))
                {
                    if(QString(buffer).section("##",1,1)==QString("no_user"))
                    {//用户不存在
                        QMessageBox::warning(this, "Warning!", "用户不存在", QMessageBox::Yes);
                        ui->lineEditUserName->clear();
                        ui->lineEditPwd->clear();
                        ui->lineEditUserName->setFocus();
                    }
                    else if(QString(buffer).section("##",1,1)==QString("errpwd"))
                    {
                        QMessageBox::warning(this, "Warning!", "密码错误", QMessageBox::Yes);
                        ui->lineEditPwd->clear();
                        ui->lineEditPwd->setFocus();
                    }
                }
            });
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
        ui->lineEditUserName->clear();
        ui->lineEditPwd->clear();
        ui->lineEditUserName->setFocus();
    }
}

void login::on_pushButtonReg_clicked()//处理点击注册按钮操作
{
    this->close();
    registerdialog *reg = new registerdialog();
    reg->setWindowTitle("注册");
    reg->show();
}
