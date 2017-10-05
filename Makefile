#i knwo i should be using autoconf or smth but whatever

CC=g++
FLAGS="-std=c++11"

all: main.cc
	mkdir -p bin
	$(CC) $(FLAGS) -o bin/dotenv $<

clean:
	rm -rf bin
	rm -r *.o