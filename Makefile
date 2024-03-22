CFLAGS += -Wall --std=c18

.PHONY: default clean coverage

SOURCES:= src/ast.c src/c_compiler.c src/codegen.c src/symbol.c
HEADERS:= src/ast.h src/codegen.h src/symbol.h

default: bin/c_compiler

bin/c_compiler: $(SOURCES) $(HEADERS) build/parser.tab.c build/parser.tab.h build/lexer.yy.c
	@mkdir -p build
	@mkdir -p bin
	gcc $(SOURCES) $(CFLAGS) -Ibuild build/parser.tab.c build/lexer.yy.c -o bin/c_compiler

build/parser.tab.c build/parser.tab.h: src/parser.y
	@mkdir -p build
	bison -v -d src/parser.y -o build/parser.tab.c

build/lexer.yy.c: src/lexer.flex build/parser.tab.h
	@mkdir -p build
	flex -o build/lexer.yy.c src/lexer.flex

coverage:
	@rm -rf coverage/
	@mkdir -p coverage
	lcov -c -d bin -o coverage/cov.info
	genhtml coverage/cov.info -o coverage
	@find . -name "*.gcda" -delete

clean:
	@rm -rf bin/
	@rm -rf build/

