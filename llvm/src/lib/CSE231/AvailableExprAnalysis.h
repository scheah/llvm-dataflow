#include "AvailableExprLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class AvailableExprAnalysis {
public:
	AvailableExprAnalysis(Instruction * inst, AvailableExprLattice * incoming);
    Instruction * getInstruction();
    AvailableExprLattice * getOutgoingEdge();
    void setIncomingEdge(AvailableExprLattice * incoming);
    void applyFlowFunction();
    void dump();
    
    static AvailableExprLattice * merge(AvailableExprLattice * edge_1, AvailableExprLattice * edge_2);
    static bool equal(AvailableExprLattice * edge1, AvailableExprLattice * edge2);
private:
	Instruction * _instruction;
	AvailableExprLattice * _incomingEdge;
	AvailableExprLattice * _outgoingEdge;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
    void handleBinaryOp(Instruction * inst);
    ConstantInt * tryGetConstantValue(Value * value);
};

