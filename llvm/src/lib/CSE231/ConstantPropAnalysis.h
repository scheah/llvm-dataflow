#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace std;

class ConstantPropAnalysis {
public:
	ConstantPropAnalysis(Instruction * inst, map<string, unsigned> incoming);
    void applyFlowFunction();
    void dump();
private:
	Instruction * _instruction;
	map<string, unsigned> _incomingEdge;
	map<string, unsigned> _outgoingEdge;
    void reset();
    void handleStoreInst(StoreInst * storeInst);

};

