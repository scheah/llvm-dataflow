#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace std;

class ConstantPropAnalysis {
public:
	ConstantPropAnalysis(Instruction * inst, map<string, unsigned> incoming);
    Instruction * getInstruction();
    map<string, unsigned> getOutgoingEdge();
    void setIncomingEdge(map<string,unsigned> incoming);
    void applyFlowFunction();
    void dump();
    
    static map<string,unsigned> merge(map<string,unsigned> edge1, map<string,unsigned> edge2);
    static bool equal(map<string,unsigned> edge1, map<string,unsigned> edge2);
private:
	Instruction * _instruction;
	map<string, unsigned> _incomingEdge;
	map<string, unsigned> _outgoingEdge;
    void reset();
    void handleStoreInst(StoreInst * storeInst);

};

