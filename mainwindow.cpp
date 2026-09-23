#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QDebug>
#include <QMediaDevices>
#include <QVideoWidget>
#include <QFileInfo>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Formatting
    ui->textAscii->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->textAscii->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->textAscii->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textAscii->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textAscii->document()->setDocumentMargin(0);
    // Contrast Slider
    ui->ContrastSlider->setRange(100, 200);
    ui->ContrastSlider->setValue(115);

    setWindowTitle("Video to ASCII");
    showMaximized();
    // create the media players, audio output and video sinks used for processing
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    videoSink = new QVideoSink;
    cameraSink = new QVideoSink;
    // Camera using the system's default video input device
    camera = new QCamera(QMediaDevices::defaultVideoInput());
    // capture session to manage the camera and its video output
    captureSession = new QMediaCaptureSession(this);
    // connect the media player to its audio and video outputs
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    player->setVideoOutput(videoSink);
    // connect the camera to the capture session and send the captures frames to the camera sink
    captureSession->setCamera(camera);
    captureSession->setVideoSink(cameraSink);
    // connections to process each frame from the media player or the camera
    connect(videoSink, &QVideoSink::videoFrameChanged, this, &MainWindow::processFrame);
    connect(cameraSink, &QVideoSink::videoFrameChanged, this, &MainWindow::processFrame);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// PLAY: button to play the input video
void MainWindow::on_pushButton_clicked()
{
    if(newFile){
    player->setSource(QUrl::fromLocalFile(file));
        newFile = false;
    }
    if(camera->isActive()){
        camera->stop();
    }
    player->play();
    timer.start();

}
// CAMERA: button to start camera (and stop video if there is one)
void MainWindow::on_Camera_clicked(){
    if(camera->isActive()){
        camera->stop();
    }
    else{
        if(player->isPlaying()) player->pause();
        timer.start();
        camera->start();
    }
}
// ADD VIDEO: button to add a local video
void MainWindow::on_pushButton_2_clicked()
{
    file = QFileDialog::getOpenFileName(this, "Open Video", "", "Videos (*.mkv *.mov *.mp4)");
    if (file.isEmpty()){
        qDebug() << "No video selected";
        ui->label->setText("No file selected");
        return;
    }
    QFileInfo fileInfo(file);
    ui->label->setText("Selected: " + fileInfo.fileName());
    newFile = true;

}
// PAUSE: button to pause the current video
void MainWindow::on_pushButton_3_clicked(){
    player->pause();
    if(camera->isActive()){
        camera->stop();
    }
}
// CONTRAST SLIDER: slider to increase or decrease contrast
void MainWindow::on_ContrastSlider_valueChanged(int value){
    contrastThreshold = value / 100.0;
}
// method to process every frame recieved from video input or camera feed
void MainWindow::processFrame(const QVideoFrame &frame){

{
    // timer to ensure a stable frame rate
    if(timer.elapsed() < targetIntervals) return;
    timer.restart();
    // turn each frame into an image for processing
    QImage image = frame.toImage();
    if(image.isNull()) return;
    // data to make the text window behave
    const QSize box = ui->textAscii->viewport()->size();
    const QFontMetrics fontMetrics(ui->textAscii->font());
    const int cellWidth = fontMetrics.horizontalAdvance('@');
    const int cellHeight = fontMetrics.lineSpacing();
    const double scale = std::min(double(box.width())/image.width(), double(box.height())/image.height());
    const int columns = std::max(1, int(image.width() * scale/cellWidth));
    const int rows = std::max(1, int(image.height() * scale/cellHeight));
    // if we're processing grayscale frames
    if(grayscaleFrame){
        // scale the image to fit in our text window and turn it into grayscale
        image = image.scaled(columns, rows, Qt::IgnoreAspectRatio, Qt::FastTransformation).convertToFormat(QImage::Format_Grayscale8);
        // characters to draw stuff
        QString chars = "@#S%?*+;:,.' ";
        QString result;
        // go through every row and column
        for (int y = 0; y < rows; y++) {
            const uchar *line = image.constScanLine(y);
            for (int x = 0; x < columns; x++) {
                // get the brightness of the pixel at this row and column
                int gray = std::clamp(int((line[x]-128) * contrastThreshold + 128), 0, 255);
                // if inverted, invert the brightness
                if (ui->checkBoxInvert->isChecked()){
                    gray = 255 - gray;
                }
                // add the corresponding character to our result string
                result += chars[((255 - gray) * (chars.size()-1))/255];
            }
            // add a new line at the end of each row
            result += "\n";
        }
        // set our final result
        ui->textAscii->setPlainText(result);
    }
    // if we're processing color frames
    if(originalColorFrame){
        // scale the image to fit in our text window
        image = image.scaled(columns, rows, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        // clear previous output
        ui->textAscii->clear();
        // create a cursor (needed in order to format our text)
        QTextCursor cursor(ui->textAscii->document());
        cursor.beginEditBlock();
        // characters to draw stuff
        QString chars = "@#S%?*+;:,.' ";
        QString result;
        QTextCharFormat format;
        QColor color;
        QChar character;
        // go through every column and row
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < columns; x++) {
                // get color for this pixel
                color = image.pixelColor(x, y);
                int gray = qGray(color.rgb());
                // get brightness for this pixel
                int index = qBound(0, ((255 - gray) *(chars.size()-1)/255), chars.size()-1);
                // get the corresponding character
                character = chars[index];
                // set the character's color to what the original pixel color was
                format.setForeground(color);
                // insert our character to the output
                cursor.insertText(character, format);
            }
            // insert a newline to the output
            cursor.insertText("\n");
        }
        // finish the edit block
        cursor.endEditBlock();
        }
    }
}
// COLOR: checkbox to enable and disable color
void MainWindow::on_ColorButton_toggled(bool checked)
{
    grayscaleFrame = !grayscaleFrame;
    originalColorFrame = !originalColorFrame;
}

