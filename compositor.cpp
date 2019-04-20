#include "compositor.h"
#include "mpvinterface.h"

#include <QDebug>
#include <QtGlobal> // Q_ASSERT
#include <QDir>


Compositor::Compositor(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    for (int i = 0; i < 3; ++i) {
        mpvInstances.emplace_back(new MpvInterface());
        alphas.push_back(1.f);
    }
    
    prev = mpvInstances[0];
    current = mpvInstances[1];
    next = mpvInstances[2];
    
    prevAlpha = &alphas[0];
    currentAlpha = &alphas[1];
    nextAlpha = &alphas[2];
    
    reset();
    
    // Slideshow management
    nextTimer.setSingleShot(true);
    connect(&nextTimer, SIGNAL(timeout()), this, SLOT(nextFile()));
    
    imageFormats << "png" << "jpg" << "jpeg" << "tiff" << "tif"
                 << "ppm" << "bmp" << "xpm" << "gif";
    videoFormats << "mp4" << "mov";
    mediaNameFilter = imageFormats + videoFormats;
    for (QString &entry : mediaNameFilter) {
        entry.prepend("*.");
    }
}

void Compositor::reset() {
    paused = true;
    fadeBackwards = false;
    nextTimer.stop();
    currentDirPath.clear();
    index = -1;
    firstLoad = true;
}

Compositor::~Compositor() {
    for (MpvInterface *mpv : mpvInstances) {
        makeCurrent();
        delete mpv;
        mpv = nullptr;
    }
}

void Compositor::openDir(const QString &path, int index) {
    Q_ASSERT(path.size());
    
    reset();
    
    QStringList files = getMediaFilesInDir(path);
    if (files.empty())
        return;
    
    currentDirPath = path;
    paths = files;
    
    if (index >= 0 && index < files.size())
        this->index = index - 1;
    else
        this->index = -1;
    
    nextFile();
    firstLoad = false;
}

bool Compositor::togglePause() {
    if (paths.size() == 0)
        return paused;
    
    paused = !paused;
    if (paused) {
        nextTimer.stop();
    } else {
        startNextTimer();
    }
    return paused;
}

//------------------------------------------------------------------
// public slots

void Compositor::previousFile() {
    const int newIndex = index - 1;
    if (newIndex < 0) {
        qDebug() << "Can not load previous: index out of range:" << newIndex;
        return;
    }
    index = newIndex;
    
    next->stop();
    MpvInterface *temp = next;
    next = current;
    current = prev;
    prev = temp;
    
    float *tempAlpha = nextAlpha;
    nextAlpha = currentAlpha;
    currentAlpha = prevAlpha;
    prevAlpha = tempAlpha;
    
    *prevAlpha = 0.f;
    *currentAlpha = 1.f;
    *nextAlpha = 0.f;
    
    current->setPaused(false);
    if (index - 1 >= 0) {
        prev->load(paths.at(index - 1));
        prev->setPaused(true);
    }
    
    fadeBackwards = true;
    fadeTimer.start();
    if (!paused) {
        startNextTimer();
    }
}

void Compositor::nextFile() {
    qDebug() << "nextFile";
    const int newIndex = index + 1;
    if (newIndex >= paths.size()) {
        qDebug() << "Can not load next: index out of range:" << newIndex;
        return;
    }
    index = newIndex;
    
    prev->stop();
    MpvInterface *temp = prev;
    prev = current;
    current = next;
    next = temp;
    
    float *tempAlpha = prevAlpha;
    prevAlpha = currentAlpha;
    currentAlpha = nextAlpha;
    nextAlpha = tempAlpha;
    
    *prevAlpha = 0.f;
    *currentAlpha = 1.f;
    *nextAlpha = 0.f;
    
    if (firstLoad) {
        qDebug() << "loading first in list";
        current->load(paths.at(index));
    } 
    current->setPaused(false);
    
    // Already buffer the next image, if possible
    if (index + 1 < paths.size()) {
        next->load(paths.at(index + 1));
        next->setPaused(true);
    }
    
    fadeBackwards = false;
    fadeTimer.start();
    if (!paused) {
        startNextTimer();
    }
}

//------------------------------------------------------------------
// protected

void Compositor::initializeGL() {
    initializeOpenGLFunctions();
    
    for (MpvInterface *mpv : mpvInstances) {
        mpv->initializeGL();
        // TODO is this needed?
        //connect(mpv, SIGNAL(updateSignal()), this, SLOT(maybeUpdate()));
    }
    
    connect(this, SIGNAL(frameSwapped()), this, SLOT(swapped()));
}

