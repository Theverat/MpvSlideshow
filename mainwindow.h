#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>

class MpvWidget;
class Slideshow;
class QSlider;
class QPushButton;

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
    void open(QString path);
    void seek(int pos);
    void togglePause();
    
private slots:
    void setSliderRange(int duration);
    void openDialog();
    
private:
    Ui::MainWindow *ui;
    MpvWidget *mpv;
    QSlider *slider;
    QPushButton *openBtn;
    QPushButton *playBtn;
    Slideshow *slideshow;
};

#endif // MainWindow_H
