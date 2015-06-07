#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "BaseLattice.h"
#include "Expression.h"

#include <map>
#include <string>

using namespace llvm;
using namespace std;

struct expressionComp {
    bool operator()(Expression * a, Expression * b) {
        return a->isEqualTo(b);
    }
};

class AvailableExprLattice : public BaseLattice {
public:
	AvailableExprLattice();
	AvailableExprLattice(bool isTop, bool isBottom, map<Expression *,vector<string>,expressionComp> facts);
	AvailableExprLattice(AvailableExprLattice& other);
	AvailableExprLattice& operator=(const AvailableExprLattice& other);
	~AvailableExprLattice();
	map<Expression *,vector<string>,expressionComp> getFacts();
	void setNewFacts(bool isTop, bool isBottom, map<Expression *,vector<string>,expressionComp> facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
    map<Expression *,vector<string>,expressionComp> _facts;

};

