#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTime>
#include <QFileInfo>
#include <QTimer>
#include <QVector>
#include <QPaintEvent>
#include <QPainter>
#include <QMimeData>

class VisualizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerWidget(QWidget *parent = nullptr);
    void setAudioData(const QVector<qreal> &data);
    void setPlaying(bool playing);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<qreal> audioData;
    bool isPlaying;
    QTimer *updateTimer;
    int colorOffset;

private slots:
    void updateColors();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openFileFromPath(const QString &filePath); // Новый метод для открытия файла по пути

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void openFile();
    void playPause();
    void stop();
    void setPosition(int position);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QPushButton *openButton;
    QPushButton *playButton;
    QPushButton *stopButton;
    QSlider *positionSlider;
    QLabel *timeLabel;
    QLabel *fileLabel;
    QSlider *volumeSlider;
    VisualizerWidget *visualizer;

    QString formatTime(qint64 milliseconds);
    QString currentFilePath;
};

#endif // MAINWINDOW_H
