// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <utils/type.h>

// Maximum number of processes allowed to run simultaneously
#define PM_MAX_PROCESSES 1024

u16_t pm_get_pid();

#endif