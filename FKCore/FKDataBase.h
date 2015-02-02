#ifndef FKDATABASE_H
#define FKDATABASE_H

#include <QObject>

#include "fkcore_global.h"

#include <QStringList>
#include <QVariant>

struct FKCORESHARED_EXPORT FKDBIndex{
    FKDBIndex(){}
    FKDBIndex(const FKDBIndex& other):_nodeNames(other._nodeNames){}
    FKDBIndex(const QStringList& nodeNames):_nodeNames(nodeNames){}
    FKDBIndex(const QList<FKDBIndex>& indexList);
    FKDBIndex& operator>>(const QString& offset){_nodeNames.append(offset);return *this;}
    FKDBIndex& operator>>(const FKDBIndex& children){_nodeNames.append(children._nodeNames);return *this;}
    FKDBIndex& operator<<(const int offsetDepth);
    QString property()const{return _nodeNames.last();}
    int depth()const{return _nodeNames.size();}
    QStringList sequentialPath()const{return _nodeNames;}
    bool isValid()const{return !_nodeNames.isEmpty();}
    static bool isNodeName(const QString& nodeName);
private:
    QStringList _nodeNames;

    friend struct FKDBValue;
};
Q_DECLARE_METATYPE(FKDBIndex)

struct FKDBValue{
    enum ValueType{Number=0,String=1,Index=2};
    FKDBValue(const FKDBValue& other):_valueType(other._valueType),_value(other._value){}
    FKDBValue(const QVariant& value,ValueType t):_valueType(t),_value(value){}
    FKDBValue(const qreal value=0.0):_valueType(Number),_value(value){}
    FKDBValue(const bool value):FKDBValue(qreal(value)){}
    FKDBValue(const QString& value):_valueType(String),_value(value){}
    FKDBValue(const FKDBIndex& value):_valueType(Index),_value(value._nodeNames){}
    ValueType valueType()const{return _valueType;}
    QVariant value()const{return _value;}
    qreal number()const{return _valueType==Number ? _value.toReal() : 0.0;}
    bool boolean()const{return _valueType==Number && _value.toReal();}
    QString string()const{return _valueType==String ? _value.toString() : QString();}
    FKDBIndex index()const{return _valueType==Index ? FKDBIndex(_value.toStringList()) : QStringList();}
    bool operator==(const FKDBValue& other)const;
private:
    ValueType _valueType;
    QVariant _value;
#ifdef FK_DB_DESCRIPTIONS
public:
    QString description()const{return _description;}
    void setDescription(const QString& descr){_description=descr;}
private:
    QString _description;
#endif
    friend QDataStream& operator>>(QDataStream& in,FKDBValue& value);
    friend QDataStream& operator<<(QDataStream& out, const FKDBValue& value);
};
Q_DECLARE_METATYPE(FKDBValue)

class FKCORESHARED_EXPORT FKDataBase:public QObject{
    Q_OBJECT
public:
    FKDataBase(QObject *parent = 0);
    ~FKDataBase();
    virtual void setPath(const QString& path){this->path=path;}
    virtual void reset()=0;
    virtual bool hasIndex(const FKDBIndex& index)const=0;
    void addIndex(const FKDBIndex& index)const;
    virtual void removeIndex(const FKDBIndex& index)=0;
    FKDBIndex getIndex(FKDBIndex index)const;
    FKDBIndex getIndex(const QList<FKDBIndex>& indexes)const;
    FKDBValue getValue(const FKDBIndex& index,bool recursive=true)const;
    FKDBValue getValue(const QList<FKDBIndex>& indexes)const;
    bool setValue(const FKDBValue& value,const FKDBIndex& index,bool recursive=true)const;
    void writeValue(const FKDBValue& value,const FKDBIndex& index,bool recursive)const;
    virtual FKDBIndex findIndex(const FKDBValue& value,const FKDBIndex& parentIndex,bool recursive=false)const=0;
    virtual qint32 countValues(const FKDBIndex& ind)const=0;
    virtual QList<FKDBValue> getValues(const FKDBIndex& parentIndex)const=0;
    virtual QStringList getProperties(const FKDBIndex& parentIndex)const=0;
    virtual QMap<QString,FKDBValue> mapValues(const FKDBIndex& parentIndex)const=0;
signals:
    void dataChanged(const FKDBIndex index,const FKDBValue value);
protected:
    QString path;
private:
    virtual FKDBValue retriveValue(const FKDBIndex& index)const=0;
    virtual void writeValue(const FKDBValue& value,const FKDBIndex& index)const=0;
};

#endif // FKDATABASE_H
