CC=gcc
GG = go build
OUT=amd
FLAGS=-pthread

all: amd

amd: amd.c
	$(CC) $(FLAGS) -o $(OUT) amd.c
	${GG} -o average main.go 

debug: amd.c
	$(CC) $(FLAGS) -o $(OUT).debug amd.c -DDEBUG=1 -Wall

clean:
	rm -f $(OUT) $(OUT).debug
	