# VARIABLES #
COMPILER = c++
INC_DIR = includes/
INCLUDES = $(INC_DIR)*.h*
FLAGS = -std=c++98
FLAGS += -Wall -Werror -Wextra
FLAGS += -g3
FLAGS += -I$(INC_DIR)

BINARY = serv

SOURCE_DIR = srcs/
SOURCES_FILES =	main.cpp \
				Request.cpp \
				CGI.cpp \
				utils.cpp \
				parseConfig.cpp \
				Server.cpp \
				Location.cpp \
				response.cpp

SRCS = $(addprefix $(SOURCE_DIR), $(SOURCES_FILES))

OBJ_DIR = objs/
OBJ_FILES = $(SOURCES_FILES:.cpp=.o)
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

# RULES #
all: $(BINARY)

$(BINARY): $(OBJS) $(INCLUDES)
	$(COMPILER) $(FLAGS) $(SRCS) -o $(BINARY)

$(OBJ_DIR)%.o: $(SOURCE_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	@echo "deleting objects"
	@rm -rf $(OBJ_DIR)

fclean:
	@make --no-print-directory clean
	@echo "deleting binary"
	@rm -rf $(BINARY)

re:
	@make --no-print-directory clean
	@make --no-print-directory all

.PHONY: clean fclean re all
