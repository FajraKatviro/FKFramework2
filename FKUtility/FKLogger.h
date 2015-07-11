#ifndef FK_LOGGER //исключим повторные объявления
#define FK_LOGGER

/*
Данный инструмент предоставляет возможность простого вывода отладочной
информации и контроля образования мусора

Доступ через следующие макросы:

FK_CBEGIN указывает начало конструктора класса, производного от QObject
FK_CEND   --------- конец  конструктора -------------------------------

FK_DBEGIN --------- начало деструктора  -------------------------------
FK_DEND   --------- конец  деструктора  -------------------------------

FK_MLOGV(const char* | QString description, QVariant value)
                            вывод сообщения descriptoion со значением

FK_MLOG(const char* | QString description)
                            вывод сообщения descriptoion без значения

FK_ELOG(const char* | QString description, QObject* object)
                            вывод сообщения descriptoion с проверкой
                            существования объекта по ссылке
                            метод не тестировался

пример использования в конструкторе:

FK_CBEGIN
FK_MLOG("_id",_id)
FK_ELOG("parent",parent())
FK_CEND

пример использования в деструкторе:

FK_DBEGIN
FK_MLOG("_id",_id)
FK_ELOG("parent",parent())
FK_DEND
*/

#ifdef FK_DEBUG //если включен режим специальной отладки

#include <QDebug> //подключим нужные модули (возможно, кое-что тут лишнее)
#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>

class FKLogger{ //статический класс для специальной отладки
#ifndef FK_NO_CONSTRUCTORS
    static QMap<QString,int> objects; //переменная, хранящая количество созданных объектов
#endif
public:
    static void writeLog(const char* fileName,int line,QString description, QVariant value){ //функция, непосредственно выводящая информацию
        qDebug()<<fileName<<" line "<<line<<". "<<description<<":\t\t"<<value;
     }
    static bool existTest(QObject* obj){ //функция, проверяющая существование объекта по ссылке
        if(obj==0)return false;
        try{			//если ссылка не обнулена, то вызов функции по ней вызовет исключение, которое мы обработаем
           obj->objectName();
           return true;
        }catch(...){
            return false;
        }
    }
};

#ifndef FK_NO_CONSTRUCTORS
#define FK_CBEGIN \
    {int i=FKLogger::objects.value(this->metaObject()->className()); \
    ++i; \
    FKLogger::objects[this->metaObject()->className()]=i; \
    FKLogger::writeLog(QString("Constructing ")+this->metaObject()->className(),i);}
#define FK_DBEGIN \
    {int i=FKLogger::objects.value(this->metaObject()->className());\
    --i;\
    FKLogger::objects[this->metaObject()->className()]=i;\
    FKLogger::writeLog(QString("Enter destructor. Elapsed ")+this->metaObject()->className(),i);}
#define FK_CEND FKLogger::writeLog(QString("Exit constructor"), QVariant());
#define FK_DEND FKLogger::writeLog(QString("Exit destructor"), QVariant());
#else
#define FK_CBEGIN ;
#define FK_DBEGIN ;
#define FK_CEND ;
#define FK_DEND ;
#endif

#define FK_MLOGV(description, value) FKLogger::writeLog(__FILE__,__LINE__,QString(description), value);
#define FK_MLOG(description) FKLogger::writeLog(__FILE__,__LINE__,QString(description), QVariant());
#define FK_ELOG(description, object) FKLogger::writeLog(QString(description), FKLogger::existTest(object));

#else //если режим специальной отладки выключен, отладочный код будет полностью исключен (в т.ч. исключены инклюды). Ниже приведено описание макросов.

#define FK_CBEGIN ;
#define FK_DBEGIN ;
#define FK_CEND ;
#define FK_DEND ;
#define FK_MLOGV(description, value) ;
#define FK_MLOG(description) ;
#define FK_ELOG(description, object) ;

#endif

#endif
