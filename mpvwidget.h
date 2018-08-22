#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QElapsedTimer>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>


class MpvWidget Q_DECL_FINAL: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MpvWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~MpvWidget();
    void command(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    //QSize sizeHint() const { return QSize(480, 270);}
    void setFadeDuration(double seconds);
public slots:
    void startFade();
signals:
    void durationChanged(int value);
    void positionChanged(int value);
protected:
    void drawFade();
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private slots:
    void swapped();
    void on_mpv_events();
    void maybeUpdate();
private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);

    mpv::qt::Handle mpv;
    mpv_opengl_cb_context *mpv_gl;
    QTimer fadeTriggerTimer;
    QElapsedTimer fadeElapsedTimer;
    double fadeDuration = 3.0;
    bool fadeRunning = false;
};



#endif // PLAYERWINDOW_H
