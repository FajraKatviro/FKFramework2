#include "FKDataBase.h"

#include "FKLogger.h"

/*!
 * \struct FKDBIndex
 * \brief Use this struct to point FKDataBase element. Internally index consist of string list that sequentially determines child node beginning from the root
 */

/*!
 * \fn FKDBIndex::FKDBIndex()
 * \brief Constructs empty index (pointing root element)
 */

/*!
 * \fn FKDBIndex::FKDBIndex(const FKDBIndex& other)
 * \brief Constructs copy of other
 */

/*!
 * \fn FKDBIndex::FKDBIndex(const QStringList& nodeNames)
 * \brief Constructs index points nodeNames
 */

/*!
 * \brief Constructs final index by index list. Assumes that the first index is relative to root and other indexes are relative to previos index target
 */

FKDBIndex::FKDBIndex(const QList<FKDBIndex>& indexList){
    for(auto i=indexList.constBegin();i!=indexList.constEnd();++i){
        _nodeNames.append(i->_nodeNames);
    }
}

/*!
 * \fn FKDBIndex& FKDBIndex::operator>>(const QString& offset)
 * \brief Make index point to child element of current element
 */

/*!
 * \fn FKDBIndex& FKDBIndex::operator>>(const FKDBIndex& children)
 * \brief Make index point to subelement. Assumes offset index is relative to current element
 */

/*!
 * \brief Make index point to N levels up. If N >= depth, index would point to root element
 */

FKDBIndex& FKDBIndex::operator<<(const int offsetDepth){
    int d=qMin(depth(),offsetDepth);
    for(int i=0;i<d;++i)_nodeNames.removeLast();
    return *this;
}

/*!
 * \brief Check \i nodeName can be valid index node
 */

bool FKDBIndex::isNodeName(const QString& nodeName){
    return QRegExp("[0-9a-zA-Z]+").exactMatch(nodeName);
}

/*!
 * \fn QStringList FKDBIndex::sequentialPath()const
 * \brief Return sequential hierarchical node names
 */

/*!
 * \fn bool FKDBIndex::isValid()const
 * \brief Return false if index is empty and points to root element
 */

/*!
 * \fn QString FKDBIndex::property()const
 * \brief Return target element node name
 */

/*!
 * \fn int FKDBIndex::depth()const
 * \brief Returns depth-level of target element
 */


/*!
 * \struct FKDBValue
 * \brief Struct represents common value type that can be reed or written to FKDataBase. Value can be numeric, string or database index
 */

/*!
 * \fn FKDBValue::FKDBValue(const FKDBValue& other)
 * \brief Constructs copy of other
 */

/*!
 * \fn FKDBValue::FKDBValue(const QVariant& value,ValueType t)
 * \brief Creates value of type t. QVariant should has appropriate internal type to be properly casted toT() later
 */

/*!
 * \fn FKDBValue::FKDBValue(const qreal value)
 * \brief Constructs value with numeric content. This is default constructor that creates 0.0 value
 */

/*!
 * \fn FKDBValue::FKDBValue(const QString& value)
 * \brief Constructs value with string content.
 */

/*!
 * \fn FKDBValue::FKDBValue(const FKDBIndex& value)
 * \brief Constructs value with index content.
 */

/*!
\enum FKDBValue::ValueType

\brief Determines casting behaivour for internal data

\value Number
        Value contains a number
\value String
        Value contains a QString
\value Index
        Value contains a FKDBIndex
*/

/*!
 * \fn ValueType FKDBValue::valueType()const
 * \brief Returns ValueType of value
 */

/*!
 * \fn QVariant FKDBValue::value()const
 * \brief Returns value as non-casted QVariant
 */

/*!
 * \fn qreal FKDBValue::number()const
 * \brief Returns value as qreal if ValueType is Numeric, otherwise return 0.0
 */

/*!
 * \fn QString FKDBValue::string()const
 * \brief Returns value as QString if ValueType is String, otherwise return empty QString
 */

/*!
 * \fn FKDBIndex FKDBValue::index()const
 * \brief Returns value as FKDBIndex if ValueType is Index, otherwise return empty FKDBIndex
 */

/*!
 * \brief Returns true if 2 values completely equal, include containing type
 */

bool FKDBValue::operator==(const FKDBValue& other)const{
    return _valueType==other._valueType && _value==other._value;
}

/*!
 * \brief Read value from the stream. Assumes stream has valid data.
 */

QDataStream& operator>>(QDataStream& in,FKDBValue& value){
    qint8 valueType;
    in>>valueType;
    in>>value._value;
    value._valueType=static_cast<FKDBValue::ValueType>(valueType);
#ifdef FK_DB_DESCRIPTIONS
    _description.clear();
    if(!in.atEnd())in>>value._description;
#endif
    return in;
}

/*!
 * \brief Write value to the stream.
 */

QDataStream& operator<<(QDataStream& out, const FKDBValue& value){
    qint8 valueType=static_cast<qint8>(value._valueType);
    out<<valueType;
    out<<value._value;
#ifdef FK_DB_DESCRIPTIONS
    if(!value._description.isEmpty())out<<value._description;
#endif
    return out;
}

