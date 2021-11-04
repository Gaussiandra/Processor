#include <cstdio>
#include <cstdlib>
#include "assembler.hpp"

int main(const int argc, const char *argv[]) {
    char *inpFilePath = nullptr, *outFilePath = nullptr;
    CONTINUE_IFN0(get2StrArgs(argc, argv, &inpFilePath, &outFilePath));

    char *rawData = nullptr;
    stringData *strings = nullptr;
    size_t nStrings = 0;
    CONTINUE_IFN0(prepareStringsFromPath(inpFilePath, &rawData, &strings, &nStrings));

    processorData_t outputArray[MAX_OUTPUT_ARRAY_LEN] = {0};
    size_t nArrayElements = 0;
    CONTINUE_IFN0(mapTextToCodes(strings, nStrings, outputArray, &nArrayElements));

    CONTINUE_IFN0(saveArrayToBinFile(outputArray, nArrayElements, outFilePath));
    
    free(rawData);
    rawData = nullptr;
    free(strings);
    strings = nullptr;

    printf("Done!\n");

    return 0;
}