#include "util.h"

// takes in a value in KiB and outputs a string scaled with the suffix.
// messy temp solution, use a switch statement in the future maybe?
char *format_memory(double input) {
	static char output[20];
	char suffix;
	float value;
	int prec = 0;

	if (input < 1024) {
		suffix = 'K';
		value = input;
	} else if (input < 1048576) {
		suffix = 'M';
		value = input / 1024;
	} else if (input < 1073741824) {
		suffix = 'G';
		value = input / 1048576;
	} else {
		suffix = 'T';
		value = input / 1073741824;
	}

	if (value < 10) {
		prec = 2;
	} else if (value < 100) {
		prec = 1;
	}

#ifdef SMALLSCREEN
	sprintf(output, "%.*f%c", prec, value, suffix);
#else
	sprintf(output, "%.*f %c", prec, value, suffix);
#endif

	return output;
}

// generic white text. The json format is annoying.
void white_text(char* text, char* output, size_t output_size) {
	snprintf(output, output_size, "{\"full_text\":\"%s\",\"separator_block_width\":0}", text);
}

// generic red (error) text.
void error_text(char* text, char* output, size_t output_size) {
	snprintf(output, output_size, "{\"full_text\":\"%s\",\"color\":\"#ff0000\",\"separator_block_width\":0}", text);
}

// text with any color
void color_text(char* text, char* color, char* output, size_t output_size) {
	snprintf(output, output_size, "{\"full_text\":\"%s\",\"color\":\"%s\",\"separator_block_width\":0}", text, color);
}

// text formatted with pango markup
void pango_text(char* text, char* output, size_t output_size) {
	snprintf(output, output_size, "{\"full_text\":\"%s\",\"markup\":\"pango\",\"separator_block_width\":0}", text);
}

// convert raw text to json escaped text
void json_escape(char* in, char* out) {
	while (*in != '\0') {
		switch (*in) {
		case '"':
			*(out++) = '\\';
			*out = '"';
			break;
		case '\\':
			*(out++) = '\\';
			*out = '\\';
			break;
		case '/':
			*(out++) = '\\';
			*out = '/';
			break;
		case '\b':
			*(out++) = '\\';
			*out = 'b';
			break;
		case '\f':
			*(out++) = '\\';
			*out = 'f';
			break;
		case '\n':
			*(out++) = '\\';
			*out = 'n';
			break;
		case '\r':
			*(out++) = '\\';
			*out = 'r';
			break;
		case '\t':
			*(out++) = '\\';
			*out = 't';
			break;
		default:
			*out = *in;
			break;
		}
		out++;
		in++;
	}
	*out = '\0';
}
