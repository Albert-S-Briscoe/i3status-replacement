#ifndef NETINFO_H
#define NETINFO_H

#include "shared.h"


#ifdef NETINTERFACE
extern json_object *get_ip(char *interface);
#endif

extern int get_ssh(char* output, size_t output_size);


#endif
