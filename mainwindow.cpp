#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QDebug>



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

    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    player->setVideoOutput(videoSink);

    connect(videoSink, &QVideoSink::videoFrameChanged, this, &MainWindow::processFrame);

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
    QVideoFrame cloneFrame = frame;

    if(cloneFrame.map(QVideoFrame::ReadOnly)){

        QImage image = cloneFrame.toImage();
        if(!image.isNull()){
            qDebug() << "processing...";
            int outHeight = image.height() * outWidth / image.width() *0.45;
            image = image.scaled(outWidth, outHeight);
            QString chars = "@#S%?*+;:,.' ";
            QString result;

            for (int y = 0; y < image.height(); y++) {
                for (int x = 0; x < image.width(); x++) {
                    QRgb pixel = image.pixel(x,y);
                    int gray = 0.2126 * qRed(pixel) + 0.7152 * qGreen(pixel) + 0.0722 * qBlue(pixel);
                    gray = (gray - 128) * 1.15 + 128;
                    gray = std::clamp(gray, 0, 255);
                    if (ui->checkBoxInvert->isChecked()){
                        gray = 255 - gray;
                    }
                    int index = ((255-gray)*(chars.size()-1))/255;
                    result += chars[index];
                }
                result += "\n";
            }
            ui->textAscii->setPlainText(result);
        }
        cloneFrame.unmap();
    }
    }
}