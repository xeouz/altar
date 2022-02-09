srcfldr= ./Compiler
outfldr= ./Executable
srcfile= main.c
src= $(srcfldr)/$(srcfile)
dst= $(outfldr)/runner.exe
Cflags= -Wall -Werror -g 

Vflags= --track-origins=yes --leak-check=full

Gflags= -f
clean:
	sudo rm $(dst) -f
run:
	sudo gcc $(src) -o $(dst) $(Cflags)
	sudo $(dst)
push:
	sudo git push github $(Gflags)
grind:
	sudo valgrind $(dst)