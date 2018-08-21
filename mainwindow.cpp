
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mpvwidget.h"
#include "slideshow.h"

#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mpv = ui->mpvWidget;
    slideshow = new Slideshow(ui->mpvWidget, this);
    
    connect(ui->open, SIGNAL(released()), this, SLOT(openDialog()));
    connect(ui->prev, SIGNAL(released()), slideshow, SLOT(previous()));
    connect(ui->togglePause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(ui->next, SIGNAL(released()), slideshow, SLOT(next()));
    connect(ui->videoSeekBar, SIGNAL(sliderMoved(int)), slideshow, SLOT(seek(int)));
    connect(ui->imageDuration, SIGNAL(valueChanged(double)), slideshow, SLOT(setImageDuration(double)));
    
    connect(mpv, SIGNAL(positionChanged(int)), ui->videoSeekBar, SLOT(setValue(int)));
    connect(mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));
}

MainWindow::~MainWindow() {
    delete ui;
}

//------------------------------------------------------------------
// public slots

void MainWindow::togglePause() {
    const bool paused = slideshow->togglePause();
    ui->togglePause->setText(paused ? "Resume" : "Pause");
}

//------------------------------------------------------------------
// private slots

void MainWindow::setSliderRange(int duration) {
    ui->videoSeekBar->setRange(0, duration);
}

// Show a directory selection dialog and open the images in the chosen directory
void MainWindow::openDialog() {
    // test
//    open("/home/simon/Videos/vsync tearing test-9hIRq5HTh5s.mp4");
//    return;
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QDir::homePath());
    
    const bool ok = dialog.exec();
    if(!ok) {
        return;
    }
    
    const QString path = dialog.selectedFiles().at(0);
    slideshow->open(path);
}
