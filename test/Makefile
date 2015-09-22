CC=gcc
CFLAGS=
#LDFLAGS= `pkg-config --libs libusb` -L../../../r820t -lr820t -Wl,-rpath=../../../r820t
LDFLAGS=`pkg-config --libs libusb` -lrtlsdr
SOURCE = $(wildcard *.c)
OBJECTS = 
BIN = $(SOURCE:.c=)


%: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) $<  -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(BUILD_DIR)$@

make: $(BIN)


clean:
	rm -f $(BIN)