#include <unistd.h>	// sleep()

#include "main.h"


#define PLACEHOLDER white_text_old("")

int main (int argc, char *argv[]) {
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
	char time_str[256];


	// start infinite json
#ifdef CLICKEVENTS
	printf ("{\"version\":1,\"click_events\":true}\n[\n[],\n");
#else
	printf ("{\"version\":1}\n[\n[],\n");
#endif
	fflush(stdout);

	// output a json array every second while updating the status, forever
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
			get_time(time_str, 256);

			// Output json
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
			for (int x = 0; x < BATTERIES; x++)
				fprintf(stdout, "%s,\n", bat_str[x]);
#endif
			fprintf(stdout, "%s,\n", fs1_str);
			fprintf(stdout, "%s,\n", fs2_str);
			fprintf(stdout, "%s,\n", mem_str);
			fprintf(stdout, "%s\n", time_str);
			fprintf(stdout, "],\n\n");
			fflush(stdout);

			sleep(1);
		}
	}

	exit (EXIT_SUCCESS);
}
