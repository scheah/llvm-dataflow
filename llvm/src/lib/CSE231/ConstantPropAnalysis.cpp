#include "ConstantPropAnalysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

void ConstantPropAnalysis::applyFlowFunction(Instruction * instruction) {
    if (StoreInst::classof(instruction)) {
        handleStoreInst((StoreInst *) &(*instruction));
    }
}

void ConstantPropAnalysis::dump() {
    for (map<string,unsigned>::iterator i = _map.begin(); i != _map.end(); i++) {
        errs() << i->first << "  " << i->second << "\n";
    }
}

void ConstantPropAnalysis::reset() {
    _map.clear();
}

void ConstantPropAnalysis::handleStoreInst(StoreInst * storeInst) {

    ConstantInt* CI = dyn_cast<ConstantInt>(storeInst->getValueOperand());
    if (!CI || CI->getBitWidth() > 32) {
        return;
    }

    string name = storeInst->getPointerOperand()->getName().str();
    if (_map.count(name) == 1) {
        _map.insert(pair<string, unsigned>(name, CI->getSExtValue()));
    }
    else {
        _map[name] = CI->getSExtValue();
    }
}

