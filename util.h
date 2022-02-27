

#ifndef UTIL_H
#define UTIL_H

#include "shared.h"

extern json_object *white_text(char *text);
extern json_object *error_text(char *text);
extern json_object *color_text(char *text, char *color);
extern char *format_memory(double input);

#endif
