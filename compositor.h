#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>


class MpvInterface;

class Compositor Q_DECL_FINAL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    Compositor(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~Compositor();
    
    std::vector<MpvInterface*> mpvInstances;
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    
private slots:
    void swapped();
    void maybeUpdate();
};

#endif // COMPOSITOR_H
