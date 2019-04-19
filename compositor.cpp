#include "compositor.h"
#include "mpvinterface.h"

#include <QMetaObject>
#include <QDebug>


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
}

Compositor::~Compositor() {
    for (MpvInterface *mpv : mpvInstances) {
        makeCurrent();
        delete mpv;
    }
}

void Compositor::setPaths(const QStringList &paths) {
    this->paths = paths;
}

void Compositor::loadNext() {
    qDebug() << "loadNext";
    
    const int newIndex = index + 1;
    if (index >= paths.size()) {
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
    
    if (index == 0) {
        qDebug() << "loading first in list";
        current->load(paths.at(index));
    } else {
        current->setPaused(false);
    }
    next->load(paths.at(index + 1));
    next->setPaused(true);
    
    fadeTimer.start();
}

//------------------------------------------------------------------
// protected

void Compositor::initializeGL() {
    initializeOpenGLFunctions();
    
    for (MpvInterface *mpv : mpvInstances) {
        mpv->initializeGL();
        connect(mpv, SIGNAL(updateSignal()), this, SLOT(maybeUpdate()));
    }
    
    connect(this, SIGNAL(frameSwapped()), this, SLOT(swapped()));
}

void Compositor::paintGL() {
    const float elapsed = fadeTimer.elapsed() / 1000.f;
    const float elapsedNormalized = elapsed / fadeDuration;
    *prevAlpha = 1.f - elapsedNormalized;
    *currentAlpha = elapsedNormalized;
    
    if (elapsed >= fadeDuration) {
        // We are done fading
        // TODO maybe move to own method
        if (!prev->isPaused())
            prev->setPaused(true);
        
        qDebug() << "NOT FADING";
    } else {
        qDebug() << "elapsedNorm:" << elapsedNormalized;
    }
    
    for (MpvInterface *mpv : mpvInstances) {
//        qDebug() << "paintGL" << index++;
        mpv->paintGL(width(), height());
    }
    makeCurrent();
    
    for (int i = 0; i < 3; ++i) {
        MpvInterface *mpv = mpvInstances[i];
        const float alpha = alphas[i];
        
        // debug
//        float r, g, b;
//        r = g = b = 0.f;
//        if (mpv == prev)
//            r = 1.f;
//        if (mpv == current)
//            g = 1.f;
//        if (mpv == next)
//            b = 1.f;
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mpv->getFbo()->texture());
//        drawFullscreenQuad(r, g, b, alpha);
        drawFullscreenQuad(alpha);
        glDisable(GL_TEXTURE_2D);
    }
}

//------------------------------------------------------------------
// private slots

void Compositor::swapped() {
    for (MpvInterface *mpv : mpvInstances) {
        mpv->swapped();
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
