Based on your VS Code project workspace (**Image 1**), your local DNS mapping in `/etc/hosts` (**Image 2**), and your highly successful NGINX "dummy test," here is a precise audit of what you have completed so far, followed by an exhaustive step-by-step master plan of what is left to build until you finish the entire project.

---

### Part 1: What You Have Completed So Far

1. **Host and DNS Configuration:** You successfully intercepted local DNS routing by mapping your custom domain `monajjar.42.fr` to the local loopback address `127.0.0.1` inside your `/etc/hosts` file.
2. **Base Directory Scaffolding:** You created the correct directory structure matching the strict specifications of the project architecture (with `srcs/requirements/`, `.env`, `docker-compose.yml`, and `Makefile`).
3. **Finished NGINX Container:**
* Created a custom `Dockerfile` built strictly from the penultimate stable base image (`FROM debian:bookworm`).
* Automated a non-interactive self-signed SSL/TLS certificate generation using `openssl req -x509 -nodes` valid for 365 days.
* Configured a correct `nginx.conf` that strictly enforces port 443 with modern security protocols (`TLSv1.2` or `TLSv1.3` only).
* Implemented correct FastCGI routing (`location ~ \.php$`) pointing downstream dynamic requests to your future WordPress container on port 9000.
* Solved the PID 1 container lifecycle issue by forcing NGINX to execute natively in the foreground via `ENTRYPOINT ["nginx", "-g", "daemon off;"]`.
* Successfully executed a manual live "dummy test" to prove NGINX handles DNS requests and encrypts local web traffic.



*(Now, please make sure you run `docker stop test-nginx && docker rm test-nginx` on your terminal to free up port 443 before continuing!)*.

---

### Part 2: What's Next (The Complete Blueprint to the End of the Project)

To finalize your architecture, you must build, parameterize, and orchestrate the remaining components of your LEMP stack.

```
                   +------------------------------+
                   |       Computer HOST          |
                   |   (monajjar.42.fr:443)       |
                   +--------------+---------------+
                                  |
                                  v
+-----------------------------------------------------------------+
|                         Docker Network                          |
|                                                                 |
|  +------------------+    FastCGI     +-----------------------+  |
|  |    Container     |  ===========>  |       Container       |  |
|  |      NGINX       |  (Port 9000)   |    WordPress + PHP    |  |
|  +--------+---------+                +-----------+-----------+  |
|           |                                      |              |
|           |                                      | TCP (3306)   |
|           | Shared Volume (wordpress_files)      v              |
|           | (Host: /home/monajjar/data/wp)   +---+-----------+  |
|           |                                  |   Container   |  |
|           +--------------------------------> |    MariaDB    |  |
|                                              +---+-----------+  |
|                                                  |              |
|                                                  | SQL Volume   |
|                                                  v (db_files)   |
|                                        (Host: /home/monajjar/   |
|                                              data/mariadb)      |
+-----------------------------------------------------------------+
``` [1]

#### Step 1: Containerizing the Database Layer (MariaDB)
Your database is a stateful container. It must store tables, users, and passwords, and survive restarts.

*   **Create the Dockerfile (`srcs/requirements/mariadb/Dockerfile`):**
    *   Start from `FROM debian:bookworm`.
    *   Use `RUN apt-get update && apt-get install -y mariadb-server`.
    *   **The Sockets Requirement:** MariaDB requires a secure folder to write its local connection socket file. You must run `mkdir -p /var/run/mysqld && chown -R mysql:mysql /var/run/mysqld` in your Dockerfile to prevent boot-up permission crashes.
*   **Configure Port Binding (`50-server.cnf`):**
    *   Create a configuration folder `conf/` next to your Dockerfile.
    *   Copy the default database configurations into it, making sure you update the bind address to `bind-address = 0.0.0.0`. This ensures MariaDB listens on all internal virtual network interfaces so your WordPress container can connect to it.
*   **Write the Runtime Initialization Script (`tools/init.sh`):**
    *   You must write an entrypoint shell script. Because containers execute non-interactively, this script must check if your database directories are empty.
    *   If empty (first-run detection), it runs `mariadb-install-db` to generate internal system privilege tables.
    *   It starts a temporary, local-only background server (using `mysqld --skip-networking`) and runs SQL commands to create your WordPress database and user (granting permissions) using environment variables.
    *   It safely shuts down the temporary database.
    *   Finally, it launches the live database in the foreground as PID 1 using: `exec mysqld --user=mysql --datadir=/var/lib/mysql`.

#### Step 2: Containerizing the Application Layer (WordPress + PHP-FPM)
This container is responsible for executing the PHP code of your CMS.

