#ifndef CREATEGROUPCHAT_H
#define CREATEGROUPCHAT_H

#include <QDialog>

namespace Ui {
class creategroupchat;
}

class creategroupchat : public QDialog
{
    Q_OBJECT

public:
    explicit creategroupchat(QWidget *parent = nullptr);
    ~creategroupchat();

private:
    Ui::creategroupchat *ui;
};

#endif // CREATEGROUPCHAT_H
