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

class BaseLattice {
public:
	bool isTop();
	bool isBottom();
    virtual void dump() = 0;

protected:
	bool _isTop;
	bool _isBottom;

};

