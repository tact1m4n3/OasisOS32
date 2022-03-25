BIN := ../../bin
OBJ := $(BIN)/obj

LIBC := $(BIN)/oasis-libc.a

NASM := nasm
CC := x86_64-linux-gnu-gcc
AR := x86_64-linux-gnu-ar

SFLAGS := -f elf
CFLAGS := -c --freestanding -m32 -I include
ARFLAGS := -rcs

S_SRC_FILES := $(shell find . -name '*.s')
S_OBJ_FILES := $(patsubst %.s, $(OBJ)/libc/%.o, $(S_SRC_FILES))

C_SRC_FILES := $(shell find . -name '*.c')
C_OBJ_FILES := $(patsubst %.c, $(OBJ)/libc/%.o, $(C_SRC_FILES))

ALL_OBJ_FILES := $(S_OBJ_FILES) $(C_OBJ_FILES)

build: $(LIBC)

$(S_OBJ_FILES) : $(OBJ)/libc/%.o : $(S_SRC_FILES)
	mkdir -pv $(dir $@)
	$(NASM) $(SFLAGS) $(patsubst $(OBJ)/libc/%.o, %.s, $@) -o $@

$(C_OBJ_FILES) : $(OBJ)/libc/%.o : $(C_SRC_FILES)
	mkdir -pv $(dir $@)
	$(CC) $(CFLAGS) $(patsubst $(OBJ)/libc/%.o, %.c, $@) -o $@

$(LIBC): $(ALL_OBJ_FILES)
	mkdir -pv $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
