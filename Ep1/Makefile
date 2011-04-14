CXXFLAGS=-O2 -Wall
LINUXFLAGS=-lGL -lglut -lGLU
MACOSFLAGS=-framework OpenGL -framework GLUT

SRC_DIR=src

SRC=trecos.cpp
OBJS=trecos.o

TARGET=trecos

all: linux

linux:
	$(CXX) -c $(CXXFLAGS) -D LINUX -o $(SRC_DIR)/$(OBJS) $(SRC_DIR)/$(SRC)
	$(CXX) $(CXXFLAGS) -$(LINUXFLAGS) o $(TARGET) $(SRC_DIR)/$(OBJS)

mac: $(SRC_DIR)/$(SRC)
	$(CXX) -c $(CXXFLAGS) -D MACOS -o $(SRC_DIR)/$(OBJS) $(SRC_DIR)/$(SRC)
	$(CXX) $(CXXFLAGS) $(MACOSFLAGS) -o $(TARGET) $(SRC_DIR)/$(OBJS)


clean:
	rm -f $(SRC_DIR)/$(OBJS) $(TARGET)

