BIN := ../../bin
OBJ := $(BIN)/obj

KERNEL := $(BIN)/oasis-kernel

NASM := nasm
CC := x86_64-linux-gnu-gcc
LD := x86_64-linux-gnu-ld

SFLAGS := -f elf
CFLAGS := -c --freestanding -m32 -D OASIS_DEBUG -Iinclude
LDFLAGS := -n -nostdlib -m elf_i386 -T link.ld

S_SRC_FILES := $(shell find . -name '*.s')
S_OBJ_FILES := $(patsubst %.s, $(OBJ)/kernel/%.o, $(S_SRC_FILES))

C_SRC_FILES := $(shell find . -name '*.c')
C_OBJ_FILES := $(patsubst %.c, $(OBJ)/kernel/%.o, $(C_SRC_FILES))

ALL_OBJ_FILES := $(S_OBJ_FILES) $(C_OBJ_FILES)

build: $(KERNEL)

$(S_OBJ_FILES) : $(OBJ)/kernel/%.o : $(S_SRC_FILES)
	mkdir -pv $(dir $@)
	$(NASM) $(SFLAGS) $(patsubst $(OBJ)/kernel/%.o, %.s, $@) -o $@

$(C_OBJ_FILES) : $(OBJ)/kernel/%.o : $(C_SRC_FILES)
	mkdir -pv $(dir $@)
	$(CC) $(CFLAGS) $(CDEFINES) $(patsubst $(OBJ)/kernel/%.o, %.c, $@) -o $@

$(KERNEL): $(ALL_OBJ_FILES)
	mkdir -pv $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^
