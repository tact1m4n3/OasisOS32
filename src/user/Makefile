BIN := ../../bin
OBJ := $(BIN)/obj

NASM := nasm
CC := x86_64-linux-gnu-gcc
LD := x86_64-linux-gnu-ld

SFLAGS := -f elf
CFLAGS := -c --freestanding -m32
LDFLAGS := -n -nostdlib -m elf_i386 -T link.ld

C_SRC_FILES := $(shell find . -name '*.c')
C_OBJ_FILES := $(patsubst %.c, $(OBJ)/user/%.o, $(C_SRC_FILES))
C_BIN_FILES := $(patsubst %.c, $(BIN)/%, $(C_SRC_FILES))

ENTRY_SRC := entry.s
ENTRY_OBJ := $(OBJ)/user/entry.o

build: $(ENTRY_OBJ) $(C_BIN_FILES)

$(ENTRY_OBJ): $(ENTRY_SRC)
	mkdir -pv $(dir $@)
	$(NASM) $(SFLAGS) $^ -o $@

$(C_OBJ_FILES) : $(OBJ)/user/%.o : $(C_SRC_FILES)
	mkdir -pv $(dir $@)
	$(CC) $(CFLAGS) $(patsubst $(OBJ)/user/%.o, %.c, $@) -o $@

$(C_BIN_FILES) : $(BIN)/% : $(C_OBJ_FILES)
	mkdir -pv $(dir $@)
	$(LD) $(LDFLAGS) $(ENTRY_OBJ) $(patsubst $(BIN)/%, $(OBJ)/user/%.o, $@) -o $@
