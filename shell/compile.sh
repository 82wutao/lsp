gcc -c commands.c -o commands.o
gcc -c shell.c   -o shell.o
gcc shell.o commands.o -o shello.out -lreadline