void Compositor::paintGL() {    
    const int msSinceLast = betweenPaints.isValid() ? betweenPaints.elapsed() : 0;
    
    QElapsedTimer t;
    t.start();
    
    const float elapsed = fadeTimer.isValid() ? fadeTimer.elapsed() / 1000.f : 0.f;
    float elapsedNormalized = clamp(elapsed / getFadeDuration());
    
    if (fadeBackwards) {
        *prevAlpha = 0.f;
        *nextAlpha = 1.f - elapsedNormalized;
    } else {
        *prevAlpha = 1.f - elapsedNormalized;
        *nextAlpha = 0.f;
    }
    *currentAlpha = elapsedNormalized;
    
    if (elapsed >= getFadeDuration()) {
        // We are done fading
        // TODO maybe move to own method
        prev->setPaused(true);
        next->setPaused(true);
    }
    
    for (int i = 0; i < 3; ++i) {
        const float alpha = alphas[i];
        
        if (alpha > 0.f) {
            MpvInterface *mpv = mpvInstances[i];
            mpv->paintGL(width(), height());
        }
    }
    makeCurrent();
    
    for (int i = 0; i < 3; ++i) {
        const float alpha = alphas[i];
        
        if (alpha > 0.f) {
            MpvInterface *mpv = mpvInstances[i];
            
            // debug
//            float r, g, b;
//            r = g = b = 0.f;
//            if (mpv == prev)
//                r = 1.f;
//            if (mpv == current)
//                g = 1.f;
//            if (mpv == next)
//                b = 1.f;
            
            mpv->setProperty("volume", static_cast<int>(alpha * 100.f));
            
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, mpv->getFbo()->texture());
//            drawFullscreenQuad(r, g, b, alpha);
            drawFullscreenQuad(alpha);
            glDisable(GL_TEXTURE_2D);
        }
    }
    
//    qDebug() << "paint" << t.elapsed() << "ms, since last:" << msSinceLast << "ms, elapsedNorm:" << elapsedNormalized;
    betweenPaints.start();
    
//    Q_ASSERT(msSinceLast < 100);
}

//------------------------------------------------------------------
// private slots

void Compositor::swapped() {
//    for (MpvInterface *mpv : mpvInstances) {
//        mpv->swapped();
//    }
    for (int i = 0; i < 3; ++i) {
        const float alpha = alphas[i];
        
        if (alpha > 0.f) {
            MpvInterface *mpv = mpvInstances[i];
            mpv->swapped();
        }
    }
    // Immediately schedule the next paintGL() call
    update();
}

// Make Qt invoke mpv_opengl_cb_draw() to draw a new/updated video frame.
void Compositor::maybeUpdate() {
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's opengl-cb API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        swapped();
        doneCurrent();
    } else {
        update();
    }
}

//------------------------------------------------------------------
// private

void Compositor::drawFullscreenQuad(float alpha) {
    drawFullscreenQuad(1.f, 1.f, 1.f, alpha);
}

void Compositor::drawFullscreenQuad(float r, float g, float b, float alpha) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, alpha);
    
    glBegin(GL_QUADS);
    {
        const float offset_x = -1.f;
        const float offset_y = -1.f;
        const float width = 2.f;
        const float height = 2.f;
        
        glTexCoord2f(0, 0);
        glVertex2f(offset_x, offset_y);
        glTexCoord2f(1, 0);
        glVertex2f(offset_x + width, offset_y);
        glTexCoord2f(1, 1);
        glVertex2f(offset_x + width, offset_y + height);
        glTexCoord2f(0, 1);
        glVertex2f(offset_x, offset_y + height);
    }
    glEnd();
    
    glDisable(GL_BLEND);
    glColor4f(1.f, 1.f, 1.f, 1.f);
}

QStringList Compositor::getMediaFilesInDir(const QString &dirPath) const {
    QDir directory(dirPath);
    QStringList entryList = directory.entryList(mediaNameFilter, QDir::Files);
    
    // The entryList only contains filenames, not full paths - prepend the path
    for (QString &entry : entryList) {
        entry = dirPath + QDir::separator() + entry;
    }
    return entryList;
}

bool Compositor::isImage(const QString &filepath) const {
    QFileInfo fileInfo(filepath);
    const QString &extension = fileInfo.completeSuffix();
    return imageFormats.contains(extension.toLower());
}

void Compositor::startNextTimer() {
    if (isImage(paths.at(index))) {
        nextTimer.start(imageDuration * 1000);
    } else {
        const double videoLength = current->getProperty("playtime-remaining").toDouble();
        const double epsilon = 0.1;
        nextTimer.start((videoLength - getFadeDuration() - epsilon) * 1000);
    }
}
