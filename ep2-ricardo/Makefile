CXXFLAGS=-O2 -Wall
LINUXFLAGS=-lGL -lglut -lGLU
MACOSFLAGS=-framework OpenGL -framework GLUT

SRC_DIR=src

SRC=simulador.cpp
OBJS=simulador.o

TARGET=simulador

all: linux

linux:
	$(CXX) -c $(CXXFLAGS) -D LINUX -o $(OBJS) $(SRC_DIR)/$(SRC)
	$(CXX) $(CXXFLAGS) $(LINUXFLAGS) -o $(TARGET) $(OBJS)

mac: $(SRC_DIR)/$(SRC)
	$(CXX) -c $(CXXFLAGS) -D MACOS -o $(OBJS) $(SRC_DIR)/$(SRC)
	$(CXX) $(CXXFLAGS) $(MACOSFLAGS) -o $(TARGET) $(OBJS)


clean:
	rm -f $(OBJS) $(TARGET)

