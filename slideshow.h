#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QObject>
#include <QTimer>

class MpvWidget;


class Slideshow : public QObject
{
    Q_OBJECT
public:
    Slideshow(MpvWidget *mpvWidget, QObject* parent = 0);
    
public slots:
    void openDir(const QString &path);
//    void resume();
//    void pause();
    bool togglePause();
    void next();
    void previous();
    void seek(int pos);
    void setImageDuration(double seconds);
    
private:
    MpvWidget *mpv;
    double imageDuration = 7.0;
    QString currentDirPath;
    QString currentFilePath;
    QTimer nextTimer;
    bool paused = false;
    
    QStringList getMediaFilesInDir(const QString &currentDirPath) const;
    void loadNeighbour(bool right);
    void loadFile(const QString &filepath);
};

#endif // SLIDESHOW_H
