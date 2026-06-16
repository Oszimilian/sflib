all:
	gcc main.c -o main

example:
	gcc examples/example_module.c -o examples/example

clean:
	rm -f main examples/example
