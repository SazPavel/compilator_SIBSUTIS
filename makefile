CC=g++
YYAC=bison
LEX=flex
SOURCE=source/

default: all test clean

all: parser.tab.o ast.o scanner.o main.o
	$(CC) -g -o $@ $+
test: parser.tab.o scanner.o ast.o
	./cxxtest/bin/cxxtestgen --error-printer -o $(SOURCE)test.cpp $(SOURCE)MyTest.h
	$(CC) -c $(SOURCE)test.cpp
	$(CC) -o test test.o parser.tab.o scanner.o ast.o
%.o: $(SOURCE)%.cpp
	$(CC) -c $< -o $@
parser.tab.o: scan
	$(YYAC) -d $(SOURCE)parser.y -o $(SOURCE)parser.tab.c
	$(CC) $(SOURCE)parser.tab.c -c
	$(CC) -c $(SOURCE)scanner.cpp

scan: $(SOURCE)scanner.l
	$(LEX) --header-file=$(SOURCE)scanner.h --outfile=$(SOURCE)scanner.cpp $(SOURCE)scanner.l

clean:
	@rm -f *.o $(SOURCE)parser.tab.* $(SOURCE)scanner.cpp $(SOURCE)test.cpp $(SOURCE)scanner.h
