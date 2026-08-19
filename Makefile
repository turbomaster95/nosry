
.PHONY: vm
vm: vm.c vm.h
	$(CC) $(CFLAGS) -o $@ $<

all: vm
