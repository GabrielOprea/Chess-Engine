CC = g++
CFLAGS = -g -O3
LIBS =
SOURCES = pieces/bishop.cpp pieces/king.cpp pieces/knight.cpp main.cpp pieces/pawn.cpp pieces/piece.cpp pieces/queen.cpp pieces/rook.cpp game/table.cpp game/move.cpp
OBJS = $(SOURCES:.cpp=.o)
EXE=main

build: $(EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ 

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(EXE)  $(LIBS)

run:
	./$(EXE)

clean:
	rm -f *.o $(EXE)
	rm -f pieces/*.o
	rm -f game/*.o
