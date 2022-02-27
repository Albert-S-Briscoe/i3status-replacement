/*
functions to get network related info. I generally like this code less, so I put it in a different file.
*/
#define _GNU_SOURCE
#include <unistd.h>			// get_ip for close()
#include <sys/socket.h>		// get_ip
#include <net/if.h>			// get_ip
#include <sys/types.h>		// get_ip
#include <sys/ioctl.h>		// get_ip
#include <netinet/in.h>		// get_ip
#include <arpa/inet.h>		// get_ip

#include "netinfo.h"


// I don't think I want to use this permanently, but whatever, I probably will. This took a while to figure out and it seems to work well, might as well use it.
// lets me know when a ssh client is connected. if it's only one, shows the ip, if multiple, shows number of clients. also works for sftp.
// just rewrote most of the 1am code.
void get_ssh(json_object *blocks[2]) {
	char output_str[100];
	int addrs = 0;
	char addr[30];
	
	// run `ss -tn` (lists all tcp connections) and get lines of stdout
	FILE *pp;
	pp = popen("ss -tn", "r");
	if (pp == NULL) {
		blocks[0] = white_text("");
		blocks[1] = white_text("");
	}
	while (1) {
		char *line;
		char buff[200];
		line = fgets(buff, sizeof(buff), pp);
		if (line == NULL)
			break;
		
		// remove uninteresting fields
		strtok(line, " "); // state
		strtok(NULL, " "); // recv-q value
		strtok(NULL, " "); // send-q value
		
		// check if local port is 22
		if (strcmp(strchrnul(strtok(NULL, " "), ':'), ":22") != 0)
			continue;
		
		if (addrs == 0) {
			strcpy(addr, strtok(NULL, " "));	// get remote ip
			strrchr(addr, ':')[0] = '\0';		// remove port number (by replacing the : with null)
		}
		addrs += 1;
	}
	pclose(pp);
	
	if (addrs == 0) {
		blocks[0] = white_text("");
		blocks[1] = white_text("");
		return;
	}
	
	if (addrs == 1)
		sprintf(output_str, "SSH: %s", addr);
	else
		sprintf(output_str, "SSH: %d addrs", addrs);
	
	blocks[0] = color_text(output_str, "#ff7f00");
	blocks[1] = white_text(" | ");
	return;
}

// copied most of this, basically no idea what's going on here, should be good enough tho
// get ip of specific network interface
#ifdef NETINTERFACE
json_object *get_ip(char *interface) {
	char output_str[100];
	int fd;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	ifr.ifr_addr.sa_family = AF_INET;
	
	// specify the interface
	strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
	
	// system call
	ioctl(fd, SIOCGIFADDR, &ifr);
	
	close(fd);
	
#ifdef SHOWINTERFACE
	sprintf(output_str, "%s: %s", interface, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
#else
	sprintf(output_str, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
#endif
	
	return white_text(output_str);
}
#endif
