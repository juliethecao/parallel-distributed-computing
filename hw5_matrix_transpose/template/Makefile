NAME = hw5
CCOPTS = -Wall -pthread
#CCOPTS = -Wall -fopenmp
CCLIBS = -lm
SRC = main.c util.c transpose.c
BIN = $(NAME).bin


.PHONY: build clean test
build: $(BIN)
$(BIN): $(SRC)
	gcc $(CCOPTS) -o $@ $^ $(CCLIBS) 


clean:
	rm -rf $(BIN) sequential.txt parallel.txt


MAT_SIZE = 50
GRA = 4
THR = 4
VERB = 1
test: 
	./$(BIN) $(MAT_SIZE) s $(VERB) | head -n -1 > sequential.txt
	./$(BIN) $(MAT_SIZE) p $(GRA) $(THR) $(VERB) | head -n -1 > parallel.txt
	diff sequential.txt parallel.txt
