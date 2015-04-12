#ifndef FKROOMINVITEDATA_H
#define FKROOMINVITEDATA_H

class QString;
class QStringList;
class QVariant;
class QMap;

class FKRoomInviteData{
public:
    FKRoomInviteData();
    FKRoomInviteData(const QString& client);
    FKRoomInviteData(const QString& client,const QString& address,const qint32 port);
    FKRoomInviteData(const QVariant& data);
    FKRoomInviteData(const FKRoomInviteData& other);
    ~FKRoomInviteData();

    bool addUser(const QString& user);
    bool addUser(const QString& user,const QString& password);

    QVariant toVariant()const;
    bool isValid()const;
    QString client()const;
    QStringList users()const;
    QMap<QString,QString> passwords()const;
    QString address()const;
    qint32 port()const;

    void setAddress(const QString& ip);
private:
    bool tryAddUser(const QString& user,const QString& password);

    QString _client;
    QMap<QString,QString> _passwords;
    QString _address;
    qint32 _port;
    bool _isValid;

    static const struct Identifiers{
        Identifiers():roomType("rt"),roomName("rn"){}
        const QString client;
        const QString users;
        const QString passwords;
        const QString address;
        const QString port;
    } identifiers;
};

#endif // FKROOMINVITEDATA_H
