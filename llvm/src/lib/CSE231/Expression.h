#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"

using namespace llvm;
using namespace std;

class Expression {
public:
    static Expression * CreateExpression(Instruction * instruction);
    virtual void dump() = 0;

protected:
    Expression(Instruction * instruction);
    string getNameOf(Value * value);

    Instruction * _instruction;
};


class UnaryExpression : public Expression {
public:
    UnaryExpression(Instruction * instruction);

    Value * getOperand();
    void dump();
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(Instruction * instruction);

    Value * getOperand1();
    Value * getOperand2();
    void dump();
};


