#include <cstdio>
#include <cstdlib>
#include "../SaveStack/stack.hpp"
#include "../processorUtils.hpp"
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
    size_t nCommands = szFile / sizeof(processorData_t);

    for (int i = 0; i < nCommands; ++i)
        printf("%d\n", rawData[i]);
}