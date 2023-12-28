#ifndef CHANGEAVATAR_H
#define CHANGEAVATAR_H

#include <QDialog>

namespace Ui {
class changeavatar;
}

class changeavatar : public QDialog
{
    Q_OBJECT

public:
    explicit changeavatar(QWidget *parent = nullptr);
    ~changeavatar();
signals:
    void sig();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::changeavatar *ui;
};

#endif // CHANGEAVATAR_H
