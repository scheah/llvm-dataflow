#include "RangeLattice.h"

RangeLattice::RangeLattice() {}

RangeLattice::RangeLattice(bool isTop, bool isBottom, map<string, vector<int> > facts) {
	// May Analysis, bottom is empty set, top is full set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

RangeLattice::RangeLattice(RangeLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

RangeLattice& RangeLattice::operator=( const RangeLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

RangeLattice::~RangeLattice() {}

map<string, vector<int> > RangeLattice::getFacts() {
	return _facts; //returns a copy
}

void RangeLattice::setNewFacts(bool isTop, bool isBottom, map<string, vector<int> > facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

bool RangeLattice::isTop() {
	return _isTop;
}

bool RangeLattice::isBottom() {
	return _isBottom;
}

void RangeLattice::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<string, vector<int> >::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t" << i->first << " -> (" << i->second[0] << ", " << i->second[1] << ")\n";
            //errs() << "\t\t\tKey " << i->first << "\n";
            //errs() << "\t\t\tVal " << i->second->getSExtValue() << "\n";
        }
    }
}

