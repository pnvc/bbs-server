FOREXECUTE = exe

CC = gcc
CFLAGS = -Wall -g -ansi -pedantic

SOURCE_DIR = ./src/
HEADERS_DIR = ./headers/
OBJECTS_DIR = ./obj/

HDS = $(wildcard $(HEADERS_DIR)*.h)
SRC = $(wildcard $(SOURCE_DIR)*.c)
OBJ = $(patsubst $(SOURCE_DIR)%.c, $(OBJECTS_DIR)%.o, $(SRC))

$(OBJECTS_DIR)%.o : $(SOURCE_DIR)%.c $(HEADERS_DIR)%.h $(OBJECTS_DIR)
	@echo "CREATING OF $@ OBJECT FILE"
	@$(CC) $(CFLAGS) -c $< -o $@

compilation : $(OBJ)
	@echo "------------------------------------------------------------"
	@echo "MODULES LINKING, EXECUTE FILE LOOKS LIKE $(FOREXECUTE)"
	@$(CC) $^ -o $(FOREXECUTE)
	@echo "------------------------------------------------------------"

$(OBJECTS_DIR) :
	@echo "CREATED OBJECT DIRECTORY AS $(OBJECTS_DIR)"
	@mkdir $(OBJECTS_DIR)

clean :
	@rm -R $(OBJECTS_DIR)
