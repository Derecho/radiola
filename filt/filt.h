#ifndef __RADIOLA_FILT_H
#define __RADIOLA_FILT_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//fifth order filter from rtlsdr
void f_5th(int16_t *data, int length, int16_t *hist);

#endif