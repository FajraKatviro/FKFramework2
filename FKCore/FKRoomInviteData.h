#ifndef FKROOMINVITEDATA_H
#define FKROOMINVITEDATA_H

#include <QString>
#include <QMetaType>

class QString;
class QStringList;
class QVariant;

class FKRoomInviteData{
public:
    FKRoomInviteData();
    FKRoomInviteData(const QString& client,const QString& password);
    FKRoomInviteData(const QVariant& data);
    FKRoomInviteData(const FKRoomInviteData& other);
    ~FKRoomInviteData();

    bool addUser(const QString& user);

    QVariant toClientInvite()const;
    QVariant toServerInvite()const;
    bool isValid()const;
    QString client()const;
    QString password()const;
    QStringList users()const;
    QString address()const;
    qint32 port()const;
    QString roomType()const;

    void setAddress(const QString& ip);
    void setPort(const qint32 port);
    void setRoomType(const QString& rt);
private:
    bool tryAddUser(const QString& user);

    QString _client;
    QString _password;
    QStringList _users;
    QString _address;
    qint32 _port=-1;
    bool _isValid;
    QString _roomType;

    static const struct Identifiers{
        Identifiers():client("c"),users("u"),password("p"),
            address("a"),port("o"),roomType("r"){}
        const QString client;
        const QString users;
        const QString password;
        const QString address;
        const QString port;
        const QString roomType;
    } identifiers;
};
Q_DECLARE_METATYPE(FKRoomInviteData)

#endif // FKROOMINVITEDATA_H
