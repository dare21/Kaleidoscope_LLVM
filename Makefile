CPPFLAGS=$(shell llvm-config-12 --cxxflags)
LDFLAGS=$(shell llvm-config-12 --ldflags --libs)

kaleidoscope: lex.yy.o parser.tab.o ast.o
	clang++-12 $(LDFLAGS) -Wall -o $@ $^
lex.yy.o: lex.yy.c parser.tab.hpp ast.hpp
	clang++-12 $(CPPFLAGS) -Wno-deprecated -Wall -c -o $@ $<
lex.yy.c: lexer.lex
	flex $<
parser.tab.o: parser.tab.cpp parser.tab.hpp ast.hpp
	clang++-12 $(CPPFLAGS) -Wall -c -o $@ $<
parser.tab.cpp parser.tab.hpp: parser.ypp
	bison -dv $<
ast.o: ast.cpp ast.hpp
	clang++-12 $(CPPFLAGS) -Wall -c -o $@ $<

.PHONY: clean

clean:
	rm -f *~ *.tab.* *.o lex.yy.c kaleidoscope *.output
