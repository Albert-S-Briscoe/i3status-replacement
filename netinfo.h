#ifndef NETINFO_H
#define NETINFO_H

#include "shared.h"


#ifdef NETINTERFACE
extern void get_ip(char *interface, char* output, size_t output_size);
#endif

extern int get_ssh(char* output, size_t output_size);


#endif
