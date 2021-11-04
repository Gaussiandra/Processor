#include <cstdio>
#include <cstdlib>
#include "../processorUtils.hpp"
#include "../globalUtils.hpp"
#include "disassembler.hpp"

int main(const int argc, const char *argv[]) {
    char *inpFilePath = nullptr, *outFilePath = nullptr;
    CONTINUE_IFN0(get2StrArgs(argc, argv, &inpFilePath, &outFilePath));

    processorData_t *codeArr = nullptr;
    size_t szFile = 0;
    CONTINUE_IFN0(readDataFromPath(inpFilePath, (char **)&codeArr, &szFile, true));
    size_t codeLen = szFile / sizeof(processorData_t);
    FILE *outputFile = nullptr;
    openFile(outFilePath, &outputFile, "w");

    char strToReturn[INT_TO_STR_BUFF_SZ] = {0}; 
    bool hasArg = false;
    int labels[MAX_LABEL_COUNT] = {0};
    for (int i = 0; i < MAX_LABEL_COUNT; ++i) { labels[i] = -1; }
    size_t instructionPtr = 0; 

    // First pass
    while (instructionPtr < codeLen) {
        processorData_t curNumCmd = (codeArr[instructionPtr] >> COMMAND_OFFSET_CMD);

        #define DEFINE_CMD_(name, index, argType, ...)                                  \
            case index: {                                                               \
                if (argType == JMP_TYPE) {                                              \
                    CONTINUE_IFN0(insertUniqueIdx(labels, MAX_LABEL_COUNT,              \
                                                  codeArr[++instructionPtr]));          \
                                                                                        \
                }                                                                       \
                else if (argType == WITH_NUMERIC_ARGUMENT) {                            \
                    ++instructionPtr;                                                   \
                }                                                                       \
                ++instructionPtr;                                                       \
                break;                                                                  \
            }                                                       

        switch (curNumCmd) {
            #include "../commands.hpp"
            default:
                ABORT_WITH_PRINTF(("Unknown command %d", codeArr[instructionPtr]));
                break;
        }
        #undef DEFINE_CMD_
    }
    
    // Second pass
    instructionPtr = 0;
    while (instructionPtr < codeLen) {
        processorData_t curNumCmd = (codeArr[instructionPtr] >> COMMAND_OFFSET_CMD);

    #define DEFINE_CMD_(name, index, argType, ...)                                    \
            case index: {                                                             \
                int labelIdx = getIdxFromArr(labels, MAX_LABEL_COUNT, instructionPtr);\
                if (labelIdx != -1) {                                                 \
                    fprintf(outputFile, "label_%d:\n", labelIdx);                     \
                }                                                                     \
                                                                                      \
                if (argType == WO_ARGUMENTS) {                                        \
                    fprintf(outputFile, "%s\n", #name);                               \
                    ++instructionPtr;                                                 \
                }                                                                     \
                else if (argType == WITH_NUMERIC_ARGUMENT) {                          \
                    CONTINUE_IFN0(convertNumericCmd(codeArr[instructionPtr],          \
                                                    codeArr[instructionPtr + 1],      \
                                                    strToReturn, &hasArg));           \
                    fprintf(outputFile, "%s %s\n", #name, strToReturn);               \
                    instructionPtr += 1 + hasArg;                                     \
                }                                                                     \
                else if (argType == JMP_TYPE) {                                       \
                    fprintf(outputFile, "%s label_%d\n", #name,                       \
                        getIdxFromArr(labels, MAX_LABEL_COUNT, codeArr[instructionPtr + 1]));\
                    instructionPtr += 2;                                              \
                }                                                                     \
                                                                                      \
            break;                                                                    \
        }

        switch (curNumCmd) {
            #include "../commands.hpp"
        }
        #undef DEFINE_CMD_
    }

    fclose(outputFile);

    free(codeArr);
    codeArr = nullptr;
}

ProcErrorCodes convertNumericCmd(processorData_t curCmd, processorData_t nextCmd, char strToReturn[], bool *hasArg) {
    processorData_t curCmdFlags = curCmd & FLAGS_RANGE_CMD;

    if (0 == curCmdFlags) {
        strToReturn = (char*) "";
        *hasArg = false;
    }
    else {
        if (IMMEDIATE_CONST_CMD == curCmdFlags) {
            sprintf(strToReturn, "%d", nextCmd);
        }
        else if ((IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
            sprintf(strToReturn, "[%d]", nextCmd);
        }
        else if (REGISTER_CMD == curCmdFlags) {
            sprintf(strToReturn, "%cx", (nextCmd >> REGISTER_OFFSET_ARG ) + 'a');
        }
        else if ((REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
            sprintf(strToReturn, "[%cx]", (nextCmd >> REGISTER_OFFSET_ARG ) + 'a');
        }
        else if ((IMMEDIATE_CONST_CMD | REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
            sprintf(strToReturn, "[%cx + %d]", (nextCmd >> REGISTER_OFFSET_ARG ) + 'a', 
                                                nextCmd & ARGUMENT_VALUE_RANGE_ARG);
        }
        else {
            printf("Unknown commands %d %d.\n", curCmd, nextCmd);
            return UNKNOWN_COMMAND;
        }
        *hasArg = true;
    }

    return OKAY;
}

ProcErrorCodes insertUniqueIdx(int array[], int szArr, int value) {
    for (size_t i = 0; i < szArr; ++i) {
        if (array[i] == value) {
            return OKAY;
        }
        else if (array[i] == -1) {
            array[i] = value;
            return OKAY;
        }
    }
    return NO_FREE_SPACE_IN_ARRAY;
}

int getIdxFromArr(int array[], int szArr, int value) {
    for (int i = 0; i < szArr; ++i) {
        if (array[i] == value) {
            return i;
        }
    }

    return -1;
}