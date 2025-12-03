CC = g++
CFLAGS = -std=c++17 -Wall -Iinclude -I/home/aluslu/shinra_tensei/local_libs/include
LIBS = `sdl2-config --cflags` -L/home/aluslu/shinra_tensei/local_libs/lib -lSDL2_ttf `sdl2-config --libs` -Wl,-rpath,'/home/aluslu/shinra_tensei/local_libs/lib'

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

NAME = shinra_tensei

SOURCES = Game.cpp Grid.cpp main.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

# Règle principale
all: $(NAME)

# Linking des .o vers l'exécutable
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME) $(LIBS)

# Compilation des .cpp en .o avec dépendances .hpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.hpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Règle spéciale pour main.cpp (pas de main.hpp généralement)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Créer le dossier obj s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re