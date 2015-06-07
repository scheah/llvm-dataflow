#include "MustPointerLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class MustPointerAnalysis {
public:
	MustPointerAnalysis(Instruction * inst, MustPointerLattice * incoming);
    Instruction * getInstruction();
    MustPointerLattice * getOutgoingEdge();
	MustPointerLattice * getOutgoingEdge(BasicBlock * toSuccessor);
    void setIncomingEdge(MustPointerLattice * incoming);
	bool isConditionalBranch();
    void applyFlowFunction();
    void dump();
    
    static MustPointerLattice * merge(MustPointerLattice * edge_1, MustPointerLattice * edge_2);
    static bool equal(MustPointerLattice * edge1, MustPointerLattice * edge2);

private:
	Instruction * _instruction;
	MustPointerLattice * _incomingEdge;
	MustPointerLattice * _outgoingEdge;
	MustPointerLattice * _outgoingTrueEdge;
	MustPointerLattice * _outgoingFalseEdge;
	bool _isConditionalBranch;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
};

