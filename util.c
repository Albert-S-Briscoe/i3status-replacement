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
	sprintf(output, "%.*f%c", value, prec, suffix);
#else
	sprintf(output, "%.*f %c", value, prec, suffix);
#endif
	
	return output;
}

// generic white text. The json format is annoying.
json_object *white_text(char *text) {
	json_object *text_json = json_object_new_object();
	json_object_object_add(text_json, "full_text", json_object_new_string(text));
//	json_object_object_add(text_json, "color", json_object_new_string("#ffffff"));
//	json_object_object_add(text_json, "separator", json_object_new_boolean(0));
	json_object_object_add(text_json, "separator_block_width", json_object_new_int(0));
	
	return text_json;
}

// generic red (error) text.
json_object *error_text(char *text) {
	json_object *text_json = json_object_new_object();
	json_object_object_add(text_json, "full_text", json_object_new_string(text));
	json_object_object_add(text_json, "color", json_object_new_string("#ff0000"));
//	json_object_object_add(text_json, "separator", json_object_new_boolean(0));
	json_object_object_add(text_json, "separator_block_width", json_object_new_int(0));
	
	return text_json;
}

// text with any color
json_object *color_text(char *text, char *color) {
	json_object *text_json = json_object_new_object();
	json_object_object_add(text_json, "full_text", json_object_new_string(text));
	json_object_object_add(text_json, "color", json_object_new_string(color));
//	json_object_object_add(text_json, "separator", json_object_new_boolean(0));
	json_object_object_add(text_json, "separator_block_width", json_object_new_int(0));
	
	return text_json;
}

// text formatted with pango markup
json_object *pango_text(char *text) {
	json_object *text_json = json_object_new_object();
	json_object_object_add(text_json, "full_text", json_object_new_string(text));
//	json_object_object_add(text_json, "color", json_object_new_string("#ffffff"));
//	json_object_object_add(text_json, "separator", json_object_new_boolean(0));
	json_object_object_add(text_json, "separator_block_width", json_object_new_int(0));
	json_object_object_add(text_json, "markup", json_object_new_string("pango"));
	
	return text_json;
}
