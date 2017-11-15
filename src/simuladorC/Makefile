
main: model.o parser.o
	gcc main.c parser.o model.o -o out `pkg-config --libs --cflags opencv` -ldl -lm -g;

clean:
	rm -f main main.o model.o parser.o
