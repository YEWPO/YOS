# make fat32 image
img:
	dd if=/dev/zero of=$(IMG) bs=1M count=128
	mkfs.fat -F 32 $(IMG)

.PHONY: img
