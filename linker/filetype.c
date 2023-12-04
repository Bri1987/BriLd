#include "filetype.h"

FileType GetFileType(const char* contents){
    if (contents == NULL || *contents == '\0') {
        return FileTypeEmpty;
    }

    if (CheckMagic(contents)) {
        uint16_t et;
        Read(&et,contents+16,sizeof(uint16_t));
        switch (et) {
            case 1:  //ET_REL
                return FileTypeObject;
                break;
            default:
                return FileTypeUnknown;
        }
    }
    return FileTypeUnknown;
}