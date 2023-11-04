# VARIABLES #

COMPILER = c++
INC_DIR = includes/
INCLUDES = $(INC_DIR)*.h*
LOG ?= 1
VALUE_FILE = .log_value

FLAGS = -std=c++98
FLAGS += -Wall -Werror -Wextra
#FLAGS += -g3 #-fno-limit-debug-info
FLAGS += -I$(INC_DIR)
FLAGS += -D LOG_LEVEL=$(LOG)

BINARY = serv
SOURCE_DIR = srcs/
SOURCES_FILES = main.cpp Request.cpp CGI.cpp utils.cpp parseConfig.cpp Server.cpp Location.cpp response.cpp Mime.cpp handleConnections.cpp
SRCS = $(addprefix $(SOURCE_DIR), $(SOURCES_FILES))

OBJ_DIR = objs/
OBJ_FILES = $(SOURCES_FILES:.cpp=.o)
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

.PHONY: clean fclean re all run

# RULES #
all: $(BINARY)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BINARY): $(OBJS) $(INCLUDES)
	$(COMPILER) $(FLAGS) $(SRCS) -o $(BINARY)

$(OBJ_DIR)%.o: $(SOURCE_DIR)%.cpp $(INCLUDES)
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	@echo "Deleting objects"
	@rm -rf $(OBJ_DIR)
	@rm -rf $(VALUE_FILE)

fclean: clean
	@echo "Deleting binary"
	@rm -rf $(BINARY)

re: fclean all
