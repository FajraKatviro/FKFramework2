#include "FKSystemObject.h"

#include "FKLogger.h"

FKFactory<FKSystemObject> FKSystemObject::_factory;
const QString FKSystemObject::objectIdentifier=QStringLiteral("fko");

/*!
\class FKSystemObject
\brief Basic object class provides save & load and re-constructing mechanisms
*/

/*!
 * \brief Constructs FKSystemObject
 */

FKSystemObject::FKSystemObject(){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs FKSystemObject
 */

FKSystemObject::~FKSystemObject(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Send object to the stream. This function use FK_SYSTEM_OBJECT macro to write object's bounds and packObject(stream) virtual function to write data. Return true on success.
 */

bool FKSystemObject::pack(QDataStream& stream)const{
    stream<<objectIdentifier;
    if(packObject(stream)){
        stream<<objectIdentifier;
        return true;
    }
    return false;
}

/*!
 * \brief Load object from the stream. This function use FK_SYSTEM_OBJECT macro to check object's bounds and loadObject(stream) virtual function to read data. Return true on success.
 */

bool FKSystemObject::load(QDataStream& stream){
    QString validator;
    stream>>validator;
    if(validator==objectIdentifier){
        if(loadObject(stream)){
            stream>>validator;
            if(validator==objectIdentifier){
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief Creates object of class with given className. Class must inherit FKSystemObject class and have default constructor marked Q_INVOKABLE. Also allowConstructing() function for that class must be called before first object constructing
 */

FKSystemObject* FKSystemObject::create(const QString& className){
    return _factory.create(className);
}

/*!
 * \fn void FKSystemObject::allowConstructing()
 * \brief Call this function for class, inherited from FKSystemObject, to let create instancies of that class
 */

/*!
 * \fn FKSystemObject* FKSystemObject::clone()const
 * \brief Create copy instance of the object
 */
