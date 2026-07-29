NAME = inception
COMPOSE_FILE = srcs/docker-compose.yml
DATA_PATH = /home/monajjar/data

all: up

up:
	@echo "Creating local persistent data storage directories..."
	@mkdir -p $(DATA_PATH)/mariadb
	@mkdir -p $(DATA_PATH)/wordpress
	@echo "Building and launching containers in detached mode..."
	@docker compose -f $(COMPOSE_FILE) up -d --build

down:
	@echo "Stopping active container processes..."
	@docker compose -f $(COMPOSE_FILE) down

clean: down
	@echo "Tearing down containers and removing internal docker volumes..."
	@docker compose -f $(COMPOSE_FILE) down -v

fclean: clean
	@echo "Performing deep system prune and deleting local physical data..."
	@docker system prune -af
	@sudo rm -rf $(DATA_PATH)
	@echo "Cleanup complete. System reset to blank slate."

re: fclean up

.PHONY: all up down clean fclean re