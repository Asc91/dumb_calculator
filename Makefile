SRC_DIR := src
INCLUDE_DIR := include
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR := obj/release
DBG_OBJ_DIR := obj/debug
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) $(OBJ_DIR)/ui.o
DBG_OBJS := $(patsubst $(SRC_DIR)/%.c, $(DBG_OBJ_DIR)/%.o, $(SRCS)) $(DBG_OBJ_DIR)/ui.o
CFLAGS := -Iinclude -Wall -Werror
GTKC= $(shell pkg-config --cflags gtk+-3.0)
GTKL= $(shell pkg-config --libs gtk+-3.0)
UI = $(SRC_DIR)/ui
UI_RESOURCES = $(UI)/ui.c

$(UI_RESOURCES): $(UI)/dumb-calc.gresource.xml $(UI)/calc-gtk.ui
	@glib-compile-resources --target=$@ --sourcedir=$(UI) --generate-source $<

$(OBJ_DIR)/ui.o: $(UI_RESOURCES)
	@mkdir -p $(OBJ_DIR)
	@gcc $(CFLAGS) $(GTKC) -c $< -o $@

$(DBG_OBJ_DIR)/ui.o: $(UI_RESOURCES)
	@mkdir -p $(DBG_OBJ_DIR)
	@gcc -DDEBUG -g $(CFLAGS) $(GTKC) -c $< -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@gcc $(CFLAGS) $(GTKC) -c $< -o $@

$(DBG_OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(DBG_OBJ_DIR)
	@gcc -DDEBUG -g $(CFLAGS) $(GTKC) -c $< -o $@

calc: $(OBJS)
	gcc $(GTKL) $^ -o $@


debug: $(DBG_OBJS)
	gcc -DDEBUG -g $(GTKL) $^ -o $@

clean:
	rm -f calc debug $(UI_RESOURCES) $(OBJ_DIR)/*.o $(DBG_OBJ_DIR)/*.o
