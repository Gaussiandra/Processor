#include <cstdio>
#include <cstdlib>
#include "disassembler.hpp"
#include "../processorUtils.hpp"
#include "../globalUtils.hpp"

int main(const int argc, const char *argv[]) {
    const char *inpFilePath = nullptr, *outFilePath = nullptr;
    RETURN_ON_ERROR(get2StrArgs(argc, argv, &inpFilePath, &outFilePath));

    processorData_t *codeArr = nullptr;
    size_t szFile = 0;
    RETURN_ON_ERROR(readDataFromPath(inpFilePath, (char **)&codeArr, &szFile, true));
    size_t codeLen = szFile / sizeof(processorData_t);
    FILE *outputFile = nullptr;
    openFile(outFilePath, &outputFile, "w");

    int labels[MAX_LABEL_COUNT] = {0};
    for (int i = 0; i < MAX_LABEL_COUNT; ++i) { labels[i] = -1; } 

    RETURN_ON_ERROR(execFirstPass (codeLen, codeArr, labels));
    RETURN_ON_ERROR(execSecondPass(codeLen, codeArr, labels, outputFile));

    fclose(outputFile);

    free(codeArr);
    codeArr = nullptr;
}

ProcErrorCodes execFirstPass(size_t codeLen, processorData_t codeArr[], int labels[]) {
    size_t instructionPtr = 0;

    while (instructionPtr < codeLen) {
        processorData_t curNumCmd = (codeArr[instructionPtr] >> COMMAND_OFFSET_CMD);

        #define DEFINE_CMD_(name, index, argType, ...)                                  \
            case index: {                                                               \
                if (argType == JMP_TYPE) {                                              \
                    RETURN_ON_ERROR(insertUniqueIdx(labels, MAX_LABEL_COUNT,            \
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
            {
                printf("Unknown command %d", codeArr[instructionPtr]);
                return UNKNOWN_COMMAND;
            }
        }
        #undef DEFINE_CMD_
    }

    return OKAY;
}

ProcErrorCodes execSecondPass(size_t codeLen, processorData_t codeArr[], int labels[], FILE *outputFile) {
    char strToReturn[INT_TO_STR_BUFF_SZ] = {0}; 
    bool hasArg = false;
    size_t instructionPtr = 0;

    while (instructionPtr < codeLen) {
        processorData_t curNumCmd = (codeArr[instructionPtr] >> COMMAND_OFFSET_CMD);

        #define DEFINE_CMD_(name, index, argType, ...)                                \
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
                    RETURN_ON_ERROR(convertNumericCmd(codeArr[instructionPtr],        \
                                                    codeArr[instructionPtr + 1],      \
                                                    strToReturn, &hasArg));           \
                    fprintf(outputFile, "%s %s\n", #name, strToReturn);               \
                    instructionPtr += 1 + hasArg;                                     \
                }                                                                     \
                else if (argType == JMP_TYPE) {                                       \
                    fprintf(outputFile, "%s label_%d\n", #name,                       \
                            getIdxFromArr(labels, MAX_LABEL_COUNT, codeArr[instructionPtr + 1])); \
                    instructionPtr += 2;                                              \
                }                                                                     \
                                                                                      \
            break;                                                                    \
        }

        switch (curNumCmd) {
            #include "../commands.hpp"
            default:
            {
                printf("Unknown command %d", codeArr[instructionPtr]);
                return UNKNOWN_COMMAND;
            }
        }
        #undef DEFINE_CMD_
    }

    return OKAY;
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