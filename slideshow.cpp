
#include "slideshow.h"
#include "mpvwidget.h"

// Q_ASSERT
#include <QtGlobal>
#include <QDebug>


Slideshow::Slideshow(MpvWidget *mpvWidget, QObject* parent)
    : QObject(parent),
      mpv(mpvWidget)
{}

//------------------------------------------------------------------
// public slots

void Slideshow::open(QString path) {
    Q_ASSERT(path.size());
    
    mpv->command(QStringList() << "loadfile" << path);
    this->applyImageDuration();
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

void Slideshow::seek(int pos) {
    mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void Slideshow::setImageDuration(double seconds) {
    Q_ASSERT(seconds >= 0.0);
    
    this->imageDuration = seconds;
    this->applyImageDuration();
}

//------------------------------------------------------------------
// private

void Slideshow::applyImageDuration() {
    mpv->setProperty("image-display-duration", imageDuration);
}
