TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	-./$(TARGET) -f ./mynewdb.db -n 
	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,111"
	./$(TARGET) -f ./mynewdb.db -a "John Doe,456 Elm St,40"
	./$(TARGET) -f ./mynewdb.db -a "A,B,0"
	./$(TARGET) -f ./mynewdb.db -a "AAAAAAAAAAAAAAAAAAAAAAAAAA,BBBBBBBBBBBBBBBB,999"

	# Empty or whitespace-only
	./$(TARGET) -f ./mynewdb.db -a ""
	./$(TARGET) -f ./mynewdb.db -a " "
	./$(TARGET) -f ./mynewdb.db -a "   "
	./$(TARGET) -f ./mynewdb.db -a "\t"

	# Missing fields (too few commas)
	./$(TARGET) -f ./mynewdb.db -a "John"
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street"
	./$(TARGET) -f ./mynewdb.db -a ",123 Street,50"
	./$(TARGET) -f ./mynewdb.db -a "John,,50"
	./$(TARGET) -f ./mynewdb.db -a ",,50"
	./$(TARGET) -f ./mynewdb.db -a ",,"

	# Extra fields (too many commas)
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street,40,Extra"
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street,40,Extra,More"
	./$(TARGET) -f ./mynewdb.db -a ",,,," 

	# Only whitespace in fields
	./$(TARGET) -f ./mynewdb.db -a " , , "
	./$(TARGET) -f ./mynewdb.db -a "John, ,50"
	./$(TARGET) -f ./mynewdb.db -a " ,123 Street,50"
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street, "

	# Malformed numeric field
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street,abc"
	./$(TARGET) -f ./mynewdb.db -a "John,123 Street,-10"

	# Overly long fields (buffer overflow tests)
	./$(TARGET) -f ./mynewdb.db -a "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA,Addr,10"
	./$(TARGET) -f ./mynewdb.db -a "Name,BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB,10"

	# Leading/trailing whitespace in valid strings
	./$(TARGET) -f ./mynewdb.db -a " John , 123 Street , 40 "
	./$(TARGET) -f ./mynewdb.db -a "Alice, 456 Elm St, 50"

	# Combinations of missing and whitespace
	./$(TARGET) -f ./mynewdb.db -a " , , "
	./$(TARGET) -f ./mynewdb.db -a "John, , "
	./$(TARGET) -f ./mynewdb.db -a " ,123 Street,"

	



default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -fsanitize=leak -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude