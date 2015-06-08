#include "PointerLattice.h"

PointerLattice::PointerLattice() {}

PointerLattice::PointerLattice(bool isTop, bool isBottom, map<string, set<Value*,valueComp> > facts) {
	// Must Analysis, bottom is Full set, top is empty set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

PointerLattice::PointerLattice(PointerLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

PointerLattice& PointerLattice::operator=( const PointerLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

PointerLattice::~PointerLattice() {}

map<string, set<Value*,valueComp> > PointerLattice::getFacts() {
	return _facts; //returns a copy
}

void PointerLattice::setNewFacts(bool isTop, bool isBottom, map<string, set<Value*,valueComp> > facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

bool PointerLattice::isTop() {
	return _isTop;
}

bool PointerLattice::isBottom() {
	return _isBottom;
}

void PointerLattice::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<string, set<Value*,valueComp> >::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t" << i->first << " -> "; //<< i->second->getName() << "\n";

            //for (unsigned int j = 0; j < i->second.size(); j++) {
            /*for (set<Value*,valueComp>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                errs() << (*j)->getName().str();

                if (j + 1 == i->second.end())
                    errs() << ",";
            }*/

            set<Value*,valueComp>::iterator j = i->second.begin();
            while(j != i->second.end()) {
                errs() << (*j)->getName().str();
            
                if (++j != i->second.end()) {
                    errs() << ",";
                }
            }

            errs() << "\n";
            //errs() << "\t\t\t\t" << i->first << " -> " << i->second->getSExtValue() << "\n";
            //errs() << "\t\t\tKey " << i->first << "\n";
            //errs() << "\t\t\tVal " << i->second->getSExtValue() << "\n";
        }
    }
}

