
OBJ = temp/main.o
GPP = em++
OPT = -O2
FLAGS = -Wall -std=c++17 -g --shell-file custom_html/custom_page.html

all: $(OBJ)
	$(GPP) -o game.html $(FLAGS) $<
	
temp/%.o: %.cc 
	$(GPP) -c $< $(OPT) -o $@

.PHONY: clean run

run:
	./game.exe

clean:
	rm -f temp/*.o *.exe *.html *.js *.wasm
