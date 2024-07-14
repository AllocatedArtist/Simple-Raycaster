
OBJ = temp/main.o
GPP = em++
OPT = -O2
FLAGS = -Wall \
				-std=c++17 \
				-g \
				--shell-file custom_html/custom_page.html \
				-s USE_GLFW=3 \
				-s ASSERTIONS=1 \
				--embed-file assets


LIBS = -L lib -l raylib
HEADER = -I headers/raylib

all: $(OBJ)
	$(GPP) -o index.html $(FLAGS) $(LIBS) $<
	
temp/%.o: %.cc 
	$(GPP) -c $< $(OPT) $(HEADER) -o $@

.PHONY: clean run

run:
	./game.exe

clean:
	rm -f temp/*.o *.exe *.html *.js *.wasm
