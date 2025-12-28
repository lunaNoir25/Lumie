OS := lumie
KERNEL := $(OS).bin
ISO := $(OS).iso

CC := x86_64-elf-gcc
ASM := nasm
LD := x86_64-elf-gcc

CFLAGS := -ffreestanding -O2 -Wall -Wextra -std=gnu11 -m64 -mno-red-zone -mcmodel=kernel -Ikernel -Ikernel/includes
ASMFLAGS := -f elf64
LDFLAGS := -T kernel/linker.ld -ffreestanding -O2 -nostdlib -z max-page-size=0x1000 -Wl,--no-warn-rwx-segments

OBJS := boot.o kernel.o font.o

.PHONY: all clean run iso

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

boot.o: kernel/boot.s
	$(ASM) $(ASMFLAGS) $< -o $@

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

font.o: font.psf
	objcopy -I binary -O elf64-x86-64 -B i386 --rename-section .data=.font font.psf font.o

iso: $(KERNEL)
	rm -rf isodir
	mkdir -p isodir/boot/limine
	mkdir -p isodir/EFI/BOOT
	cp $(KERNEL) isodir/boot/lumie.bin
	cp kernel/limine.conf isodir/boot/limine/
	cp /usr/share/limine/limine-bios.sys isodir/boot/limine/
	cp /usr/share/limine/limine-bios-cd.bin isodir/boot/limine/
	cp /usr/share/limine/limine-uefi-cd.bin isodir/boot/limine/
	cp /usr/share/limine/BOOTX64.EFI isodir/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		isodir -o $(ISO)
	limine bios-install $(ISO)

run: iso
	cp /usr/share/ovmf/x64/OVMF_VARS.4m.fd ./OVMF_VARS.fd
	chmod 644 ./OVMF_VARS.fd
	qemu-system-x86_64 -cdrom $(ISO) \
		-m 256M \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/ovmf/x64/OVMF_CODE.4m.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=./OVMF_VARS.fd

clean:
	rm -rf *.o $(KERNEL) $(ISO) isodir OVMF_VARS.fd