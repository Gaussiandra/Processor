#include <cstdio>
#include "../processorUtils.hpp"
#include "../globalUtils.hpp"

const processorData_t TEMP_CMD    = -1;
const size_t MAX_INPUT_STR_LEN    = 100;
const size_t MAX_LABEL_COUNT      = 25;
const size_t MAX_OUTPUT_ARRAY_LEN = 1000;

struct label_t {
    int instructionPtr = -1;
    char labelName[MAX_INPUT_STR_LEN] = {0};
};

ProcErrorCodes mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[], size_t *nArrayElements);
ProcErrorCodes parseNumericArgument(char *string, size_t cmdLen, processorData_t *idxCmd, processorData_t *arg, size_t *nChanged);
ProcErrorCodes saveArrayToBinFile(processorData_t outputArray[], size_t nArrayElements, char *outFilePath);
ProcErrorCodes getIdxByName(label_t arr[], size_t arrSize, size_t *indexToReturn, char name[]);