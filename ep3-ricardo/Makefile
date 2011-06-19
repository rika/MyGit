CXXFLAGS=-O2 -Wall

TARGET=tracer

SDIR=./src

_DEPS = control.h data.h vector.h
DEPS = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = Tracer.o Control.o Data.o Vector.o
OBJ = $(patsubst %,$(SDIR)/%,$(_OBJ))

all: $(TARGET)

$(SDIR)/%.o: $(SDIR)/%.cpp $(SDIR)/$(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJ)

