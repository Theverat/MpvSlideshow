#include "slideshow.h"

// Q_ASSERT
#include <QtGlobal>


Slideshow::Slideshow(MpvWidget *mpvWidget, QObject* parent)
    : QObject(parent),
      mpv(mpvWidget)
{
    
}

void Slideshow::resume() {
    
}

void Slideshow::pause() {
    
}

void Slideshow::next() {
    
}

void Slideshow::previous() {
    
}

void Slideshow::setImageDuration(float seconds) {
    Q_ASSERT(seconds >= 0.f);
    this->imageDuration = seconds;
}
