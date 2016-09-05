CC     = gcc
FLAGS  = -std=c99 -Wall -Iinclude/lispy
LIBS   = -ledit -lm
OUT    = lispy
SRC    = src/main.c    \
	 src/mpc.c     \
	 src/eval.c    \
	 src/parser.c  \
	 src/types.c   \
	 src/builtin.c \
	 src/prompt.c

default: build

build:
	@echo "building.."
	@$(CC) $(FLAGS) $(SRC) $(LIBS) -o $(OUT)
	@echo "done"

install:
	@mv $(OUT) ~/.local/bin/$(OUT)

uninstall:
	@rm ~/.local/bin/$(OUT)

.PHONY: clean
clean:
	@echo "cleaning up"
	@rm $(OUT)