/*!
 * \macro FK_DB_DESCRIPTIONS
 * \brief Define this statement to use DataBase description values. If database architecture supply record's description, use setDescription() to add description string to value and description() to retrive it.
 */

/*!
 * \fn QString FKDBValue::description()const
 * \brief Returns description string of value. To use this function, define FK_DB_DESCRIPTIONS at compile time
 */

/*!
 * \fn void FKDBValue::setDescription(const QString& descr)
 * \brief Sets description string for value. To use this function, define FK_DB_DESCRIPTIONS at compile time
 */


/*!
 * \class FKDataBase
 * \brief Provides access to tree-hierarched database and operations with it.
 */

/*!
 * \brief Constructs instance of class. This would not create new or connect existing database before setPath() call
 */

FKDataBase::FKDataBase(QObject *parent):QObject(parent){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs instance of class
 */

FKDataBase::~FKDataBase(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \fn void FKDataBase::setPath(const QString& path)
 * \brief Set path to db and connect to it. Default implementation only remember path string
 */

/*!
 * \fn void FKDataBase::reset()
 * \brief Resets DataBase values
 */

/*!
 * \fn bool FKDataBase::hasIndex(const FKDBIndex& index)const
 * \brief Check for existing record for given index
 */

/*!
 * \brief Creates record for given index with 0.0 value. If record already exists, the value would be overriten
 */

void FKDataBase::addIndex(const FKDBIndex& index)const{
    writeValue(FKDBValue(),index);
}

/*!
 * \fn void FKDataBase::removeIndex(const FKDBIndex& index)
 * \brief Removes record for given index. All child records should be also removed
 */

/*!
 * \brief Go by the index recursively and returns final index
 */

FKDBIndex FKDataBase::getIndex(FKDBIndex index)const{
    FKDBValue last(index);
    do{
        index=last.index();
        last=retriveValue(last.index());
    }while(last.valueType()==FKDBValue::Index);
    return index;
}

/*!
 * \brief Go by the first index recursively, appends to result next index from list, and repeat for all elements
 */

FKDBIndex FKDataBase::getIndex(const QList<FKDBIndex>& indexes)const{
    FKDBIndex index;
    for(auto i=indexes.constBegin();i!=indexes.constEnd();++i){
        index=getIndex(index>>(*i));
    }
    return index;
}

/*!
 * \brief Returns value for given index. If \i recursive argument is true, function follows gotten value before get not-index value
 */

FKDBValue FKDataBase::getValue(const FKDBIndex& index,bool recursive)const{
    FKDBValue val=retriveValue(index);
    if(recursive){
        while(val.valueType()==FKDBValue::Index){
            val=retriveValue(val.index());
        }
    }
    return val;
}

/*!
 * \brief Go by the first index recursively, appends to result next index from list, and repeat for all elements. Return value for final index
 */

FKDBValue FKDataBase::getValue(const QList<FKDBIndex>& indexes)const{
    if(indexes.isEmpty())return FKDBValue();
    FKDBIndex index;
    auto f=indexes.constEnd()-1;
    for(auto i=indexes.constBegin();i!=f;++i){
        index=getIndex(index>>(*i));
    }
    return getValue(index>>indexes.last(),true);
}

/*!
 * \brief Try write value for given index. If record does not exists, return false and happung nothing.  If \i recursive argument is true, function follows gotten value before get not-index value and then change that value
 */

bool FKDataBase::setValue(const FKDBValue& value,const FKDBIndex& index,bool recursive)const{
    if(hasIndex(index)){
        if(recursive){
            FKDBValue val=retriveValue(index);
            if(val.valueType()==FKDBValue::Index){
                return setValue(value,val.index(),true);
            }else{
                writeValue(value,index);
                return true;
            }
        }else{
            writeValue(value,index);
            return true;
        }
    }
    return false;
}

/*!
 * \brief Writes data to database at given index. Creates record if needed.
 */

void FKDataBase::writeValue(const FKDBValue& value,const FKDBIndex& index,bool recursive)const{
    if(recursive){
        writeValue(value,getIndex(index));
    }else{
        writeValue(value,index);
    }
}

/*!
 * \fn FKDBIndex FKDataBase::findIndex(const FKDBValue& value,const FKDBIndex& parentIndex)const
 * \brief Returns index for child of given parent that has given value. If there are more than 1 value, returns the first. If there is no equal value or given index, returns invalid index.
 */

/*!
 * \fn qint32 FKDataBase::countValues(const FKDBIndex& ind)const
 * \brief Returns number of sub-nodes for given index
 */

/*!
 * \fn QList<FKDBValue> FKDataBase::getValues(const FKDBIndex& parentIndex)const
 * \brief Returns values list for given parent children. Order the same as getProperties() return order.
 */

/*!
 * \fn QStringList FKDataBase::getProperties(const FKDBIndex& parentIndex)const
 * \brief Returns nodeName list for given parent children. Order the same as getValues() return order.
 */

/*!
 * \fn QMap<QString,FKDBValue> FKDataBase::mapValues(const FKDBIndex& parentIndex)const
 * \brief Returns parent's children as nodeName-value pairs.
 */

/*!
 * \fn void FKDataBase::dataChanged(const FKDBIndex index,const FKDBValue value)
 * \brief This signal emitting after some data of database was changed by this object
 */
