CPP  = g++
CC   = gcc
SRC  = $(wildcard src/*.cpp)
OBJ  = $(patsubst src/%.cpp, %.o, $(SRC))
LIBS = -static-libgcc -static-libstdc++ -L"deps/freeglut/lib" -lfreeglut -lfreeglut_static -lopengl32 -lglu32 -s
INCS = -I"deps/freeglut/include"
BIN  = Project2.exe
CXXFLAGS = $(INCS) -fexpensive-optimizations -O3 -std=c++11
RM = rm -f

.PHONY: all clean

all: $(BIN)

clean:
	$(RM) $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS)

%.o: src/%.cpp
	$(CPP) $(CXXFLAGS) -c $^ -o $@
