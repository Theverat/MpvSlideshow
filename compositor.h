#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QStringList>
#include <QElapsedTimer>

#include <vector>


class MpvInterface;

class Compositor Q_DECL_FINAL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    Compositor(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~Compositor();
    void setPaths(const QStringList& paths);
    void loadNext();
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    
private slots:
    void swapped();
    void maybeUpdate();
    
private:
    void drawFullscreenQuad(float alpha);
    void drawFullscreenQuad(float r, float g, float b, float alpha);
    
    std::vector<MpvInterface*> mpvInstances;
    std::vector<float> alphas;
    MpvInterface *prev;
    MpvInterface *current;
    MpvInterface *next;
    float *prevAlpha;
    float *currentAlpha;
    float *nextAlpha;
    QStringList paths;
    int index = -1;
    QElapsedTimer fadeTimer;
    float fadeDuration = 2.f;
    // for debugging the stutter
    QElapsedTimer betweenPaints;
};

#endif // COMPOSITOR_H
