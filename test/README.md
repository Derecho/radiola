Here is list programm that used as test to verify that stuff works or
looks like it works.

**sdr_fm**- fm demod, possible to listen radio stations
**ui_gl_waterfall** - draw waterfall graphial version
**ui_tui_waterfall** - waterfall drawing terminal version

## sdr_fm

Usage:
	./sdr_fm -f 101100000 | aplay -r 100k -f S16_LE

## ui_gl_waterfall

	./ui_gl_waterfall -f 101100000

## ui_tui_waterfall

Now seats and uses at 

> #define CENTER_FREQ  445500000

	./ui_tui_waterfall