#include "ConstantPropAnalysis.h"
#include "llvm/Support/raw_ostream.h"

ConstantPropAnalysis::ConstantPropAnalysis(Instruction * inst, map<string, int> incoming) {
	_instruction = inst;
	_incomingEdge = incoming;
}


void ConstantPropAnalysis::applyFlowFunction() {
    if (StoreInst::classof(_instruction)) {
		_outgoingEdge.clear(); 
		_outgoingEdge.insert(_incomingEdge.begin(), _incomingEdge.end());
        handleStoreInst((StoreInst *) _instruction);
    }
    else if (_instruction->isBinaryOp()) {
            
    }
	else {//temp 
		_outgoingEdge.clear(); 
		_outgoingEdge.insert(_incomingEdge.begin(), _incomingEdge.end());
	}
}

Instruction * ConstantPropAnalysis::getInstruction() {
    return _instruction;
}

map<string,int> ConstantPropAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

void ConstantPropAnalysis::setIncomingEdge(map<string,int> incoming) {
    _incomingEdge.clear();    
    _incomingEdge.insert(incoming.begin(), incoming.end());
}

map<string,int> ConstantPropAnalysis::merge(map<string,int> edge1, map<string,int> edge2) {
    map<string,int> outgoingEdge;

    for (map<string,int>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isEqualInBothEdges = false;
		
        for (map<string,int>::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges and are equal, add to outgoing edge
            if ( (i->first == j->first) && (i->second == j->second) ) {
                edge2.erase(i->first);
                isEqualInBothEdges = true;
                break;
            }
        }

        if (isEqualInBothEdges) {
            outgoingEdge[i->first] = i->second;
        }
    }

    return outgoingEdge;
}

bool ConstantPropAnalysis::equal(map<string,int> edge1, map<string,int> edge2) {
    if(edge1.size() != edge2.size())
        return false;

    for (map<string,int>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        if (edge1[i->first] != edge2[i->first])
            return false;
    }

    return true;
}

void ConstantPropAnalysis::dump() {
    errs() << "\t\t\tINCOMING:\n";
    for (map<string,int>::iterator i = _incomingEdge.begin(); i != _incomingEdge.end(); i++) {
        errs() << "\t\t\t" << i->first << "  " << i->second << "\n";
    }
    errs() << "\t\t\tOUTGOING:\n";
    for (map<string,int>::iterator i = _outgoingEdge.begin(); i != _outgoingEdge.end(); i++) {
        errs() << "\t\t\t" << i->first << "  " << i->second << "\n";
    }
    errs() << "\t\t--------------------------------------------------------\n";
}

void ConstantPropAnalysis::reset() {
    _outgoingEdge.clear();
}

void ConstantPropAnalysis::handleStoreInst(StoreInst * storeInst) {
    ConstantInt* CI = tryGetConstant(storeInst->getValueOperand());
    if (CI == NULL) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return;
    }

    string name = storeInst->getPointerOperand()->getName().str();
    _outgoingEdge[name] = CI->getSExtValue();
}

ConstantInt * ConstantPropAnalysis::tryGetConstant(Value * value) {
    ConstantInt* CI = dyn_cast<ConstantInt>(value);
    if (!CI || CI->getBitWidth() > 32) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return NULL;
    }

    return CI;
}

void ConstantPropAnalysis::handleBinaryOp(Instruction * inst) {
    ConstantInt* CI = tryGetConstant(inst->getOperand(0));
    string variable = inst->getOperand(1)->getName().str();
    int position = 1;

    if (CI == NULL) {
        CI = tryGetConstant(inst->getOperand(1));
        variable = inst->getOperand(0)->getName().str();
        position = 0;
    }

    if (CI == NULL || _incomingEdge.count(variable) == 0) {
        errs() << "No constant in binary op or variable is not in incoming edge.\n";
        return;
    }

    switch (inst->getOpcode()) {
        case Instruction::Add:
            _outgoingEdge[variable] = CI->getSExtValue() + _incomingEdge[variable]; 
            break;

        case Instruction::Mul:
            _outgoingEdge[variable] = CI->getSExtValue() * _incomingEdge[variable];
            break;

        case Instruction::Sub:
            if (position == 1) {
                _outgoingEdge[variable] = CI->getSExtValue() - _incomingEdge[variable];
            }
            else {
                _outgoingEdge[variable] = _incomingEdge[variable] - CI->getSExtValue();
            }
            break;

        case Instruction::SDiv:
        case Instruction::UDiv:
            if (position == 1) {
                _outgoingEdge[variable] = CI->getSExtValue() / _incomingEdge[variable];
            }
            else {
                _outgoingEdge[variable] = _incomingEdge[variable] / CI->getSExtValue();
            }
            break;
    }
}

