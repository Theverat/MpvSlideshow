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
    
    // test TODO remove
    QString filepath = "/home/simon/Videos/Kazam_screencast_00001.mp4";
    mpvInstances[0]->command(QStringList() << "loadfile" << filepath);
    mpvInstances[0]->setProperty("image-display-duration", "inf");
    
    filepath = "/home/simon/Videos/mix_example.mp4";
    mpvInstances[1]->command(QStringList() << "loadfile" << filepath);
    mpvInstances[1]->setProperty("image-display-duration", "inf");
    
    filepath = "/home/simon/Videos/chains_problem.mp4";
    mpvInstances[2]->command(QStringList() << "loadfile" << filepath);
    mpvInstances[2]->setProperty("image-display-duration", "inf");
}

void Compositor::paintGL() {
    for (MpvInterface *mpv : mpvInstances) {
        mpv->paintGL(width(), height());
    }
//    mpvInstances[0]->paintGL(width(), height());
    makeCurrent();
    
//    glEnable(GL_COLOR_MATERIAL);
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, mpvInstances[0]->getFbo()->texture());
//    drawFullscreenQuad(0.5f);
//    glDisable(GL_TEXTURE_2D);
//    glDisable(GL_COLOR_MATERIAL);
    
    // TODO use a fragment shader for mixing
    float alpha = 1.f;
    for (MpvInterface *mpv : mpvInstances) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mpv->getFbo()->texture());
        drawFullscreenQuad(alpha);
        glDisable(GL_TEXTURE_2D);
        
        alpha -= 0.33f;
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.f, 1.f, 1.f, alpha);
    
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
