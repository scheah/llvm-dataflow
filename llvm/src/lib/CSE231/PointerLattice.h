#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <string>

using namespace llvm;
using namespace std;

class PointerLattice {
public:
	PointerLattice();
	PointerLattice(bool isTop, bool isBottom, map<string, vector<Value*> > facts);
	PointerLattice(PointerLattice& other);
	PointerLattice& operator=(const PointerLattice& other);
	~PointerLattice();
	map<string, vector<Value*> > getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<string, vector<Value*> > facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
	map<string, vector<Value*> > _facts;
	bool _isTop;
	bool _isBottom;

};

