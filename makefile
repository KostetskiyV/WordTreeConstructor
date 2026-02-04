CC := gcc
CFLAGS := -std=c99 -Werror -Wall -Wextra -Wpedantic -g

app.exe: main.o text_manager.o node.o tree.o
	$(CC) -o $@ $^ -lm

./%.o : ./%.c ./*.h
	$(CC) $(CFLAGS) -c $< -o $@

./data/tree_%.png : ./data/tree_%.gv
	dot -T png -o $@ $^

.PHONY: clean graph
clean:
	rm -f *.o *.exe

graph: ./data/tree_input.png ./data/tree_final.png
	open ./data/tree_input.png
	open ./data/tree_final.png