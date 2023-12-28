#include "server.h"
#include "ui_server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMap>
#include <QList>

QMap <int, int> userSockets;
QList <int> list;
int currentsize=0;//不同用户的socket号
int requestId=0;//询问请求编号

server::server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::server)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer();

    ui->listWidget->clear();
    ui->listWidget->insertItem(0,tr("当前无在线用户"));
    for(int i = 0; i < M; i++)
    {
        tcpSocket[i]=new QTcpSocket();
        list.append(i);
    }
    tcpServer->listen(QHostAddress::Any,8888);

    //init
    db = QSqlDatabase::addDatabase("QSQLITE");
    //判断是否建立了用户表
    db.setDatabaseName("./info.db");
    db.open();
    QSqlQuery sqlquery;
    sqlquery.exec("CREATE TABLE if not exists people (id INTEGER,name TEXT NOT NULL,password TEXT NOT NULL,ip TEXT,port TEXT,islogin INTEGER,ava INTEGER,PRIMARY KEY(id))");
    sqlquery.clear();
    sqlquery.exec("INSERT INTO people VALUES(10000,'root','1',NULL,NULL,0,0);");

    //建立朋友关系表
    sqlquery.clear();
    sqlquery.exec("CREATE TABLE if not exists friendship (id_1 INTEGER,id_2 INTEGER,PRIMARY KEY(id_1,id_2));");

    //建立聊天记录表
    sqlquery.clear();
    sqlquery.exec("CREATE TABLE if not exists friendchat (id_1 INTEGER,id_2 INTEGER,time TEXT,content TEXT,PRIMARY KEY(id_1,id_2,time));");

    //建立群聊记录表
    sqlquery.clear();
    sqlquery.exec("CREATE TABLE if not exists groupchat (id_1 INTEGER,id_2 INTEGER,time TEXT,content TEXT,PRIMARY KEY(id_1,id_2,time));");

    //建立群聊信息表
    sqlquery.clear();
    sqlquery.exec("CREATE TABLE if not exists groupinfo (id INTEGER,name TEXT,PRIMARY KEY(id));");
    sqlquery.clear();
    sqlquery.exec("INSERT INTO groupinfo VALUES(1000000,'root');");

    //建立群聊成员表
    sqlquery.clear();
    sqlquery.exec("CREATE TABLE if not exists groupmember (Gid INTEGER,Uid INTEGER, Uname TEXT,PRIMARY KEY(Gid,Uid));");
    db.close();

    connect(tcpServer,&QTcpServer::newConnection,[=](){
        int socketid = list.takeFirst();
        list.removeAll(socketid);
        tcpSocket[socketid] = tcpServer->nextPendingConnection();
        QString ip = tcpSocket[socketid]->peerAddress().toString().section(":",3,3);
        int port = tcpSocket[socketid]->peerPort();
        //预留currentsize以用作多用户同时连接所用
        QString str = QString("[%1:%2]").arg(ip).arg(port);
        //qDebug() << str ;

        connect(tcpSocket[socketid],&QTcpSocket::readyRead,[=](){
            //读取缓冲区数据
            QByteArray buffer = tcpSocket[socketid]->readAll();
            int userid = QString(buffer).section("##",1,1).toInt();
            if(userSockets.contains(userid))
            {
                list.append(userSockets[userid]);
                userSockets.remove(userid);
            }
            userSockets.insert(userid,socketid);

            if("login" == QString(buffer).section("##",0,0))
            {//登陆
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.prepare("select * from people where id = :id");
                sqlquery.bindValue(":id",QString(buffer).section("##",1,1));
                sqlquery.exec();
                if(!sqlquery.next())
                {//未查找到该用户
                    tcpSocket[socketid]->write(QString("login error##no_user").toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
                else
                {//用户存在
                    int id = sqlquery.value(0).toInt();
                    QString name = sqlquery.value(1).toString();
                    QString pwd = sqlquery.value(2).toString();
                    if(pwd == QString(buffer).section("##",2,2))
                    {//登录成功
                        tcpSocket[socketid]->write(QString("login successed##%1##%2").arg(id).arg(name).toUtf8());
                        sqlquery.clear();
                        sqlquery.prepare("update people set ip=:ip, port=:port, islogin=1 where id = :id");
                        sqlquery.bindValue(":ip",ip);
                        sqlquery.bindValue(":port",port);
                        sqlquery.bindValue(":id",QString(buffer).section("##",1,1));
                        sqlquery.exec();
                        tcpSocket[socketid]->flush();

                        //更新服务器界面
                        ui->listWidget->clear();
                        sqlquery.prepare("select * from people where islogin = 1");
                        sqlquery.exec();
                        if(sqlquery.next())
                        {
                            QString userid = sqlquery.value(0).toString();
                            QString username = sqlquery.value(1).toString();
                            QString userip = sqlquery.value(3).toString();
                            //qDebug()<<userid;
                            ui->listWidget->insertItem(0,"用户ID："+userid+",用户昵称:"+username+",用户IP:"+userip);
                            int rownum = 1;
                            while (sqlquery.next())
                            {
                                QString userid = sqlquery.value(0).toString();
                                QString username = sqlquery.value(1).toString();
                                QString userip = sqlquery.value(3).toString();
                                ui->listWidget->insertItem(rownum,"用户ID："+userid+",用户昵称:"+username+",用户IP:"+userip);
                                rownum++;
                            }
                        }
                        else
                        {
                            ui->listWidget->clear();
                            ui->listWidget->insertItem(0,tr("当前无在线用户"));
                        }
                    }
                    else
                    {//密码错误
                        tcpSocket[socketid]->write(QString("login error##errpwd").toUtf8());
                        tcpSocket[socketid]->flush();
                        db.close();
                    }
                }
            }
            else if("register" == QString(buffer).section("##",0,0))
            {//注册环节
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                //注册用户的时候需要进行判重
                sqlquery.prepare("select * from people where name = :name");
                sqlquery.bindValue(":name",QString(buffer).section("##",1,1));
                sqlquery.exec();
                if(!sqlquery.next())
                {//可以新建
                    //获得新建的用户的id
                    sqlquery.clear();
                    sqlquery.prepare("select MAX(id) from people;");
                    sqlquery.exec();
                    sqlquery.next();
                    int newid = sqlquery.value(0).toInt()+1;

                    //存储当前注册用户信息
                    sqlquery.clear();
                    sqlquery.prepare("insert into people values (:newid,:name,:password,null,null,0,1)");
                    sqlquery.bindValue(":newid",newid);
                    sqlquery.bindValue(":name",QString(buffer).section("##",1,1));
                    sqlquery.bindValue(":password",QString(buffer).section("##",2,2));
                    sqlquery.exec();

                    //将自己添加为好友(功能类型文件传输助手可以允许给自己发消息)
                    sqlquery.clear();
                    sqlquery.prepare("INSERT INTO friendship VALUES(:newid,:newid);");
                    sqlquery.bindValue(":newid",newid);
                    sqlquery.exec();

                    tcpSocket[socketid]->write(QString("register successed##"+QString::number(newid)).toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
                else
                {//有重名
                     tcpSocket[socketid]->write(QString("register error##same_name").toUtf8());
                     tcpSocket[socketid]->flush();
                     db.close();
                }
            }
            else if("getfriendlist" == QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                //查询所有好友
                sqlquery.exec("select * from friendship WHERE id_1=" + QString(buffer).section("##",1,1) );

                if(sqlquery.next())
                {
                    QList <int> friendlist;
                    friendlist.append(sqlquery.value(1).toInt());

                    QString friends = "";
                    while(sqlquery.next())
                    {
                        friendlist.append(sqlquery.value(1).toInt());
                    }

                    int onlinefriendnum = 0;
                    for( int i = 0; i < friendlist.length(); i++)
                    {
                        sqlquery.clear();
                        sqlquery.prepare("select * from people where id = :id");
                        sqlquery.bindValue(":id",QString::number(friendlist.at(i)));
                        sqlquery.exec();
                        sqlquery.next();
                        //qDebug()<<QString::number(friendlist.at(i))<<endl;
                        QString peopleip = sqlquery.value(3).toString();
                        QString peoplename = sqlquery.value(1).toString();
                        QString peopleava = sqlquery.value(6).toString();
                        if(sqlquery.value(5).toInt() == 1)
                        {
                            onlinefriendnum++;
                            friends += "##" + QString::number(friendlist.at(i)) +"##" + peoplename + "##1##" + peopleip + "##" + peopleava;
                        }
                        else
                        {
                            friends += "##" + QString::number(friendlist.at(i)) +"##" + peoplename + "##0##" + peopleip + "##" + peopleava;
                        }
                    }
                    friends = "getfriendlist_ok##" + QString::number(friendlist.length()) +"##"+QString::number(onlinefriendnum)+ friends;
                    //qDebug()<<friends;
                    tcpSocket[socketid]->write(friends.toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
                else
                {//无朋友
                    tcpSocket[0]->write(QString("getfriendlist_error").toUtf8());
                    tcpSocket[0]->flush();
                    db.close();
                }
            }
            else if("add_friend" == QString(buffer).section("##",0,0))
            {
                int whowantadd_id = QString(buffer).section("##",1,1).toInt();
                int friend_id = QString(buffer).section("##",2,2).toInt();
                QString whowantadd_name = QString(buffer).section("##",3,3);
                //检测被加者id是否存在
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.prepare("select * from people where id = :id");
                sqlquery.bindValue(":id",friend_id);
                sqlquery.exec();
                if(sqlquery.next())
                {
                    QString friend_ip = QString(buffer).section("##",3,3);
                    sqlquery.clear();
                    tcpSocket[userSockets[friend_id]]->write(QString("add_friend_request##%1##%2##%3").arg(whowantadd_id).arg(requestId).arg(whowantadd_name).toUtf8());
                    tcpSocket[userSockets[friend_id]]->flush();
                    //qDebug()<<"add_friend_request_send"<<endl;
                    db.close();
                }
                else
                {//没这人
                    //qDebug() <<"e";
                    tcpSocket[socketid]->write(QString("add_friend_error").toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
            }
            else if("add_friend_agreement" == QString(buffer).section("##",0,0))
            {
                int whowantadd_id = QString(buffer).section("##",1,1).toInt();
                int friend_id = QString(buffer).section("##",2,2).toInt();
                //1加2好友
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                QString sqlstring = "INSERT INTO friendship VALUES(:id1,:id2)";
                sqlquery.prepare(sqlstring);
                sqlquery.bindValue(":id1",whowantadd_id);
                sqlquery.bindValue(":id2",friend_id);
                sqlquery.exec();
                //2加1好友
                sqlquery.clear();
                sqlstring = "INSERT INTO friendship VALUES(:id2,:id1)";
                sqlquery.prepare(sqlstring);
                sqlquery.bindValue(":id1",whowantadd_id);
                sqlquery.bindValue(":id2",friend_id);
                sqlquery.exec();

                qDebug()<<sqlquery.lastError();

                tcpSocket[socketid]->write(QString("add_friend_ok").toUtf8());
                tcpSocket[socketid]->flush();
                db.close();
            }
            else if("add_friend_deny" == QString(buffer).section("##",0,0)){
                tcpSocket[socketid]->write(QString("add_friend_deny").toUtf8());
                tcpSocket[socketid]->flush();
            }
            else if("del_friend" == QString(buffer).section("##",0,0))
            {
                QString whowantadd_id = QString(buffer).section("##",1,1);
                QString friend_id = QString(buffer).section("##",2,2);
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.prepare("select * from people where id = :id");
                sqlquery.bindValue(":id",friend_id);
                sqlquery.exec();
                if(sqlquery.next())
                {
                    sqlquery.clear();
                    sqlquery.prepare("select * from friendship where id_1 = :id1 and id_2 = :id2");
                    sqlquery.bindValue(":id1",friend_id);
                    sqlquery.bindValue(":id2",whowantadd_id);
                    sqlquery.exec();
                    //qDebug()<<sqlquery.lastQuery()<<endl<<sqlquery.lastError().text()<<endl;
                    if(sqlquery.next())
                    {
                        sqlquery.clear();
                        sqlquery.prepare("DELETE FROM friendship WHERE id_1 = :id1 AND id_2 = :id2");
                        sqlquery.bindValue(":id1",friend_id);
                        sqlquery.bindValue(":id2",whowantadd_id);
                        sqlquery.exec();
                        sqlquery.clear();
                        sqlquery.prepare("DELETE FROM friendship WHERE id_1 = :id1 AND id_2 = :id2");
                        sqlquery.bindValue(":id2",friend_id);
                        sqlquery.bindValue(":id1",whowantadd_id);
                        sqlquery.exec();
                        tcpSocket[socketid]->write(QString("del_friend_ok").toUtf8());
                        tcpSocket[socketid]->flush();
                        //qDebug()<<"add_friend_request_send"<<endl;
                    }
                    else{
                        tcpSocket[socketid]->write(QString("del_friend_not_friend").toUtf8());
                        tcpSocket[socketid]->flush();
                    }
                    db.close();
                }
                else
                {//没这人
                    //qDebug() <<"e";
                    tcpSocket[socketid]->write(QString("del_friend_error").toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
            }
            else if("logout" == QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.prepare("update people set islogin=0 where id = :id");
                sqlquery.bindValue(":id",QString(buffer).section("##",1,1));
                sqlquery.exec();

                //更新服务器界面
                ui->listWidget->clear();
                sqlquery.prepare("select * from people where islogin = 1");
                sqlquery.exec();
                if(sqlquery.next())
                {
                    QString userid = sqlquery.value(0).toString();
                    QString username = sqlquery.value(1).toString();
                    QString userip = sqlquery.value(3).toString();
                    //qDebug()<<userid;
                    ui->listWidget->insertItem(0,"用户ID："+userid+",用户昵称:"+username+",用户IP:"+userip);
                    int rownum = 1;
                    while (sqlquery.next())
                    {
                        QString userid = sqlquery.value(0).toString();
                        QString username = sqlquery.value(1).toString();
                        QString userip = sqlquery.value(3).toString();
                        ui->listWidget->insertItem(rownum,"用户ID："+userid+",用户昵称:"+username+",用户IP:"+userip);
                        rownum++;
                    }
                }
                else
                {
                    ui->listWidget->clear();
                    ui->listWidget->insertItem(0,tr("当前无在线用户"));
                }
            }
            else if("chat_send" == QString(buffer).section("##",0,0))
            {
                if(QString(buffer).section("##",3,3).toInt()<1000000)
                {
                    db.setDatabaseName("./info.db");
                    db.open();
                    QSqlQuery sqlquery;
                    //注册用户的时候需要进行判重
                    sqlquery.prepare("INSERT INTO friendchat VALUES(:sender,:receiver,:time,:content)");
                    sqlquery.bindValue(":sender",QString(buffer).section("##",1,1).toInt());
                    sqlquery.bindValue(":receiver",QString(buffer).section("##",3,3).toInt());
                    sqlquery.bindValue(":content",QString(buffer).section("##",4,4));
                    sqlquery.bindValue(":time",QString(buffer).section("##",2,2));
                    sqlquery.exec();
                    db.close();
                }
                else
                {
                    db.setDatabaseName("./info.db");
                    db.open();
                    QSqlQuery sqlquery;
                    //注册用户的时候需要进行判重
                    sqlquery.prepare("INSERT INTO groupchat VALUES(:sender,:receiver,:time,:content)");
                    sqlquery.bindValue(":sender",QString(buffer).section("##",1,1).toInt());
                    sqlquery.bindValue(":receiver",QString(buffer).section("##",3,3).toInt());
                    sqlquery.bindValue(":content",QString(buffer).section("##",4,4));
                    sqlquery.bindValue(":time",QString(buffer).section("##",2,2));
                    sqlquery.exec();
                    db.close();
                }

            }
            else if("chat_history" == QString(buffer).section("##",0,0))
            {
                if(QString(buffer).section("##",2,2).toInt()<1000000)
                {
                    db.setDatabaseName("./info.db");
                    db.open();
                    QSqlQuery sqlquery;
                    //查询所有好友
                    sqlquery.prepare("SELECT * FROM friendchat WHERE (id_1=:userid AND id_2=:othersid) OR (id_2=:userid AND id_1=:othersid) ORDER BY time DESC;");
                    sqlquery.bindValue(":userid",QString(buffer).section("##",1,1).toInt());
                    sqlquery.bindValue(":othersid",QString(buffer).section("##",2,2).toInt());
                    sqlquery.exec();

                    if(sqlquery.next())
                    {
                        //qDebug()<<"chat_history_not_null"<<endl;
                        QString msg="";
                        QString time = sqlquery.value(2).toString();
                        QString senderid = sqlquery.value(0).toString();
                        QString txt = sqlquery.value(3).toString();
                        QSqlQuery sqlquery2;
                        sqlquery2.clear();
                        sqlquery2.prepare("SELECT * FROM people WHERE id=:id");
                        sqlquery2.bindValue(":id",senderid);
                        sqlquery2.exec();
                        sqlquery2.next();
                        QString sendername = sqlquery2.value(1).toString();
                        msg += "##" + time + "##" + senderid + "##" + txt + "##" + sendername;
                        int num = 1;
                        while(sqlquery.next())
                        {
                            QString time = sqlquery.value(2).toString();
                            QString senderid = sqlquery.value(0).toString();
                            QString txt = sqlquery.value(3).toString();
                            //QSqlQuery sqlquery2;
                            sqlquery2.clear();
                            sqlquery2.prepare("SELECT * FROM people WHERE id=:id");
                            sqlquery2.bindValue(":id",senderid);
                            sqlquery2.exec();
                            sqlquery2.next();
                            QString sendername = sqlquery2.value(1).toString();
                            msg += "##" + time + "##" + senderid + "##" + txt + "##" + sendername;
                            num++;
                        }
                        QString final = "chat_history_ok##" + QString::number(num) + msg;
                        //qDebug()<<final;
                        tcpSocket[socketid]->write(final.toUtf8());
                        tcpSocket[socketid]->flush();
                        db.close();
                    }
                    else
                    {//无朋友
                        tcpSocket[0]->write(QString("chat_history_error").toUtf8());
                        tcpSocket[0]->flush();
                        db.close();
                    }
                }
                //group_chat
                else
                {
                    db.setDatabaseName("./info.db");
                    db.open();
                    QSqlQuery sqlquery;
                    //查询是否在该群聊
                    sqlquery.clear();
                    sqlquery.prepare("SELECT * FROM groupmember WHERE Uid=:userid AND Gid=:othersid;");
                    sqlquery.bindValue(":userid",QString(buffer).section("##",1,1).toInt());
                    sqlquery.bindValue(":othersid",QString(buffer).section("##",2,2).toInt());
                    sqlquery.exec();
                    //qDebug()<<"fine I come here"<<endl;
                    if(sqlquery.next())
                    {
                        //qDebug()<<"enter success"<<endl;
                        sqlquery.clear();
                        sqlquery.prepare("SELECT * FROM groupchat WHERE id_2=:othersid ORDER BY time DESC;");
                        sqlquery.bindValue(":othersid",QString(buffer).section("##",2,2).toInt());
                        sqlquery.exec();
                        QString msg="";
                        int num = 0;
                        while(sqlquery.next())
                        {
                            QString time = sqlquery.value(2).toString();
                            QString senderid = sqlquery.value(0).toString();
                            QString txt = sqlquery.value(3).toString();
                            QSqlQuery sqlquery2;
                            sqlquery2.clear();
                            sqlquery2.prepare("SELECT * FROM people WHERE id=:id");
                            sqlquery2.bindValue(":id",senderid);
                            sqlquery2.exec();
                            sqlquery2.next();
                            QString sendername = sqlquery2.value(1).toString();
                            msg += "##" + time + "##" + senderid + "##" + txt + "##" + sendername;
                            num++;
                        }
                        QString final = "chat_history_ok##" + QString::number(num) + msg;
                        qDebug()<<final;
                        tcpSocket[socketid]->write(final.toUtf8());
                        tcpSocket[socketid]->flush();
                        db.close();
                    }
                    else
                    {//无朋友
                        tcpSocket[0]->write(QString("chat_history_error").toUtf8());
                        tcpSocket[0]->flush();
                        db.close();
                    }
                }
            }
            else if("create_group" == QString(buffer).section("##",0,0))
            {
                QString create_user_id = QString(buffer).section("##",1,1);
                QString group_name = QString(buffer).section("##",2,2);
                QString user_name = QString(buffer).section("##",3,3);
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                //获取群聊id
                sqlquery.clear();
                sqlquery.prepare("select MAX(id) from groupinfo;");
                sqlquery.exec();
                sqlquery.next();
                int newid = sqlquery.value(0).toInt()+1;
                //获取群聊名称
                sqlquery.clear();
                sqlquery.prepare("insert into groupinfo values (:newid,:name)");
                sqlquery.bindValue(":newid",QString::number(newid));
                sqlquery.bindValue(":name",group_name);
                sqlquery.exec();
                //将该成员加入群聊
                sqlquery.clear();
                sqlquery.prepare("insert into groupmember values (:Gid,:Uid,:Uname)");
                sqlquery.bindValue(":Gid",newid);
                sqlquery.bindValue(":Uid",create_user_id.toInt());
                sqlquery.bindValue(":Uname",user_name);
                sqlquery.exec();
                db.close();
                tcpSocket[socketid]->write(QString("create group successed##"+QString::number(newid)).toUtf8());
                tcpSocket[socketid]->flush();
            }
            else if("add_group" == QString(buffer).section("##",0,0))
            {
                QString wanted = QString(buffer).section("##",1,1);
                QString group_id = QString(buffer).section("##",2,2);
                QString user_name = QString(buffer).section("##",3,3);
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("insert into groupmember values (:Gid,:Uid,:Uname)");
                sqlquery.bindValue(":Gid",group_id.toInt());
                sqlquery.bindValue(":Uid",wanted.toInt());
                sqlquery.bindValue(":Uname",user_name);
                sqlquery.exec();
                db.close();
                tcpSocket[socketid]->write(QString("add group successed##"+group_id).toUtf8());
                tcpSocket[socketid]->flush();
            }
            else if("leave_group" == QString(buffer).section("##",0,0))
            {
                QString wanted = QString(buffer).section("##",1,1);
                QString group_id = QString(buffer).section("##",2,2);
                QString user_name = QString(buffer).section("##",3,3);
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("DELETE FROM groupmember WHERE Uid=:uid AND Gid=:gid");
                sqlquery.bindValue(":uid",wanted.toInt());
                sqlquery.bindValue(":gid",group_id.toInt());
                sqlquery.exec();
                db.close();
                tcpSocket[socketid]->write(QString("del group successed##"+group_id).toUtf8());
                tcpSocket[socketid]->flush();
            }
            else if("getgrouplist" == QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                int userid=QString(buffer).section("##",1,1).toInt();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("SELECT * FROM groupmember WHERE Uid=:uid");
                sqlquery.bindValue(":uid",userid);
                sqlquery.exec();
                if(sqlquery.next())
                {
                    QList <int> grouplist;
                    grouplist.append(sqlquery.value(0).toInt());

                    QString friends = "";
                    while(sqlquery.next())
                    {
                        grouplist.append(sqlquery.value(0).toInt());
                    }

                    int groupnum = 0;
                    for( int i = 0; i < grouplist.length(); i++)
                    {
                        sqlquery.clear();
                        sqlquery.prepare("select * from groupinfo where id = :id");
                        sqlquery.bindValue(":id",QString::number(grouplist.at(i)));
                        sqlquery.exec();
                        sqlquery.next();
                        //qDebug()<<QString::number(friendlist.at(i))<<endl;
                        QString groupname = sqlquery.value(1).toString();
                        friends += "##" + QString::number(grouplist.at(i)) +"##" + groupname;
                    }
                    friends = "getgrouplist_ok##" + QString::number(grouplist.length()) + friends;
                    //qDebug()<<friends;
                    tcpSocket[socketid]->write(friends.toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
                else
                {//无朋友
                    tcpSocket[socketid]->write(QString("getgrouplist_error").toUtf8());
                    tcpSocket[socketid]->flush();
                    db.close();
                }
            }
            else if("name_change"== QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("UPDATE people SET name=:name WHERE id=:id;");
                sqlquery.bindValue(":id",QString(buffer).section("##",1,1).toInt());
                sqlquery.bindValue(":name",QString(buffer).section("##",2,2));
                sqlquery.exec();
            }
            else if("avatar_change"== QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("UPDATE people SET ava=:ava WHERE id=:id;");
                sqlquery.bindValue(":id",QString(buffer).section("##",1,1).toInt());
                sqlquery.bindValue(":ava",QString(buffer).section("##",2,2).toInt());
                sqlquery.exec();
            }
            else if("get_ip"== QString(buffer).section("##",0,0))
            {
                db.setDatabaseName("./info.db");
                db.open();
                QSqlQuery sqlquery;
                sqlquery.clear();
                sqlquery.prepare("SELECT ip FROM people WHERE id=:id;");
                sqlquery.bindValue(":id",QString(buffer).section("##",2,2).toInt());
                sqlquery.exec();
                sqlquery.next();
                tcpSocket[socketid]->write(QString("get ip successfully##%1").arg(sqlquery.value(0).toString()).toUtf8());
                tcpSocket[socketid]->flush();
                db.close();
            }
        });
    });
}

server::~server()
{
    tcpServer->close();
    tcpServer->deleteLater();
    for(int i = 0; i < M; i++)
    {
        tcpSocket[i]->close();
        delete tcpSocket[i];
    }
    delete tcpServer;
    delete ui;
}
