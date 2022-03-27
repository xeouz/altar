srcfldr= ./Transpiler
outfldr= ./Executable
srcfile= main.c
src= $(srcfldr)/$(srcfile)
dst= $(outfldr)/runner.exe
Cflags= -Wall -Werror -g -lm 
CDFlags = -fsanitize=address

Vflags= --track-origins=yes --leak-check=full --memcheck --leak-resolution=high

Gflags= -f
clean:
	sudo rm $(dst) -f
run:
	sudo gcc $(src) -o $(dst) $(Cflags)
	sudo $(dst)
push:
	sudo git push github $(Gflags)
grind:
	sudo valgrind $(dst) $(Vflags)