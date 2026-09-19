#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QDebug>
#include <QMediaDevices>
#include <QVideoWidget>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ui->textAscii->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->textAscii->setContentsMargins(0,0,0,0);
    ui->textAscii->setStyleSheet(
        "QTextEdit { padding: 0px; }"
        );
    setWindowTitle("Video to ASCII");
    showMaximized();
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    videoSink = new QVideoSink;
    cameraSink = new QVideoSink;
    camera = new QCamera(QMediaDevices::defaultVideoInput());
    captureSession = new QMediaCaptureSession(this);

    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    player->setVideoOutput(videoSink);

    captureSession->setCamera(camera);
    captureSession->setVideoSink(cameraSink);

    connect(videoSink, &QVideoSink::videoFrameChanged, this, &MainWindow::processFrame);
    connect(cameraSink, &QVideoSink::videoFrameChanged, this, &MainWindow::processFrame);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(newFile){
    player->setSource(QUrl::fromLocalFile(file));
        newFile = false;
    }
    qDebug() << "reached first checkpoint";
    player->play();
    timer.start();

}
void MainWindow::on_Camera_clicked(){
    if(camera->isActive()){
        camera->stop();
    }
    else{
        timer.start();
        camera->start();
    }
}
void MainWindow::on_pushButton_2_clicked()
{
    file = QFileDialog::getOpenFileName(this, "Open Video", "", "Videos (*.mkv *.mov *.mp4)");
    if (file.isEmpty()){
        qDebug() << "No video selected";
        ui->label->setText("No file selected");
        return;
    }

    ui->label->setText("Selected: " + (file));
    newFile = true;

}
void MainWindow::on_pushButton_3_clicked(){
    player->pause();
}
void MainWindow::processFrame(const QVideoFrame &frame){

{
    if(timer.elapsed() < targetIntervals) return;
    timer.restart();
    QImage image = frame.toImage();
    if(image.isNull()) return;
    const int columns = 200;
    const int rows = columns * image.height() / image.width() / 2;
    image = image.scaled(columns, rows, Qt::IgnoreAspectRatio, Qt::FastTransformation).convertToFormat(QImage::Format_Grayscale16);
    qDebug() << "processing...";
    QString chars = "@#S%?*+;:,.' ";
    QString result;

    for (int y = 0; y < rows; y++) {
        const uchar *line = image.constScanLine(y);
        for (int x = 0; x < columns; x++) {
            QRgb pixel = image.pixel(x,y);
            int gray = std::clamp(int((line[x]-128) * 1.15 + 128), 0, 255);
            if (ui->checkBoxInvert->isChecked()){
                gray = 255 - gray;
            }
            result += chars[((255 - gray) * (chars.size()-1))/255];
        }
        result += "\n";
    }
    ui->textAscii->setPlainText(result);
    ui->textAscii->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));


    }
}