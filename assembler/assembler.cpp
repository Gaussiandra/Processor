#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "assembler.hpp"

ProcErrorCodes mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[], size_t *nArrayElements) {
    assert(strings);
    assert(nStrings >= 0);
    assert(outputArray);

    label_t labels[MAX_LABEL_COUNT] = {0};
    size_t instructionPtr = 0, szLabels = 0;

    char curCmd[MAX_INPUT_STR_LEN] = "";
    for (int kthPass = 0; kthPass < 2; ++kthPass) {
        instructionPtr = 0;
        for (size_t stringIdx = 0; stringIdx < nStrings; ++stringIdx) {  
            sscanf(strings[stringIdx].str, "%s", curCmd);
            size_t curCmdLastIdx = strlen(curCmd) - 1;
            if (kthPass == 0 && curCmd[curCmdLastIdx] == ':') {
                curCmd[curCmdLastIdx] = '\0';
                strcpy(labels[szLabels].labelName, curCmd);
                labels[szLabels++].instructionPtr = instructionPtr;
            }
            else if (curCmd[curCmdLastIdx] != ':') {
                #define DEFINE_CMD_(name, index, argType, ...)                                          \
                    if (strcmp(curCmd, #name) == 0) {                                                   \
                        processorData_t idxCmd = (index) << COMMAND_OFFSET_CMD, arg = 0;                \
                        size_t nChanged = 0;                                                            \
                                                                                                        \
                        if (argType == WITH_NUMERIC_ARGUMENT) {                                         \
                            RETURN_ON_ERROR(parseNumericArgument(strings[stringIdx].str, strlen(curCmd), &idxCmd, &arg, &nChanged)); \
                            outputArray[instructionPtr++] = idxCmd;                                     \
                            if (nChanged > 1) {                                                         \
                                outputArray[instructionPtr++] = arg;                                    \
                            }                                                                           \
                        }                                                                               \
                        else {                                                                          \
                            outputArray[instructionPtr++] = idxCmd;                                     \
                            if (argType == JMP_TYPE) {                                                  \
                                if (kthPass == 0) {                                                     \
                                    outputArray[instructionPtr++] = TEMP_CMD;                           \
                                }                                                                       \
                                else {                                                                  \
                                    int nReadArgs = sscanf(strings[stringIdx].str, "%*s %s", curCmd);   \
                                    assert(nReadArgs && "Incorrect jmp argument.");                     \
                                                                                                        \
                                    size_t labelIdx = 0;                                                \
                                    RETURN_ON_ERROR(getIdxByName(labels, szLabels, &labelIdx, curCmd)); \
                                    outputArray[instructionPtr++] = labels[labelIdx].instructionPtr;    \
                                }                                                                       \
                            }                                                                           \
                        }                                                                               \
                    }                                                                                   \
                    else                                                                                    

                #include "../commands.hpp" 
                {
                    printf("Unknown command at line %zu.\n", stringIdx + 1);
                    return UNKNOWN_COMMAND;
                }
                #undef DEFINE_CMD_
            }   
        }
    }

    *nArrayElements = instructionPtr;
    return OKAY;
}

ProcErrorCodes parseNumericArgument(char *string, size_t cmdLen, processorData_t *idxCmd, 
                                    processorData_t *arg, size_t *nChanged) {
    *arg = 0;
    *nChanged = 0;
    char strArg[2] = "";

    if (strlen(string) > cmdLen) {
        if (sscanf(string, "%*s %d", arg) == 1) {
            *idxCmd |= IMMEDIATE_CONST_CMD;
        }
        else if (sscanf(string, "%*s [%d]", arg) == 1) {
            *idxCmd |= IMMEDIATE_CONST_CMD | MEMORY_CMD;
        }
        else if (sscanf(string, "%*s [%1[abcd]x + %d]", strArg, arg) == 2) {
            *idxCmd |= IMMEDIATE_CONST_CMD | REGISTER_CMD | MEMORY_CMD;
            *arg = ((strArg[0] - 'a') << REGISTER_OFFSET_ARG) + *arg;
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

ProcErrorCodes getIdxByName(label_t arr[], size_t arrSize, size_t *indexToReturn, char name[]) {
    for (size_t i = 0; i < arrSize; ++i) {
        if(!strcmp(arr[i].labelName, name)) {
            *indexToReturn = i;
            return OKAY;
        }
    }

    return LABEL_INDEX_NOT_FOUND;
}