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

class AvailableExprLattice {
public:
	AvailableExprLattice();
	AvailableExprLattice(bool isTop, bool isBottom, map<string, ConstantInt *> facts);
	AvailableExprLattice(AvailableExprLattice& other);
	AvailableExprLattice& operator=(const AvailableExprLattice& other);
	~AvailableExprLattice();
	map<string, ConstantInt *> getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<string, ConstantInt *> facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
	map<string, ConstantInt*> _facts;
	bool _isTop;
	bool _isBottom;

};

