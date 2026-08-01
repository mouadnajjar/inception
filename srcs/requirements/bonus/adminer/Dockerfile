FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    php8.2-fpm \
    php8.2-mysql \
    wget \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /run/php

COPY conf/www.conf /etc/php/8.2/fpm/pool.d/www.conf
COPY tools/adminer_setup.sh /adminer_setup.sh

RUN chmod +x /adminer_setup.sh

ENTRYPOINT ["/adminer_setup.sh"]