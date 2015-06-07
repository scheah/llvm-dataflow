#include "AvailableExprLattice.h"

AvailableExprLattice::AvailableExprLattice() {}

AvailableExprLattice::AvailableExprLattice(bool isTop, bool isBottom, map<Expression *,vector<string>, expressionComp> facts) {
	// Must Analysis, bottom is Full set, top is empty set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

AvailableExprLattice::AvailableExprLattice(AvailableExprLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

AvailableExprLattice& AvailableExprLattice::operator=( const AvailableExprLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

AvailableExprLattice::~AvailableExprLattice() {}

map<Expression *,vector<string>,expressionComp> AvailableExprLattice::getFacts() {
	return _facts; //returns a copy
}

void AvailableExprLattice::setNewFacts(bool isTop, bool isBottom, map<Expression *,vector<string>, expressionComp> facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

bool AvailableExprLattice::isTop() {
	return _isTop;
}

bool AvailableExprLattice::isBottom() {
	return _isBottom;
}

void AvailableExprLattice::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<Expression *,vector<string>, expressionComp>::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t";
            i->first->dump();
            errs() << " -> ";

            for (unsigned int j = 0; j < i->second.size(); j++) {
                errs() << i->second[j];

                if ( j != i->second.size() - 1) {
                    errs() << ", ";
                }
            }
            
            errs() << "\n";
        }
    }
}

