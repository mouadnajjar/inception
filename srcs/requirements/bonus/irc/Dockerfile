FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /var/www/irc

COPY app/ .

RUN make

EXPOSE 6667

CMD ["./ircserv", "6667", "112233"]