CC = gcc

ifeq ($(OS),Windows_NT)
	LIBS += -lmingw32
	FLAGS += -mwindows
	RM = cmd.exe /c del
else
	RM = rm -f
endif

LIBS += -lSDL2main -lSDL2 -lSDL2_mixer
CFLAGS += -O2 -Wall -Wextra -pedantic -std=c17

EXE = spinning_cube

ifeq ($(OS),Windows_NT)
	EXE := $(EXE).exe
endif

all: main.c
	$(CC) main.c -o $(EXE) $(CFLAGS) $(LIBS) $(FLAGS)

clean:
	$(RM) $(EXE)
