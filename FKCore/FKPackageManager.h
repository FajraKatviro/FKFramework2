#ifndef FKPACKAGEMANAGER_H
#define FKPACKAGEMANAGER_H

#include <QObject>

#include "fkcore_global.h"
#include "FKPackage.h"

class FKCORESHARED_EXPORT FKPackageManager:public QObject{
    Q_OBJECT
public:
    FKPackageManager(QObject* parent=0);
    ~FKPackageManager();
    void initializeManager();
    FKPackage getPackage(const QString& packageName)const;
protected:
    FKPackage addPackage(const QString& packageName,const QString& sourceFile);
private:
    virtual void initializePackages(){}
    bool _packagesInitialized;
    QMap<QString,FKPackage> _packages;
};

#endif // FKPACKAGEMANAGER_H
