PROJECT=radiola
CC=gcc
CFLAGS=
LDFLAGS=

BUILD_DIR=build/

SOURCES= 
OBJECTS= 
OBJECTS_FINAL=



include hw/make.mk

make: $(OBJECTS) 
	$(CC) $(OBJECTS_FINAL) $(PROJECT).c -o $(PROJECT) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(BUILD_DIR)$@

clean:
	rm -f $(PROJECT)
