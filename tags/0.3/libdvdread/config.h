/* Automatically generated by configure, do not edit */
#include "version.h"

#include <sys/time.h>

int gekko_gettimeofday(struct timeval *tv, void *tz);

#define gettimeofday(TV, TZ) gekko_gettimeofday((TV), (TZ))
