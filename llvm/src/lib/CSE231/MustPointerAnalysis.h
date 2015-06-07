#include "ConstantLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class PointerAnalysis {
public:
	ConstantPropAnalysis(Instruction * inst, ConstantLattice * incoming);
    Instruction * getInstruction();
    ConstantLattice * getOutgoingEdge();
	ConstantLattice * getOutgoingEdge(BasicBlock * toSuccessor);
    void setIncomingEdge(ConstantLattice * incoming);
	bool isConditionalBranch();
    void applyFlowFunction();
    void dump();
    
    static ConstantLattice * merge(ConstantLattice * edge_1, ConstantLattice * edge_2);
    static bool equal(ConstantLattice * edge1, ConstantLattice * edge2);

private:
	Instruction * _instruction;
	ConstantLattice * _incomingEdge;
	ConstantLattice * _outgoingEdge;
	ConstantLattice * _outgoingTrueEdge;
	ConstantLattice * _outgoingFalseEdge;
	bool _isConditionalBranch;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
    void handleBinaryOp(Instruction * inst);
	void handleConditionalBranchInst(BranchInst * inst);
    ConstantInt * tryGetConstantValue(Value * value);
};

