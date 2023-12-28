#ifndef changename_H
#define changename_H

#include <QDialog>

namespace Ui {
class changename;
}

class changename : public QDialog
{
    Q_OBJECT

public:
    explicit changename(QWidget *parent = nullptr);
    ~changename();
signals:
    void namechange(const QString &nickname);
private slots:
    void on_buttonBox_accepted();
    void confirm();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::changename *ui;
};

#endif // changename_H
