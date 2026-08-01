#!/bin/bash
set -e

mkdir -p /var/www/html/adminer
cd /var/www/html/adminer

if [ ! -f "index.php" ]; then
    echo "Adminer: Downloading the latest version..."
    
    wget "https://github.com/vrana/adminer/releases/download/v4.8.1/adminer-4.8.1.php" -O index.php
    
    chown -R www-data:www-data /var/www/html/adminer
    chmod 755 index.php
    
    echo "Adminer: Download complete!"
else
    echo "Adminer: Already installed."
fi

echo "Adminer: Starting PHP-FPM..."
exec /usr/sbin/php-fpm8.2 -F