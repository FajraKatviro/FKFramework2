#ifndef FKCONSOLETESTER
#define FKCONSOLETESTER

#include <QtTest/QtTest>
#include <QFile>
#include <QXmlSimpleReader>

#include "FKApplication.h"
#include "FKSimpleCore.h"
#include "FKConsoleInterface.h"
#include "FKRoomEngine.h"
#include "FKCommandResolver.h"
#include "FKPathResolver.h"

class Handler:public QXmlDefaultHandler{
    bool fatalError(const QXmlParseException &/*exception*/){return false;}
    bool startElement(const QString &/*namespaceURI*/,
                      const QString &localName,
                      const QString &/*qName*/,
                      const QXmlAttributes &atts){
        if(localName == "Worksheet"){
            _actualSheet = atts.value("ss:Name");
            _actualRow = 0;
        }else if(localName == "Row"){
            _column=0;
        }else if(localName == "Data" && _actualRow>0 && _actualSheet == _targetSheet){
            _doRead = true;
            ++_column;
        }
        return true;
    }
    bool endElement(const QString &/*namespaceURI*/,
                    const QString &localName,
                    const QString &/*qName*/){
        if(localName == "Row"){
            ++_actualRow;
        }else if(localName == "Data"){
            if(_doRead){
                _doRead = false;
                if(_column!=2){
                    _collected.append(_readen);
                }
                _readen.clear();
            }
        }
        return true;
    }
    bool characters(const QString &ch){
        if(_doRead){
            _readen.append(ch);
        }
        return true;
    }
    qint32 _actualRow=0;
    QString _actualSheet;
    QString _targetSheet;
    QString _readen;
    QStringList _collected;
    bool _doRead=false;
    qint32 _column;
public:
    Handler(const QString targetSheet):QXmlDefaultHandler(),_targetSheet(targetSheet){}
    QStringList collected()const{return _collected;}
};

class FKConsoleTester: public QObject{
    Q_OBJECT
public:
    FKConsoleTester(QObject* parent=0):QObject(parent){
        app.setCore<FKSimpleCore>();
        app.setUserInterface<FKConsoleInterface>();
        app.setRoomEngine<FKRoomEngine>();
        app.setInterfaceBridge(QStringLiteral("ConsoleBridge"));
        app.start();
        static_cast<FKConsoleInterface*>(app.getUi())->disableConsoleReader();
    }
protected:
    void sendCommand(QString msg,QString arg=QString()){
        if(!msg.isEmpty()){
            app.getUi()->commandResolver()->processInput(msg+" "+arg);
        }
    }
    void loadTestData(const qint32 columnCount){
        QFile f(QFINDTESTDATA("testData.xml"));
        QXmlSimpleReader reader;
        QXmlInputSource source(&f);
        Handler handler(QTest::currentTestFunction());
        reader.setContentHandler(&handler);
        reader.setErrorHandler(&handler);
        bool ok = reader.parse(source);
        if (!ok){
            QWARN("Unable read data file");
        }else{
            QStringList dataList(handler.collected());
            qint32 size = dataList.size();
            for(auto i=0;i<size;i+=columnCount+1){
                QString dataName(dataList.at(i));
                QTestData& row = QTest::newRow(dataName.toLatin1());
                for(auto j=1;j<=columnCount;++j){
                    row<<dataList.at(i+j);
                }
            }
        }
    }
    bool resetDatabase(){
        QDir dir(FKPathResolver::databasePath());
        return dir.removeRecursively();
    }
    FKApplication app;
};

#endif // FKCONSOLETESTER

