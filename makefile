#Flags
CC = gcc
CFLAGS = -Wall -std=c11 -g

#rules
#main: 
#	$(CC) $(CFLAGS) StructListDemo.c LinkedListAPI.c -o main -g
	
list:
	$(CC) $(CFLAGS) -c src/LinkedListAPI.c -Iinclude -o bin/LinkedListAPI.o
	ar rc bin/LinkedListAPI.a bin/LinkedListAPI.o
	ranlib bin/LinkedListAPI.a

parser:
	$(CC) $(CFLAGS) -c src/VCardParser.c -Iinclude -o bin/VCardParser.o
	ar rc bin/VCardParser.a bin/VCardParser.o
	ranlib bin/VCardParser.a

clean:
	rm -f bin/*.o bin/*.a

testBuild:
	$(CC) $(CFLAGS) src/testAPI.c bin/VCardParser.a bin/LinkedListAPI.a -Iinclude -o bin/testAPI -g

test:
	cd bin; \
	  valgrind --leak-check=full --track-origins=yes ./testAPI ../test_files/testCard.vcf
	  cd ..;

all: list parser testBuild test


