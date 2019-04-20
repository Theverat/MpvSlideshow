#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QTimer>

class Compositor;
class QShortcut;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    
public slots:
    void togglePause();
    
private slots:
    void setSliderRange(int duration);
    void handleVideoPositionChange(int pos);
    void openDialog();
    
protected:
    void mouseMoveEvent(QMouseEvent *event);
    
private:
    Ui::MainWindow *ui;
    Compositor *compositor;
    
    QShortcut *shortcutOpen;
    QShortcut *shortcutPrev;
    QShortcut *shortcutNext;
    QShortcut *shortcutTogglePause;
};

#endif // MainWindow_H
