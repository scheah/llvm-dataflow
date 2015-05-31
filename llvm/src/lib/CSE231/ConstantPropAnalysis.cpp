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

map<string,unsigned> ConstantPropAnalysis::merge(map<string,unsigned> edge1, map<string,unsigned> edge2) {
    map<string,unsigned> outgoingEdge;

    for (map<string,unsigned>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isInBothEdges = false;

        for (map<string,unsigned>::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges, erase from outgoing edge and the second map.
            if (i->first == j->first) {
                edge2.erase(i->first);
                isInBothEdges = true;
                break;
            }
        }

        if (!isInBothEdges) {
            outgoingEdge[i->first] = i->second;
        }
    }

    // Add any remaining items in second map.
    for (map<string,unsigned>::iterator i = edge2.begin(); i != edge2.end(); i++) {
        outgoingEdge[i->first] = i->second;
    }

    return outgoingEdge;
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

