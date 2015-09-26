#ifndef FKROOMDATA_H
#define FKROOMDATA_H

#include <QString>
#include <QMap>

class FKRoomDataFilter;
class QVariant;

class QDateTime;

class FKRoomData{
public:
    FKRoomData(const qint32 server, const QString& newRoomType, const QString& ownerId, const QDateTime& roomCreationTime, const bool isCustom);
    FKRoomData(const QVariant& data);
    FKRoomData();
    FKRoomData(const FKRoomData& other);
    FKRoomData& operator=(const FKRoomData& other);
    bool isValid()const;
    QVariant toVariant()const;
    static QVariant createDelta(const qint32 propName, const QVariant& value);
    void change(const QVariant& value);
    bool fit(const FKRoomDataFilter& filter)const;

    void setValue(const qint32 propName,const QVariant& value);
    QVariant value(const qint32 propName)const;

    QString roomType()const;
private:
    QMap<qint32,QVariant> _values;
};

class FKRoomRequestData{ //this class used for client->realm requests
public:
    FKRoomRequestData(const QString& id,const QString& roomType,const QStringList& users, const bool custom);
    FKRoomRequestData(const QString& id,const QStringList& users);
    FKRoomRequestData(const QVariant& data);
    FKRoomRequestData(const FKRoomRequestData& other);
    QVariant toVariant()const;
    QString roomId()const{return _id;}
    QString roomType()const{return _roomType;}
    QStringList users()const{return _users;}
    bool custom()const{return _custom;}
private:
    QString _id;
    QString _roomType;
    QStringList _users;
    bool _custom;
    static const struct Identifiers{
        Identifiers():roomType("rt"),roomName("rn"),users("us"),custom("cu"){}
        const QString roomType;
        const QString roomName;
        const QString users;
        const QString custom;
    } identifiers;
};

#endif // FKROOMDATA_H
