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

    if (memcmp(contents, "!<arch>\n", 8) == 0) {
        return FileTypeArchive;
    }
    return FileTypeUnknown;
}

//void CheckFileCompatibility(Context* ctx, File* file) {
//    FileType mt = GetMachineTypeFromContents(file->Contents);
//    if (mt != ctx->Args.Emulation) {
//        fatal("incompatible file type");
//    }
//}