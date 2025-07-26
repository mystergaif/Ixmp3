#include "mainwindow.h"
#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QFileOpenEvent>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Установка темной темы
    QPalette darkPalette;
    QColor darkColor = QColor(45, 45, 45);
    QColor disabledColor = QColor(120, 120, 120);

    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    app.setPalette(darkPalette);
    app.setStyle("Fusion");

    // Установка имени приложения для интеграции
    app.setApplicationName("IxMP3");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("IxMP3");

    MainWindow window;
    window.show();

    // Проверяем аргументы командной строки
    if (argc > 1) {
        QString fileName = QString::fromLocal8Bit(argv[1]);
        if (QFile::exists(fileName)) {
            window.openFileFromPath(fileName);
        }
    }

    return app.exec();
}
