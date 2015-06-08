#include "PointerLattice.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"


using namespace llvm;
using namespace std;

class PointerAnalysis {
public:
    Instruction * getInstruction();
    PointerLattice< map<string, set<Value*,valueComp> > > * getOutgoingEdge();
	PointerLattice< map<string, set<Value*,valueComp> > > * getOutgoingEdge(BasicBlock * toSuccessor);
    void setIncomingEdge(PointerLattice< map<string, set<Value*,valueComp> > > * incoming);
	bool isConditionalBranch();
    void applyFlowFunction();
    void dump();
    
    static bool equal(PointerLattice< map<string, set<Value*,valueComp> > > * edge1, PointerLattice< map<string, set<Value*,valueComp> > > * edge2);

protected:
	PointerAnalysis(Instruction * inst, PointerLattice< map<string, set<Value*,valueComp> > > * incoming);

	Instruction * _instruction;
	PointerLattice< map<string, set<Value*,valueComp> > > * _incomingEdge;
	PointerLattice< map<string, set<Value*,valueComp> > > * _outgoingEdge;
	PointerLattice< map<string, set<Value*,valueComp> > > * _outgoingTrueEdge;
	PointerLattice< map<string, set<Value*,valueComp> > > * _outgoingFalseEdge;
	bool _isConditionalBranch;
    void handleStoreInst(StoreInst * storeInst);
    void handleLoadInst(LoadInst * loadInst);
};

class MustPointerAnalysis : public PointerAnalysis {
public:
	MustPointerAnalysis(Instruction * inst, PointerLattice< map<string, set<Value*,valueComp> > > * incoming);
    
    static PointerLattice< map<string, set<Value*,valueComp> > > * merge(PointerLattice< map<string, set<Value*,valueComp> > > * edge_1, PointerLattice< map<string, set<Value*,valueComp> > > * edge_2);
};

class MayPointerAnalysis : public PointerAnalysis {
public:
	MayPointerAnalysis(Instruction * inst, PointerLattice< map<string, set<Value*,valueComp> > > * incoming);
    
    static PointerLattice< map<string, set<Value*,valueComp> > > * merge(PointerLattice< map<string, set<Value*,valueComp> > > * edge_1, PointerLattice< map<string, set<Value*,valueComp> > > * edge_2);
};

