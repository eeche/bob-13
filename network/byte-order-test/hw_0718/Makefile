TARGET=add-nbo-reviewed
CXXFLAGS=-g

all: $(TARGET)

$(TARGET): add-nbo-reviewed.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f $(TARGET)
	rm -f *.o
