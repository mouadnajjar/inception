#!/bin/bash
set -e

if [ ! -d "/var/lib/mysql/${SQL_DATABASE}" ]; then
    mysql_install_db --user=mysql --datadir=/var/lib/mysql > /dev/null

    mysqld --skip-networking --socket=/run/mysqld/mysqld.sock --user=mysql &
    pid="$!"

    until mysqladmin --socket=/run/mysqld/mysqld.sock ping >/dev/null 2>&1; do
        sleep 1
    done

    mysql --socket=/run/mysqld/mysqld.sock -u root << EOF
CREATE DATABASE IF NOT EXISTS \`${SQL_DATABASE}\`;
CREATE USER IF NOT EXISTS '${SQL_USER}'@'%' IDENTIFIED BY '${SQL_PASSWORD}';
GRANT ALL PRIVILEGES ON \`${SQL_DATABASE}\`.* TO '${SQL_USER}'@'%';
ALTER USER 'root'@'localhost' IDENTIFIED BY '${SQL_ROOT_PASSWORD}';
FLUSH PRIVILEGES;
EOF

    mysqladmin --socket=/run/mysqld/mysqld.sock -u root -p"${SQL_ROOT_PASSWORD}" shutdown
    wait "$pid"
fi

exec mysqld --user=mysql --datadir=/var/lib/mysql --socket=/run/mysqld/mysqld.sock