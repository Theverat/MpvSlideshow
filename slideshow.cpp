
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

void Slideshow::next() {
    
}

void Slideshow::previous() {
    
}

void Slideshow::setImageDuration(float seconds) {
    Q_ASSERT(seconds >= 0.f);
    this->imageDuration = seconds;
    mpv->setProperty("image-display-duration", seconds);
}
