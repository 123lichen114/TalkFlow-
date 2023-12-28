#ifndef CHAT_1V1_H
#define CHAT_1V1_H
#include <QTcpSocket>
#include <QDialog>
#include <QString>
#include <QTimer>
#include <QtWidgets>
namespace Ui {
class chat_1v1;
}

class chat_1v1 : public QDialog
{
    Q_OBJECT
private:
    Ui::chat_1v1 *ui;
    QTcpSocket *tcpSocket;
public:
    QTimer *timer;
    explicit chat_1v1(QDialog *parent = nullptr);
    ~chat_1v1();
protected:
    ;
private slots:

    void getchathistory();
    void on_Button_sendMsg_clicked();
    void on_Button_sendfile_clicked();
    void getReady_for_file();
    void on_Button_receivefile_clicked();
    void on_pushButton_clicked();
};

#endif // CHAT_1V1_H
