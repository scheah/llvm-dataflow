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
        handleStoreInst((StoreInst *) _instruction);
    }
    else if (LoadInst::classof(_instruction)) {
        handleLoadInst((LoadInst *) _instruction);
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
        for (map<string, Value *>::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges and are equal, add to outgoing edge
            if ( (i->first == j->first) && (i->second == j->second) ) {
                outgoingEdge[i->first] = i->second;
                break;
            }
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

    const Type* type = storeInst->getPointerOperand()->getType();

    if (type->isPointerTy() && type->getContainedType(0)->isPointerTy()) {
        edgeMap[name] = storeInst->getValueOperand();
    }

    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

void MustPointerAnalysis::handleLoadInst(LoadInst * loadInst) {
    map<string, Value *> edgeMap = _incomingEdge->getFacts();
	edgeMap.erase(loadInst->getOperandUse(0).getUser()->getName().str());
	
    const Type* type = loadInst->getOperand(0)->getType();
    if (type->isPointerTy() && type->getContainedType(0)->isPointerTy()) {
        edgeMap[loadInst->getOperandUse(0).getUser()->getName().str()] = loadInst->getOperand(0);
    }
    
    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

