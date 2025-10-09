TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	-./$(TARGET) -f ./mynewdb.db -n 
	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,111"
	./$(TARGET) -f ./mynewdb.db -a "Tyler J., ,333"
	./$(TARGET) -f ./mynewdb.db -a "Cam P.,Test,444"
	./$(TARGET) -f ./mynewdb.db -a " "
	./$(TARGET) -f ./mynewdb.db -a ""
	./$(TARGET) -f ./mynewdb.db -a "dwadawdwa"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -fsanitize=leak -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude