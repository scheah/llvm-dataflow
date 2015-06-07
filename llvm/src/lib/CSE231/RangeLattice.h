#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <vector>
#include <string>

using namespace llvm;
using namespace std;

class RangeLattice {
public:
	RangeLattice();
	RangeLattice(bool isTop, bool isBottom, map<string, vector<int> > facts);
	RangeLattice(RangeLattice& other);
	RangeLattice& operator=(const RangeLattice& other);
	~RangeLattice();
	map<string, vector<int> > getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<string, vector<int> > facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
	map<string, vector<int> > _facts;
	bool _isTop;
	bool _isBottom;

};

