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
    void setPath(const QString& path);
    void reset();
    bool hasIndex(const FKDBIndex& index)const;
    void removeIndex(const FKDBIndex& index);
    FKDBIndex findIndex(const FKDBValue& value,const FKDBIndex& parentIndex,bool recursive=false)const;
    qint32 countValues(const FKDBIndex& ind) const;
    QList<FKDBValue> getValues(const FKDBIndex& parentIndex)const;
    QStringList getProperties(const FKDBIndex& parentIndex)const;
    QMap<QString,FKDBValue> mapValues(const FKDBIndex& parentIndex)const;
private:
    FKDBValue retriveValue(const FKDBIndex& index)const;
    void writeValue(const FKDBValue& value,const FKDBIndex& index)const;
    FKDBValue readValueFromDirectory(const QDir& path)const;
    void writeValueToDirectory(const FKDBValue& value, const QDir& path)const;
    QDir getIndexDirectory(const FKDBIndex& index)const;
    void cutNode(const FKDBIndex& finalIndex) override;
    QDir _root;
};

#endif // FKFSDB_H
