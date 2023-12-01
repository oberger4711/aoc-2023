TARGET ?= solve
SRC_DIRS ?= ./
#SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
#CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(TARGET): $(SRCS)
	g++ $(SRCS) -o $@

.PHONY: clean
clean:
	$(RM) $(TARGET)