#include "ConstantPropAnalysis.h"
#include "llvm/Support/raw_ostream.h"

ConstantPropAnalysis::ConstantPropAnalysis(Instruction * inst, ConstantLattice * incoming) {
	_instruction = inst;
	map<string,ConstantInt*> empty;
	_incomingEdge = new ConstantLattice(false,true,empty);
	*_incomingEdge = *incoming;
	_outgoingEdge = new ConstantLattice(false,true,empty);
}

void ConstantPropAnalysis::applyFlowFunction() {
    if (StoreInst::classof(_instruction)) {
        handleStoreInst((StoreInst *) _instruction);
    }
    else if (LoadInst::classof(_instruction)) {
        handleLoadInst((LoadInst *) _instruction);
    }
    else if (_instruction->isBinaryOp()) {
        handleBinaryOp(_instruction);            
    }
	else {//temp 
		*_outgoingEdge = *_incomingEdge;
	}
}

Instruction * ConstantPropAnalysis::getInstruction() {
    return _instruction;
}

ConstantLattice * ConstantPropAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

void ConstantPropAnalysis::setIncomingEdge(ConstantLattice * incoming) {    
    *_incomingEdge = *incoming;
}

// will be a join
ConstantLattice * ConstantPropAnalysis::merge(ConstantLattice * edge_1, ConstantLattice * edge_2) {
	map<string,ConstantInt*> outgoingEdge;
	if (edge_1->isTop() || edge_2->isTop()) {
		return new ConstantLattice(true, false, outgoingEdge); // return Top
	}
	else if (edge_1->isBottom() && edge_2->isBottom()) {
		return new ConstantLattice(false, true, outgoingEdge);
	}
	else if (edge_1->isBottom()) {
		return new ConstantLattice(false, false, edge_2->getFacts());
	}
	else if (edge_2->isBottom()) {
		return new ConstantLattice(false, false, edge_1->getFacts());
	}
	map<string, ConstantInt *> edge1 = edge_1->getFacts();
	map<string, ConstantInt *> edge2 = edge_2->getFacts();
	
    for (map<string, ConstantInt *>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isEqualInBothEdges = false;
        for (map<string, ConstantInt *>::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges and are equal, add to outgoing edge
            if ( (i->first == j->first) && (i->second == j->second) ) {
                //edge2.erase(i->first);
                isEqualInBothEdges = true;
                break;
            }
        }
        if (isEqualInBothEdges) {
            outgoingEdge[i->first] = i->second;
        }
    }
    return new ConstantLattice(false, false, outgoingEdge);
}

bool ConstantPropAnalysis::equal(ConstantLattice * edge_1, ConstantLattice * edge_2) {
	if(edge_1->isTop() && edge_2->isTop()) 
		return true;
	else if(edge_1->isBottom() && edge_2->isBottom())
		return true;
	map<string, ConstantInt *> edge1 = edge_1->getFacts();
	map<string, ConstantInt *> edge2 = edge_2->getFacts();
    if(edge1.size() != edge2.size())
        return false;
    for (map<string, ConstantInt *>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        if (edge1[i->first] != edge2[i->first])
            return false;
    }

    return true;
}

void ConstantPropAnalysis::dump() {
    errs() << "\t\t\tINCOMING:\n";
    _incomingEdge->dump();

    errs() << "\t\t\tOUTGOING:\n";
    _outgoingEdge->dump();
    
    errs() << "\t\t--------------------------------------------------------\n";
}

void ConstantPropAnalysis::handleStoreInst(StoreInst * storeInst) {
    ConstantInt* CI = tryGetConstantValue(storeInst->getValueOperand());
    if (CI == NULL) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return;
    }

    string name = storeInst->getPointerOperand()->getName().str();
	map<string, ConstantInt *> incomingEdge = _incomingEdge->getFacts(); 
    incomingEdge[name] = CI;
	_outgoingEdge->setNewFacts(false,false,incomingEdge);
}

void ConstantPropAnalysis::handleLoadInst(LoadInst * loadInst) {
    map<string, ConstantInt *> edgeMap = _incomingEdge->getFacts();

    edgeMap[loadInst->getOperandUse(0).getUser()->getName().str()] = edgeMap[loadInst->getOperand(0)->getName().str()];
    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

ConstantInt * ConstantPropAnalysis::tryGetConstantValue(Value * value) {
    ConstantInt * CI = dyn_cast<ConstantInt>(value);
    if (CI && CI->getBitWidth() <= 32) {
        return CI;
    }

    map<string,ConstantInt*>::iterator iterator = _incomingEdge->getFacts().find(value->getName().str());

    if (iterator != _incomingEdge->getFacts().end()) {
        return iterator->second;
    }

    return NULL;
}

void ConstantPropAnalysis::handleBinaryOp(Instruction * inst) {
    ConstantInt * operandConstant1 = tryGetConstantValue(inst->getOperand(0));
    ConstantInt * operandConstant2 = tryGetConstantValue(inst->getOperand(1));

    errs() << "Operands:  " <<  inst->getOperand(0)->getName().str() << "\t" << inst->getOperand(1)->getName().str() <<
    "\tdest" << inst->getOperandUse(0).getUser()->getName().str() << "\n";

    if (operandConstant1 == NULL || operandConstant2 == NULL) {
        errs() << "No constant in binary op or variable is not in incoming edge.\n";
        return;
    }

    int64_t operand1 = operandConstant1->getSExtValue();
    int64_t operand2 = operandConstant2->getSExtValue();
    int64_t result;

    switch (inst->getOpcode()) {
        case Instruction::Add:
            result = operand1 + operand2;
            break;

        case Instruction::Mul:
            result = operand1 * operand2;
            break;

        case Instruction::Sub:
            result = operand1 - operand2;
            break;

        case Instruction::SDiv:
        case Instruction::UDiv:
            result = operand1 / operand2;
            break;
    }

    IntegerType * integerType = IntegerType::get(inst->getContext(), 32);
    ConstantInt * resultConstant = ConstantInt::getSigned(integerType, result);
    map<string,ConstantInt*> constantMap = _incomingEdge->getFacts();
    string dest = inst->getOperandUse(0).getUser()->getName().str();

    constantMap[dest] = resultConstant;
    _outgoingEdge->setNewFacts(false, false, constantMap);
}

