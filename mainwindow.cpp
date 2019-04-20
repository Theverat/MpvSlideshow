
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sliderstyle.h"

#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QFileDialog>
#include <QShortcut>
#include <QMouseEvent>
#include <QSettings>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->videoSeekBar->setStyle(new MyStyle(ui->videoSeekBar->style()));
    compositor = ui->compositor;
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    ui->compositor->setMouseTracking(true);
    
    shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O", "Open")), this);
    shortcutPrev = new QShortcut(QKeySequence(tr("Left", "Previous")), this);
    shortcutNext = new QShortcut(QKeySequence(tr("Right", "Next")), this);
    shortcutTogglePause = new QShortcut(QKeySequence(Qt::Key_Space), this);
    
    connect(shortcutOpen, SIGNAL(activated()), this, SLOT(openDialog()));
    connect(shortcutPrev, SIGNAL(activated()), compositor, SLOT(previousFile()));
    connect(shortcutNext, SIGNAL(activated()), compositor, SLOT(nextFile()));
    connect(shortcutTogglePause, SIGNAL(activated()), this, SLOT(togglePause()));
    
    connect(ui->open, SIGNAL(released()), this, SLOT(openDialog()));
    connect(ui->prev, SIGNAL(released()), compositor, SLOT(previousFile()));
    connect(ui->togglePause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(ui->next, SIGNAL(released()), compositor, SLOT(nextFile()));
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
    qDebug() << "ui deleted";
}

//------------------------------------------------------------------
// public slots

void MainWindow::togglePause() {
    const bool paused = compositor->togglePause();
    ui->togglePause->setText(paused ? tr("Resume") : tr("Pause"));
}

//------------------------------------------------------------------
// protected

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    qDebug() << "wrote settings";
    event->accept();
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
//    compositor->openDir("/home/simon/Bilder/mpvslideshowteset/marokko");
    
//    return;
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    const QString currentDir = compositor->getCurrentDirPath();
    dialog.setDirectory(currentDir.size() ? currentDir : QDir::homePath());
    
    const bool ok = dialog.exec();
    if(!ok) {
        return;
    }
    
    const QString path = dialog.selectedFiles().at(0);
    qDebug() << "opening Dir:" << path;
    compositor->openDir(path);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (ui->bottomControls->geometry().contains(event->pos())) {
        ui->bottomControls->show();
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    readSettings();
}

void MainWindow::writeSettings() {
    QSettings qsettings("simon", "mpvslideshow");

    qsettings.beginGroup("mainwindow");

    qsettings.setValue("geometry", saveGeometry());
    qsettings.setValue("savestate", saveState());
    qsettings.setValue("maximized", isMaximized());

    if ( !isMaximized() ) {
        qsettings.setValue("pos", pos());
        qsettings.setValue("size", size());
    }
    
    qsettings.setValue("lastDir", compositor->getCurrentDirPath());
    qsettings.setValue("lastIndex", compositor->getCurrentIndex());
    qsettings.setValue("imageDuration", ui->imageDuration->value());
    qsettings.setValue("fadeDuration", ui->fadeDuration->value());

    qsettings.endGroup();
}

void MainWindow::readSettings() {
    QSettings qsettings("simon", "mpvslideshow");

    qsettings.beginGroup("mainwindow");

    restoreGeometry(qsettings.value("geometry", saveGeometry()).toByteArray());
    restoreState(qsettings.value("savestate", saveState()).toByteArray());
    move(qsettings.value("pos", pos()).toPoint());
    resize(qsettings.value("size", size()).toSize());

    if ( qsettings.value("maximized", isMaximized()).toBool())
        showMaximized();

    const QString lastDir = qsettings.value("lastDir").toString();
    const int lastIndex = qsettings.value("lastIndex").toInt();
    if (lastDir.size() > 0) {
        qDebug() << "Restoring session:" << lastDir << "index:" << lastIndex;
        compositor->openDir(lastDir, lastIndex);
    }
    if (qsettings.contains("imageDuration"))
        ui->imageDuration->setValue(qsettings.value("imageDuration").toDouble());
    if (qsettings.contains("fadeDuration"))
        ui->fadeDuration->setValue(qsettings.value("fadeDuration").toDouble());

    qsettings.endGroup();
}
