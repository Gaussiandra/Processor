#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "assembler.hpp"

//дефайн, va_args/...
size_t mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[]) {
    assert(strings);
    assert(nStrings >= 0);
    assert(outputArray);

    size_t instructionPtr = 0;

    for (size_t stringIdx = 0; stringIdx < nStrings; ++stringIdx) {
        char curCmd[MAX_INPUT_STR_LEN] = "";
        sscanf(strings[stringIdx].str, "%s", curCmd);

//        #define DEFINE_CMD_(name, index, nArguments, code) {                                        \
//            if (strcmp(curCmd, #name) == 0) {                                                       \
//                processorData_t idxCmd = (index), arg = 0;                                          \
//                size_t nChanged = 0;                                                                \
//                                                                                                    \
//                if (nArguments) {                                                                   \
//                    CONTINUE_IFN0(parseArgument(strings[stringIdx].str, &idxCmd, &arg, &nChanged)); \
//                    outputArray[instructionPtr++] = idxCmd;                                         \
//                    if (nChanged > 1) {                                                             \
//                        outputArray[instructionPtr++] = arg;                                        \
//                    }                                                                               \
//                }                                                                                   \
//            }                                                                                       \
//            else                                                                                    \
//        }

        if (strcmp(curCmd, "push") == 0) {
            processorData_t idxCmd = 1, arg = 0;
            size_t nChanged = 0;

            CONTINUE_IFN0(parseArgument(strings[stringIdx].str, &idxCmd, &arg, &nChanged));
            outputArray[instructionPtr++] = idxCmd;
            if (nChanged > 1) {
                outputArray[instructionPtr++] = arg;
            }
        }
        else if (strcmp(curCmd, "pop") == 0) {
            processorData_t idxCmd = 2, arg = 0;
            size_t nChanged = 0;

            CONTINUE_IFN0(parseArgument(strings[stringIdx].str, &idxCmd, &arg, &nChanged));
            outputArray[instructionPtr++] = idxCmd;
            if (nChanged > 1) {
                outputArray[instructionPtr++] = arg;
            }
        }
        else if (strcmp(curCmd, "mul") == 0) {
            processorData_t idxCmd = 3 << COMMAND_OFFSET_CMD;
            outputArray[instructionPtr++] = idxCmd;
        }
        else if (strcmp(curCmd, "add") == 0) {
            processorData_t idxCmd = 4 << COMMAND_OFFSET_CMD;
            outputArray[instructionPtr++] = idxCmd;
        }
        else if (strcmp(curCmd, "sub") == 0) {
            processorData_t idxCmd = 5 << COMMAND_OFFSET_CMD;
            outputArray[instructionPtr++] = idxCmd;
        }
        else if (strcmp(curCmd, "hlt") == 0) {
            processorData_t idxCmd = 0 << COMMAND_OFFSET_CMD;
            outputArray[instructionPtr++] = idxCmd;
        }
        else {
            printf("Unknown command at line %zu.\n", stringIdx + 1);
            abort();
        }
    }

    return instructionPtr;
}

ErrorCodes parseArgument(char *string, processorData_t *idxCmd, processorData_t *arg, size_t *nChanged) {
    *idxCmd = (*idxCmd << COMMAND_OFFSET_CMD);
    *arg = 0;
    *nChanged = 0;
    char strArg[2] = "";

    // 3??
    if (strlen(string) > 3) {
        if (sscanf(string, "%*s %[0-9]", arg) == 1) {
            *arg -= '0';
            *idxCmd |= IMMEDIATE_CONST_CMD;
        }
        else if (sscanf(string, "%*s [%d]", arg) == 1) {
            *idxCmd |= IMMEDIATE_CONST_CMD | MEMORY_CMD;
        }
        else if (sscanf(string, "%*s [%1[abcd]x + %c]", strArg, arg) == 2) {
            // как поведёт себя с минусом?
            *idxCmd |= IMMEDIATE_CONST_CMD | REGISTER_CMD | MEMORY_CMD;
            *arg = ((strArg[0] - 'a') << REGISTER_OFFSET_ARG) + *arg - '0';
        }
        else if (sscanf(string, "%*s [%1[abcd]x]", strArg) == 1) {
            *arg = (strArg[0] - 'a') << REGISTER_OFFSET_ARG;
            *idxCmd |= REGISTER_CMD | MEMORY_CMD;
        }
        else if (sscanf(string, "%*s %1[abcd]x", strArg) == 1) {
            *arg = (strArg[0] - 'a') << REGISTER_OFFSET_ARG;
            *idxCmd |= REGISTER_CMD;
        }
        else {
            printf("Unknown command at line %s.\n", string);
            return UNKNOWN_COMMAND;
        }
        *nChanged = 2;
    }
    else {
        *nChanged = 1;
    }
    return OKAY;
}

ErrorCodes saveArrayToFile(processorData_t outputArray[], size_t nArrayElements, char *outFilePath) {
    FILE *outFile = fopen(outFilePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", outFilePath);
        return FILE_OPENING_ERROR;
    }
    
    for (size_t i = 0; i < nArrayElements; ++i) {
        fprintf(outFile, "%d ", outputArray[i]);
    }

    return OKAY;
}

ErrorCodes getRawDataFromFile(const char *inpFilePath, char **rawData, stringData **strings, size_t *nStrings) {
    assert(inpFilePath);
    assert(rawData);

    FILE *inpFile = fopen(inpFilePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", inpFilePath);
        return FILE_OPENING_ERROR;
    }
    long signedSzFile = getFileSize(inpFile);
    if (signedSzFile == -1) {
        return SIZE_GETTING_ERROR;
    }
    size_t szFile = (size_t) signedSzFile;

    // +1 due to possible absence \n at the end of the file.
    *rawData = (char*) calloc(szFile + 1, sizeof(char));
    if (!*rawData) {
        printf("Memory wasn't allocated.\n");
        return MEM_ALLOCATION_ERROR;
    }

    readDataFromFile(inpFile, *rawData, &szFile);
    fclose(inpFile);

    replaceChars(';', '\0', *rawData);
    *nStrings = replaceChars('\n', '\0', *rawData) + 1;
    // nStrings + 1 for nullptr at the end of array for security purposes
    *strings = (stringData*) calloc(*nStrings + 1, sizeof(stringData));

    // szFile + 1 is correct because of alloc(szFile + 1)
    assert(initStringPtrs(*rawData, *strings, szFile + 1) == *nStrings);

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

long getFileSize(FILE *inpFile) {
    assert(inpFile);

    fseek(inpFile, 0, SEEK_END);
    long szFile = ftell(inpFile);
    rewind(inpFile);

    return szFile;
}

void readDataFromFile(FILE *inpFile, char *rawData, size_t *szFile) {
    assert(inpFile);
    assert(rawData);
    assert(szFile);

    // fread on Windows returns readingResult <= szFile due to deleting /r from stream
    size_t readingResult = fread(rawData, sizeof(char), *szFile, inpFile);
    *szFile = readingResult;
    *(rawData + readingResult) = '\0';

    rewind(inpFile);
}