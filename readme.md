
*This project has been created as part of the 42 curriculum by monajjar.*

# 🐳 Inception

## Description
This project is an introduction to system administration and Docker. The goal is to build a complete, containerized web infrastructure from scratch using `docker-compose`. By strictly utilizing microservices, this project demonstrates how to securely route traffic, manage databases, and isolate internal networks without relying on heavy virtual machines.

### 🏗️ Architecture & Services
The infrastructure runs on a custom Docker bridge network (`inception_network`) to ensure backend services remain hidden. It consists of:
*   **NGINX:** The sole entry point (Port 443), configured strictly with TLSv1.2/1.3.
*   **WordPress:** The core web application running via PHP-FPM.
*   **MariaDB:** The relational database, isolated from the host network.
*   **Adminer (Bonus):** A lightweight database management interface.
*   **Redis (Bonus):** An in-memory cache to optimize WordPress database queries.
*   **Static Site (Bonus):** A custom portfolio site routed through Nginx.
*   **IRC Server (Bonus):** A custom C++ chat server proving internal network communication.

### 🔍 Technical Comparisons

**Virtual Machines vs Docker**
*   **Virtual Machines:** Virtualize the hardware. Each VM runs its own full guest operating system and kernel, providing strong absolute security but consuming significant system resources.
*   **Docker:** Virtualizes the operating system. All containers share the host machine's kernel and use namespaces to create the illusion of a separate OS. This sacrifices a layer of hardware security in exchange for massive performance gains, resource efficiency, and lightweight microservices.

**Secrets vs Environment Variables**
*   **Environment Variables (Our Setup):** Loaded globally into the container's environment, making them easily accessible but vulnerable if a bad actor gains terminal access (visible via commands like `env`).
*   **Docker Secrets:** Securely mounted as temporary, in-memory files (tmpfs) that restrict access strictly to the application that needs them, protecting credentials even if the container is compromised.

**Docker Network vs Host Network**
*   **Docker Network (Bridge):** Uses network namespaces to isolate containers in a private virtual network. Ports are only exposed if explicitly published, keeping backend services entirely secure from external access.
*   **Host Network:** Removes network isolation, sharing the host’s IP address and network interfaces directly. This exposes sensitive backend services directly to the host's network.

**Docker Volumes vs Bind Mounts**
*   **Docker Volumes:** Managed completely by Docker and stored in its internal directory (e.g., `/var/lib/docker/volumes`). They are easily managed via the Docker CLI and remain independent of the host machine's file structure.
*   **Bind Mounts (Our Setup):** Directly link a container directory to a specific absolute path on the host machine. The host operating system retains ultimate control over the files, permissions, and directory structure.

## ⚙️ Instructions

1. **Clone the repository:**
   ```bash
   git clone <your_repo_link>
   cd inception
```
2. **Setup the Environment Variables:**
```
For security reasons, the `.env` file containing database credentials is not tracked by Git. During the evaluation, copy the prepared `.env` file from the secure local directory into the `srcs` folder:
```bash
cp ~/env/.env srcs/.env

```


3. **Build and launch the infrastructure:**
```bash
make    

```


4. **Access the site:**
Navigate to `https://monajjar.42.fr` in your web browser. (Note: Accept the self-signed SSL certificate warning).

## 📚 Resources

* [Docker: Dockerfile Parser Directives](https://docs.docker.com/reference/dockerfile/#parser-directives)
* [OpenSSL: Certificate Generation Examples](https://docs.openssl.org/master/man1/openssl-req/#examples)
* [NGINX: Core Module Configuration](https://nginx.org/en/docs/ngx_core_module.html#example)
* [Project Architecture Diagram (tldraw)](https://www.google.com/search?q=https://www.tldraw.com/f/xXo5MoplQ5Ndhc2P8kdhS%3Fd%3Dv-636.-56.6155.3585.page)

**AI Usage:** 
```
Artificial Intelligence was utilized strictly as a tutoring tool to gain a deeper theoretical understanding of infrastructure concepts, network namespaces, and Docker security mechanics.
```
