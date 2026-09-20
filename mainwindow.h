#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QElapsedTimer>
#include <QCamera>
#include <QMediaCaptureSession>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_Camera_clicked();
    void on_ContrastSlider_valueChanged(int value);
    void processFrame(const QVideoFrame &frame);

    void on_ColorButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QImage currentImage;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QVideoSink* videoSink;
    QVideoSink* cameraSink;
    QString file;
    bool newFile = false;
    QElapsedTimer timer;
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    const int targetIntervals = 33;
    double contrastThreshold = 1.15;
    bool grayscaleFrame = true;
    bool originalColorFrame = false;

    int outWidth = 400;

};
#endif // MAINWINDOW_H
