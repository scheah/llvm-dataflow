#include "RangeAnalysis.h"
#include "llvm/Support/raw_ostream.h"

RangeAnalysis::RangeAnalysis(Instruction * inst, RangeLattice * incoming) {
	_instruction = inst;
	map<string,ConstantInt*> empty;
	_incomingEdge = new RangeLattice(false,true,empty);
	*_incomingEdge = *incoming;
	_outgoingEdge = new RangeLattice(false,true,empty);
	_outgoingTrueEdge = new RangeLattice(false,true,empty);
	_outgoingFalseEdge = new RangeLattice(false,true,empty);
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

bool RangeAnalysis::isConditionalBranch() {
	return _isConditionalBranch;
}

void RangeAnalysis::applyFlowFunction() {
    if (StoreInst::classof(_instruction)) {
        handleStoreInst((StoreInst *) _instruction);
    }
    else if (LoadInst::classof(_instruction)) {
        handleLoadInst((LoadInst *) _instruction);
    }
    else if (_instruction->isBinaryOp()) {
        handleBinaryOp(_instruction);            
    }
	else if (_isConditionalBranch) {
		handleConditionalBranchInst((BranchInst *) _instruction);
	}
	else { //temp 
		*_outgoingEdge = *_incomingEdge;
	}
}

Instruction * RangeAnalysis::getInstruction() {
    return _instruction;
}

RangeLattice * RangeAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

RangeLattice * RangeAnalysis::getOutgoingEdge(BasicBlock * toSuccessor) {
	if(!_isConditionalBranch) {
		//errs() << "[RangeAnalysis::getOutgoingEdge(BasicBlock * toSuccessor)] not a conditional branch predecessor, return normal outgoing edge\n";
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
		errs() << "[getOutgoingEdge(BasicBlock * toSuccessor)] the world is weird.\n";
	}
	return NULL;
}


void RangeAnalysis::setIncomingEdge(RangeLattice * incoming) {    
    *_incomingEdge = *incoming;
}

// will be a join
RangeLattice * RangeAnalysis::merge(RangeLattice * edge_1, RangeLattice * edge_2) {
	return NULL;
}

bool RangeAnalysis::equal(RangeLattice * edge_1, RangeLattice * edge_2) {
	return true;
}

void RangeAnalysis::dump() {
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

void RangeAnalysis::handleStoreInst(StoreInst * storeInst) {

}

void RangeAnalysis::handleLoadInst(LoadInst * loadInst) {

}


void RangeAnalysis::handleBinaryOp(Instruction * inst) {
}

void RangeAnalysis::handleConditionalBranchInst(BranchInst * inst) {

}


