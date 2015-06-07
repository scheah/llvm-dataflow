#include "MustPointerLattice.h"

MustPointerLattice::MustPointerLattice() {}

MustPointerLattice::MustPointerLattice(bool isTop, bool isBottom, map<string, Value*> facts) {
	// Must Analysis, bottom is Full set, top is empty set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

MustPointerLattice::MustPointerLattice(MustPointerLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

MustPointerLattice& MustPointerLattice::operator=( const MustPointerLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

MustPointerLattice::~MustPointerLattice() {}

map<string, Value*> MustPointerLattice::getFacts() {
	return _facts; //returns a copy
}

void MustPointerLattice::setNewFacts(bool isTop, bool isBottom, map<string, Value*> facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

bool MustPointerLattice::isTop() {
	return _isTop;
}

bool MustPointerLattice::isBottom() {
	return _isBottom;
}

void MustPointerLattice::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<string, Value*>::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t" << i->first << " -> " << i->second->getName() << "\n";

            //errs() << "\t\t\t\t" << i->first << " -> " << i->second->getSExtValue() << "\n";
            //errs() << "\t\t\tKey " << i->first << "\n";
            //errs() << "\t\t\tVal " << i->second->getSExtValue() << "\n";
        }
    }
}

