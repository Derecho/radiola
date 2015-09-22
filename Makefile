PROJECT=radiola
CC=gcc
CFLAGS=
LDFLAGS=
CLEAN=

BUILD_DIR=build/

SOURCES= 
OBJECTS= 
OBJECTS_FINAL=

include draw/make.mk
include hw/make.mk

make: $(OBJECTS) 
	$(CC) $(OBJECTS_FINAL) $(PROJECT).c -o $(PROJECT) $(LDFLAGS)
	ld -r $(OBJECTS_FINAL) -o $(PROJECT).o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(BUILD_DIR)$@

clean:
	rm -f $(PROJECT)
	rm -f $(OBJECTS_FINAL)
