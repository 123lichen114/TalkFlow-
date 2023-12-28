#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QInputDialog>

namespace Ui {
class addfriends;
}

class addfriends : public QInputDialog
{
    Q_OBJECT

public:
    explicit addfriends(QWidget *parent = nullptr);
    ~addfriends();

private:
    Ui::addfriends *ui;
};

#endif // ADDFRIENDS_H
