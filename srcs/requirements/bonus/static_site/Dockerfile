FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

RUN npm install -g http-server

RUN mkdir -p /var/www/static_site

COPY app /var/www/static_site

CMD ["http-server", "/var/www/static_site", "-p", "3000", "-c-1"]