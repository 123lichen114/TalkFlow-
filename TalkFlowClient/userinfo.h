#ifndef USERINFO_H
#define USERINFO_H

#include <QString>

class userinfo
{
public:
   int id;
   QString name;
   bool islogin;//true在线
   QString ip;
   int port;
   QString signature;//used as path of the avatar due to some reason
   bool hasnamechanged=false;
   bool hasavatarchanged=false;
   QString avatartype="0";

   userinfo(int id_,QString name_);
   userinfo();

};


#endif // USERINFO_H
