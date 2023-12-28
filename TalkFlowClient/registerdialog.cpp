#include "login.h"
#include "userinfo.h"
#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QString>
#include <string>

extern userinfo user;
extern QString hostip;
extern int hosthost;

registerdialog::registerdialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerdialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket();
}

registerdialog::~registerdialog()
{
    delete ui;
    delete tcpSocket;
}

bool registerdialog::checkInputInfo()//检查初步检查输入注册信息
{
    QString username=ui->lineEditUserName->text();
    QString pwd=ui->lineEditPwd->text();
    QString confirm=ui->lineEditConfirmPwd->text();
    int flag=0;
    QRegExp rx("^(?=.*[a-zA-Z])(?=.*[0-9])(?=.*[._~!@#$^&*])[A-Za-z0-9._~!@#$^&*]{8,16}$");
    if (QString::compare(pwd, confirm,Qt::CaseSensitive)!=0) flag=1;
    else if (username==""||pwd=="") flag=2;
    else if (pwd.length()<8||pwd.length()>16) flag=3;
    else if (!rx.exactMatch(pwd)) flag=4;
    //qDebug()<<confirm.toStdString().c_str()<<endl;
    switch (flag)
    {
        case 1:
            QMessageBox::warning(this, "Warning!", "两次输入密码不同", QMessageBox::Yes);
            return false;
        case 2:
            QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
            return false;
        case 3:
            QMessageBox::warning(this, "Warning!", "密码长度要求为8-16", QMessageBox::Yes);
            return false;
        case 4:
            QMessageBox::warning(this, "Warning!", "密码必须由字母、数字、特殊符号组成", QMessageBox::Yes);//不需要大小写都有
            return false;
        default:
            return true;
    }
}

void registerdialog::on_checkBoxShowPwd_stateChanged(int arg1)
{
    if(arg1){
        ui->lineEditPwd->setEchoMode(QLineEdit::EchoMode::Normal);
        ui->lineEditConfirmPwd->setEchoMode(QLineEdit::EchoMode::Normal);
    }
    else{
        ui->lineEditPwd->setEchoMode(QLineEdit::EchoMode::Password);
        ui->lineEditConfirmPwd->setEchoMode(QLineEdit::EchoMode::Password);
    }
}


void registerdialog::on_pushButtonLogin_clicked()//处理点击登陆按钮操作
{
    this->close();
    login * logi=new login();
    logi->setWindowTitle("登录");
    logi->show();
}



void registerdialog::on_pushButtonReg_clicked()//处理点击注册按钮操作
{
    if(checkInputInfo())
    {
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
            QString loginmessage = QString("register##%1##%2").arg(ui->lineEditUserName->text()).arg(ui->lineEditPwd->text());
            tcpSocket->write(loginmessage.toUtf8());
            tcpSocket->flush();
            QString ip = tcpSocket->peerAddress().toString().section(":",3,3);
            int port = tcpSocket->peerPort();
            QString str = QString("[%1:%2]").arg(ip).arg(port);
            qDebug() << str ;
            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QByteArray buffer = tcpSocket->readAll();
                if(QString(buffer).section("##",0,0)==QString("register successed"))
                {//注册成功
                    QMessageBox::warning(this, "注册成功!", "你的id是"+QString(buffer).section("##",1,1), QMessageBox::Yes);
                    ui->lineEditUserName->clear();
                    ui->lineEditPwd->clear();
                    ui->lineEditConfirmPwd->clear();
                    ui->lineEditUserName->setFocus();
                }
                else if(QString(buffer).section("##",0,0)==QString("register error"))
                {
                    if(QString(buffer).section("##",1,1)==QString("same_name"))
                    {
                        QMessageBox::warning(this, "Warning!", "昵称有重复", QMessageBox::Yes);
                        ui->lineEditUserName->clear();
                        ui->lineEditUserName->setFocus();
                    }
                }
            });
        }
    }
    else
    {
        ui->lineEditUserName->clear();
        ui->lineEditPwd->clear();
        ui->lineEditConfirmPwd->clear();
        ui->lineEditUserName->setFocus();
    }
}
