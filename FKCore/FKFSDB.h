#ifndef FKFSDB_H
#define FKFSDB_H

#include "FKDataBase.h"

#include <QDir>

#define FKFSDB_VALUE QStringLiteral("v.v")

class FKFSDB : public FKDataBase{
    Q_OBJECT
public:
    FKFSDB(QObject *parent = 0);
    ~FKFSDB();
    void setPath(const QString& path)override;
    void reset()override;
    bool hasIndex(const FKDBIndex& index)const override;
    void removeIndex(const FKDBIndex& index)override;
    FKDBIndex findIndex(const FKDBValue& value,const FKDBIndex& parentIndex,bool recursive=false)const override;
    qint32 countValues(const FKDBIndex& ind)const override;
    QList<FKDBValue> getValues(const FKDBIndex& parentIndex)const override;
    QStringList getProperties(const FKDBIndex& parentIndex)const override;
    QMap<QString,FKDBValue> mapValues(const FKDBIndex& parentIndex)const override;
private:
    FKDBValue retriveValue(const FKDBIndex& index)const override;
    void writeValue(const FKDBValue& value,const FKDBIndex& index)const override;
    FKDBValue readValueFromDirectory(const QDir& path)const;
    void writeValueToDirectory(const FKDBValue& value, const QDir& path)const;
    QDir getIndexDirectory(const FKDBIndex& index)const;
    void cutNode(const FKDBIndex& finalIndex)override;
    QDir _root;
};

#endif // FKFSDB_H
