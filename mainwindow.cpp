
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mpvwidget.h"
#include "slideshow.h"
#include "sliderstyle.h"
#include "exifparser.h"

#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->videoSeekBar->setStyle(new MyStyle(ui->videoSeekBar->style()));
    mpv = ui->mpvWidget;
    slideshow = new Slideshow(ui->mpvWidget, this);
    
    connect(ui->open, SIGNAL(released()), this, SLOT(openDialog()));
    connect(ui->prev, SIGNAL(released()), slideshow, SLOT(previous()));
    connect(ui->togglePause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(ui->next, SIGNAL(released()), slideshow, SLOT(next()));
    connect(ui->imageDuration, SIGNAL(valueChanged(double)), slideshow, SLOT(setImageDuration(double)));
    connect(ui->videoSeekBar, SIGNAL(valueChanged(int)), slideshow, SLOT(seek(int)));
    
    connect(mpv, SIGNAL(positionChanged(int)), this, SLOT(handleVideoPositionChange(int)));
    connect(mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));
    connect(mpv, SIGNAL(endFile()), slideshow, SLOT(next()));
    
//    ExifParser exif("/home/simon/Bilder/Fotos/Fotos_Berlin/000008_000007.JPG");
//    qDebug() << "orientation:" << exif.isValid() << exif.getOrientation();
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
    ui->videoSeekBar->blockSignals(true);
    ui->videoSeekBar->setRange(0, duration);
    ui->videoSeekBar->blockSignals(false);
}

void MainWindow::handleVideoPositionChange(int pos) {
    // We have to block all signals of the videoSeekBar, otherwise there
    // are feedback loops and the playback stutters every few seconds
    ui->videoSeekBar->blockSignals(true);
    ui->videoSeekBar->setValue(pos);
    ui->videoSeekBar->blockSignals(false);
}

// Show a directory selection dialog and open the images in the chosen directory
void MainWindow::openDialog() {
    // test
    slideshow->openDir("/home/simon/Bilder/Fotos/Fotos_Berlin");
//    slideshow->open("/home/simon/Bilder/Fotos/Fotos_Heimgarten/Auswahl/von Opa");
    return;
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QDir::homePath());
    
    const bool ok = dialog.exec();
    if(!ok) {
        return;
    }
    
    const QString path = dialog.selectedFiles().at(0);
    slideshow->openDir(path);
}
