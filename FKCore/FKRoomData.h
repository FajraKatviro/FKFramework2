#ifndef FKROOMDATA_H
#define FKROOMDATA_H

#include <QString>
#include <QDateTime>

class FKRoomDataFilter;
class QVariant;

class FKRoomData{
public:
    FKRoomData(const qint32 server, const QString& newRoomType, const QString& ownerId, const QDateTime& roomCreationTime, const bool isCustom);
    FKRoomData(const QVariant& data);
    FKRoomData();
    FKRoomData(const FKRoomData& other);
    bool isValid()const;
    QVariant toVariant()const;
    static QVariant createDelta(const qint32 maxActorsChange, const qint32 actorsChange, const qint32 maxUsersChange, const qint32 usersChange);
    void change(const QVariant& delta);
    bool fit(const FKRoomDataFilter& filter)const;
    void setReady(const bool val){_ready=val;}

    QString roomType()const{return _roomType;}
    QString owner()const{return _owner;}
    QDateTime creationTime()const{return _creationTime;}
    bool isCustom()const{return _custom;}
    qint32 maximumUsers()const{return _maximumUsers;}
    qint32 users()const{return _users;}
    qint32 maximumActors()const{return _maximumActors;}
    qint32 actors()const{return _actors;}
    qint32 server()const{return _server;}
    bool ready()const{return _ready;}

    void changeUsers(const qint32 userCount);
private:
    const QString _roomType;
    const QString _owner;
    const QDateTime _creationTime;
    const bool _custom;
    qint32 _maximumUsers;
    qint32 _users;
    qint32 _maximumActors;
    qint32 _actors;
    const qint32 _server;
    bool _ready;

    static const struct Identifiers{
        Identifiers():roomType("rt"),maximumUsers("mu"),users("cu"),
            maximumActors("ma"),actors("ca"),creationTime("ct"),
            custom("c"),owner("oc"){}
        const QString roomType;
        const QString maximumUsers;
        const QString users;
        const QString maximumActors;
        const QString actors;
        const QString creationTime;
        const QString custom;
        const QString owner;
    } identifiers;
};

class FKRoomCreateData{
public:
    FKRoomCreateData(const QString& id,const QString& roomType);
    FKRoomCreateData(const QVariant& data);
    FKRoomCreateData(const FKRoomCreateData& other);
    QVariant toVariant()const;
    QString roomId()const{return _id;}
    QString roomType()const{return _roomType;}
private:
    QString _id;
    QString _roomType;
    static const struct Identifiers{
        Identifiers():roomType("rt"),roomName("rn"){}
        const QString roomType;
        const QString roomName;
    } identifiers;
};

#endif // FKROOMDATA_H
