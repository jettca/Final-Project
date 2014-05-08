CXX = g++ -O0

INCLUDES = -iquote src
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = 

executable = bin/main
srcd = src
objd = obj
objects = scene.o input.o mesh.o
objects := $(addprefix $(objd)/, $(objects))

GL = includes/gl_include.h

.PHONY: default
default: $(executable)

$(executable): $(objects)
	$(CXX) $(INCLUDES) $(LDLIBS) $(LDFLAGS) $(objects) -o $(executable)

# Include dependencies

$(objd)/scene.o: $(srcd)/engine/mesh.hpp
	$(CXX) -c $(INCLUDES) $(srcd)/engine/scene.cpp -o $(objd)/scene.o

$(objd)/input.o: $(srcd)/engine/scene.hpp
	$(CXX) -c $(INCLUDES) $(srcd)/input/input.cpp -o $(objd)/input.o

$(objd)/mesh.o:
	$(CXX) -c $(INCLUDES) $(srcd)/engine/mesh.cpp -o $(objd)/mesh.o

.PHONY: clean
clean:
	rm -f $(objd)/* $(executable)

.PHONY: all
all: clean default
