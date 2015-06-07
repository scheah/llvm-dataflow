#include "RangeLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class RangeAnalysis {
public:
	RangeAnalysis(Instruction * inst, RangeLattice * incoming);
    Instruction * getInstruction();
    RangeLattice * getOutgoingEdge();
	RangeLattice * getOutgoingEdge(BasicBlock * toSuccessor);
    void setIncomingEdge(RangeLattice * incoming);
	bool isConditionalBranch();
    void applyFlowFunction();
    void dump();
    
    static RangeLattice * merge(RangeLattice * edge_1, RangeLattice * edge_2);
    static bool equal(RangeLattice * edge1, RangeLattice * edge2);
private:
	Instruction * _instruction;
	RangeLattice * _incomingEdge;
	RangeLattice * _outgoingEdge;
	RangeLattice * _outgoingTrueEdge;
	RangeLattice * _outgoingFalseEdge;
	bool _isConditionalBranch;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
    void handleBinaryOp(Instruction * inst);
	void handleConditionalBranchInst(BranchInst * inst);
};

