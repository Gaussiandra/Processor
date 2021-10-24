#include <cstdlib>
#include "../processorUtils.hpp"

const size_t RAM_LEN = 1000;
const size_t RAM_ACCESS_DELAY_MS = 150;

processorData_t getFromRam(processorData_t ram[], size_t addr);
void putInRam(processorData_t ram[], size_t addr, processorData_t value);