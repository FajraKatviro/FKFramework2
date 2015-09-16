#ifndef FKVERSIONLIST_H
#define FKVERSIONLIST_H

#include <QList>
#include <QMetaType>

class QVariant;

class FKVersionString{
public:
    FKVersionString(const QString& ver=QString()):_str(ver){}
    FKVersionString(const FKVersionString& other):_str(other._str){}
    inline QString string()const{return _str;}
    bool operator>(const FKVersionString& other)const;
    bool operator<(const FKVersionString& other)const;
    bool operator==(const FKVersionString& other)const;
    bool operator!=(const FKVersionString& other)const;
private:
    const QString _str;
    QList<qint32> numbers()const;
};

class FKVersionList{
public:
    FKVersionList(){}
    FKVersionList(const FKVersionList& other):_enteties(other._enteties){}
    ~FKVersionList(){}

    bool isValid()const;

    void addFile(const QString& path,const QString& version,const qint8 platform);
    void addFile(const QString& path,const QString& version,const QList<qint8>& platforms);
    //use path relative to rooms folder

    const FKVersionString actual()const;
    //const FKVersionList operator-(const FKVersionList& other)const;
    //const FKVersionList operator+(const FKVersionList& other)const;
    //const FKVersionList operator[](const qint8 platform)const;
    //const FKVersionList operator[](const QList<qint8>& platforms)const;

    struct Data{
        Data(const QString& path,const FKVersionString& version,const qint8 platform,const qint64 size):
            path(path),version(version),platform(platform),size(size){}
        const QString path;
        const FKVersionString version;
        const qint8 platform=0;
        const qint64 size=0;
    };
    inline const QList<FKVersionList::Data>& enteties()const{return _enteties;}
    QPair<QString,qint64> getInfo(const QString& path,const qint8 platform);
private:
    QList<FKVersionList::Data> _enteties;

    friend QDataStream& operator<<(QDataStream& stream, const FKVersionList& lst);
    friend QDataStream& operator>>(QDataStream& stream, FKVersionList& lst);
};

QDataStream& operator<<(QDataStream& stream, const FKVersionList& lst);
QDataStream& operator>>(QDataStream& stream, FKVersionList& lst);

Q_DECLARE_METATYPE(FKVersionList)

#endif // FKVERSIONLIST_H
