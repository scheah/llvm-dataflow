#include "ConstantLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class ConstantPropAnalysis {
public:
	ConstantPropAnalysis(Instruction * inst, ConstantLattice * incoming);
    Instruction * getInstruction();
    ConstantLattice * getOutgoingEdge();
    void setIncomingEdge(ConstantLattice * incoming);
    void applyFlowFunction();
    void dump();
    
    static ConstantLattice * merge(ConstantLattice * edge_1, ConstantLattice * edge_2);
    static bool equal(ConstantLattice * edge1, ConstantLattice * edge2);
private:
	Instruction * _instruction;
	ConstantLattice * _incomingEdge;
	ConstantLattice * _outgoingEdge;
    void handleStoreInst(StoreInst * storeInst);
    void handleBinaryOp(Instruction * inst);
    bool tryGetConstantValue(Value * value, int * constant);
};

