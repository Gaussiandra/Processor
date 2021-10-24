#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "assembler.hpp"

size_t mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[]) {
    assert(strings);
    assert(nStrings >= 0);
    assert(outputArray);

    size_t instructionPtr = 0;

    for (size_t stringIdx = 0; stringIdx < nStrings; ++stringIdx) {
        char curCmd[MAX_INPUT_STR_LEN] = "";
        sscanf(strings[stringIdx].str, "%s", curCmd);

       #define DEFINE_CMD_(name, index, nArguments, ...)                                                            \
            if (strcmp(curCmd, #name) == 0) {                                                                       \
                processorData_t idxCmd = (index) << COMMAND_OFFSET_CMD, arg = 0;                                    \
                size_t nChanged = 0;                                                                                \
                                                                                                                    \
                if (nArguments) {                                                                                   \
                    CONTINUE_IFN0(parseArgument(strings[stringIdx].str, strlen(curCmd), &idxCmd, &arg, &nChanged)); \
                    outputArray[instructionPtr++] = idxCmd;                                                         \
                    if (nChanged > 1) {                                                                             \
                        outputArray[instructionPtr++] = arg;                                                        \
                    }                                                                                               \
                }                                                                                                   \
                else {                                                                                              \
                    outputArray[instructionPtr++] = idxCmd;                                                         \
                }                                                                                                   \
           }                                                                                                        \
           else                                                                                    

        #include "../commands.hpp" 
        {
            printf("Unknown command at line %zu.\n", stringIdx + 1);
            abort();
        }
        #undef DEFINE_CMD_
    }

    return instructionPtr;
}

ProcErrorCodes parseArgument(char *string, size_t cmdLen, processorData_t *idxCmd, processorData_t *arg, size_t *nChanged) {
    *arg = 0;
    *nChanged = 0;
    char strArg[2] = "";

    if (strlen(string) > cmdLen) {
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

ProcErrorCodes saveArrayToBinFile(processorData_t outputArray[], size_t nArrayElements, char *outFilePath) {
    FILE *outFile = nullptr;
    CONTINUE_IFN0(openFile(outFilePath, &outFile, "wb"));
    
    fwrite(outputArray, sizeof(processorData_t), nArrayElements, outFile);
    fclose(outFile);

    return OKAY;
}