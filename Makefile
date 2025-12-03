CC = g++
NAME = shinra_tensei

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# --- Dépendances locales ---
LOCAL_LIB_DIR = libs
SDL2_TTF_PATH = $(LOCAL_LIB_DIR)/SDL2

# --- Flags de compilation et de liaison ---

# Détecte automatiquement tous les fichiers .cpp dans src/
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Flags pour le compilateur (chemins des headers)
# -I$(INC_DIR) pour vos headers locaux (ex: Game.h)
# -I$(SDL2_TTF_PATH)/include pour SDL_ttf.h
CPPFLAGS = -I$(INC_DIR) -I$(LOCAL_LIB_DIR) $(shell sdl2-config --cflags)
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Flags pour l'éditeur de liens (chemins des bibliothèques)
LDFLAGS = -L$(SDL2_TTF_PATH)/.libs

# Bibliothèques à lier
LDLIBS = $(shell sdl2-config --libs) -lSDL2_ttf

# Règle principale
all: $(NAME)

# Linking des .o vers l'exécutable
$(NAME): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

# Compilation des .cpp en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Créer le dossier obj s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re