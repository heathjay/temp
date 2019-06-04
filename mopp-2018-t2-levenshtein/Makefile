CC = g++
GG = go build

.PHONY = all clean

all : lev

clean :
	rm -rf lev

lev : lev_dist.cc
	${CC} -O3 lev_dist.cc -o lev

	${GG} -o levMain main.go 