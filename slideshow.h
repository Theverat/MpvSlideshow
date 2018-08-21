#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QObject>

class MpvWidget;


class Slideshow : public QObject
{
    Q_OBJECT
public:
    Slideshow(MpvWidget *mpvWidget, QObject* parent = 0);
    
public slots:
    void open(QString path);
    void resume();
    void pause();
    bool togglePause();
    void next();
    void previous();
    void seek(int pos);
    void setImageDuration(double seconds);
    
private:
    MpvWidget *mpv;
    double imageDuration = 7.0;
    
    void applyImageDuration();
};

#endif // SLIDESHOW_H
