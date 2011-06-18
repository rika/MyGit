CXXFLAGS=-O2 -Wall

TARGET=tracer

SDIR=./src

_DEPS = control.h
DEPS = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = Control.o Tracer.o 
OBJ = $(patsubst %,$(SDIR)/%,$(_OBJ))

all: $(TARGET)

$(SDIR)/%.o: $(SDIR)/%.cpp $(SDIR)/$(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJ)

