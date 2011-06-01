CXXFLAGS=-O2 -Wall
LINUXFLAGS=-lGL -lglut -lGLU
MACOSFLAGS=-framework OpenGL -framework GLUT


SIM=src/simulador.cpp
BMP=src/bmp.cpp
SIM_OBJ=simulador.o
BMP_OBJ=bmp.o
OBJS=$(BMP_OBJ) $(SIM_OBJ)

TARGET=simulador

all: linux

linux:
	$(CXX) -c $(CXXFLAGS) -D LINUX -o $(SIM_OBJ) $(SIM)
	$(CXX) -c $(CXXFLAGS) -D LINUX -o $(BMP_OBJ) $(BMP)
	$(CXX) $(CXXFLAGS) $(LINUXFLAGS) -o $(TARGET) $(OBJS)

mac:
	$(CXX) -c $(CXXFLAGS) -D MACOS -o $(SIM_OBJ) $(SIM)
	$(CXX) -c $(CXXFLAGS) -D MACOS -o $(BMP_OBJ) $(BMP)
	$(CXX) $(CXXFLAGS) $(MACOSFLAGS) -o $(TARGET) $(OBJS)


clean:
	rm -f $(OBJS) $(TARGET)

