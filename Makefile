srcfldr=
dstfldr= 
releasefldr= ./Release
srcfile= cli.c
dstfile= runner
releasefile= /altar

src= $(srcfldr)$(srcfile)
dst= $(dstfldr)$(dstfile)
Cflags= -Wall -Werror -g -lm 
CDFlags = -fsanitize=address

Vflags= --track-origins=yes --leak-check=full --memcheck --leak-resolution=high

Gflags= -f

Optimization= -O3

cmd= compile test.a

run:
	clear
	gcc $(src) -o $(dst) $(Cflags)
	$(dst) $(cmd)

release-win:
	clear
	gcc $(Optimization) $(src) -o $(releasefldr)$(releasefile)

clean:
	rm $(dst) -f

push:
	sudo git push github $(Gflags)

grind:
	sudo valgrind $(dst) $(Vflags)
