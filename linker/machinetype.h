#ifndef BRILINKER_MACHINETYPE_H
#define BRILINKER_MACHINETYPE_H

#include <stdint.h>
#include <stdlib.h>
#include "filetype.h"

typedef uint8_t MachineType;

#define MachineTypeNone    ((MachineType)0)
#define MachineTypeRISCV64 ((MachineType)1)


const char* MachineType_String(MachineType m);
MachineType GetMachineTypeFromContents(const char* contents);

#endif //BRILINKER_MACHINETYPE_H
