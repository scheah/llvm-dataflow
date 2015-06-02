#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace std;

class ConstantPropAnalysis {
public:
	ConstantPropAnalysis(Instruction * inst, map<string, int> incoming);
    Instruction * getInstruction();
    map<string, int> getOutgoingEdge();
    void setIncomingEdge(map<string,int> incoming);
    void applyFlowFunction();
    void dump();
    
    static map<string,int> merge(map<string,int> edge1, map<string,int> edge2);
    static bool equal(map<string,int> edge1, map<string,int> edge2);
private:

	Instruction * _instruction;
	map<string, int> _incomingEdge;
	map<string, int> _outgoingEdge;
    void reset();
    void handleStoreInst(StoreInst * storeInst);
    void handleBinaryOp(Instruction * inst);
    ConstantInt * tryGetConstant(Value * value);

};

