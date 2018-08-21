
#include "slideshow.h"
#include "mpvwidget.h"

// Q_ASSERT
#include <QtGlobal>


Slideshow::Slideshow(MpvWidget *mpvWidget, QObject* parent)
    : QObject(parent),
      mpv(mpvWidget)
{
    
}

void Slideshow::resume() {
    mpv->setProperty("pause", false);
}

void Slideshow::pause() {
    mpv->setProperty("pause", true);
}

bool Slideshow::togglePause() {
    const bool paused = mpv->getProperty("pause").toBool();
    mpv->setProperty("pause", !paused);
    return !paused;
}

void Slideshow::next() {
    mpv->command(QStringList() << "playlist-next");
}

void Slideshow::previous() {
    mpv->command(QStringList() << "playlist-prev");
}

void Slideshow::setImageDuration(float seconds) {
    Q_ASSERT(seconds >= 0.f);
    this->imageDuration = seconds;
    mpv->setProperty("image-display-duration", seconds);
}
