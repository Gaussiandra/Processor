#pragma once

#include <cstdio>
#include <cstdlib>

#define RETURN_ON_ERROR(code) {        \
    ProcErrorCodes errorCode = (code); \
    if (errorCode) {                   \
        return errorCode;              \
    }                                  \
}

struct stringData {
    char *str = nullptr;
    size_t length = 0;
};

enum ProcErrorCodes {
    OKAY = 0,
    ARGUMENTS_HANDLING_ERROR,
    FILE_OPENING_ERROR,
    SIZE_GETTING_ERROR,
    MEM_ALLOCATION_ERROR,
    UNKNOWN_COMMAND,
    LABEL_INDEX_NOT_FOUND,
    NO_FREE_SPACE_IN_ARRAY,
};

ProcErrorCodes prepareStringsFromPath(const char *inpFilePath, char **rawData, stringData **strings, size_t *nStrings);
size_t replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(char *rawData, stringData strings[], size_t szFile);
ProcErrorCodes openFile(const char *inpFilePath, FILE **inpFile, const char *mode);
long getFileSize(FILE *inpFile);
ProcErrorCodes readDataFromPath(const char *inpFilePath, char **rawData, size_t *szFile, bool isBinary);
ProcErrorCodes get2StrArgs(const int argc, const char *argv[], const char **inpFilePath, const char **outFilePath);
ProcErrorCodes saveArrayToBinFile(char outputArray[], size_t nArrayElements, size_t elementSize, const char *outFilePath);
