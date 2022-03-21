# Build docker envirorment: docker build buildenv -t oasisos
# Run docker envirorment:
# --- Mac OS: docker run --rm -it -v "$(pwd)":/env oasisos
# --- Windows: docker run --rm -it -v "%cd%":/env oasisos

BIN := bin
SYSROOT := sysroot

IMAGE := oasis-os.iso
KERNEL := $(BIN)/oasis-kernel
INITRD := $(BIN)/oasis-initrd

INITRD_FILES := test.txt

all: image

image: $(IMAGE)

kernel:
ifeq ($(shell make -sqC src/kernel || echo 1), 1)
	$(MAKE) -C src/kernel build
endif

initrd: $(INITRD)

$(IMAGE): kernel initrd
	cp $(KERNEL) $(SYSROOT)/boot
	cp $(INITRD) $(SYSROOT)/boot
	grub-mkrescue -o $@ $(SYSROOT)

test.txt:
	echo "Hello world!" > $(BIN)/test.txt

$(INITRD): $(INITRD_FILES)
	mkdir -pv $(dir $@)
	tar cf $@ -C $(dir $@) $(INITRD_FILES)

.PHONY: clean
clean:
	rm -rf $(IMAGE)
	rm -rf $(BIN)
	rm -rf $(SYSROOT)/boot/oasis-kernel
	rm -rf $(SYSROOT)/boot/oasis-initrd
