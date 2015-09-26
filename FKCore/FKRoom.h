#ifndef FKROOM_H
#define FKROOM_H

#include "FKObject.h"

class FKVersionList;
class FKRoomData;
class FKRoomInviteData;

class FKCORESHARED_EXPORT FKRoom:public FKObject{
    Q_OBJECT
    Q_PROPERTY(qint32 o_usersSize READ ro_usersSize WRITE wo_usersSize)
public:
    static FKVersionList version();
    virtual void setup(const FKRoomData& data);
    virtual bool addUsers(const FKRoomInviteData& data);
    void enableUsers(const QList<qint32> userList);
    qint32 usersSize()const;
protected:
    inline QVariant getSetting(const qint32 setting)const{return _roomSettings.value(setting);}
    inline void setSetting(const qint32 setting,const QVariant& value){_roomSettings.setValue(setting,value);}
signals:
    void roomDataChanged(const QString propName, const QVariant value);
    void clientInviteResolved(const FKRoomInviteData data,const QList<qint32> userObjects);
private:
    qint32 ro_usersSize()const;
    void wo_usersSize(const qint32 size);

    FKRoomData _roomSettings;
    qint32 _usersSize;
};


#endif // FKROOM_H
