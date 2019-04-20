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
    virtual ~Compositor();
    void openDir(const QString &path);
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
    QElapsedTimer fadeTimer;
    QTimer nextTimer;
    double fadeDuration = 1.0;
    double imageDuration = 3.0;
    QString currentDirPath;
    QString currentFilePath;
    QStringList paths;
    bool paused = true;
    QStringList imageFormats;
    QStringList videoFormats;
    QStringList mediaNameFilter;
    
    // for debugging the stutter
    QElapsedTimer betweenPaints;
};

#endif // COMPOSITOR_H
