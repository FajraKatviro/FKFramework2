#include "FKPackageManager.h"

#include "FKLogger.h"

#include <QSharedPointer>

FKPackageManager::FKPackageManager(QObject* parent):QObject(parent),_packagesInitialized(false){
    FK_CBEGIN
    FK_CEND
}

FKPackageManager::~FKPackageManager(){
    FK_DBEGIN
    FK_DEND
}

void FKPackageManager::initializeManager(){
    if(!_packagesInitialized){
        initializePackages();
        _packagesInitialized=true;
    }
}

FKPackage FKPackageManager::getPackage(const QString& packageName) const{
    auto i=_packages.constFind(packageName);
    if(i!=_packages.constEnd()){
        return i.value();
    }else{
        FK_MLOGV("Package not found",packageName)
        return FKPackage();
    }
}

FKPackage FKPackageManager::addPackage(const QString& packageName, const QString& sourceFile){
    auto i=_packages.constFind(packageName);
    if(i==_packages.constEnd()){
        FKPackage newPackage=FKPackage(new FKPackageObject(sourceFile));
        _packages.insert(packageName,newPackage);
        return newPackage;
    }else{
        FK_MLOGV("Package already exists",packageName)
        return i.value();
    }
}
