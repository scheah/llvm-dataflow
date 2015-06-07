#include "MustPointerAnalysis.h"
#include "llvm/Support/raw_ostream.h"

MustPointerAnalysis::MustPointerAnalysis(Instruction * inst, MustPointerLattice * incoming) {
	_instruction = inst;
	map<string,Value*> empty;
	_incomingEdge = new MustPointerLattice(false,true,empty);
	*_incomingEdge = *incoming;
	_outgoingEdge = new MustPointerLattice(false,true,empty);
	_outgoingTrueEdge = new MustPointerLattice(false,true,empty);
	_outgoingFalseEdge = new MustPointerLattice(false,true,empty);
	if(BranchInst::classof(_instruction)) {
		BranchInst * branchInst = (BranchInst *)_instruction;
		if (branchInst->isConditional()) 
			_isConditionalBranch = true;
		else 
			_isConditionalBranch = false;
	}
	else
		_isConditionalBranch = false;
}

bool MustPointerAnalysis::isConditionalBranch() {
	return _isConditionalBranch;
}

void MustPointerAnalysis::applyFlowFunction() {
    if (StoreInst::classof(_instruction)) {
        //handleStoreInst((StoreInst *) _instruction);
    }
    else if (LoadInst::classof(_instruction)) {
        //handleLoadInst((LoadInst *) _instruction);
    }
    else if (_instruction->isBinaryOp()) {
        //handleBinaryOp(_instruction);            
    }
	else if (_isConditionalBranch) {
		//handleConditionalBranchInst((BranchInst *) _instruction);
	}
	else { //temp 
		*_outgoingEdge = *_incomingEdge;
	}
}

Instruction * MustPointerAnalysis::getInstruction() {
    return _instruction;
}

MustPointerLattice * MustPointerAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

MustPointerLattice * MustPointerAnalysis::getOutgoingEdge(BasicBlock * toSuccessor) {
	if(!_isConditionalBranch) {
		//errs() << "[MustPointerAnalysis::getOutgoingEdge(BasicBlock * toSuccessor)] not a conditional branch predecessor, return normal outgoing edge\n";
		return _outgoingEdge;
	}
	BranchInst * branchInst = (BranchInst * )_instruction;
	BasicBlock * trueBlock = branchInst->getSuccessor(0);
	BasicBlock * falseBlock = branchInst->getSuccessor(1);
	if (toSuccessor == trueBlock) {
		return _outgoingTrueEdge;
	}
	else if(toSuccessor == falseBlock) {
		return _outgoingFalseEdge;
	}
	else {
		errs() << "[MustPointerAnalysis::getOutgoingEdge(BasicBlock * toSuccessor)] the world is weird.\n";
	}
	return NULL;
}

void MustPointerAnalysis::setIncomingEdge(MustPointerLattice * incoming) {    
    *_incomingEdge = *incoming;
}

