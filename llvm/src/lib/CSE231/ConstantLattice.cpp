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
