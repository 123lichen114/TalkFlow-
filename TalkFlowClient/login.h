#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:

    void on_checkBoxShowPwd_stateChanged(int arg1);

    void on_pushButtonLogin_clicked();

    void on_pushButtonReg_clicked();

private:
    Ui::login *ui;
    QTcpSocket *tcpSocket;
};

#endif // LOGIN_H
