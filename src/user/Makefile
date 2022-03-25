BIN := ../../bin
OBJ := $(BIN)/obj

NASM := nasm
CC := x86_64-linux-gnu-gcc
LD := x86_64-linux-gnu-ld

SFLAGS := -f elf
CFLAGS := -c --freestanding -m32 -I ../libc/include
LDFLAGS := -n -nostdlib -m elf_i386 -T link.ld
LDLIBS := -L $(BIN) -l:oasis-libc.a

C_SRC_FILES := $(shell find . -name '*.c')
C_OBJ_FILES := $(patsubst %.c, $(OBJ)/user/%.o, $(C_SRC_FILES))
C_BIN_FILES := $(patsubst %.c, $(BIN)/%, $(C_SRC_FILES))

build: $(C_BIN_FILES)

$(C_OBJ_FILES) : $(OBJ)/user/%.o : $(C_SRC_FILES)
	mkdir -pv $(dir $@)
	$(CC) $(CFLAGS) $(patsubst $(OBJ)/user/%.o, %.c, $@) -o $@

$(C_BIN_FILES) : $(BIN)/% : $(C_OBJ_FILES)
	mkdir -pv $(dir $@)
	$(LD) $(LDFLAGS) $(patsubst $(BIN)/%, $(OBJ)/user/%.o, $@) -o $@ $(LDLIBS)
