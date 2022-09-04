

#ifndef UTIL_H
#define UTIL_H

#include "shared.h"

#define WHITE_TEXT(a) "{\"full_text\":\"" a "\",\"separator_block_width\":0}"
#define ERROR_TEXT(a) "{\"full_text\":\"" a "\",\"color\":\"#ff0000\",\"separator_block_width\":0}"

extern void white_text(char* text, char* output, size_t output_size);
extern void error_text(char* text, char* output, size_t output_size);
extern void color_text(char* text, char* color, char* output, size_t output_size);
extern void pango_text(char* text, char* output, size_t output_size);

extern char *format_memory(double input);

extern void json_escape(char* in, char* out);

#endif
