#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include "compositor.h"

#include <QObject>

class FileBuffer : public QObject
{
    Q_OBJECT
public:
    explicit FileBuffer(QObject *parent = 0);
    
signals:
    
public slots:
    void load(const QStringList &filepaths);
    void next();
    
private:
    Compositor *compositor;
    QStringList filepaths;
    int pos = -1;
};

#endif // FILEBUFFER_H
