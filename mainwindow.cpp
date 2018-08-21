
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
    
//    mpv = new MpvWidget(this);
    mpv = ui->mpvWidget; // todo remove
    slideshow = new Slideshow(ui->mpvWidget, this);
    
//    slider = new QSlider();
//    slider->setOrientation(Qt::Horizontal);
//    openBtn = new QPushButton("Open");
//    playBtn = new QPushButton("Pause");
//    QHBoxLayout *hb = new QHBoxLayout();
//    hb->addWidget(openBtn);
//    hb->addWidget(playBtn);
//    QVBoxLayout *vl = new QVBoxLayout();
//    vl->addWidget(mpv);
//    vl->addWidget(slider);
//    vl->addLayout(hb);
//    setLayout(vl);
    
//    connect(slider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
//    connect(openBtn, SIGNAL(clicked()), SLOT(openDialog()));
//    connect(playBtn, SIGNAL(clicked()), SLOT(pauseResume()));
//    connect(mpv, SIGNAL(positionChanged(int)), slider, SLOT(setValue(int)));
//    connect(mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));
    
    connect(ui->open, SIGNAL(released()), this, SLOT(openDialog()));
    connect(ui->prev, SIGNAL(released()), slideshow, SLOT(previous()));
    connect(ui->togglePause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(ui->next, SIGNAL(released()), slideshow, SLOT(next()));
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------------------------------------
// public slots

void MainWindow::open(QString path) {
    mpv->command(QStringList() << "loadfile" << path);
}

void MainWindow::seek(int pos) {
    mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void MainWindow::togglePause() {
    const bool paused = slideshow->togglePause();
    ui->togglePause->setText(paused ? "Resume" : "Pause");
}

//------------------------------------------------------------------
// private slots

void MainWindow::setSliderRange(int duration) {
//    slider->setRange(0, duration);
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
    open(path);
}
