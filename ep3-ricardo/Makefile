CXXFLAGS=-O2 -Wall

TARGET=tracer
SOURCE=tracer.cpp, control.cpp
OBJ=./obj/tracer.o, ./obj/control.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

./obj/tracer.o: ./src/tracer.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGET) $(OBJ)

