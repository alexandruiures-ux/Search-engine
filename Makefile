
build:
	gcc -o search_index main.c engine.c
run:
	./engine

valgrind:
	valgrind --leak-check=full --track-origins=yes ./search_index

clean:
	rm engine