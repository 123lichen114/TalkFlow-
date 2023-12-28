#ifndef SUCESSADDFRIEND_H
#define SUCESSADDFRIEND_H

#include <QDialog>

namespace Ui {
class sucessaddfriend;
}

class sucessaddfriend : public QDialog
{
    Q_OBJECT

public:
    explicit sucessaddfriend(QWidget *parent = nullptr);
    ~sucessaddfriend();

private:
    Ui::sucessaddfriend *ui;
};

#endif // SUCESSADDFRIEND_H
