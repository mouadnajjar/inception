FROM debian:bookworm

# Install MariaDB server and utility packages
RUN apt-get update && apt-get install -y \
    mariadb-server \
    && rm -rf /var/lib/apt/lists/*

# Set up the secure socket directory with correct ownership
RUN mkdir -p /var/run/mysqld \
    && chown -R mysql:mysql /var/run/mysqld \
    && chmod 755 /var/run/mysqld

# Copy configurations and the runtime script from host to container
COPY conf/50-server.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/init.sh /init.sh

# Grant execution rights to our entrypoint script
RUN chmod +x /init.sh

# Run our configuration entrypoint script
ENTRYPOINT ["/init.sh"]