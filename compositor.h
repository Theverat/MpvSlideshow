#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QStringList>
#include <QElapsedTimer>
#include <QTimer>

#include <vector>


class MpvInterface;

class Compositor Q_DECL_FINAL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    Compositor(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void reset();
    virtual ~Compositor();
    void openDir(const QString &path, int index=0);
    QString getCurrentDirPath() const {
        return currentDirPath;
    }
    int getCurrentIndex() const {
        return index;
    }
    bool togglePause();
    
public slots:
    void previousFile();
    void nextFile();
    void setImageDuration(double seconds) {
        imageDuration = seconds;
    }
    void setFadeDuration(double seconds) {
        fadeDuration = seconds;
    }
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    
private slots:
    void swapped();
    void maybeUpdate();
    
private:
    void drawFullscreenQuad(float alpha);
    void drawFullscreenQuad(float r, float g, float b, float alpha);
    QStringList getMediaFilesInDir(const QString &dirPath) const;
    bool isImage(const QString &filepath) const;
    void startNextTimer();
    static float clamp(float value, float min=0.f, float max=1.f) {
        return std::max(min, std::min(max, value));
    }
    double getFadeDuration() const {
        return clamp(fadeDuration, 0.f, imageDuration);
    }
    
    std::vector<MpvInterface*> mpvInstances;
    std::vector<float> alphas;
    MpvInterface *prev;
    MpvInterface *current;
    MpvInterface *next;
    float *prevAlpha;
    float *currentAlpha;
    float *nextAlpha;
    
    // Slideshow management
    int index = -1; // Current index
    bool firstLoad = true;
    QElapsedTimer fadeTimer;
    QTimer nextTimer;
    double fadeDuration = 1.0;
    bool fadeBackwards = false;
    double imageDuration = 3.0;
    QString currentDirPath;
    QStringList paths;
    bool paused = true;
    QStringList imageFormats;
    QStringList videoFormats;
    QStringList mediaNameFilter;
    
    // for debugging the stutter
    QElapsedTimer betweenPaints;
};

#endif // COMPOSITOR_H
