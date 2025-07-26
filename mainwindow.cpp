#include "mainwindow.h"
#include <QRandomGenerator>
#include <cmath>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMessageBox>

// Реализация визуализатора
VisualizerWidget::VisualizerWidget(QWidget *parent)
    : QWidget(parent), isPlaying(false), colorOffset(0)
{
    setMinimumHeight(100);
    setStyleSheet("background-color: #1e1e1e; border: 1px solid #555555;");

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &VisualizerWidget::updateColors);
    updateTimer->start(50); // Обновляем каждые 50 мс
}

void VisualizerWidget::setAudioData(const QVector<qreal> &data)
{
    audioData = data;
    update();
}

void VisualizerWidget::setPlaying(bool playing)
{
    isPlaying = playing;
    if (isPlaying && !updateTimer->isActive()) {
        updateTimer->start(50);
    } else if (!isPlaying) {
        updateTimer->stop();
    }
    update();
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    painter.fillRect(rect, QColor(30, 30, 30));

    if (!isPlaying) {
        // Рисуем статичные линии если не играет
        for (int i = 0; i < 64; ++i) {
            int x = i * (rect.width() / 64);
            int height = 5 + QRandomGenerator::global()->bounded(10);
            QColor color = QColor::fromHsv((colorOffset + i * 5) % 360, 200, 200, 150);
            painter.setPen(QPen(color, 2));
            painter.drawLine(x, rect.height() - height, x, rect.height());
        }
        return;
    }

    if (audioData.isEmpty()) {
        // Генерируем случайные данные если нет реальных
        for (int i = 0; i < 64; ++i) {
            qreal value = 0.1 + QRandomGenerator::global()->bounded(0.4);
            int x = i * (rect.width() / 64);
            int barHeight = value * rect.height();
            QColor color = QColor::fromHsv((colorOffset + i * 5) % 360, 255, 255);
            painter.setPen(QPen(color, 3));
            painter.drawLine(x, rect.height(), x, rect.height() - barHeight);
        }
    } else {
        // Рисуем реальные аудиоданные
        int bars = qMin(64, audioData.size());
        for (int i = 0; i < bars; ++i) {
            qreal value = qAbs(audioData[i]);
            int x = i * (rect.width() / bars);
            int barHeight = value * rect.height() * 2; // Увеличиваем для лучшей видимости
            QColor color = QColor::fromHsv((colorOffset + i * 5) % 360, 255, 255);
            painter.setPen(QPen(color, 2));
            painter.drawLine(x, rect.height(), x, rect.height() - barHeight);
        }
    }
}

void VisualizerWidget::updateColors()
{
    colorOffset = (colorOffset + 2) % 360;
    update();
}

