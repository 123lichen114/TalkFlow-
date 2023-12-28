#include "addfriends.h"
#include "ui_addfriends.h"

addfriends::addfriends(QWidget *parent) :
    QInputDialog(parent),
    ui(new Ui::addfriends)
{
    ui->setupUi(this);
}

addfriends::~addfriends()
{
    delete ui;
}
