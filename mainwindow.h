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
    
protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void setSliderRange(int duration);
    void handleVideoPositionChange(int pos);
    void openDialog();
    
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void showEvent(QShowEvent *);
    
private:
    void writeSettings();
    void readSettings();
    
    Ui::MainWindow *ui;
    Compositor *compositor;
    
    QShortcut *shortcutOpen;
    QShortcut *shortcutPrev;
    QShortcut *shortcutNext;
    QShortcut *shortcutTogglePause;
};

#endif // MainWindow_H
