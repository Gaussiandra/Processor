#include <cstdio>
#include <cstdlib>
#include "assembler.hpp"

int main(const int argc, const char *argv[]) {
    const char *inpFilePath = nullptr, *outFilePath = nullptr;
    RETURN_ON_ERROR(get2StrArgs(argc, argv, &inpFilePath, &outFilePath));

    char *rawData = nullptr;
    stringData *strings = nullptr;
    size_t nStrings = 0;
    RETURN_ON_ERROR(prepareStringsFromPath(inpFilePath, &rawData, &strings, &nStrings));

    processorData_t outputArray[MAX_OUTPUT_ARRAY_LEN] = {0};
    size_t nArrayElements = 0;
    RETURN_ON_ERROR(mapTextToCodes(strings, nStrings, outputArray, &nArrayElements));

    RETURN_ON_ERROR(saveArrayToBinFile((char*) outputArray, nArrayElements, sizeof(processorData_t), outFilePath));
    
    free(rawData);
    rawData = nullptr;
    free(strings);
    strings = nullptr;

    printf("Done!\n");

    return 0;
}