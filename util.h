

#ifndef UTIL_H
#define UTIL_H

#include "shared.h"

#define WHITE_TEXT(a) "{\"full_text\":\"" a "\",\"separator_block_width\":0}"
#define ERROR_TEXT(a) "{\"full_text\":\"" a "\",\"color\":\"#ff0000\",\"separator_block_width\":0}"


extern json_object *white_text_old(char *text);
extern void white_text(char* text, char* output, size_t output_size);

extern json_object *error_text_old(char *text);
extern void error_text(char* text, char* output, size_t output_size);

extern json_object *color_text_old(char *text, char *color);
extern void color_text(char* text, char* color, char* output, size_t output_size);

extern json_object *pango_text_old(char *text);
extern void pango_text(char* text, char* output, size_t output_size);

extern char *format_memory(double input);

#endif
