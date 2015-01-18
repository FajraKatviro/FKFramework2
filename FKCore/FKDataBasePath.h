#ifndef FKDATABASEPATH_H
#define FKDATABASEPATH_H

#include <QString>

class FKDBIndex;

class FKDataBasePath{
public:
    FKDataBasePath(FKDataBasePath* parent=0);
    FKDBIndex path()const;
    void bindNode(const QString& nodeName);
private:
    virtual FKDBIndex index()const;
    QString _nodeName;
    FKDataBasePath* _parent;
};

class FKDataBasePathCommon:public FKDataBasePath{
public:
    FKDataBasePathCommon(const QString& path,FKDataBasePath* parent=0);
private:
    virtual FKDBIndex index()const;
    const QString _path;
};

#endif // FKDATABASEPATH_H
