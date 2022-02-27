#ifndef NETINFO_H
#define NETINFO_H

#include "shared.h"

extern void get_ssh(json_object *blocks[2]);
extern json_object *get_ip(char *interface);

#endif
