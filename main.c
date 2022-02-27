#include <unistd.h>			// sleep()

#include "main.h"

int main (int argc, char *argv[]) {
	int status_length;
	json_object *sshblocks[2];
	
#ifdef BATTERYBAR
	json_object *batterybar[2];
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
	
	json_object *separator = white_text(" | ");
	json_object *placeholder = white_text("");
	
	get_ssh(sshblocks);
	json_object_array_add(status_json, sshblocks[0]);
	json_object_array_add(status_json, sshblocks[1]);
#ifdef NETINTERFACE
	json_object_array_add(status_json, get_ip(NETINTERFACE));
	json_object_array_add(status_json, separator);
#endif
#ifdef NETINTERFACE2
	json_object_array_add(status_json, get_ip(NETINTERFACE2));
	json_object_array_add(status_json, separator);
#endif
#if BATTERIES > 0
	json_object_array_add(status_json, placeholder); // BAT0
	json_object_array_add(status_json, BATTERYSEP);
#endif
#if BATTERIES > 1
	json_object_array_add(status_json, placeholder); // BAT1
	json_object_array_add(status_json, BATTERYSEP);
#endif
	json_object_array_add(status_json, get_fs("/home"));
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, get_fs("/"));
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, get_mem_info()); // swap and warning on full memory/swap usage, built in seperators if not shortened
	json_object_array_add(status_json, get_time()); // built in seperators
	
	// update length every time it's changed. Not sure exactly how that'll work at the moment.
	status_length = json_object_array_length(status_json);
	
//	fprintf(stdout, "%s,\n", json_object_to_json_string(status_json));
//	fflush(stdout);
	
	while (1) {
		// every 30 seconds
		get_ssh(sshblocks);
		json_object_array_put_idx(status_json, 0, sshblocks[0]);
		json_object_array_put_idx(status_json, 1, sshblocks[1]);
#ifdef NETINTERFACE
		json_object_array_put_idx(status_json, 2, get_ip(NETINTERFACE));
#endif
#ifdef NETINTERFACE2
		json_object_array_put_idx(status_json, 2 + IPOFFSET, get_ip(NETINTERFACE2));
#endif
		
		for (int i = 0; i < 30; i++) {
			// every 10 seconds
			if (i % 10 == 0) {
				
#if BATTERIES > 0
				json_object_array_put_idx(status_json, 2 + IPOFFSET + IPOFFSET2, get_battery("BAT0"));
#endif
#if BATTERIES > 1
				json_object_array_put_idx(status_json, 4 + IPOFFSET + IPOFFSET2, get_battery("BAT1"));
#endif
				
				json_object_array_put_idx(status_json, 2 + (BATTERIES * 2) + IPOFFSET + IPOFFSET2, get_fs("/"));
				json_object_array_put_idx(status_json, 4 + (BATTERIES * 2) + IPOFFSET + IPOFFSET2, get_fs("/home"));
				
				json_object_array_put_idx(status_json, status_length - 2, get_mem_info());
			}
			
			// every second
			json_object_array_put_idx(status_json, status_length - 1, get_time());
			
			fprintf(stdout, "%s,\n", json_object_to_json_string(status_json));
			fflush(stdout);
			
			sleep(1);
		}
	}
	status_json = json_object_new_array();
	
	
	exit (EXIT_SUCCESS);
}
