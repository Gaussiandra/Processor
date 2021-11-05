#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include "processor.hpp"
#include "../SaveStack/stack.hpp"
#include "../globalUtils.hpp" 

int main(const int argc, const char *argv[]) {
    const char *inpFilePath = nullptr;
    if (argc == 2) {
        inpFilePath = argv[1];
    }
    else {
        printf("Invalid command line arguments.\nUsage: processor.out input_path.\n");
        return ARGUMENTS_HANDLING_ERROR;
    }

    processorData_t *codeArr = nullptr;
    size_t szFile = 0;
    RETURN_ON_ERROR(readDataFromPath(inpFilePath, (char **)&codeArr, &szFile, true));

    RETURN_ON_ERROR(execBinCode(codeArr));

    free(codeArr);
    codeArr = nullptr;
    

    return 0;
}

ProcErrorCodes execBinCode(processorData_t *codeArr) {
    size_t instructionPtr = 0;
    processorData_t ram[RAM_LEN] = {0};
    processorData_t registers[N_REGISTERS] = {0};
    stack_t stack;
    stackCtor(stack);

    while (codeArr[instructionPtr]) {
        processorData_t curNumCmd = codeArr[instructionPtr] >> COMMAND_OFFSET_CMD;

        #define DEFINE_CMD_(name, index, argType, code)                             \
            case index: {                                                           \
                code;                                                               \
                break;                                                              \
            }

        switch (curNumCmd) {
            #include "../commands.hpp"
            default:
                printf("Unknown command %d", codeArr[instructionPtr]);
                return UNKNOWN_COMMAND;
        }
        #undef DEFINE_CMD_
    }

    stackDtor(&stack);
    return OKAY;
}

processorData_t* accessRam(processorData_t ram[], processorData_t addr) {
    usleep(RAM_ACCESS_DELAY_MS);
    return ram + addr;
}

processorData_t* getCmdDestByFlags(processorData_t *curCmdPtr, processorData_t registers[], processorData_t ram[]) {
    processorData_t nextFullCmd = *(curCmdPtr + 1),
                    curCmdFlags = *curCmdPtr & FLAGS_RANGE_CMD;

    if (REGISTER_CMD == curCmdFlags) {
        return registers + ((nextFullCmd & REGISTER_VALUE_ARG) >> (FLAGS_RANGE_CMD - 1));
    }
    else if ((IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
        return accessRam(ram, nextFullCmd & ARGUMENT_VALUE_RANGE_ARG);
    }
    else if ((REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
        return accessRam(ram, registers[(nextFullCmd & REGISTER_VALUE_ARG) >> (FLAGS_RANGE_CMD - 1)]);
    }
    else if ((REGISTER_CMD | IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
        processorData_t registerValue = registers[nextFullCmd & REGISTER_VALUE_ARG],
                        argumentValue = nextFullCmd & ARGUMENT_VALUE_RANGE_ARG;
        return accessRam(ram, registerValue + argumentValue);
    }

    return nullptr;
}