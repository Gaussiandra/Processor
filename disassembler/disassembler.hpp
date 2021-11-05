#include <cstdlib>
#include "../processorUtils.hpp"
#include "../globalUtils.hpp"

const size_t INT_TO_STR_BUFF_SZ = 20;
const size_t MAX_LABEL_COUNT    = 25;

ProcErrorCodes execFirstPass(size_t codeLen, processorData_t codeArr[], int labels[]);
ProcErrorCodes execSecondPass(size_t codeLen, processorData_t codeArr[], int labels[], FILE *outputFile);
ProcErrorCodes convertNumericCmd(processorData_t curCmd, processorData_t nextCmd, char strToReturn[], bool *hasArg);
ProcErrorCodes insertUniqueIdx(int array[], int szArr, int value);
int getIdxFromArr(int array[], int szArr, int value);