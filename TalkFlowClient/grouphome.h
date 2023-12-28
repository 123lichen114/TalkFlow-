#ifndef GROUPHOME_H
#define GROUPHOME_H

#include <QWidget>
#include <QtNetwork>
#include <QCloseEvent>
#include <QTime>
#include <QAction>
#include <QLineEdit>
#include "QListWidgetItem"
#include"changeavatar.h"
#include"changename.h"

namespace Ui {
class grouphome;
}

class grouphome : public QWidget
{
    Q_OBJECT
    QTimer *timer;

public:
    explicit grouphome(QWidget *parent = nullptr);
    ~grouphome();

protected:

    //void closeEvent(QCloseEvent *event);

private slots:
    void getGroupList();

    void on_pushButtonCreate_clicked();

    void on_toolButtonFriend_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonDel_clicked();

    void on_listWidgetGroup_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::grouphome *ui;
    QTcpSocket *tcpSocket;
};

#endif // HOME_H
