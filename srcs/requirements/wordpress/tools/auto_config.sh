#!/bin/bash
set -e

echo "WordPress: Waiting for MariaDB to start..."
until mysql -h mariadb -u "${SQL_USER}" -p"${SQL_PASSWORD}" -e "SELECT 1;" >/dev/null 2>&1; do
    sleep 2
done
echo "WordPress: MariaDB is up and running!"

if [ ! -f "/var/www/html/wp-config.php" ]; then
    echo "WordPress: Commencing fresh installation..."

    mkdir -p /var/www/html
    cd /var/www/html

    curl -O https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar
    chmod +x wp-cli.phar
    mv wp-cli.phar /usr/local/bin/wp

    wp core download --allow-root

    # Create wp-config.php using the environment variables from your.env file
    wp config create \
        --dbname="${SQL_DATABASE}" \
        --dbuser="${SQL_USER}" \
        --dbpass="${SQL_PASSWORD}" \
        --dbhost="mariadb:3306" \
        --allow-root

    # Perform the master core installation
    wp core install \
        --url="monajjar.42.fr" \
        --title="${WP_TITLE}" \
        --admin_user="${WP_ADMIN_USER}" \
        --admin_password="${WP_ADMIN_PASSWORD}" \
        --admin_email="${WP_ADMIN_EMAIL}" \
        --allow-root

    # Create a secondary, non-administrator user
    wp user create \
        "${WP_USER}" \
        "${WP_USER_EMAIL}" \
        --role=author \
        --user_pass="${WP_USER_PASSWORD}" \
        --allow-root

    echo "WordPress: Installation complete!"
else
    echo "WordPress: Already installed. Skipping setup..."
fi

#BONUS
# ==============================================================================
echo "WordPress: Setting up Redis Cache..."
wp config set WP_REDIS_HOST "redis" --allow-root || echo "Redis: Failed to set host"
wp config set WP_REDIS_PORT 6379 --raw --allow-root || echo "Redis: Failed to set port"
wp plugin install redis-cache --activate --allow-root || echo "Redis: Plugin install failed"
wp redis enable --allow-root || echo "Redis: Enable failed"
# ==============================================================================

chown -R www-data:www-data /var/www/html

echo "WordPress: Starting PHP-FPM..."
exec /usr/sbin/php-fpm8.2 -F