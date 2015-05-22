#ifndef FKROOMMODULE_H
#define FKROOMMODULE_H

#include <QObject>
#include <QPluginLoader>

class FKRoomLoader;
class FKPackageObject;
typedef QSharedPointer<FKPackageObject> FKPackage;
class FKPackageManager;

class FKRoomModule:public QObject{
    Q_OBJECT
public:
    FKRoomModule(const QString& module,QObject* parent=0);
    ~FKRoomModule();
    bool load();

    QString version()const;
    QString roomClass()const;

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
};

#endif // FKROOMMODULE_H
