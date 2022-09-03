#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>

#include "config.h"
#include "util.h"

#ifdef BATTERIES_ONE_INDEXED
#define BATTERY_START 1
#else
#define BATTERY_START 0
#endif

#endif
