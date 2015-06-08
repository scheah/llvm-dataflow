#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <set>
#include <map>
#include <string>

using namespace llvm;
using namespace std;
    
struct valueComp {
    bool operator()(Value* const a, Value* const b) {
        return a->getName() < b->getName();
    }
};

template <class T>
class PointerLattice {
public:
	PointerLattice();
	PointerLattice(bool isTop, bool isBottom, T facts);
	PointerLattice(PointerLattice& other);
	PointerLattice& operator=(const PointerLattice& other);
	~PointerLattice();
	T getFacts();
	void setNewFacts(bool isTop, bool isBottom, T facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
	T _facts;
	bool _isTop;
	bool _isBottom;

};

template <class T>
PointerLattice<T>::PointerLattice() {}

template <class T>
PointerLattice<T>::PointerLattice(bool isTop, bool isBottom, T facts) {
	// Must Analysis, bottom is Full set, top is empty set
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

template <class T>
PointerLattice<T>::PointerLattice(PointerLattice& other) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
}

template <class T>
PointerLattice<T>& PointerLattice<T>::operator=( const PointerLattice& other ) {
	_facts = other._facts;
	_isTop = other._isTop;
	_isBottom = other._isBottom;
	return *this;
}

template <class T>
PointerLattice<T>::~PointerLattice() {}

template <class T>
T PointerLattice<T>::getFacts() {
	return _facts; //returns a copy
}

template <class T>
void PointerLattice<T>::setNewFacts(bool isTop, bool isBottom, T facts) {
	_facts = facts;
	_isTop = isTop;
	_isBottom = isBottom;
}

template <class T>
bool PointerLattice<T>::isTop() {
	return _isTop;
}

template <class T>
bool PointerLattice<T>::isBottom() {
	return _isBottom;
}

template <class T>
void PointerLattice<T>::dump() {
    if (isTop()) {
        errs() << "\t\t\tis Top\n";
    }
    else if (isBottom()) {
        errs() << "\t\t\tis Bottom\n";
    }
    else {
        for (map<string, set<Value*,valueComp> >::iterator i = _facts.begin(); i != _facts.end(); i++) {
            errs() << "\t\t\t\t" << i->first << "\n";
      
      //errs() << "\t\t\t\t" << i->first << " -> "; //<< i->second->getName() << "\n";

            //for (unsigned int j = 0; j < i->second.size(); j++) {
            /*for (set<Value*,valueComp>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                errs() << (*j)->getName().str();

                if (j + 1 == i->second.end())
                    errs() << ",";
            }*/

            //set<Value*,valueComp>::iterator j = i->second.begin();
            //while(j != i->second.end()) {
            //    errs() << (*j)->getName().str();
            
            //    if (++j != i->second.end()) {
            //        errs() << ",";
            //    }
            //}

            //errs() << "\n";
            //errs() << "\t\t\t\t" << i->first << " -> " << i->second->getSExtValue() << "\n";
            //errs() << "\t\t\tKey " << i->first << "\n";
            //errs() << "\t\t\tVal " << i->second->getSExtValue() << "\n";
        }
    }
}

