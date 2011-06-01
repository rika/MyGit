CXXFLAGS=-O2 -Wall
LINUXFLAGS=-lGL -lglut -lGLU
MACOSFLAGS=-framework OpenGL -framework GLUT


SRC=src/simulador.cpp
OBJ=simulador.o

TARGET=simulador

all: linux

linux:
	$(CXX) -c $(CXXFLAGS) -D LINUX -o $(OBJ) $(SRC)
	$(CXX) $(CXXFLAGS) $(LINUXFLAGS) -o $(TARGET) $(OBJ)

mac:
	$(CXX) -c $(CXXFLAGS) -D MACOS -o $(OBJ) $(SRC)
	$(CXX) $(CXXFLAGS) $(MACOSFLAGS) -o $(TARGET) $(OBJ)


clean:
	rm -f $(OBJS) $(TARGET)

