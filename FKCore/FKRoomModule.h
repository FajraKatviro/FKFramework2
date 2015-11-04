#ifndef FKROOMMODULE_H
#define FKROOMMODULE_H

#include <QObject>
#include <QMutex>
#include <QPluginLoader>

class FKVersionList;
class FKRoomLoader;
class FKPackageObject;
typedef QSharedPointer<FKPackageObject> FKPackage;
class FKPackageManager;
class FKRoomContext;

class FKRoomModule:public QObject{
    Q_OBJECT
public:
    FKRoomModule(QObject* parent=0,const QString& module=QString("FKChatRoom"));
    ~FKRoomModule();
    bool load();

    FKVersionList version()const;
    QString contextManager()const;
    QString contextComponent()const;
    FKRoomContext* createContextObject()const;

    FKPackage getPackage(const QString& package);
    QVariant resourceData(const QString& package, const QString& resource);
private:
    void loadPackageManager();
    bool loadRoomPlugin();
    bool _loaded;
    const QString _module;
    QPluginLoader _loader;
    FKRoomLoader* _room;
    FKPackageManager* _packageManager;
    static QMutex listMutex;
    static QMap<QString,qint32> loadedModules;
};

#endif // FKROOMMODULE_H
