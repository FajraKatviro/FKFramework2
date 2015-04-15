#include "FKDataBasePath.h"

#include "FKDataBase.h"

FKDataBasePath::FKDataBasePath(FKDataBasePath* parent):_parent(parent){}

FKDBIndex FKDataBasePath::path() const{
    FKDBIndex ind=_parent ? _parent->path() : FKDBIndex();
    ind>>index();
    return ind;
}

void FKDataBasePath::bindNode(const QString& nodeName) const{
    _nodeName=nodeName;
}

FKDBIndex FKDataBasePath::index() const{
    FKDBIndex ind;
    if(!_nodeName.isEmpty())ind>>_nodeName;
    return ind;
}


FKDataBasePathCommon::FKDataBasePathCommon(const QString& path, FKDataBasePath* parent):FKDataBasePath(parent),_path(path){}

FKDBIndex FKDataBasePathCommon::index() const{
    FKDBIndex ind;
    ind>>_path;
    return ind;
}

