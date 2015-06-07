#include "RangeAnalysis.h"
#include "llvm/Support/raw_ostream.h"


RangeAnalysis::RangeAnalysis(Instruction * inst, RangeLattice * incoming) {
	_instruction = inst;
	map<string, vector<int> > empty;
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
RangeLattice * RangeAnalysis::join(RangeLattice * edge_1, RangeLattice * edge_2) {
	map<string, vector<int> > outgoingEdge;
	if (edge_1->isTop() || edge_2->isTop()) {
		return new RangeLattice(true, false, outgoingEdge); // return Top
	}
	else if (edge_1->isBottom() && edge_2->isBottom()) { // both are bottom (empty) return bottom
		return new RangeLattice(false, true, outgoingEdge);
	}
	map<string, vector<int> > edge1 = edge_1->getFacts();
	map<string, vector<int> > edge2 = edge_2->getFacts();
	
    for (map<string, vector<int> >::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isInBothEdges = false;
        for (map<string, vector<int> >::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges, add to outgoing edge
            if ( i->first == j->first ) {
                isInBothEdges = true;
                break;
            }
        }
        if (isInBothEdges) {
            outgoingEdge[i->first].push_back( max(edge1[i->first][0], edge2[i->first][0]) );
			outgoingEdge[i->first].push_back( min(edge1[i->first][1], edge2[i->first][1]) );
			edge2.erase(i->first); //erase...in order to get remainders later
        }
    }
	for (map<string, vector<int> >::iterator i = edge2.begin(); i != edge2.end(); i++) {
    	outgoingEdge[i->first] = i->second;
    }
    return new RangeLattice(false, false, outgoingEdge);
}

// will be a meet
RangeLattice * RangeAnalysis::merge(RangeLattice * edge_1, RangeLattice * edge_2) {
	map<string, vector<int> > outgoingEdge;
	if (edge_1->isTop() || edge_2->isTop()) {
		return new RangeLattice(true, false, outgoingEdge); // return Top
	}
	else if (edge_1->isBottom() && edge_2->isBottom()) { //both are bottom (empty) return bottom
		return new RangeLattice(false, true, outgoingEdge);
	}
	map<string, vector<int> > edge1 = edge_1->getFacts();
	map<string, vector<int> > edge2 = edge_2->getFacts();
	
    for (map<string, vector<int> >::iterator i = edge1.begin(); i != edge1.end(); i++) {
        bool isInBothEdges = false;
        for (map<string, vector<int> >::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges, add to outgoing edge
            if ( i->first == j->first ) {
                isInBothEdges = true;
                break;
            }
        }
        if (isInBothEdges) {
            outgoingEdge[i->first].push_back( min(edge1[i->first][0], edge2[i->first][0]) );
			outgoingEdge[i->first].push_back( max(edge1[i->first][1], edge2[i->first][1]) );
			edge2.erase(i->first); //erase...in order to get remainders later
        }
    }
	for (map<string, vector<int> >::iterator i = edge2.begin(); i != edge2.end(); i++) {
    	outgoingEdge[i->first] = i->second;
    }
    return new RangeLattice(false, false, outgoingEdge);

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
	string name = storeInst->getPointerOperand()->getName().str();
	map<string, vector<int> > edgeMap = _incomingEdge->getFacts(); 
	edgeMap.erase(name);
    ConstantInt* CI = tryGetConstantValue(storeInst->getValueOperand()); 
	vector<int> storedRange = tryGetRange(storeInst->getValueOperand()); 
    if (CI == NULL) { // X = unknown
		errs() << "[ConstantPropAnalysis::handleStoreInst] not a ConstantInt\n";
		_outgoingEdge->setNewFacts(_incomingEdge->isTop(), _incomingEdge->isBottom(), edgeMap);
    }
	else if (CI) { // X = C
    	edgeMap[name].push_back(CI->getSExtValue());
		edgeMap[name].push_back(CI->getSExtValue());
		_outgoingEdge->setNewFacts(false,false,edgeMap);
	}
	else { // X = Y where Y is in map
		edgeMap[name] = storedRange;
		_outgoingEdge->setNewFacts(false,false,edgeMap);
	}
}

void RangeAnalysis::handleLoadInst(LoadInst * loadInst) { //of the form X = Y
	map<string, vector<int> > edgeMap = _incomingEdge->getFacts();
	edgeMap.erase(loadInst->getOperandUse(0).getUser()->getName().str());
	if (edgeMap.count(loadInst->getOperand(0)->getName().str()) > 0)
    	edgeMap[loadInst->getOperandUse(0).getUser()->getName().str()] = edgeMap[loadInst->getOperand(0)->getName().str()];
    _outgoingEdge->setNewFacts(false,false,edgeMap);
}


void RangeAnalysis::handleBinaryOp(Instruction * inst) {
}

void RangeAnalysis::handleConditionalBranchInst(BranchInst * inst) {
	// Branch flow: 
	// X == 0
	// true will change, false will not
	// X != 0
	// false will change, true will not
	*_outgoingEdge = *_incomingEdge;//temp may remove later
	map<string,vector<int> > trueMap = _incomingEdge->getFacts();
	map<string,vector<int> > falseMap = _incomingEdge->getFacts();

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
		ConstantInt * rhsConstant = tryGetConstantValue(rhs);
		ConstantInt * lhsConstant = tryGetConstantValue(lhs);
		vector<int> lhsRange = tryGetRange(lhs);
		vector<int> rhsRange = tryGetRange(rhs); 
		Value * variable = NULL;
		ConstantInt * constant = NULL;
		vector<int> range; //empty
		if ((lhsConstant && rhsConstant) || (!lhsConstant && !rhsConstant) || (!lhsRange.empty() && !rhsRange.empty()) ) { //both constants, no information,just copy and exit
			*_outgoingTrueEdge  = *_incomingEdge;
			*_outgoingFalseEdge = *_incomingEdge;
			return;
		}
		else if(rhsConstant)  { //rhs is a constant int, but lhs is not
			variable = lhs;
			constant = rhsConstant;
		}
		else if(lhsConstant) {  //lhs is a constant int, but rhs is not
			variable = rhs;
			constant = lhsConstant;
		}
		else { // both variables
			if (!lhsRange.empty()) {
				range = lhsRange;
				variable = rhs;
			}
			else {
				range = rhsRange;
				variable = lhs;
			}
		}
		if (variable && constant) {
			if(predicate == CmpInst::ICMP_EQ) { // X == C
				trueMap.erase(variable->getName().str());
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()); // max
				_outgoingTrueEdge->setNewFacts(false, false, trueMap);
				*_outgoingFalseEdge = *_incomingEdge;
			}
			else if(predicate == CmpInst::ICMP_NE) { // X != C
				falseMap.erase(variable->getName().str());
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()); // max
				_outgoingFalseEdge->setNewFacts(false, false, falseMap);
				*_outgoingTrueEdge = *_incomingEdge;
			}
			else if( ((predicate == CmpInst::ICMP_UGT || predicate == CmpInst::ICMP_SGT) && variable == lhs) || ((predicate == CmpInst::ICMP_ULT || predicate == CmpInst::ICMP_SLT) && variable == rhs) ) { // X > C
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()+1); // min
				trueMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;
			}
			else if( ((predicate == CmpInst::ICMP_ULT || predicate == CmpInst::ICMP_SLT) && variable == lhs) || ((predicate == CmpInst::ICMP_UGT || predicate == CmpInst::ICMP_SGT) && variable == rhs) ) { // X < C
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()-1); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;		
			}
			else if( ((predicate == CmpInst::ICMP_UGE || predicate == CmpInst::ICMP_SGE) && variable == lhs) || ((predicate == CmpInst::ICMP_ULE || predicate == CmpInst::ICMP_SLE) && variable == rhs) ) { // X >= C
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()-1); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;
			}
			else if( ((predicate == CmpInst::ICMP_ULE || predicate == CmpInst::ICMP_SLE) && variable == lhs) || ((predicate == CmpInst::ICMP_UGE || predicate == CmpInst::ICMP_SGE) && variable == rhs) ) { // X <= C
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(constant->getSExtValue()); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(constant->getSExtValue()+1); // min
				falseMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;		
			}
		}
		else if (variable && !range.empty()) {
			if(predicate == CmpInst::ICMP_EQ) { // X == Y, Y->(min,max)
				trueMap.erase(variable->getName().str());
				trueMap[variable->getName().str()].push_back(range[0]); // min
				trueMap[variable->getName().str()].push_back(range[1]); // max
				_outgoingTrueEdge->setNewFacts(false, false, trueMap);
				*_outgoingFalseEdge = *_incomingEdge;
			}
			else if(predicate == CmpInst::ICMP_NE) { // X != Y, Y->(min,max)
				falseMap.erase(variable->getName().str());
				falseMap[variable->getName().str()].push_back(range[0]); // min
				falseMap[variable->getName().str()].push_back(range[1]); // max
				_outgoingFalseEdge->setNewFacts(false, false, falseMap);
				*_outgoingTrueEdge = *_incomingEdge;
			}
			else if( ((predicate == CmpInst::ICMP_UGT || predicate == CmpInst::ICMP_SGT) && variable == lhs) || ((predicate == CmpInst::ICMP_ULT || predicate == CmpInst::ICMP_SLT) && variable == rhs) ) { // X > Y, Y->(min,max)
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(range[0]+1); // min
				trueMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(range[1]); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;
			}
			else if( ((predicate == CmpInst::ICMP_ULT || predicate == CmpInst::ICMP_SLT) && variable == lhs) || ((predicate == CmpInst::ICMP_UGT || predicate == CmpInst::ICMP_SGT) && variable == rhs) ) { // X < Y, Y->(min,max)
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(range[1]-1); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(range[0]); // min
				falseMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;		
			}
			else if( ((predicate == CmpInst::ICMP_UGE || predicate == CmpInst::ICMP_SGE) && variable == lhs) || ((predicate == CmpInst::ICMP_ULE || predicate == CmpInst::ICMP_SLE) && variable == rhs) ) { // X >= Y, Y->(min,max)
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(range[0]); // min
				trueMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				falseMap[variable->getName().str()].push_back(range[1]-1); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;
			}
			else if( ((predicate == CmpInst::ICMP_ULE || predicate == CmpInst::ICMP_SLE) && variable == lhs) || ((predicate == CmpInst::ICMP_UGE || predicate == CmpInst::ICMP_SGE) && variable == rhs) ) { // X <= Y, Y->(min,max)
				trueMap.erase(variable->getName().str());				
				trueMap[variable->getName().str()].push_back(APInt::getSignedMinValue(32).getSExtValue()); // min
				trueMap[variable->getName().str()].push_back(range[1]); // max

				falseMap.erase(variable->getName().str());				
				falseMap[variable->getName().str()].push_back(range[0]+1); // min
				falseMap[variable->getName().str()].push_back(APInt::getSignedMaxValue(32).getSExtValue()); // max

				RangeLattice * joinElementTrue = new RangeLattice(false, false, trueMap);
				RangeLattice * joinElementFalse = new RangeLattice(false, false, falseMap);
				RangeLattice * joinResultTrue = join(joinElementTrue, _incomingEdge);
				RangeLattice * joinResultFalse = join(joinElementFalse, _incomingEdge);
				*_outgoingTrueEdge = *joinResultTrue;				
				*_outgoingFalseEdge = *joinResultFalse;		
			}
		}
	}

}

ConstantInt * RangeAnalysis::tryGetConstantValue(Value * value) {
    ConstantInt * CI = dyn_cast<ConstantInt>(value);
    if (CI && CI->getBitWidth() <= 32) {
        return CI;
    }
    return NULL;
}

vector<int> RangeAnalysis::tryGetRange(Value * value) {
    map<string,vector<int> > edgeMap = _incomingEdge->getFacts();
    if( edgeMap.count(value->getName().str()) > 0 )
		return edgeMap[value->getName().str()];
	vector<int> empty;
    return empty;
}


