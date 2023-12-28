#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class registerdialog;
}

class registerdialog : public QWidget
{
    Q_OBJECT

public:
    explicit registerdialog(QWidget *parent = nullptr);
    ~registerdialog();

private slots:
    void on_checkBoxShowPwd_stateChanged(int arg1);

    void on_pushButtonLogin_clicked();

    void on_pushButtonReg_clicked();

private:
    Ui::registerdialog *ui;
    QTcpSocket *tcpSocket;

    bool checkInputInfo();
};

#endif // REGISTERDIALOG_H
