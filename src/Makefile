CXX = g++
CXXFLAGS = -Wall -O0 -fpermissive -std=c++0x
LDLIBS = -lglfw -lGL -lGLU -lGLEW -pthread

OBJ_DIR = bin
LIB_DIR = -L/usr/lib
INC_DIR = -I/usr/include

SOURCE = main.cpp
OBJECTS = ${SOURCE:%.cpp=$(OBJ_DIR)/%.o}
EXECUTABLE = main

all: clean init $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE):
	$(CXX) $(LDFLAGS) $(LIB_DIR) -o $@ $(OBJECTS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(INC_DIR) -c $< -o $@

init:
	@mkdir -p "$(OBJ_DIR)"

clean:
	@rm -rf $(OBJ_DIR) $(EXECUTABLE)