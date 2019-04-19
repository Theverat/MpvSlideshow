
#include "slideshow.h"
#include "compositor.h"

#include <QDir>
#include <QFileInfo>
#include <QtGlobal> // Q_ASSERT
#include <QDebug>


Slideshow::Slideshow(Compositor *compositor, QObject* parent)
    : QObject(parent),
      compositor(compositor)
{
    nextTimer.setSingleShot(true);
    connect(&nextTimer, SIGNAL(timeout()), this, SLOT(next()));
    
    fadeTimer.setSingleShot(true);
//    connect(&fadeTimer, SIGNAL(timeout()), mpv, SLOT(startFadeToBlack()));
    
    imageFormats << "png" << "jpg" << "jpeg" << "tiff" << "tif"
                 << "ppm" << "bmp" << "xpm" << "gif";
    videoFormats << "mp4" << "mov";
}

//------------------------------------------------------------------
// public slots

void Slideshow::openDir(const QString &path) {
    Q_ASSERT(path.size());
    
    this->currentDirPath = path;
    
    QStringList files = getMediaFilesInDir(path);
    if (files.empty())
        return;
    
    compositor->setPaths(files);
    compositor->loadNext();
}

bool Slideshow::togglePause() {
    paused = !paused;
    if (paused) {
        nextTimer.stop();
    } else {
        maybeStartTimer();
    }
    return paused;
}

void Slideshow::next() {
    loadNeighbour(true);
}

void Slideshow::previous() {
    loadNeighbour(false);
}

void Slideshow::seek(int pos) {
//    mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void Slideshow::setImageDuration(double seconds) {
    Q_ASSERT(seconds >= 0.0);
    
    this->imageDuration = seconds;
}

void Slideshow::handleFileLoaded(const QString &filepath) {
//    qDebug() << "handleFileLoaded" << filepath;
    
    // activate time if not video etc.
}

//------------------------------------------------------------------
// private

QStringList Slideshow::getMediaFilesInDir(const QString &dirPath) const {
    QStringList nameFilter = imageFormats + videoFormats;
    for (QString &entry : nameFilter) {
        entry.prepend("*.");
    }
    
    QDir directory(dirPath);
    QStringList entryList = directory.entryList(nameFilter, QDir::Files);
    
    // The entryList only contains filenames, not full paths - prepend the path
    for (QString &entry : entryList) {
        entry = dirPath + QDir::separator() + entry;
    }
    return entryList;
}

void Slideshow::loadNeighbour(bool right) {
    QStringList files = getMediaFilesInDir(this->currentDirPath);
    if (files.empty())
        return;
    
    const int currentIndex = files.indexOf(this->currentFilePath);
    const int offset = right ? 1 : -1;
    const int neighbourIndex = currentIndex + offset;
    
    if (neighbourIndex < 0 || neighbourIndex >= files.size())
        // TODO show black image or something?
        // Or some text ("the end")
        return;
    
//    loadFile(files.at(neighbourIndex));
    compositor->setPaths(files);
    if (right) {
        compositor->loadNext();
    } else {
        qDebug() << "loadPrev() not implemented yet";
    }
}

void Slideshow::loadFile(const QString &filepath) {
    this->currentFilePath = filepath;
    
//    mpv->setProperty("image-display-duration", "inf");
//    mpv->command(QStringList() << "loadfile" << filepath);
    
    nextTimer.stop();
    maybeStartTimer();
}

bool Slideshow::isImage(const QString &filepath) {
    QFileInfo fileInfo(filepath);
    const QString &extension = fileInfo.completeSuffix();
    return imageFormats.contains(extension.toLower());
}

void Slideshow::maybeStartTimer() {
    if (isImage(this->currentFilePath) && !paused) {
        nextTimer.start(this->imageDuration * 1000);
//        const double timeUntilFade = this->imageDuration - mpv->getFadeDuration() / 2.0;
//        fadeTimer.start(timeUntilFade * 1000);
        qDebug() << "started timer";
    }
}
