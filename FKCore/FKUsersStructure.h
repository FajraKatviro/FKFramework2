#ifndef FKUSERSSTRUCTURE_H
#define FKUSERSSTRUCTURE_H

#include <QAbstractItemModel>

class FKUsersStructure : public QAbstractItemModel{
    Q_OBJECT
public:
    explicit FKUsersStructure(QObject* parent = 0);
    //virtual QVariant toVariant()const;
    //virtual void applyEvent(const QVariant& data);
    //QVariant getByRows(const QList<qint32> rows, int role);
    //QVariant getByNames(const QStringList names, int role);
};

#endif // FKUSERSSTRUCTURE_H
