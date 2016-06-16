CC     = gcc
FLAGS  = -std=c99 -Wall
LIBS   = -ledit -lm
SRC    = main.c mpc.c eval.c parser.c types.c builtin.c prompt.c
OUT    = lisp

default: build

build:
	@echo "building.."
	@$(CC) $(FLAGS) $(SRC) $(LIBS) -o $(OUT)
	@echo "done"

.PHONY: clean
clean:
	@echo "cleaning up"
	@rm $(OUT)

run:
	@./$(OUT)
