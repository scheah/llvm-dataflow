#include "ConstantPropAnalysis.h"
#include "llvm/IR/Constants.h"
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
		*_outgoingEdge = *_incomingEdge;
        handleStoreInst((StoreInst *) _instruction);
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
	if (_incomingEdge->isTop())
		errs() << "\t\t\tis Top\n";
	else if (_incomingEdge->isBottom()) {
		errs() << "\t\t\tis Bottom\n";
	}
	else {
		for (map<string, ConstantInt *>::iterator i = _incomingEdge->getFacts().begin(); i != _incomingEdge->getFacts().end(); i++) {
			//if (i->first != NULL) {
				errs() << "\t\t\tKey " << i->first << "\n"; 
				errs() << "\t\t\t\tVal ";
				i->second->dump();
			//}
		}
	}
    errs() << "\t\t\tOUTGOING:\n";
	if (_outgoingEdge->isTop())
		errs() << "\t\t\tis Top\n";
	else if (_outgoingEdge->isBottom()) {
		errs() << "\t\t\tis Bottom\n";
	}
	else {
		for (map<string, ConstantInt *>::iterator i = _outgoingEdge->getFacts().begin(); i != _outgoingEdge->getFacts().end(); i++) {
			//if (i->first != NULL) {
				errs() << "\t\t\tKey " << i->first << "\n"; 
				errs() << "\t\t\t\tVal ";
				i->second->dump();
			//}
		}
	}
    errs() << "\t\t--------------------------------------------------------\n";
}

void ConstantPropAnalysis::handleStoreInst(StoreInst * storeInst) {
	map<string, ConstantInt *> outgoingEdge = _outgoingEdge->getFacts(); 
    ConstantInt* CI = dyn_cast<ConstantInt>(storeInst->getValueOperand());
    if (!CI || CI->getBitWidth() > 32) {
		errs() << "[ConstantPropAnalysis::handleStoreInst] ERROR\n";
        return;
    }
    string name = storeInst->getPointerOperand()->getName().str();
    outgoingEdge[name] = CI;
	_outgoingEdge->setNewFacts(false,false,outgoingEdge);
}

