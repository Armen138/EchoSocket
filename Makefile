CXX = g++
CXX_FLAGS = -Wall -fexceptions  -std=c++0x -g -c
CXX_LDFLAGS = -Lz
OBJS = main websocket/src/Connection websocket/src/HandShake websocket/src/WebSocket websocket/src/base64 websocket/src/sha1 src/Client src/Server

all :
	$(foreach OBJ,$(OBJS),$(CXX) $(CXX_FLAGS) $(OBJ).cpp -o $(OBJ).o;)
	$(CXX) -o server $(foreach OBJ,$(OBJS),$(OBJ).o) $(CXX_LDFLAGS)
clean :
	$(foreach OBJ,$(OBJS),rm $(OBJ).o;)
	rm server