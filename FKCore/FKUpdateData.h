#ifndef FKUPDATEDATA_H
#define FKUPDATEDATA_H

#include "FKSystemObject.h"

class FKUpdateData:public FKSystemObject{
public:
    FKUpdateData();
    ~FKUpdateData();

    FKUpdateData(const QString& src, const qint32 index);
    FKUpdateData(const qint32 index, const QByteArray& data, const bool final);

    bool isFinal()const;
    QString path()const;
    qint32 index()const;
private:
    virtual bool packObject(QDataStream& stream)const override;
    virtual bool loadObject(QDataStream& stream)override;
    qint32 _index=-1;
    QString _path;
    QByteArray _data;
    bool _final=false;
};


#endif // FKUPDATEDATA_H
