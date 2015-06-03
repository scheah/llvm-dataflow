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
    int value;
    if (!tryGetConstantValue(storeInst->getValueOperand(), &value)) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return;
    }

    string name = storeInst->getPointerOperand()->getName().str();
    _outgoingEdge[name] = value;
}

bool ConstantPropAnalysis::tryGetConstantValue(Value * value, int * constant) {
    ConstantInt * CI = dyn_cast<ConstantInt>(value);
    if (CI && CI->getBitWidth() <= 32) {
        *constant = CI->getSExtValue();
        return true;
    }

    map<string,int>::iterator iterator = _incomingEdge.find(value->getName().str());

    if (iterator != _incomingEdge.end()) {
        *constant = iterator->second;
        return true;
    }

    return false;
}

void ConstantPropAnalysis::handleBinaryOp(Instruction * inst) {
    int operand1;
    int operand2;

    if (!tryGetConstantValue(inst->getOperand(0), &operand1) || !tryGetConstantValue(inst->getOperand(1), &operand2)) {
        errs() << "No constant in binary op or variable is not in incoming edge.\n";
        return;
    }

    switch (inst->getOpcode()) {
        case Instruction::Add:
            //_outgoingEdge[variable] = operand1 + operand2;
            break;

        case Instruction::Mul:
            //_outgoingEdge[variable] = operand1 * operand2;
            break;

        case Instruction::Sub:
            //_outgoingEdge[variable] = operand1 - operand2;
            break;

        case Instruction::SDiv:
        case Instruction::UDiv:
            //_outgoingEdge[variable] = operand1 / operand2;
            break;
    }
}

