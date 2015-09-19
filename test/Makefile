CC=gcc
CFLAGS=-I../../r820t/include  
LDFLAGS= `pkg-config --libs libusb` -L../../../r820t -lr820t -Wl,-rpath=../../../r820t

SOURCE += $(wildcard *.c)
BIN = $(SOURCE:.c=)

make: $(BIN)
	@echo $<

%: %.c
	$(CC) $(LDFLAGS) $(CFLAGS)   $<  -o $@ 

clean:
	rm -f $(BIN)