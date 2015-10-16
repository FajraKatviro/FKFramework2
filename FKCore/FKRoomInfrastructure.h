#ifndef FKROOMINFRASTRUCTURE_H
#define FKROOMINFRASTRUCTURE_H

#include <QObject>
#include <QMap>

class FKObjectManager;
class FKUserInfrastructure;

class FKRoomInfrastructure : public QObject{
    Q_OBJECT
public:
    explicit FKRoomInfrastructure(QObject* parent=0);
    ~FKRoomInfrastructure();
    void addUser(const qint32 id);
    void onSyncCompleted();
signals:

public slots:
private:
    QMap<qint32,FKUserInfrastructure*> _users;
};

#endif // FKROOMINFRASTRUCTURE_H
