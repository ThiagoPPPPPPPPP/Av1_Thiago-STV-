TARGET = Av1_Thiago

CXX = g++
CC = gcc

# --- CORREÇÃO AQUI ---
# --cflags busca os caminhos dos headers (.hpp)
OPENCV_FLAGS = $(shell pkg-config --cflags opencv4)
# --libs busca os binários das bibliotecas (.so)
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Bibliotecas de sistema para interface no Linux
GRAPHICS_LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

# INCLUDES
INCLUDES = -I. -IClass -ILibs -ILibs/imgui -ILibs/glad

# Flags de compilação (Agora com os flags do OpenCV corrigidos)
CXXFLAGS = -std=c++17 -O2 -Wall $(INCLUDES) $(OPENCV_FLAGS)
CFLAGS = $(INCLUDES) -O2

# SOURCES
SOURCES = main.cpp \
          $(wildcard Class/*.cpp) \
          $(wildcard Libs/imgui/*.cpp) \
          Libs/glad/glad.c

# Objetos
OBJ_CPP = $(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCES)))
OBJ_C = $(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))
OBJECTS = $(OBJ_CPP) $(OBJ_C)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(OPENCV_LIBS) $(GRAPHICS_LIBS) -rdynamic
	@echo "--- Compilacao Concluida com Sucesso! ---"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f Class/*.o Libs/imgui/*.o Libs/glad/*.o *.o