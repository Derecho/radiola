DIR_DRAW = draw/
SOURCES_DRAW += draw/glui.c draw/tui.c draw/ui.c
OBJECTS_DRAW += $(SOURCES_DRAW:.c=.o)
LDFLAGS += -lGL `sdl2-config --cflags --libs`


OBJECTS_DIR_DRAW += $(subst $(DIR_DRAW),$(BUILD_DIR)$(DIR_DRAW),$(OBJECTS_DRAW))

OBJECTS += $(OBJECTS_DRAW)
OBJECTS_FINAL += $(OBJECTS_DIR_DRAW)