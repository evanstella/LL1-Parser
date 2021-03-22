CC = g++
CFLAGS  = -std=c++11 -Wall -Werror 

$(shell mkdir build)

default: parser


parser:  build/main.o build/grammar.o build/lexer.o build/parser.o build/rule.o build/symbol.o 
	$(CC) $(CFLAGS) -o parser build/main.o build/grammar.o build/lexer.o build/parser.o build/rule.o build/symbol.o 

build/main.o:  src/main.cc
	$(CC) $(CFLAGS) -c src/main.cc -o build/main.o

build/grammar.o:  src/grammar.cc
	$(CC) $(CFLAGS) -c src/grammar.cc -o build/grammar.o

build/lexer.o:  src/lexer.cc
	$(CC) $(CFLAGS) -c src/lexer.cc -o build/lexer.o

build/parser.o:  src/parser.cc
	$(CC) $(CFLAGS) -c src/parser.cc -o build/parser.o

build/rule.o:  src/rule.cc
	$(CC) $(CFLAGS) -c src/rule.cc -o build/rule.o

build/symbol.o:  src/symbol.cc
	$(CC) $(CFLAGS) -c src/symbol.cc -o build/symbol.o


clean: 
	$(RM) parser build/*.o ./parser