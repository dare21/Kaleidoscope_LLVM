kaleidoscope: lex.yy.o parser.tab.o #ast.o
	g++ -Wall -o $@ $^
lex.yy.o: lex.yy.c parser.tab.hpp #ast.hpp 
	g++ -Wall -c -o $@ $<
lex.yy.c: lexer.lex
	flex $<
parser.tab.o: parser.tab.cpp parser.tab.hpp #ast.hpp
	g++ -Wall -c -o $@ $<
parser.tab.cpp parser.tab.hpp: parser.ypp
	bison -dv $<
#ast.o: ast.cpp ast.hpp
#	g++ -Wall -c -o $@ $<

.PHONY: clean

clean:
	rm -f *~ *.tab.* *.o lex.yy.c kaleidoscope *.output

