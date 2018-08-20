#ifndef MainWindow_H
#define MainWindow_H

#include <QtWidgets/QWidget>

class MpvWidget;
class QSlider;
class QPushButton;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
public slots:
    void open(QString path);
    void seek(int pos);
    void pauseResume();
private slots:
    void setSliderRange(int duration);
    void openDialog();
private:
    MpvWidget *m_mpv;
    QSlider *m_slider;
    QPushButton *m_openBtn;
    QPushButton *m_playBtn;
};

#endif // MainWindow_H
