SRC_DIR := src
INCLUDE_DIR := include
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR := obj/release
DBG_OBJ_DIR := obj/debug
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DBG_OBJS := $(patsubst $(SRC_DIR)/%.c, $(DBG_OBJ_DIR)/%.o, $(SRCS))
CFLAGS := -Iinclude -Wall -Werror

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@gcc $(CFLAGS) -c $< -o $@

$(DBG_OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(DBG_OBJ_DIR)
	@gcc -DDEBUG -g $(CFLAGS) -c $< -o $@

calc: $(OBJS)
	gcc $(CFLAGS) $^ -o $@


debug: $(DBG_OBJS)
	gcc -DDEBUG -g $(CFLAGS) $^ -o $@

clean:
	rm -f calc debug $(OBJ_DIR)/*.o $(DBG_OBJ_DIR)/*.o
