#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "processor.hpp"
#include "../SaveStack/stack.hpp"
// #include "../processorUtils.hpp"
#include "../globalUtils.hpp"

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
    stackCtor(stack); //&?

    while (rawData[instuctionPtr]) {
        printf("cmd: %d\n", rawData[instuctionPtr]);
        processorData_t curNumCmd = rawData[instuctionPtr] >> COMMAND_OFFSET_CMD;

        switch (curNumCmd) {
            case 1: {
                processorData_t fullCurCmd  = rawData[instuctionPtr],
                                nextFullCmd = rawData[instuctionPtr + 1],
                                curCmdFlags = fullCurCmd & FLAGS_RANGE_CMD;

                if (IMMEDIATE_CONST_CMD == curCmdFlags) {
                    stackPush(&stack, nextFullCmd);
                }
                else if (REGISTER_CMD == curCmdFlags) {
                    stackPush(&stack, registers[nextFullCmd & REGISTER_VALUE_ARG]);
                }
                else if ((IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
                    stackPush(&stack, getFromRam(ram, nextFullCmd & ARGUMENT_VALUE_RANGE_ARG));
                }
                else if ((REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
                    stackPush(&stack, getFromRam(ram, registers[nextFullCmd & REGISTER_VALUE_ARG]));
                }
                else if ((REGISTER_CMD | IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
                    processorData_t registerValue = registers[nextFullCmd & REGISTER_VALUE_ARG],
                                    argumentValue = nextFullCmd & ARGUMENT_VALUE_RANGE_ARG;
                    stackPush(&stack, getFromRam(ram, registerValue + argumentValue));
                }
                else {
                    printf("Unknown flag set. Command = %d\n", fullCurCmd);
                    abort();
                }

                ++instuctionPtr;
                break;
            }
            case 2: {
                processorData_t fullCurCmd  = rawData[instuctionPtr],
                                nextFullCmd = rawData[instuctionPtr + 1],
                                curCmdFlags = fullCurCmd & FLAGS_RANGE_CMD;

                /*if (IMMEDIATE_CONST_CMD == curCmdFlags) {
                    stackPush(&stack, nextFullCmd);
                }
                else*/
                if (REGISTER_CMD == curCmdFlags) {
                    stackPop(&stack, &registers[nextFullCmd & REGISTER_VALUE_ARG]);
                }
                else if ((IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
                    stackPop(&stack, &ram[nextFullCmd & ARGUMENT_VALUE_RANGE_ARG]);
                }
                else if ((REGISTER_CMD | MEMORY_CMD) == curCmdFlags) {
                    stackPop(&stack, &ram[registers[nextFullCmd & REGISTER_VALUE_ARG]]);
                }
                else if ((REGISTER_CMD | IMMEDIATE_CONST_CMD | MEMORY_CMD) == curCmdFlags) {
                    processorData_t registerValue = registers[nextFullCmd & REGISTER_VALUE_ARG],
                                    argumentValue = nextFullCmd & ARGUMENT_VALUE_RANGE_ARG;
                    stackPop(&stack, &ram[registerValue + argumentValue]);
                }
                else if (0 == curCmdFlags) {
                    processorData_t value;
                    stackPop(&stack, &value);
                }
                else {
                    printf("Unknown flag set. Command = %d\n", fullCurCmd);
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

processorData_t getFromRam(processorData_t ram[], size_t addr) {
    usleep(RAM_ACCESS_DELAY_MS);
    return ram[addr];
}

void putInRam(processorData_t ram[], size_t addr, processorData_t value) {
    usleep(RAM_ACCESS_DELAY_MS);
    ram[addr] = value;
}