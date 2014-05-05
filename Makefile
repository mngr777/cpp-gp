CC = g++
BIN_DIR = bin
OUT = bin/app
OUT_SYMLINK = app
OBJ_DIR = obj
INC_DIR = include
INCLUDE = -iquote $(INC_DIR)
SRC_DIR = src
DEP_DIR = .dep
DEBUG = -g
DEFS =
CFLAGS = -Wall -std=c++11 $(DEBUG) $(DEFS)
LFLAGS =
LIBS =

MAKEDEPEND = gcc -MM -std=c++11 $(INCLUDE)
_OBJS = main.o expr.o indiv.o func.o run.o tree.o
SUBDIRS =
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
#SRCS = $(patsubst %.o,$(SRC_DIR)/%.cpp,$(_OBJS))
DEPS = $(patsubst %.o,$(DEP_DIR)/%.d,$(_OBJS))

.PHONY: all
all: directories $(OUT)

.PHONY: directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)
ifdef SUBDIRS
	@mkdir -p $(foreach subdir,$(SUBDIRS),$(OBJ_DIR)/$(subdir))
	@mkdir -p $(foreach subdir,$(SUBDIRS),$(DEP_DIR)/$(subdir))
endif

$(OUT): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)
ifdef OUT_SYMLINK
	ln -sf $(OUT) $(OUT_SYMLINK)
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MAKEDEPEND) -MF $(DEP_DIR)/$*.d -MT $(OBJ_DIR)/$*.o -MP $(SRC_DIR)/$*.cpp $(DEFS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

-include $(DEPS)

.PHONY: run
run: all
	$(OUT)

.PHONY: clean
clean:
	rm -f $(OUT)
ifdef OUT_SYMLINK
	rm -f $(OUT_SYMLINK)
endif
	rm -rf $(OBJ_DIR)/* $(DEP_DIR)/*

.PHONY: cleanbackups
cleanbackups:
	find . -name '*~' -delete

.PHONY: cleanall
cleanall: clean cleanbackups
