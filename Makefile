CXX = g++ -O0

INCLUDES = -iquote src
CXXFLAGS = -g -Wno-deprecated $(INCLUDES)

LDFLAGS = -framework OpenGL -framework GLUT -lGLEW -DGLEW_STATIC
LDLIBS = 

bin = bin
executable = $(bin)/main
srcd = src
objd = obj
objects = main.o scene.o input.o mesh.o light.o loadshaders.o
objects := $(addprefix $(objd)/, $(objects))

GL = includes/gl_include.h

.PHONY: default
default: $(executable)

$(executable): $(objects)
	mkdir -p $(bin)
	mkdir -p $(objd)
	$(CXX) $(objects) -o $(executable) $(LDLIBS) $(LDFLAGS) 

# Include dependencies

$(objd)/main.o: $(srcd)/main.cpp $(srcd)/input/input.hpp $(srcd)/engine/scene.cpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/main.cpp -o $(objd)/main.o

$(objd)/scene.o: $(srcd)/engine/scene.cpp $(srcd)/engine/mesh.hpp \
		$(srcd)/engine/shaders/loadshaders.hpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/engine/scene.cpp -o $(objd)/scene.o

$(objd)/input.o: $(srcd)/input/input.cpp $(srcd)/engine/scene.hpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/input/input.cpp -o $(objd)/input.o

$(objd)/mesh.o: $(srcd)/engine/mesh.cpp $(srcd)/engine/light.hpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/engine/mesh.cpp -o $(objd)/mesh.o

$(objd)/light.o: $(srcd)/engine/light.cpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/engine/light.cpp -o $(objd)/light.o

$(objd)/loadshaders.o: $(srcd)/engine/shaders/loadshaders.cpp
	$(CXX) $(CXXFLAGS) -c $(srcd)/engine/shaders/loadshaders.cpp -o $(objd)/loadshaders.o

.PHONY: clean
clean:
	rm -f $(objd)/* $(executable)

.PHONY: all
all: clean default
