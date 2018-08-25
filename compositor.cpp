#include "compositor.h"
#include "mpvinterface.h"

#include <QMetaObject>


Compositor::Compositor(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    for (int i = 0; i < 3; ++i) {
        mpvInstances.emplace_back(new MpvInterface());
    }
}

Compositor::~Compositor() {
    for (MpvInterface *mpv : mpvInstances) {
        makeCurrent();
        delete mpv;
    }
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
    
    QString filepath = "/home/simon/Bilder/four_leaf/1497596541312.gif";
    mpvInstances[0]->command(QStringList() << "loadfile" << filepath);
    mpvInstances[0]->setProperty("image-display-duration", "inf");
    filepath = "/home/simon/Bilder/four_leaf/1508300969612.jpg";
    mpvInstances[1]->command(QStringList() << "loadfile" << filepath);
    mpvInstances[1]->setProperty("image-display-duration", "inf");
}

void Compositor::paintGL() {
    mpvInstances[0]->paintGL(defaultFramebufferObject(), width(), height());
    makeCurrent();
    
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glColor4f(1.f, 1.f, 1.f, 0.5f);
    
//    mpvInstances[1]->paintGL(defaultFramebufferObject(), width(), height());
//    makeCurrent();
    
//    glDisable(GL_BLEND);
//    glColor4f(1.f, 1.f, 1.f, 1.f);
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

