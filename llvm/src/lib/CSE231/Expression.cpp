#include "Expression.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"


using namespace std;

#include <sstream>

template <typename T>
string NumberToString ( T Number ) {
    ostringstream ss;
    ss << Number;
    return ss.str();
}

Expression * Expression::CreateExpression(Instruction * instruction) {
    if (instruction->isBinaryOp()) {
        return new BinaryExpression(instruction);
    }
    else {
        return new UnaryExpression(instruction);
    }
}

string Expression::getNameOf(Value * value) {
    ConstantInt * CI = dyn_cast<ConstantInt>(value);
    if (CI && CI->getBitWidth() <= 32) {
        return NumberToString(CI->getSExtValue());
    }

    return value->getName().str();
}

Expression::Expression(Instruction * instruction) {
    _instruction = instruction;
}

UnaryExpression::UnaryExpression(Instruction * instruction) : Expression(instruction) { }

Value * UnaryExpression::getOperand() {
    return _instruction->getOperand(0);
}

void UnaryExpression::dump() { 
    errs() << getNameOf(getOperand());
}

BinaryExpression::BinaryExpression(Instruction * instruction) : Expression(instruction) { }

void BinaryExpression::dump() {
    errs() << getNameOf(getOperand1()) << " " << _instruction->getOpcodeName() << " " << getNameOf(getOperand2());
}

Value * BinaryExpression::getOperand1() {
    return _instruction->getOperand(0);
}

Value * BinaryExpression::getOperand2() {
    return _instruction->getOperand(1);
}

