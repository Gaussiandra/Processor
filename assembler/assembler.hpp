#include <cstdio>
#include "../processorUtils.hpp"
#include "../globalUtils.hpp"

const size_t MAX_INPUT_STR_LEN    = 100;
const size_t MAX_OUTPUT_ARRAY_LEN = 1000;

size_t mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[]);
ProcErrorCodes parseArgument(char *string, size_t cmdLen, processorData_t *idxCmd, processorData_t *arg, size_t *nChanged);
ProcErrorCodes saveArrayToBinFile(processorData_t outputArray[], size_t nArrayElements, char *outFilePath);