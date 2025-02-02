#include "mpvinterface.h"
#include "exifparser.h"

#include <stdexcept>

#include <QtGui/QOpenGLContext>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QDebug>


static void wakeup(void *ctx) {
    QMetaObject::invokeMethod((MpvInterface*)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name) {
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return NULL;
    return (void *)glctx->getProcAddress(QByteArray(name));
}


MpvInterface::MpvInterface(QObject *parent) 
    : QObject(parent), 
      fbo(NULL)
{
    mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!mpv)
        throw std::runtime_error("could not create mpv context");
    
    // Decouple our fps from the video being played
    mpv_set_option_string(mpv, "video-timing-offset", "0");
    
    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=warn");
    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Make use of the MPV_SUB_API_OPENGL_CB API.
    mpv::qt::set_option_variant(mpv, "vo", "opengl-cb");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);
    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");
    mpv_opengl_cb_set_update_callback(mpv_gl, MpvInterface::on_update, (void *)this);

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_set_wakeup_callback(mpv, wakeup, this);
}

MpvInterface::~MpvInterface() {
    if (mpv_gl)
        mpv_opengl_cb_set_update_callback(mpv_gl, NULL, NULL);
    // Until this call is done, we need to make sure the player remains
    // alive. This is done implicitly with the mpv::qt::Handle instance
    // in this class.
    mpv_opengl_cb_uninit_gl(mpv_gl);
}

void MpvInterface::initializeGL() {
    int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");
    
    fbo = new QOpenGLFramebufferObject(4, 4);
}

void MpvInterface::paintGL(int width, int height) {
    if (fbo->width() != width || fbo->height() != height) {
        delete fbo;
        fbo = new QOpenGLFramebufferObject(width, height);
    }
    
    mpv_opengl_cb_draw(mpv_gl, fbo->handle(), width, -height);
}

void MpvInterface::swapped() {
    mpv_opengl_cb_report_flip(mpv_gl, 0);
}

QOpenGLFramebufferObject *MpvInterface::getFbo() {
    return fbo;
}

void MpvInterface::command(const QVariant& params) {
//    mpv::qt::command_variant(mpv, params);  // deprecated
    mpv::qt::command(mpv, params);
}

void MpvInterface::command_async(const QVariant& params) {
    // Not really tested
    mpv::qt::node_builder node(params);
    uint64_t reply_userdata = 1;
    int err = mpv_command_node_async(mpv, reply_userdata, node.node());
    Q_ASSERT(err >= 0);
}

void MpvInterface::setProperty(const QString& name, const QVariant& value) {
//    mpv::qt::set_property_variant(mpv, name, value); // deprecated
    mpv::qt::set_property(mpv, name, value);
}

void MpvInterface::setPropertyAsync(const QString& name, const QVariant& value) {
    mpv::qt::node_builder node(value);
    int reply_userdata = 2;
    mpv_set_property_async(mpv, reply_userdata, name.toUtf8().data(), MPV_FORMAT_NODE, node.node());
}

QVariant MpvInterface::getProperty(const QString &name) const {
    return mpv::qt::get_property_variant(mpv, name);
}

//This is an asnc call that returns immediately
void MpvInterface::load(const QString &filepath) {
    currentFilePath = filepath;
    command(QStringList() << "loadfile" << filepath);
    setProperty("image-display-duration", "inf");
//    setProperty("mute", true); // TODO just for debugging
}

void MpvInterface::setPaused(bool value) {
//    if (getProperty("pause").toBool() != value)
    setPropertyAsync("pause", value);
}

bool MpvInterface::isPaused() const {
    return getProperty("pause").toBool();
}

void MpvInterface::stop() {
    command("stop");
}

void MpvInterface::rotate(int angle) {
    setPropertyAsync("video-rotate", angle);
}

void MpvInterface::rotateFromExif() {
    // Every communication with mpv in this method has to be async!
    // Otherwise stutter happens.
    
    QFileInfo info(currentFilePath);
    const QString ext = info.suffix().toLower();
    bool validExif = false;
    
    if(ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "tif" || ext == "tiff") {
        ExifParser exif(currentFilePath);
        
        // TODO implement the mirror/transpose cases
        if(exif.isValid()) {
            validExif = true;
            
            switch(exif.getOrientation()) {
            case 1:
                rotate(0);
                break;
            case 2:
//                image = image.mirrored(true, false);
                break;
            case 3:
                rotate(180);
                break;
            case 4:
//                image = image.mirrored(false, true);
                break;
            case 5:
//                transform = transform.transposed();
                break;
            case 6:
                rotate(90);
                break;
            case 7:
//                transform.rotate(-90);
//                image = image.mirrored(false, true);
                break;
            case 8:
                rotate(270);
                break;
            }
        }
    }
    if (!validExif) {
        // No EXIF information, reset rotation
        rotate(0);
    }
}

//------------------------------------------------------------------
// private slots

void MpvInterface::maybeUpdate() {
    emit updateSignal();
}

void MpvInterface::on_mpv_events() {
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handle_mpv_event(event);
    }
}

//------------------------------------------------------------------
// private

void MpvInterface::handle_mpv_event(mpv_event *event) {
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        const QString propName(prop->name);
        
        if (propName == "time-pos") {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                emit positionChanged(time);
            }
        } else if (propName == "duration") {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                emit durationChanged(time);
            }
        }
        break;
    }
    case MPV_EVENT_START_FILE: {
        rotateFromExif();
        break;
    }
    case MPV_EVENT_FILE_LOADED: {
        break;
    }
    default: ;
        // Ignore uninteresting or unknown events.
    }
}

void MpvInterface::on_update(void *ctx) {
    QMetaObject::invokeMethod((MpvInterface*)ctx, "maybeUpdate");
}
