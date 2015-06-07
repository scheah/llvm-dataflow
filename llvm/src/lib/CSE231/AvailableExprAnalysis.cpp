#include "AvailableExprAnalysis.h"
#include "llvm/Support/raw_ostream.h"

AvailableExprAnalysis::AvailableExprAnalysis(Instruction * inst, AvailableExprLattice * incoming) {
	_instruction = inst;
	map<string,Expression*> empty;
	_incomingEdge = new AvailableExprLattice(false,true,empty);
	*_incomingEdge = *incoming;
	_outgoingEdge = new AvailableExprLattice(false,true,empty);
}

void AvailableExprAnalysis::applyFlowFunction() {
    //if (StoreInst::classof(_instruction)) {
        //handleStoreInst((StoreInst *) _instruction);
    //}
    //else if (LoadInst::classof(_instruction)) {
        //handleLoadInst((LoadInst *) _instruction);
    //}
    if (_instruction->isBinaryOp()) {
        handleBinaryOp(_instruction);            
    }
	else {//temp 
		*_outgoingEdge = *_incomingEdge;
	}
}

Instruction * AvailableExprAnalysis::getInstruction() {
    return _instruction;
}

AvailableExprLattice * AvailableExprAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

void AvailableExprAnalysis::setIncomingEdge(AvailableExprLattice * incoming) {    
    *_incomingEdge = *incoming;
}

// will be a join
AvailableExprLattice * AvailableExprAnalysis::merge(AvailableExprLattice * edge_1, AvailableExprLattice * edge_2) {
	/*map<string,ConstantInt*> outgoingEdge;
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
    return new ConstantLattice(false, false, outgoingEdge);*/
	return NULL;
}

bool AvailableExprAnalysis::equal(AvailableExprLattice * edge_1, AvailableExprLattice * edge_2) {
	/*if(edge_1->isTop() && edge_2->isTop()) 
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

    return true;*/
	return true;
}

void AvailableExprAnalysis::dump() {
    errs() << "\t\t\tINCOMING:\n";
    _incomingEdge->dump();

    errs() << "\t\t\tOUTGOING:\n";
    _outgoingEdge->dump();
    
    errs() << "\t\t--------------------------------------------------------\n";
}

/*
void AvailableExprAnalysis::handleStoreInst(StoreInst * storeInst) {
    string name = storeInst->getPointerOperand()->getName().str();

    map<Expression *,vector<string>, expressionComp> incomingEdge = _incomingEdge->getFacts();
    incomingEdge[name] = new UnaryExpression(storeInst);
    _outgoingEdge->setNewFacts(false, false, incomingEdge);
}

void AvailableExprAnalysis::handleLoadInst(LoadInst * loadInst) {
    map<string, Expression *> edgeMap = _incomingEdge->getFacts();
    edgeMap.erase(loadInst->getOperandUse(0).getUser()->getName().str());

    if (edgeMap.count(loadInst->getOperand(0)->getName().str()) > 0) {
        edgeMap[loadInst->getOperandUse(0).getUser()->getName().str()] = edgeMap[loadInst->getOperand(0)->getName().str()];
    }

    _outgoingEdge->setNewFacts(false,false,edgeMap);
}
*/

void AvailableExprAnalysis::handleBinaryOp(Instruction * inst) {
    map<string, Expression*> edgeMap = _incomingEdge->getFacts();
    edgeMap[inst->getOperandUse(0).getUser()->getName().str()] = new BinaryExpression(inst);

    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

