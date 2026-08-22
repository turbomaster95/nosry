.PHONY: vm
vm: vm.c vm.h
	$(CC) $(CFLAGS) -o $@ $<

all: vm

.PHONY: clean
clean:
	rm -rf vm test *.vm *.vmo
