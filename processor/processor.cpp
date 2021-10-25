#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "processor.hpp"
#include "../SaveStack/stack.hpp"
#include "../globalUtils.hpp"

// дефайн для общего случая(всё корректно), проверку в pop в asm, проверки
int main(const int argc, const char *argv[]) {
    char *inpFilePath = nullptr;
    if (argc == 2) {
        inpFilePath = (char*) argv[1];
    }
    else {
        printf("Invalid command line arguments.\nUsage: processor.out input_path.\n");
        return ARGUMENTS_HANDLING_ERROR;
    }

    processorData_t *rawData = nullptr;
    size_t szFile = 0;
    CONTINUE_IFN0(readDataFromPath(inpFilePath, (char **)&rawData, &szFile, true));

    size_t instuctionPtr = 0;
    processorData_t ram[RAM_LEN] = {0};
    processorData_t registers[N_REGISTERS] = {0};
    stack_t stack;
    stackCtor(stack);

    while (rawData[instuctionPtr]) {
        printf("cmd: %d\n", rawData[instuctionPtr]);
        processorData_t curNumCmd = rawData[instuctionPtr] >> COMMAND_OFFSET_CMD;

        switch (curNumCmd) {
            case 1: {
                if ((rawData[instuctionPtr] & FLAGS_RANGE_CMD) == IMMEDIATE_CONST_CMD) {
                    stackPush(&stack, rawData[instuctionPtr + 1]);
                }
                else if (processorData_t *pushArg = getPtrWrtFlags(rawData + instuctionPtr, registers, ram)) {
                    stackPush(&stack, *pushArg);
                }
                else {
                    printf("Unknown flag set. Command = %d\n", rawData[instuctionPtr]);
                    fflush(stdout);
                    abort();
                }

                ++instuctionPtr;
                break;
            }
            case 2: {
                if ((rawData[instuctionPtr] & FLAGS_RANGE_CMD) == 0) {
                    processorData_t value;
                    stackPop(&stack, &value);
                }
                else if (processorData_t *popArg = getPtrWrtFlags(rawData + instuctionPtr, registers, ram)) {
                    stackPop(&stack, popArg);
                }
                else {
                    printf("Unknown flag set. Command = %d\n", rawData[instuctionPtr]);
                    fflush(stdout);
                    abort();
                }

                ++instuctionPtr;
                break;
            }
            case 3: {
                processorData_t a = 0, b = 0;
                stackPop(&stack, &a);
                stackPop(&stack, &b);
                stackPush(&stack, a * b);

                break;
            }
            case 4: {
                processorData_t a = 0, b = 0;
                stackPop(&stack, &a);
                stackPop(&stack, &b);
                stackPush(&stack, a + b);

                break;
            }
            case 5: {
                processorData_t a = 0, b = 0;
                stackPop(&stack, &a);
                stackPop(&stack, &b);
                stackPush(&stack, a - b);

                break;
            }
            case 6: {
                processorData_t a = 0;
                scanf("%d", &a);
                stackPush(&stack, a);

                break;
            }
            case 7: {
                processorData_t a = 0;
                stackPop(&stack, &a);
                printf("%d\n", a);

                break;
            }
        }

        ++instuctionPtr;
    }
    free(rawData);
    rawData = nullptr;

    return 0;
}

processorData_t* accessRam(processorData_t ram[], size_t addr) {
    usleep(RAM_ACCESS_DELAY_MS);
    return ram + addr;
}

processorData_t* getPtrWrtFlags(processorData_t *curCmdPtr, processorData_t registers[], processorData_t ram[]) {
    processorData_t nextFullCmd = *(curCmdPtr + 1),
                    curCmdFlags = *curCmdPtr & FLAGS_RANGE_CMD;

    if (REGISTER_CMD == curCmdFlags) {
        return &registers[nextFullCmd & REGISTER_VALUE_ARG];
    }
    else if ((IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
        return accessRam(ram, nextFullCmd & ARGUMENT_VALUE_RANGE_ARG);
    }
    else if ((REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
        return accessRam(ram, registers[nextFullCmd & REGISTER_VALUE_ARG]);
    }
    else if ((REGISTER_CMD | IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
        processorData_t registerValue = registers[nextFullCmd & REGISTER_VALUE_ARG],
                        argumentValue = nextFullCmd & ARGUMENT_VALUE_RANGE_ARG;
        return accessRam(ram, registerValue + argumentValue);
    }

    return nullptr;
}