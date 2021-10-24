#include <cstdio>
#include <cstdlib>
#include "assembler.hpp"

int main(const int argc, const char *argv[]) {
    char *inpFilePath = nullptr, *outFilePath = nullptr;
    if (argc == 3) {
        inpFilePath = (char*) argv[1];
        outFilePath = (char*) argv[2];
    }
    else {
        printf("Invalid command line arguments.\nUsage: main.out input_path output_path\n");
        return ARGUMENTS_HANDLING_ERROR;
    }

    char *rawData = nullptr;
    stringData *strings = nullptr;
    size_t nStrings = 0;
    CONTINUE_IFN0(getRawDataFromFile(inpFilePath, &rawData, &strings, &nStrings));

    processorData_t outputArray[MAX_OUTPUT_ARRAY_LEN] = {0};
    size_t nArrayElements = mapTextToCodes(strings, nStrings, outputArray);

    CONTINUE_IFN0(saveArrayToFile(outputArray, nArrayElements, outFilePath));
    
    free(rawData);
    rawData = nullptr;
    free(strings);
    strings = nullptr;

    return 0;
}