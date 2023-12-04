#include "machinetype.h"

const char* MachineType_String(MachineType m) {
    switch (m) {
        case MachineTypeRISCV64:
            return "riscv64";
    }

    return "none";
}

MachineType GetMachineTypeFromContents(const char* contents){
    FileType ft = GetFileType(contents);

    switch (ft) {
        case FileTypeObject: {
            uint16_t machine;
            Read(&machine, contents + 18, sizeof(uint16_t));
            if (machine == 243) { // Replace with actual value of elf.EM_RISCV
                uint8_t class = contents[4];
                switch (class) {
                    case 2: // Replace with actual value of elf.ELFCLASS64
                        return MachineTypeRISCV64;
                }
            }
        }
        break;
        default:
            return MachineTypeNone;
    }
    return MachineTypeNone;
}