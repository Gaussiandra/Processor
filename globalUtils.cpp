#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "globalUtils.hpp"

ProcErrorCodes prepareStringsFromPath(const char *inpFilePath, char **rawData, stringData **strings, size_t *nStrings) {
    assert(inpFilePath);
    assert(rawData);

    size_t szFile = 0;
    CONTINUE_IFN0(readDataFromPath(inpFilePath, rawData, &szFile, false));

    *nStrings = replaceChars('\n', '\0', *rawData) + 1;
    // nStrings + 1 for nullptr at the end of array for security purposes
    *strings = (stringData*) calloc(*nStrings + 1, sizeof(stringData));

    // szFile + 1 is correct because of alloc(szFile + 1)
    int nInitStrings = initStringPtrs(*rawData, *strings, szFile + 1);
    assert(nInitStrings == *nStrings);

    for (size_t i = 0; i < *nStrings; ++i) {
        replaceChars(';', '\0', (*strings)[i].str);
    }

    return OKAY;
}

size_t replaceChars(char fromChar, char toChar, char string[]) {
    assert(fromChar != '\0');
    assert(string);

    size_t nStrings = 0;
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == fromChar) {
            string[i] = toChar;
            nStrings++;
        }
        i++;
    }

    return nStrings;
}

int initStringPtrs(char *rawData, stringData strings[], size_t szFile) {
    assert(rawData);
    assert(strings);

    int nInits = 0;

    strings[nInits++].str = rawData;
    for (size_t i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits].str = rawData + i + 1;
            strings[nInits - 1].length = size_t(strings[nInits].str - strings[nInits - 1].str);
            ++nInits;
        }
    }
    strings[nInits - 1].length = size_t(rawData + szFile - strings[nInits-1].str);

    return nInits;
}

ProcErrorCodes openFile(const char *inpFilePath, FILE **inpFile, const char *mode) {
    assert(inpFilePath);
    assert(inpFile);
    assert(mode);

    *inpFile = fopen(inpFilePath, mode);
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", inpFilePath);
        return FILE_OPENING_ERROR;
    }

    return OKAY;
}

long getFileSize(FILE *inpFile) {
    assert(inpFile);

    fseek(inpFile, 0, SEEK_END);
    long szFile = ftell(inpFile);
    rewind(inpFile);

    return szFile;
}

ProcErrorCodes readDataFromPath(const char *inpFilePath, char **rawData, size_t *szFile, bool isBinary) {
    assert(inpFilePath);
    assert(rawData);
    assert(szFile);

    FILE *inpFile = nullptr;
    if (isBinary) {
        CONTINUE_IFN0(openFile(inpFilePath, &inpFile, "rb"));
    }
    else {
        CONTINUE_IFN0(openFile(inpFilePath, &inpFile, "r"));
    }

    long signedSzFile = getFileSize(inpFile);
    if (signedSzFile == -1) {
        return SIZE_GETTING_ERROR;
    }
    *szFile = (size_t) signedSzFile;

    // +1 due to possible absence \n at the end of the file.
    *rawData = (char*) calloc(*szFile + 1, sizeof(char));
    if (!*rawData) {
        printf("Memory wasn't allocated.\n");
        return MEM_ALLOCATION_ERROR;
    }

    // fread on Windows returns readingResult <= szFile due to deleting /r from stream
    size_t readingResult = fread(*rawData, sizeof(char), *szFile, inpFile);
    *szFile = readingResult;
    *(*rawData + readingResult) = '\0';

    fclose(inpFile);

    return OKAY;
}