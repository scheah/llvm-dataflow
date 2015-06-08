#include "PointerLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

/*
struct valueComp {
    bool operator()(Value* const a, Value* const b) {
        return a->getName() < b->getName();
    }
}
*/

class PointerAnalysis {
public:
    Instruction * getInstruction();
    PointerLattice * getOutgoingEdge();
	PointerLattice * getOutgoingEdge(BasicBlock * toSuccessor);
    void setIncomingEdge(PointerLattice * incoming);
	bool isConditionalBranch();
    void applyFlowFunction();
    void dump();
    
    static bool equal(PointerLattice * edge1, PointerLattice * edge2);

protected:
	PointerAnalysis(Instruction * inst, PointerLattice * incoming);

	Instruction * _instruction;
	PointerLattice * _incomingEdge;
	PointerLattice * _outgoingEdge;
	PointerLattice * _outgoingTrueEdge;
	PointerLattice * _outgoingFalseEdge;
	bool _isConditionalBranch;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
};

class MustPointerAnalysis : public PointerAnalysis {
public:
	MustPointerAnalysis(Instruction * inst, PointerLattice * incoming);
    
    static PointerLattice * merge(PointerLattice * edge_1, PointerLattice * edge_2);
};

class MayPointerAnalysis : public PointerAnalysis {
public:
	MayPointerAnalysis(Instruction * inst, PointerLattice * incoming);
    
    static PointerLattice * merge(PointerLattice * edge_1, PointerLattice * edge_2);
};

