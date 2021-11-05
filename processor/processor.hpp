#include <cstdlib>
#include "../globalUtils.hpp"
#include "../processorUtils.hpp"

const size_t RAM_LEN = 1000;
const size_t RAM_ACCESS_DELAY_MS = 150;

processorData_t* accessRam(processorData_t ram[], processorData_t addr);
processorData_t* getCmdDestByFlags(processorData_t *curCmdPtr, processorData_t registers[], processorData_t ram[]);
ProcErrorCodes execBinCode(processorData_t *codeArr);