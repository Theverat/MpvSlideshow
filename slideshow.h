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
    void resume();
    void pause();
    bool togglePause();
    void next();
    void previous();
    void setImageDuration(float seconds);
    
private:
    MpvWidget *mpv;
    float imageDuration = 7.f;
};

#endif // SLIDESHOW_H
