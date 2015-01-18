#ifndef FKROOMDATAFILTER_H
#define FKROOMDATAFILTER_H

#include <QString>
#include <QDateTime>

class FKRoomDataFilter{
public:
    FKRoomDataFilter(const QVariant& filterData);
    FKRoomDataFilter(const QString& roomType=QString(),const qint32 maximumUsersMax=0,const qint32 maximumUsersMin=0,
        const qint32 usersMax=0,const qint32 usersMin=0,const qint32 maximumActorsMax=0,
        const qint32 maximumActorsMin=0,const qint32 actorsMax=0,const qint32 actorsMin=0,
        const QDateTime& earliest=QDateTime(),const QDateTime& latest=QDateTime(),
        const bool custom=true,const bool notCustom=true,const QString& owner=QString());
    QVariant toVariant()const;
private:
    bool checkEmpty()const;
    bool _empty;
    QString _roomType;
    qint32 _maximumUsersMax;
    qint32 _maximumUsersMin;
    qint32 _usersMax;
    qint32 _usersMin;
    qint32 _maximumActorsMax;
    qint32 _maximumActorsMin;
    qint32 _actorsMax;
    qint32 _actorsMin;
    QDateTime _earliest;
    QDateTime _latest;
    bool _custom;
    bool _notCustom;
    QString _owner;

    static const struct Identifiers{
        Identifiers():roomType("rt"),maximumUsersMax("mua"),maximumUsersMin("mui"),
            usersMax("ua"),usersMin("ui"),maximumActorsMax("maa"),maximumActorsMin("mai"),
            actorsMax("aa"),actorsMin("ai"),earliest("e"),latest("l"),
            custom("c"),notCustom("nc"),owner("w"){}
        const QString roomType;
        const QString maximumUsersMax;
        const QString maximumUsersMin;
        const QString usersMax;
        const QString usersMin;
        const QString maximumActorsMax;
        const QString maximumActorsMin;
        const QString actorsMax;
        const QString actorsMin;
        const QString earliest;
        const QString latest;
        const QString custom;
        const QString notCustom;
        const QString owner;
    } identifiers;

    friend struct FKRoomData;
};

#endif // FKROOMDATAFILTER_H
