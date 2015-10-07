DIR_HW = hw/
SOURCES_HW += hw/aud.c hw/hw.c hw/hw_eeprom.c hw/sdr.c
OBJECTS_HW += $(SOURCES_HW:.c=.o)
LDFLAGS += -lrtlsdr -lasound


OBJECTS_DIR_HW += $(subst $(DIR_HW),$(BUILD_DIR)$(DIR_HW),$(OBJECTS_HW))

OBJECTS += $(OBJECTS_HW)
OBJECTS_FINAL += $(OBJECTS_DIR_HW)