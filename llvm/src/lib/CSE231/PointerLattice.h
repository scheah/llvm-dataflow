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

class PointerLattice {
public:
	PointerLattice();
	PointerLattice(bool isTop, bool isBottom, map<string, set<Value*,valueComp> > facts);
	PointerLattice(PointerLattice& other);
	PointerLattice& operator=(const PointerLattice& other);
	~PointerLattice();
	map<string, set<Value*,valueComp> > getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<string, set<Value*,valueComp> > facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
	map<string, set<Value*,valueComp> > _facts;
	bool _isTop;
	bool _isBottom;

};

