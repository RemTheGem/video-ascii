#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoFrame>

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
    void processFrame(const QVideoFrame &frame);

private:
    Ui::MainWindow *ui;
    QImage currentImage;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QVideoSink* videoSink;
    QString file;
    bool newFile = false;

    int outWidth = 400;

};
#endif // MAINWINDOW_H
