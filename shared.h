#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>

#include "config.h"
#include "util.h"


#ifdef NETINTERFACE
	#ifdef NETINTERFACE2
		#define IPOFFSET 4
	#else
		#define IPOFFSET 2
	#endif
#else
	#define IPOFFSET 0
#endif

#ifdef BATTERYBAR
#define BATTERYSEP (white_text(") | "))
#else
#define BATTERYSEP (separator)
#endif


#endif
