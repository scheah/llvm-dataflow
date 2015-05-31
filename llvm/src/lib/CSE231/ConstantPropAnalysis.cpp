#include "ConstantPropAnalysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

ConstantPropAnalysis::ConstantPropAnalysis(Instruction * inst, map<string, unsigned> incoming) {
	_instruction = inst;
	_incomingEdge = incoming;
}


void ConstantPropAnalysis::applyFlowFunction() {
    if (StoreInst::classof(_instruction)) {
		_outgoingEdge.clear(); 
		_outgoingEdge.insert(_incomingEdge.begin(), _incomingEdge.end());
        handleStoreInst((StoreInst *) _instruction);
    }
}

map<string,unsigned> ConstantPropAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

void ConstantPropAnalysis::dump() {
    for (map<string,unsigned>::iterator i = _outgoingEdge.begin(); i != _outgoingEdge.end(); i++) {
        errs() << i->first << "  " << i->second << "\n";
    }
}

void ConstantPropAnalysis::reset() {
    _outgoingEdge.clear();
}

void ConstantPropAnalysis::handleStoreInst(StoreInst * storeInst) {

    ConstantInt* CI = dyn_cast<ConstantInt>(storeInst->getValueOperand());
    if (!CI || CI->getBitWidth() > 32) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return;
    }
    string name = storeInst->getPointerOperand()->getName().str();
    _outgoingEdge[name] = CI->getSExtValue();
}

