CFLAGS += -Wall --std=c18

.PHONY: default clean coverage

SOURCES:= src/ast.c src/c_compiler.c src/codegen.c
HEADERS:= src/ast.h src/codegen.h

default: bin/c_compiler

bin/compiler: $(SOURCES) $(HEADERS)
	mkdir -p build
	g++ $(SOURCES) $(CFLAGS) build/parser.tab.c build/lexer.yy.c -o bin/c_compiler

build/parser.tab.c build/parser.tab.h: src/parser.y
	mkdir -p build
	bison -v -d src/parser.y -o build/parser.tab.c

build/lexer.yy.c: src/lexer.flex build/parser.tab.h
	mkdir -p build
	flex -o build/lexer.yy.c src/lexer.flex

clean:
	-rm -rf bin/
	-rm -rf build/
