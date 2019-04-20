
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sliderstyle.h"

#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>
#include <QShortcut>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->videoSeekBar->setStyle(new MyStyle(ui->videoSeekBar->style()));
    compositor = ui->compositor;
    
    shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O", "Open")), this);
    shortcutPrev = new QShortcut(QKeySequence(tr("Left", "Previous")), this);
    shortcutNext = new QShortcut(QKeySequence(tr("Right", "Next")), this);
    shortcutTogglePause = new QShortcut(QKeySequence(Qt::Key_Space), this);
    
    connect(shortcutOpen, SIGNAL(activated()), this, SLOT(openDialog()));
    connect(shortcutPrev, SIGNAL(activated()), compositor, SLOT(previous()));
    connect(shortcutNext, SIGNAL(activated()), compositor, SLOT(next()));
    connect(shortcutTogglePause, SIGNAL(activated()), this, SLOT(togglePause()));
    
    connect(ui->open, SIGNAL(released()), this, SLOT(openDialog()));
    connect(ui->prev, SIGNAL(released()), compositor, SLOT(previous()));
    connect(ui->togglePause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(ui->next, SIGNAL(released()), compositor, SLOT(next()));
    connect(ui->imageDuration, SIGNAL(valueChanged(double)), compositor, SLOT(setImageDuration(double)));
    connect(ui->fadeDuration, SIGNAL(valueChanged(double)), compositor, SLOT(setFadeDuration(double)));
//    connect(ui->videoSeekBar, SIGNAL(valueChanged(int)), slideshow, SLOT(seek(int)));
    
//    connect(mpv, SIGNAL(positionChanged(int)), this, SLOT(handleVideoPositionChange(int)));
//    connect(mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));
//    connect(mpv, SIGNAL(fileLoaded(QString)), slideshow, SLOT(handleFileLoaded(QString)));
    
    compositor->setImageDuration(ui->imageDuration->value());
    compositor->setFadeDuration(ui->fadeDuration->value());
}

MainWindow::~MainWindow() {
    delete ui;
}

//------------------------------------------------------------------
// public slots

void MainWindow::togglePause() {
    const bool paused = compositor->togglePause();
    ui->togglePause->setText(paused ? tr("Resume") : tr("Pause"));
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
    
//    slideshow->openDir("/home/simon/Bilder/mpvslideshowteset");
    compositor->openDir("/home/simon/Bilder/mpvslideshowteset/marokko");
    
    return;
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QDir::homePath());
    
    const bool ok = dialog.exec();
    if(!ok) {
        return;
    }
    
    const QString path = dialog.selectedFiles().at(0);
    qDebug() << "opening Dir:" << path;
    compositor->openDir(path);
}
