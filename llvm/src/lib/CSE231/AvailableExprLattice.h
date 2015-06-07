#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "BaseLattice.h"
#include "ExpressionContainer.h"

#include <string>

using namespace llvm;
using namespace std;

/*
struct expressionComp : public binary_function<Expression*,Expression*,bool> {
    bool operator()(Expression * const a, Expression * const b) {
        return a->isEqualTo(b);
    }
};
*/
class AvailableExprLattice : public BaseLattice {
public:
	AvailableExprLattice();
	AvailableExprLattice(bool isTop, bool isBottom, ExpressionContainer facts);
	AvailableExprLattice(AvailableExprLattice& other);
	AvailableExprLattice& operator=(const AvailableExprLattice& other);
	~AvailableExprLattice();
	ExpressionContainer getFacts();
	void setNewFacts(bool isTop, bool isBottom, ExpressionContainer facts);
	bool isTop();
	bool isBottom();
    void dump();

private:
    ExpressionContainer _facts;

};

