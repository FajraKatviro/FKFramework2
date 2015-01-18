#ifndef FKPACKAGE_H
#define FKPACKAGE_H

#include <QMap>
#include <QVariant>

class FKPackageObject{
public:
    FKPackageObject(const QString& source=QString());
    FKPackageObject(const FKPackageObject& other);

    bool setEnabled(const bool t);

    void setCacheEnabled(const bool enabled);
    void setCacheEnabled(const QString& resource,const bool enabled);
    void preload();
    void preload(const QString& resource);
    void clearCache();
    void clearCache(const QString& resource);

    QVariant resourceData(const QString& resource);
private:
    QVariant readData(const qint64 offset);
    bool _enabled;
    const QString _source;

    struct Resource{
        Resource():flags(0),offset(0){}
        Resource(const qint64 offset):flags(0),offset(offset){}
        QVariant value;
        const qint64 offset;
        bool isCached()const{return flags & cached;}
        bool isCacheEnabled()const{return flags & cacheEnabled;}

        void setCacheEnabled(){flags |= cacheEnabled;}
        void setCacheDisabled(){flags &= ~cacheEnabled;}
        void setCached(){flags |= cached;}
        void clearCache(){value.clear(); flags &= ~cached;}
    private:
        qint8 flags;
        static const qint8 cacheEnabled;
        static const qint8 cached;
    };
    typedef QSharedPointer<FKPackageObject::Resource> FKPackageResource;
    QMap<QString,FKPackageResource> _resources;
};

#endif // FKPACKAGE_H
