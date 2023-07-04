# Name: Tyler Ykema
# ID#: 1062564
# Date: Febuary 15, 2019
# Email: tykema@uoguelph.ca

UNAME := $(shell uname)
CFLAGS = -Wall -g -std=c11
BIN = bin/
CC = gcc
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2

ifeq ($(UNAME), Linux)
	INC_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	INC_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif

# Standard common makefile
parser: $(BIN)libsvgparse.so

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o ../libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(INC_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o


xmlParser: LinkedListAPI.o $(SRC)SVGParser.c $(INC)SVGParser.h
	$(CC) $(CFLAGS) -I$(INC_PATH) -I$(INC) src/SVGParser.c -lxml2 -o svgParser.o LinkedListAPI.o -o Testrun -lm

LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -I$(INC) -c $(SRC)LinkedListAPI.c -o LinkedListAPI.o


run: xmlParser
	./Testrun rect.svg

intensememtest: xmlParser
	valgrind --leak-check=full -v --track-origins=yes --show-leak-kinds=all ./Testrun Emoji_poo.svg

memtest: xmlParser
	valgrind --leak-check=full -v --show-leak-kinds=all ./Testrun Emoji_poo.svg

clean:
	rm $(BIN)*.o $(BIN)*.so *.o

exportPath:
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
