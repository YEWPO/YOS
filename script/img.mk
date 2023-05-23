# make fat32 image
IMG = fat32.img

img:
	dd if=/dev/zero of=$(IMG) bs=1M count=128
	mkfs.fat -F 32 $(IMG)

.PHONY: img
