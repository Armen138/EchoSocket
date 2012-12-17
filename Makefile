CXX = g++
CXX_FLAGS = -Wall -fexceptions  -std=c++0x -g -c
CXX_LDFLAGS = -Lz
OBJS = main src/Connection src/HandShake src/WebSocket src/base64 src/sha1

all :
	$(foreach OBJ,$(OBJS),$(CXX) $(CXX_FLAGS) $(OBJ).cpp -o $(OBJ).o;)
	$(CXX) -o server $(foreach OBJ,$(OBJS),$(OBJ).o) $(CXX_LDFLAGS)
clean :
	$(foreach OBJ,$(OBJS),rm $(OBJ).o;)
	rm server