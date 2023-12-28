#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QtNetwork>
#include <QCloseEvent>
#include <QTime>
#include <QAction>
#include <QLineEdit>
#include "QListWidgetItem"
#include"changeavatar.h"
#include "changename.h"
namespace Ui {
class home;
}

class home : public QWidget
{
    Q_OBJECT
    QTimer *timer;

public:
    explicit home(QWidget *parent = nullptr);
    ~home();

    void sendnamechange();
    void sendavatarchange();

protected:

    //void closeEvent(QCloseEvent *event);

private slots:
    void getFriendList();

    void on_pushButtonLogOut_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonDel_clicked();

    void on_listWidgetFriend_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_toolButton_2_clicked();

private:
    Ui::home *ui;
    QTcpSocket *tcpSocket;
    QString avatar;
};

#endif // HOME_H