// Реализация основного окна
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentFilePath("")
{
    // Разрешаем перетаскивание файлов
    setAcceptDrops(true);

    // Создание элементов интерфейса
    openButton = new QPushButton("📂 Открыть");
    playButton = new QPushButton("▶ Воспроизвести");
    stopButton = new QPushButton("⏹ Остановить");
    positionSlider = new QSlider(Qt::Horizontal);
    timeLabel = new QLabel("00:00 / 00:00");
    fileLabel = new QLabel("🎵 Нет выбранного файла");
    visualizer = new VisualizerWidget();

    // Стилизация кнопок
    openButton->setStyleSheet("QPushButton { background-color: #4a4a4a; color: white; padding: 8px; border-radius: 5px; }"
                              "QPushButton:hover { background-color: #5a5a5a; }");
    playButton->setStyleSheet("QPushButton { background-color: #2e7d32; color: white; padding: 8px; border-radius: 5px; }"
                              "QPushButton:hover { background-color: #388e3c; }");
    stopButton->setStyleSheet("QPushButton { background-color: #c62828; color: white; padding: 8px; border-radius: 5px; }"
                              "QPushButton:hover { background-color: #d32f2f; }");

    // Создаем слайдер для регулировки громкости
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #999999;"
        "    height: 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2e2e2e, stop:1 #4a4a4a);"
        "    margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
        "    border: 1px solid #5c5c5c;"
        "    width: 18px;"
        "    margin: -2px 0;"
        "    border-radius: 3px;"
        "}"
        );

    // Подключение сигнала valueChanged к слоту изменения громкости
    connect(volumeSlider, &QSlider::valueChanged,
            [=](int value) {
                audioOutput->setVolume(value / 100.0);
            });

    // Настройка медиаплеера
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    // Подключение сигналов и слотов
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stop);
    connect(positionSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);

    // Настройка слайдера позиции
    positionSlider->setRange(0, 0);
    positionSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #999999;"
        "    height: 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2e2e2e, stop:1 #4a4a4a);"
        "    margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
        "    border: 1px solid #5c5c5c;"
        "    width: 18px;"
        "    margin: -2px 0;"
        "    border-radius: 3px;"
        "}"
        );

    // Создание layout'ов
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(stopButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileLabel);
    mainLayout->addWidget(visualizer);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(positionSlider);
    mainLayout->addWidget(timeLabel);

    // Добавляем слайдер громкости в layout
    QLabel *volumeLabel = new QLabel("🔊 Громкость:");
    volumeLabel->setStyleSheet("color: white; font-weight: bold;");
    mainLayout->addWidget(volumeLabel);
    mainLayout->addWidget(volumeSlider);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    setWindowTitle("🎵 IxMP3 Плеер");
    resize(500, 300);

    // Запускаем визуализатор
    visualizer->setPlaying(false);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFileFromPath(const QString &filePath)
{
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
        player->setSource(QUrl::fromLocalFile(filePath));
        currentFilePath = filePath;
        fileLabel->setText("🎵 " + QFileInfo(filePath).fileName());
        playButton->setText("▶ Воспроизвести");
        visualizer->setPlaying(false);
        player->play();
        playButton->setText("⏸ Пауза");
        visualizer->setPlaying(true);
    }
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Открыть аудиофайл", "", "Аудиофайлы (*.mp3 *.wav *.ogg *.flac *.m4a)");

    if (!fileName.isEmpty()) {
        openFileFromPath(fileName);
    }
}

void MainWindow::playPause()
{
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        playButton->setText("▶ Воспроизвести");
        visualizer->setPlaying(false);
    } else {
        if (currentFilePath.isEmpty()) {
            openFile();
        } else {
            player->play();
            playButton->setText("⏸ Пауза");
            visualizer->setPlaying(true);
        }
    }
}

void MainWindow::stop()
{
    player->stop();
    playButton->setText("▶ Воспроизвести");
    visualizer->setPlaying(false);
}

void MainWindow::setPosition(int position)
{
    player->setPosition(position);
}

void MainWindow::positionChanged(qint64 position)
{
    positionSlider->setValue(position);
    timeLabel->setText(formatTime(position) + " / " + formatTime(player->duration()));
}

void MainWindow::durationChanged(qint64 duration)
{
    positionSlider->setRange(0, duration);
    timeLabel->setText("00:00 / " + formatTime(duration));
}

QString MainWindow::formatTime(qint64 milliseconds)
{
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

// Поддержка drag & drop
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString fileName = urls.first().toLocalFile();
            if (fileName.endsWith(".mp3", Qt::CaseInsensitive) ||
                fileName.endsWith(".wav", Qt::CaseInsensitive) ||
                fileName.endsWith(".ogg", Qt::CaseInsensitive) ||
                fileName.endsWith(".flac", Qt::CaseInsensitive) ||
                fileName.endsWith(".m4a", Qt::CaseInsensitive)) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    QMainWindow::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString fileName = urls.first().toLocalFile();
            if (QFile::exists(fileName)) {
                openFileFromPath(fileName);
                event->acceptProposedAction();
                return;
            }
        }
    }
    QMainWindow::dropEvent(event);
}
