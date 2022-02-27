#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>


#include "config.h"
#include "util.h"


#ifdef NETINTERFACE
#define IPOFFSET 2
#else
#define IPOFFSET 0
#endif

#endif
