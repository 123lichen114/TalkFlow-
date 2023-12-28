#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include <QWidget>

class MessageItem : public QWidget
{
    Q_OBJECT
public:
    MessageItem(const QString& text, const QString& avatarPath) : QWidget(){};

signals:

};


#endif // MESSAGEITEM_H