// will be a join
MustPointerLattice * MustPointerAnalysis::merge(MustPointerLattice * edge_1, MustPointerLattice * edge_2) {
	map<string,Value*> outgoingEdge;
	if (edge_1->isTop() || edge_2->isTop()) {
		return new MustPointerLattice(true, false, outgoingEdge); // return Top
	}
	else if (edge_1->isBottom() && edge_2->isBottom()) {
		return new MustPointerLattice(false, true, outgoingEdge);
	}
	else if (edge_1->isBottom()) {
		return new MustPointerLattice(false, false, edge_2->getFacts());
	}
	else if (edge_2->isBottom()) {
		return new MustPointerLattice(false, false, edge_1->getFacts());
	}
	map<string, Value *> edge1 = edge_1->getFacts();
	map<string, Value *> edge2 = edge_2->getFacts();
	
    for (map<string, Value *>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isEqualInBothEdges = false;
        for (map<string, Value *>::iterator j = edge2.begin(); j != edge2.end(); j++) {
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
    return new MustPointerLattice(false, false, outgoingEdge);
}

bool MustPointerAnalysis::equal(MustPointerLattice * edge_1, MustPointerLattice * edge_2) {
	if(edge_1->isTop() && edge_2->isTop()) 
		return true;
	else if(edge_1->isBottom() && edge_2->isBottom())
		return true;
	map<string, Value *> edge1 = edge_1->getFacts();
	map<string, Value *> edge2 = edge_2->getFacts();
    if(edge1.size() != edge2.size())
        return false;
    for (map<string, Value *>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        if (edge1[i->first] != edge2[i->first])
            return false;
    }

    return true;
}

void MustPointerAnalysis::dump() {
    errs() << "\t\t\tINCOMING:\n";
    _incomingEdge->dump();

	if (!_isConditionalBranch) {
		errs() << "\t\t\tOUTGOING:\n";
		_outgoingEdge->dump();
    }
	else {
		errs() << "\t\t\tOUTGOING (TRUE):\n";
		_outgoingTrueEdge->dump();
		errs() << "\t\t\tOUTGOING (FALSE):\n";
		_outgoingFalseEdge->dump();
	}
    errs() << "\t\t--------------------------------------------------------\n";
}

void MustPointerAnalysis::handleStoreInst(StoreInst * storeInst) {
	string name = storeInst->getPointerOperand()->getName().str();
	map<string, Value *> edgeMap = _incomingEdge->getFacts(); 
	edgeMap.erase(name);
    Value* CI = tryGetConstantValue(storeInst->getValueOperand());
    if (CI == NULL) {
		errs() << "[MustPointerAnalysis::handleStoreInst] not a Value\n";
    }
	else
		edgeMap[name] = CI;
	_outgoingEdge->setNewFacts(false,false,edgeMap);
}

void MustPointerAnalysis::handleLoadInst(LoadInst * loadInst) {
    map<string, Value *> edgeMap = _incomingEdge->getFacts();
	edgeMap.erase(loadInst->getOperandUse(0).getUser()->getName().str());
	if (edgeMap.count(loadInst->getOperand(0)->getName().str()) > 0)
    	edgeMap[loadInst->getOperandUse(0).getUser()->getName().str()] = edgeMap[loadInst->getOperand(0)->getName().str()];
    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

Value * MustPointerAnalysis::tryGetConstantValue(Value * value) {
    /*Value * CI = dyn_cast<Value>(value);
    if (CI && CI->getBitWidth() <= 32) {
        return CI;
    }

    map<string,Value*> edgeMap = _incomingEdge->getFacts();
    map<string,Value*>::iterator iterator = edgeMap.find(value->getName().str());

    if (iterator != edgeMap.end()) {
        return iterator->second;
    }*/

    return NULL;
}

void MustPointerAnalysis::handleBinaryOp(Instruction * inst) {
	map<string,Value*> constantMap = _incomingEdge->getFacts();
	string dest = inst->getOperandUse(0).getUser()->getName().str();
	constantMap.erase(dest);
    Value * operandConstant1 = tryGetConstantValue(inst->getOperand(0));
    Value * operandConstant2 = tryGetConstantValue(inst->getOperand(1));

    errs() << "Operands:  " <<  inst->getOperand(0)->getName().str() << "\t" << inst->getOperand(1)->getName().str() <<
    "\tdest" << inst->getOperandUse(0).getUser()->getName().str() << "\n";

    if (operandConstant1 == NULL || operandConstant2 == NULL) {
		_outgoingEdge->setNewFacts(false, false, constantMap);
        errs() << "No constant in binary op or variable is not in incoming edge.\n";
        return;
    }

    int64_t operand1;// = operandConstant1->getSExtValue();
    int64_t operand2;// = operandConstant2->getSExtValue();
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

        case Instruction::SRem:
        case Instruction::URem:
            result = operand1 % operand2;
            break;

        case Instruction::Or:
            result = operand1 | operand2;
            break;

        case Instruction::And:
            result = operand1 & operand2;
            break;

        case Instruction::Xor:
            result = operand1 ^ operand2;
            break;
    }

    IntegerType * integerType = IntegerType::get(inst->getContext(), 32);
    Value * resultConstant;// = Value::getSigned(integerType, result);
    constantMap[dest] = resultConstant;
    _outgoingEdge->setNewFacts(false, false, constantMap);
}

void MustPointerAnalysis::handleConditionalBranchInst(BranchInst * inst) {
	// Branch flow: 
	// a == 0
	// true will change, false will not
	// a != 0
	// false will change, true will not

	*_outgoingEdge = *_incomingEdge;//temp may remove later
	map<string,Value*> trueMap = _incomingEdge->getFacts();
	map<string,Value*> falseMap = _incomingEdge->getFacts();

	Value * condition = inst->getCondition();
	if (!ICmpInst::classof(condition)) { // no information, just copy and exit
		*_outgoingTrueEdge  = *_incomingEdge;
		*_outgoingFalseEdge = *_incomingEdge;
		return;
	}
	else {
		ICmpInst * cmpInst = (ICmpInst *)condition; 
		int predicate = cmpInst->isSigned() ? cmpInst->getSignedPredicate() : cmpInst->getUnsignedPredicate();
		Value * lhs = cmpInst->getOperand(0);
		Value * rhs = cmpInst->getOperand(1);
		Value * rhsConstant = tryGetConstantValue(rhs);
		Value * lhsConstant = tryGetConstantValue(lhs);
		if ((lhsConstant && rhsConstant) || (!lhsConstant && !rhsConstant)) { //both constants, no information,just copy and exit
			*_outgoingTrueEdge  = *_incomingEdge;
			*_outgoingFalseEdge = *_incomingEdge;
			return;
		}
		else if(rhsConstant)  { //rhs is a constant int, but lhs is not
			if(predicate == CmpInst::ICMP_EQ) { // X == C
				trueMap[lhs->getName().str()] = rhsConstant;
				_outgoingTrueEdge->setNewFacts(false, false, trueMap);
				*_outgoingFalseEdge = *_incomingEdge;
			}
			else if(predicate == CmpInst::ICMP_NE) { // X != C
				falseMap[lhs->getName().str()] = rhsConstant;
				_outgoingFalseEdge->setNewFacts(false, false, falseMap);
				*_outgoingTrueEdge = *_incomingEdge;
			}
		}
		else if(lhsConstant) {  //lhs is a constant int, but rhs is not
			if(predicate == CmpInst::ICMP_EQ) { // C == X
				trueMap[rhs->getName().str()] = lhsConstant;
				_outgoingTrueEdge->setNewFacts(false, false, trueMap);
				*_outgoingFalseEdge = *_incomingEdge;
			}
			else if(predicate == CmpInst::ICMP_NE) { // C != X
				falseMap[rhs->getName().str()] = lhsConstant;
				_outgoingFalseEdge->setNewFacts(false, false, falseMap);
				*_outgoingTrueEdge = *_incomingEdge;
			}
		}
		else {
			errs() << "[MustPointerAnalysis::handleConditionalBranchInst] WTF situation\n";
		}
	}
}

