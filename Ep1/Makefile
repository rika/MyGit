GLFLAGS = -framework OpenGL -framework GLUT
CXXFLAGS =	-O2 -Wall

SRC_DIR = src

OBJS =		trecos.o

TARGET =	trecos

$(TARGET):	$(SRC_DIR)/$(OBJS)
	$(CXX) $(GLFLAGS) -o $(TARGET) $(SRC_DIR)/$(OBJS) 

all:	$(TARGET)

clean:
	rm -f $(SRC_DIR)/$(OBJS) $(TARGET)
