#ifndef SUCCESSFULLYCREATEGROUPCHAT_H
#define SUCCESSFULLYCREATEGROUPCHAT_H

#include <QDialog>

namespace Ui {
class successfullycreategroupchat;
}

class successfullycreategroupchat : public QDialog
{
    Q_OBJECT

public:
    explicit successfullycreategroupchat(QWidget *parent = nullptr);
    ~successfullycreategroupchat();

private:
    Ui::successfullycreategroupchat *ui;
};

#endif // SUCCESSFULLYCREATEGROUPCHAT_H
