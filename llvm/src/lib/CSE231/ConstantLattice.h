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

class ConstantLattice {
public:
	ConstantLattice();
	ConstantLattice(bool isTop, bool isBottom, map<string, ConstantInt *> facts);
	ConstantLattice(ConstantLattice& other);
	ConstantLattice& operator=(const ConstantLattice& other);
	~ConstantLattice();
	map<string, ConstantInt *> getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<string, ConstantInt *> facts);
	bool isTop();
	bool isBottom();
private:
	map<string, ConstantInt*> _facts;
	bool _isTop;
	bool _isBottom;

};

