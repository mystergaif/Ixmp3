#!/bin/bash

# Скрипт установки IxMP3 плеера
# Использование: sudo ./install.sh

echo "🎵 Установка IxMP3 плеера..."
echo "=============================="

# Проверка прав администратора
if [ "$EUID" -ne 0 ]; then
  echo "Пожалуйста, запустите скрипт с правами администратора:"
  echo "sudo ./install.sh"
  exit 1
fi

# Создание директорий
echo "📁 Создание директорий..."
mkdir -p /usr/local/bin
mkdir -p /usr/share/applications
mkdir -p /usr/share/icons/hicolor/256x256/apps
mkdir -p /usr/share/icons/hicolor/48x48/apps
mkdir -p /usr/share/icons/hicolor/32x32/apps

# Компиляция проекта
echo "🔨 Компиляция проекта..."
cd build/Desktop_Qt_6_9_1-Debug

# Если нет исполняемого файла, собираем
if [ ! -f "Ixmp3" ]; then
    echo "⚙️  Сборка проекта..."
    cmake --build .
fi

# Копирование исполняемого файла
if [ -f "Ixmp3" ]; then
    echo "📦 Копирование исполняемого файла..."
    cp Ixmp3 /usr/local/bin/ixmp3
    chmod +x /usr/local/bin/ixmp3
else
    echo "❌ Ошибка: исполняемый файл Ixmp3 не найден!"
    echo "Пожалуйста, соберите проект в Qt Creator сначала."
    exit 1
fi

cd ../..

# Копирование иконки (если есть)
echo "🖼️  Установка иконки..."
if [ -f "Ixmp3_logo.png" ]; then
    cp Ixmp3_logo.png /usr/share/icons/hicolor/256x256/apps/ixmp3.png
    cp Ixmp3_logo.png /usr/share/icons/hicolor/48x48/apps/ixmp3.png
    cp Ixmp3_logo.png /usr/share/icons/hicolor/32x32/apps/ixmp3.png
else
    echo "⚠️  Иконка не найдена, будет использована стандартная"
    # Создаем простую иконку из текста (опционально)
    echo "🎵" > /tmp/ixmp3_icon.txt
fi

# Создание desktop-файла
echo "📄 Создание desktop-файла..."
cat > /usr/share/applications/ixmp3.desktop << 'EOF'
[Desktop Entry]
Name=IxMP3 Player
Comment=MP3 Player with Visualizer
Exec=ixmp3 %f
Icon=ixmp3
Terminal=false
Type=Application
Categories=AudioVideo;Player;Audio;
MimeType=audio/mpeg;audio/wav;audio/ogg;audio/x-flac;audio/mp4;audio/x-m4a;
StartupNotify=true
Keywords=mp3;player;audio;music;visualizer;
EOF

# Установка прав на desktop-файл
chmod +x /usr/share/applications/ixmp3.desktop

# Обновление базы данных desktop-файлов
echo "🔄 Обновление базы данных приложений..."
update-desktop-database >/dev/null 2>&1 || echo "⚠️  Не удалось обновить базу desktop-файлов"

# Обновление кэша иконок
echo "🔄 Обновление кэша иконок..."
gtk-update-icon-cache /usr/share/icons/hicolor/ >/dev/null 2>&1 || echo "⚠️  Не удалось обновить кэш иконок"

# Регистрация MIME-типов
echo "📝 Регистрация MIME-типов..."
cat > /usr/share/mime/packages/ixmp3.xml << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="audio/mpeg">
    <comment>MP3 Audio</comment>
    <glob weight="100" pattern="*.mp3"/>
  </mime-type>
  <mime-type type="audio/wav">
    <comment>WAV Audio</comment>
    <glob weight="100" pattern="*.wav"/>
  </mime-type>
  <mime-type type="audio/ogg">
    <comment>OGG Audio</comment>
    <glob weight="100" pattern="*.ogg"/>
  </mime-type>
  <mime-type type="audio/x-flac">
    <comment>FLAC Audio</comment>
    <glob weight="100" pattern="*.flac"/>
  </mime-type>
  <mime-type type="audio/mp4">
    <comment>M4A Audio</comment>
    <glob weight="100" pattern="*.m4a"/>
  </mime-type>
</mime-info>
EOF

# Обновление MIME-базы
update-mime-database /usr/share/mime >/dev/null 2>&1 || echo "⚠️  Не удалось обновить MIME-базу"

echo ""
echo "✅ Установка завершена успешно!"
echo ""
echo "Теперь вы можете:"
echo "🎵 Запускать из терминала: ixmp3"
echo "🖱️  Открывать файлы через правую кнопку мыши -> Открыть с помощью -> IxMP3 Player"
echo "🎯 Перетаскивать файлы на окно плеера"
echo "🔍 Найти приложение в меню 'Звук и видео' или поиске приложений"
echo ""
