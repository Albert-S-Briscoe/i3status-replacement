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
#if BATTERIES > 0
	char bat_str[BATTERIES][BAT_STR_SIZE];
#endif
	char fs1_str[256];
	char fs2_str[256];
	char mem_str[256];

	enum {
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
				get_batteries(bat_str, 256); // bat_str is a char[][]
#endif
				get_fs("/", fs1_str, 256);
				get_fs("/home", fs2_str, 256);
				get_mem_info(mem_str, 256);
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
				fprintf(stdout, "%s,\n", bat_str[x]);
			}
#endif
			fprintf(stdout, "%s,\n", fs1_str);
			fprintf(stdout, "%s,\n", fs2_str);
			fprintf(stdout, "%s,\n", mem_str);
			fprintf(stdout, "%s\n", json_object_to_json_string(json_object_array_get_idx(status_json, time_idx)));
			fprintf(stdout, "],\n\n");




			fflush(stdout);

			sleep(1);
		}
	}
	status_json = json_object_new_array();

	exit (EXIT_SUCCESS);
}
