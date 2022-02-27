#include <unistd.h>			// sleep()

#include "main.h"

int main (int argc, char *argv[]) {
//	get_battery_bar("../../../home/main/src/i3status-replacement/battery");
	
	
	int status_length;
	json_object *sshblocks[2];
	
#ifndef RAMSUMMARY
	json_object *ramblocks[3];
#endif
	
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
	
	
//	get_battery_bar(batterybar, "BAT0");
#ifdef BATTERYBAR
	json_object_array_add(status_json, placeholder);// batterybar[0]
//	json_object_array_add(status_json, placeholder);// batterybar[1]
	json_object_array_add(status_json, white_text(") | "));
#endif
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
	json_object_array_add(status_json, get_battery("BAT0"));
	json_object_array_add(status_json, separator);
#endif
#if BATTERIES > 1
	json_object_array_add(status_json, get_battery("BAT1"));
	json_object_array_add(status_json, separator);
#endif
	json_object_array_add(status_json, get_fs("/home"));
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, get_fs("/"));
	json_object_array_add(status_json, separator);
#ifdef RAMSUMMARY
	json_object_array_add(status_json, get_mem_info()); // swap and warning on full memory/swap usage
#else
	get_mem_info(ramblocks);
	json_object_array_add(status_json, ramblocks[0]); // used
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, ramblocks[1]); // free
	json_object_array_add(status_json, separator);
	json_object_array_add(status_json, ramblocks[2]); // avail
#endif
	json_object_array_add(status_json, get_time()); // built in seperators
	
	// update length every time it's changed. Not sure exactly how that'll work at the moment.
	status_length = json_object_array_length(status_json);
	
//	fprintf(stdout, "%s,\n", json_object_to_json_string(status_json));
//	fflush(stdout);
	
	while (1) {
		// every 30 seconds
		get_ssh(sshblocks);
		json_object_array_put_idx(status_json, 0 + BATTERYBAROFFSET, sshblocks[0]);
		json_object_array_put_idx(status_json, 1 + BATTERYBAROFFSET, sshblocks[1]);
#ifdef NETINTERFACE
		json_object_array_put_idx(status_json, 2 + BATTERYBAROFFSET, get_ip(NETINTERFACE));
#endif
#ifdef NETINTERFACE2
		json_object_array_put_idx(status_json, 2 + IPOFFSET + BATTERYBAROFFSET, get_ip(NETINTERFACE2));
#endif
		
		for (int i = 0; i < 30; i++) {
			// every 10 seconds
			if (i % 10 == 0) {
#ifdef BATTERYBAR
//				get_battery_bar(batterybar, "BAT0");
//				json_object_array_put_idx(status_json, 0, batterybar[0]);
//				json_object_array_put_idx(status_json, 1, batterybar[1]);
				json_object_array_put_idx(status_json, 0, get_battery_bar("BAT0"));
#endif
				
#if BATTERIES > 0
				json_object_array_put_idx(status_json, 2 + IPOFFSET + IPOFFSET2 + BATTERYBAROFFSET, get_battery("BAT0"));
#endif
#if BATTERIES > 1
				json_object_array_put_idx(status_json, 4 + IPOFFSET + IPOFFSET2 + BATTERYBAROFFSET, get_battery("BAT1"));
#endif
				
				json_object_array_put_idx(status_json, 2 + (BATTERIES * 2) + IPOFFSET + IPOFFSET2 + BATTERYBAROFFSET, get_fs("/"));
				json_object_array_put_idx(status_json, 4 + (BATTERIES * 2) + IPOFFSET + IPOFFSET2 + BATTERYBAROFFSET, get_fs("/home"));
				
#ifdef RAMSUMMARY
				json_object_array_put_idx(status_json, status_length - 2, get_mem_info());
#else
				get_mem_info(ramblocks);
				json_object_array_put_idx(status_json, status_length - 6, ramblocks[0]);
				json_object_array_put_idx(status_json, status_length - 4, ramblocks[1]);
				json_object_array_put_idx(status_json, status_length - 2, ramblocks[2]);
#endif
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
