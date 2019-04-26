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
    void setSeekBarVisible(bool value);
    void setSliderRange(int duration);
    void handleVideoPositionChange(int pos);
    
protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
    
private slots:
    void openDialog();
    void convertZoom(int value);
    void toggleFullscreen();
    void setInfoText(const QString &text);
    void handleEscape();
    
private:
    void writeSettings();
    void readSettings();
    
    Ui::MainWindow *ui;
    Compositor *compositor;
    
    QShortcut *shortcutOpen;
    QShortcut *shortcutPrev;
    QShortcut *shortcutNext;
    QShortcut *shortcutTogglePause;
    QShortcut *shortcutEscape;
};

#endif // MainWindow_H
