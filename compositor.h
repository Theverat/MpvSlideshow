#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QStringList>
#include <QElapsedTimer>
#include <QTimer>

#include <vector>


class MpvInterface;
class MainWindow;

class Compositor Q_DECL_FINAL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    Compositor(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void reset();
    virtual ~Compositor();
    void setMainWindow(MainWindow *mw) {
        mainwindow = mw;
    }

    void openDir(const QString &path, int index=0);
    QString getCurrentDirPath() const {
        return currentDirPath;
    }
    int getCurrentIndex() const {
        return index;
    }
    bool togglePause();
    bool isImage(const QString &filepath) const;
    
signals:
    void infoChanged(const QString &text);
    
public slots:
    void previousFile();
    void nextFile();
    void setImageDuration(double seconds) {
        imageDuration = seconds;
        updateInfo();
    }
    void setFadeDuration(double seconds) {
        fadeDuration = seconds;
    }
    void setZoom(double value);
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void drawMpvInstance(MpvInterface *mpv, float alpha, int volume);
    
private slots:
    void swapped();
    void maybeUpdate();
    
private:
    void drawFullscreenQuad(float alpha);
    void drawFullscreenQuad(float r, float g, float b, float alpha);
    QStringList getMediaFilesInDir(const QString &dirPath) const;
    void startNextTimer();
    static float clamp(float value, float min=0.f, float max=1.f) {
        return std::max(min, std::min(max, value));
    }
    double getFadeDuration() const {
        return clamp(fadeDuration, 0.f, imageDuration);
    }
    void startFade(bool backwards);
    void updateInfo();
    
    MainWindow *mainwindow;
    std::vector<MpvInterface*> mpvInstances;
    MpvInterface *prev;
    MpvInterface *current;
    MpvInterface *next;
    
    // Slideshow management
    int index = -1; // Current index
    bool firstLoad = true;
    bool firstLoadFade = true;
    QElapsedTimer fadeTimer;
    QTimer nextTimer;
    double fadeDuration = 0.8;
    bool fadeBackwards = false;
    bool fadeEndHandled = false;
    double imageDuration = 8.0;
    QString currentDirPath;
    QStringList paths;
    bool paused = true;
    QStringList imageFormats;
    QStringList videoFormats;
    QStringList mediaNameFilter;
};

#endif // COMPOSITOR_H