*   **Create the Dockerfile (`srcs/requirements/wordpress/Dockerfile`):**
    *   Start from `FROM debian:bookworm`.
    *   Install PHP, PHP-FPM, PHP-MySQL (the database drivers), alongside `curl` and `wget`.
*   **Configure FastCGI Routing (`www.conf`):**
    *   PHP-FPM defaults to running on a local UNIX socket. Since WordPress is isolated from NGINX, you must modify PHP's `www.conf` pool file.
    *   Set `listen = 9000` to instruct PHP-FPM to listen for FastCGI requests over the internal TCP network.
*   **Write the Automated Configuration Script (`tools/auto_config.sh`):**
    *   This is the entrypoint script. It must bypass the manual WordPress web-installation screen using the official `wp-cli` command-line tool.
    *   **The Readiness Probe:** The script must use a loop (e.g., polling with `mysqladmin ping`) to wait until the MariaDB container is fully initialized and open on port 3306.
    *   Downloads WordPress using `wp core download --allow-root`.
    *   Generates your database connection secrets using `wp config create`.
    *   Installs WordPress using `wp core install`. *Constraint Check:* You must create two distinct users (an administrator and a regular subscriber).[1] The administrator’s username **cannot** contain the word "admin" or "administrator" in any case.[1]
    *   Finally, launch PHP-FPM in the foreground as PID 1 using `exec php-fpm8.2 -F`.

#### Step 3: Orchestrating the Stack via Docker Compose
Your files must now be declared in `srcs/docker-compose.yml` to define network pathways and persistent storage.

*   **Define Networks:** Create a custom bridge network (e.g., `inception-network`). You must avoid using deprecated `--link` or `network_mode: host` options.
*   **Define Named Volumes:** Declaring two strict Docker named volumes is mandatory (one for database tables, and one for website files).[1] To ensure your data is stored persistently on your host machine under `/home/monajjar/data/` [1], you must use the local driver in compose:
    ```yaml
    volumes:
      db_files:
        driver: local
        driver_opts:
          type: none
          o: bind
          device: /home/monajjar/data/mariadb
      wp_files:
        driver: local
        driver_opts:
          type: none
          o: bind
          device: /home/monajjar/data/wordpress
    ```
*   **Declare Services:** Define the `nginx`, `wordpress`, and `mariadb` services, mapping ports, matching networks, injecting `.env` parameters, setting restart-on-failure options, and mounting your volumes.
*   **The shared web root:** The volume `wp_files` must be mounted into **both** NGINX (to serve CSS/HTML assets) and WordPress (to compile backend dynamic scripts).

#### Step 4: Automating with the root `Makefile`
Your repository requires a `Makefile` at the root directory to handle automated infrastructure lifecycles.[1]

*   Write target rules inside your `Makefile`:
    *   `all` or `up`: Must automatically inspect your host system, safely create the host data directories if they do not exist (`/home/monajjar/data/mariadb` and `/home/monajjar/data/wordpress`), and trigger `docker compose -f srcs/docker-compose.yml up -d --build`.
    *   `down`: Stops the running containers using `docker compose -f srcs/docker-compose.yml down`.
    *   `clean`: Stops containers and deletes all cache and volumes.
    *   `fclean`: Safely performs a complete, destructive teardown of your infrastructure. It stops the stack, deletes all volumes, wipes the certificates, and completely deletes `/home/monajjar/data/` to reset your system to a blank slate.
    *   `re`: Triggers `fclean` followed by `all` to rebuild and re-initialize your entire infrastructure cleanly.

#### Step 5: Finalizing Evaluation Documentation
Before presenting your project, you must write three specific Markdown files at the root of your Git repository:

1.  **`README.md`:** Must describe your project architecture.[1] You must include explicit comparative sections explaining [1]:
    *   *Virtual Machines vs. Docker Containers*.[1]
    *   *Secrets vs. Environment Variables*.[1]
    *   *Docker Networks vs. Host Networks*.[1]
    *   *Docker Volumes vs. Host Bind Mounts*.[1]
2.  **`USER_DOC.md`:** A simple guide showing an end-user how to start/stop the stack, log in to WordPress, access the dashboard, and verify service logs.[1]
3.  **`DEV_DOC.md`:** A technical blueprint for administrators explaining dependencies, file layout, and how stateful persistence is achieved.[1]

---

### Your Next Step to Code Right Now:
You are ready to begin **Step 1: Containerizing MariaDB**. Let's start building your database! Would you like me to walk you through constructing the MariaDB `Dockerfile` or the database configuration file next?

```