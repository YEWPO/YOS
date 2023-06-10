# Workspace
WORKDIR = $(abspath .)
BUILDDIR = $(WORKDIR)/build

# Then get the tools' name
TOOLPREFIX = riscv64-linux-gnu-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

# Kernel and user space source codes dir
K = $(WORKDIR)/kernel
U = $(WORKDIR)/user

IMG = $(WORKDIR)/fat32.img

SCRIPTDIR = $(WORKDIR)/script

# clean project not needed files
clean:
	rm -rf $(BUILDDIR)

include $(SCRIPTDIR)/build.mk
include $(SCRIPTDIR)/docs.mk
include $(SCRIPTDIR)/img.mk

.PHONY: clean
