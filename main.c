#include <unistd.h>			// sleep()

#include "main.h"

int main (int argc, char *argv[]) {
//	int status_length;

	enum {
		ssh_idx,
#ifdef NETINTERFACE
		net1_idx,
		sep1_idx,
#ifdef NETINTERFACE2
		net2_idx,
		sep2_idx,
#endif
#endif
#if BATTERIES > 0
		bat1_idx,
		sep3_idx,
#if BATTERIES > 1
		bat2_idx,
		sep4_idx,
#endif
#endif
		fs1_idx,
		sep5_idx,
		fs2_idx,
		sep6_idx,
		mem_idx,
		time_idx,
		total_idx // array total, not an actual json object index
	};

	json_object *separator = white_text(" | ");
	#define PLACEHOLDER white_text("")

#ifdef BATTERYBAR
	#define BATTERYSEP white_text(") | ")
#else
	#define BATTERYSEP separator
#endif

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


	// this mess basically just sets up the array and adds the seperator objects
	json_object_array_add(status_json, PLACEHOLDER);
#ifdef NETINTERFACE
	json_object_array_add(status_json, PLACEHOLDER);	// 1st network interface
	json_object_array_add(status_json, separator);
#ifdef NETINTERFACE2
	json_object_array_add(status_json, PLACEHOLDER);	// 2nd network interface
	json_object_array_add(status_json, separator);
#endif
#endif
#if BATTERIES > 0
	json_object_array_add(status_json, PLACEHOLDER);	// BAT0
	json_object_array_add(status_json, BATTERYSEP);
#if BATTERIES > 1
	json_object_array_add(status_json, PLACEHOLDER);	// BAT1
	json_object_array_add(status_json, BATTERYSEP);
#endif
#endif
	json_object_array_add(status_json, PLACEHOLDER);	// get_fs("/")
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, PLACEHOLDER);	// get_fs("/home")
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, PLACEHOLDER);	// get_mem_info()	swap and warning on full memory/swap usage, built in seperators if not shortened
	json_object_array_add(status_json, PLACEHOLDER);		// get_time()	built in seperators

	if (json_object_array_length(status_json) != total_idx)
		fprintf(stderr, "error: array length does not match enum length");

//	printf("%s,\n", json_object_to_json_string(status_json));
//	fflush(stdout);

	while (1) {
		// every 30 seconds
		json_object_array_put_idx(status_json, ssh_idx, get_ssh());
#ifdef NETINTERFACE
		json_object_array_put_idx(status_json, net1_idx, get_ip(NETINTERFACE));
#ifdef NETINTERFACE2
		json_object_array_put_idx(status_json, net2_idx, get_ip(NETINTERFACE2));
#endif
#endif
		for (int i = 0; i < 30; i++) {
			// every 10 seconds
			if (i % 10 == 0) {
#if BATTERIES > 0
				json_object_array_put_idx(status_json, bat1_idx, get_battery("BAT0"));
#if BATTERIES > 1
				json_object_array_put_idx(status_json, bat2_idx, get_battery("BAT1"));
#endif
#endif
				json_object_array_put_idx(status_json, fs1_idx, get_fs("/"));
				json_object_array_put_idx(status_json, fs2_idx, get_fs("/home"));
				json_object_array_put_idx(status_json, mem_idx, get_mem_info());
			}
			// every second
			json_object_array_put_idx(status_json, time_idx, get_time());

//			fprintf(stdout, "%s,\n", json_object_to_json_string_ext(status_json, JSON_C_TO_STRING_PRETTY));
			fprintf(stdout, "%s,\n", json_object_to_json_string(status_json));
			fflush(stdout);

			sleep(1);
		}
	}
	status_json = json_object_new_array();

	exit (EXIT_SUCCESS);
}
