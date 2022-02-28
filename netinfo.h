#ifndef NETINFO_H
#define NETINFO_H

#include "shared.h"


#ifdef NETINTERFACE
extern json_object *get_ip(char *interface);
#endif

extern json_object *get_ssh();


#endif
