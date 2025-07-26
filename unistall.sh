#!/bin/bash

# Скрипт удаления IxMP3 плеера
# Использование: sudo ./uninstall.sh

echo "🗑️  Удаление IxMP3 плеера..."
echo "==========================="

# Проверка прав администратора
if [ "$EUID" -ne 0 ]; then
  echo "Пожалуйста, запустите скрипт с правами администратора:"
  echo "sudo ./uninstall.sh"
  exit 1
fi

# Удаление файлов
echo "🗑️  Удаление файлов..."
rm -f /usr/local/bin/ixmp3
rm -f /usr/share/applications/ixmp3.desktop
rm -f /usr/share/icons/hicolor/256x256/apps/ixmp3.png
rm -f /usr/share/icons/hicolor/48x48/apps/ixmp3.png
rm -f /usr/share/icons/hicolor/32x32/apps/ixmp3.png
rm -f /usr/share/mime/packages/ixmp3.xml

# Обновление системных баз
echo "🔄 Обновление системных баз..."
update-desktop-database >/dev/null 2>&1 || echo "⚠️  Не удалось обновить базу desktop-файлов"
gtk-update-icon-cache /usr/share/icons/hicolor/ >/dev/null 2>&1 || echo "⚠️  Не удалось обновить кэш иконок"
update-mime-database /usr/share/mime >/dev/null 2>&1 || echo "⚠️  Не удалось обновить MIME-базу"

echo ""
echo "✅ IxMP3 плеер успешно удален!"
echo ""
