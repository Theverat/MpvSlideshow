#ifndef MPVINTERFACE_H
#define MPVINTERFACE_H

#include <QObject>
#include <QOpenGLFramebufferObject>
#include <QElapsedTimer>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>


class MpvInterface : public QObject
{
    Q_OBJECT
public:
    explicit MpvInterface(QObject *parent = 0);
    virtual ~MpvInterface();
    void initializeGL();
    void paintGL(int width, int height);
    void swapped();
    QOpenGLFramebufferObject *getFbo();
    
    void command(const QVariant& params);
    void command_async(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);
    void setPropertyAsync(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    
    void load(const QString &filepath);
    void setPaused(bool value);
    bool isPaused() const;
    void stop();
    void rotate(int angle);
    void rotateFromExif();
    
signals:
    void positionChanged(int value);
    void durationChanged(int value);
    void updateSignal(); // TODO remove?
    
private slots:
    void maybeUpdate();
    void on_mpv_events();
    
private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);
    
    mpv::qt::Handle mpv;
    mpv_opengl_cb_context *mpv_gl;
    QOpenGLFramebufferObject *fbo;
};

#endif // MPVINTERFACE_H
