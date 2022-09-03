#include <unistd.h>	// sleep()

#include "main.h"


#define PLACEHOLDER white_text_old("")

int main (int argc, char *argv[]) {
	json_object *tempobject;
	char ssh_str[256];
	int ssh_present;
#ifdef NETINTERFACE
	char net1_str[256];
#ifdef NETINTERFACE2
	char net2_str[256];
#endif
#endif
	enum {
#if BATTERIES > 0
		bat_idx,
		tmp_idx = bat_idx + (BATTERIES * OBJPERBAT) - 1,
#endif
		fs1_idx,
		fs2_idx,
		mem_idx,
		time_idx,
		total_idx // array total, not an actual json object index
	};

	// start infinite json
#ifdef CLICKEVENTS
	printf ("{\"version\":1,\"click_events\":true}\n[\n[],\n");
#else
	printf ("{\"version\":1}\n[\n[],\n");
#endif
	fflush(stdout);

	json_object *status_json = json_object_new_array();
	if (!status_json)
		fprintf(stderr, "error");


	// this mess basically just sets up the array and adds the separator objects
	for (int i = 0; i < BATTERIES; i++) {
		json_object_array_add(status_json, PLACEHOLDER);
#ifdef BATTERYBAR
		json_object_array_add(status_json, white_text_old(") | "));
#endif
	}
	json_object_array_add(status_json, PLACEHOLDER);	// get_fs("/")
	json_object_array_add(status_json, PLACEHOLDER);	// get_fs("/home")
	json_object_array_add(status_json, PLACEHOLDER);	// get_mem_info()	swap and warning on full memory/swap usage, built in separators if not shortened
	json_object_array_add(status_json, PLACEHOLDER);	// get_time()	built in separators

	if (json_object_array_length(status_json) != total_idx)
		fprintf(stderr, "error: array length does not match enum length");

	while (1) {
		// every 30 seconds
		ssh_present = get_ssh(ssh_str, 256);
#ifdef NETINTERFACE
		get_ip(NETINTERFACE, net1_str, 256);
#ifdef NETINTERFACE2
		get_ip(NETINTERFACE2, net2_str, 256);
#endif
#endif
		for (int i = 0; i < 30; i++) {
			// every 10 seconds
			if (i % 10 == 0) {
#if BATTERIES > 0
				tempobject = get_battery();
				for (int x = 0; x < BATTERIES; x++) {
					json_object_array_put_idx(status_json, bat_idx + i * OBJPERBAT, json_object_array_get_idx(tempobject, i));
				}
#endif
				json_object_array_put_idx(status_json, fs1_idx, get_fs("/"));
				json_object_array_put_idx(status_json, fs2_idx, get_fs("/home"));
				json_object_array_put_idx(status_json, mem_idx, get_mem_info());
			}
			// every second
			json_object_array_put_idx(status_json, time_idx, get_time());

//			fprintf(stdout, "%s,\n", json_object_to_json_string_ext(status_json, JSON_C_TO_STRING_PRETTY));
//			fprintf(stdout, "%s,\n", json_object_to_json_string(status_json));





			fprintf(stdout, "[\n");
			if (ssh_present > 0)
				fprintf(stdout, "%s,\n", ssh_str);
#ifdef NETINTERFACE
			fprintf(stdout, "%s,\n", net1_str);
#ifdef NETINTERFACE2
			fprintf(stdout, "%s,\n", net2_str);
#endif
#endif
#if BATTERIES > 0
			for (int x = 0; x < BATTERIES; x++) {
				fprintf(stdout, "%s,\n", json_object_to_json_string(json_object_array_get_idx(status_json, bat_idx + x * OBJPERBAT)));
#ifdef BATTERYBAR
				fprintf(stdout, WHITE_TEXT(") | ") ",\n");
#endif
			}
#endif
			fprintf(stdout, "%s,\n", json_object_to_json_string(json_object_array_get_idx(status_json, fs1_idx)));
			fprintf(stdout, "%s,\n", json_object_to_json_string(json_object_array_get_idx(status_json, fs2_idx)));
			fprintf(stdout, "%s,\n", json_object_to_json_string(json_object_array_get_idx(status_json, mem_idx)));
			fprintf(stdout, "%s\n", json_object_to_json_string(json_object_array_get_idx(status_json, time_idx)));
			fprintf(stdout, "],\n\n");




			fflush(stdout);

			sleep(1);
		}
	}
	status_json = json_object_new_array();

	exit (EXIT_SUCCESS);
}
