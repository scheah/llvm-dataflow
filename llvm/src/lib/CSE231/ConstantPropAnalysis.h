#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace std;

class Analysis {
public:
    virtual void applyFlowFunction(Instruction * instruction) = 0;

};

class ConstantPropAnalysis : Analysis {
public:
    void applyFlowFunction(Instruction * instruction);
    void dump();

private:
    void reset();
    void handleStoreInst(StoreInst * storeInst);
    

    map<string, unsigned> _map;

};

