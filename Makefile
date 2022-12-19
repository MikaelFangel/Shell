all: shell.c shell.h
	gcc $< -o shell

clean:
	rm -rf shell *.o
