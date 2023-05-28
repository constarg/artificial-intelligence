


all: make_dir
	gcc -Wall -o3 ./src/main.c -o ./bin/main

make_dir:
	mkdir -p ./bin/
