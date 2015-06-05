#include "ConstantLattice.h"

ConstantLattice::ConstantLattice() {}

ConstantLattice::ConstantLattice(bool isTop, bool isBottom, map<string, ConstantInt *> facts) {
	// Must Analysis, bottom is Full set, top is empty set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

ConstantLattice::ConstantLattice(ConstantLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

ConstantLattice& ConstantLattice::operator=( const ConstantLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

ConstantLattice::~ConstantLattice() {}

map<string, ConstantInt *> ConstantLattice::getFacts() {
	return _facts; //returns a copy
}

void ConstantLattice::setNewFacts(bool isTop, bool isBottom, map<string, ConstantInt *> facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

bool ConstantLattice::isTop() {
	return _isTop;
}

bool ConstantLattice::isBottom() {
	return _isBottom;
}

void ConstantLattice::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<string, ConstantInt *>::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t" << i->first << " -> " << i->second->getSExtValue() << "\n";
            //errs() << "\t\t\tKey " << i->first << "\n";
            //errs() << "\t\t\tVal " << i->second->getSExtValue() << "\n";
        }
    }
}

