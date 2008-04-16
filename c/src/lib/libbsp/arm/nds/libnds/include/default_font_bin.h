#ifndef default_font_bin_h_
#define default_font_bin_h_

extern void *default_font_bin;
static void *default_font_bin_ptr = &default_font_bin;

#define default_font_bin default_font_bin_ptr

#endif
