#include "DataFlowFact.h"

const char * DataflowFact::getVariableName() {
    return _variableName;
}

int ConstantDataflowFact::getValue() {
    return _value;
}

