FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    php8.2-fpm \
    php8.2-mysql \
    mariadb-client \
    curl \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /run/php

COPY conf/www.conf /etc/php/8.2/fpm/pool.d/www.conf

COPY tools/auto_config.sh /auto_config.sh

RUN chmod +x /auto_config.sh

WORKDIR /var/www/html

ENTRYPOINT ["/auto_config.sh"]