#ifndef FKIDGENERATOR_H
#define FKIDGENERATOR_H

#include <QStack>

//This struct useing to simplify generating unique id
//If id would not use anymore, use back(id) function to return id back to free-id-pool

struct FKIDGenerator{
    FKIDGenerator(qint32 initialValue=0):_last(initialValue-1){}
    qint32 take(){return _free.isEmpty() ? ++_last : _free.pop();}
    void back(qint32 id){_free.push(id);}
private:
    qint32 _last;
    QStack<qint32> _free;
};

#endif // FKIDGENERATOR_H
