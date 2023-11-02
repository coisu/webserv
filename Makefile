# VARIABLES #

COMPILER = c++
INC_DIR = includes/
INCLUDES = $(INC_DIR)*.h*
LOG ?= 1
VALUE_FILE = .log_value

FLAGS = -std=c++98
FLAGS += -Wall -Werror -Wextra
FLAGS += -g3
FLAGS += -I$(INC_DIR)
FLAGS += -D LOG_LEVEL=$(LOG)

BINARY = serv
SOURCE_DIR = srcs/
SOURCES_FILES = main.cpp Request.cpp CGI.cpp utils.cpp parseConfig.cpp Server.cpp Location.cpp response.cpp Mime.cpp handleConnections.cpp
SRCS = $(addprefix $(SOURCE_DIR), $(SOURCES_FILES))

OBJ_DIR = objs/
OBJ_FILES = $(SOURCES_FILES:.cpp=.o)
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

.PHONY: clean fclean re all run check-log

# RULES #
all: $(BINARY)

$(BINARY): $(OBJS) $(INCLUDES) | check-log
	$(COMPILER) $(FLAGS) $(OBJS) -o $(BINARY)

$(OBJ_DIR)%.o: $(SOURCE_DIR)%.cpp $(INCLUDES) | check-log
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -c $< -o $@

check-log:
	@if [ ! -f $(VALUE_FILE) ] || [ "$(LOG)" != "$$(cat $(VALUE_FILE))" ]; then \
		echo "$(LOG)" > $(VALUE_FILE); \
		find $(OBJ_DIR) -type f -name '*.o' -exec touch {} +; \
	else \
		echo "LOG_LEVEL has not changed"; \
	fi

clean:
	@echo "Deleting objects"
	@rm -rf $(OBJ_DIR)
	@rm -rf $(VALUE_FILE)

fclean: clean
	@echo "Deleting binary"
	@rm -rf $(BINARY)

re: fclean all

run: all
	@valgrind --quiet --leak-check=full --show-leak-kinds=all --track-fds=yes --error-exitcode=1 -s ./$(BINARY)
