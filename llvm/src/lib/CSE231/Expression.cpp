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

UnaryExpression::UnaryExpression(Instruction * instruction) : Expression(instruction) {
}

Value * UnaryExpression::getOperand() {
    return _instruction->getOperand(0);
}

string UnaryExpression::operandName() {
    return getOperand()->getName().str();
}

void UnaryExpression::dump() {
    errs() << operandName();
}

BinaryExpression::BinaryExpression(Instruction * instruction) : Expression(instruction) {
}

void BinaryExpression::dump() {
    errs() << operand1Name() << " " << _instruction->getOpcodeName() << " " << operand2Name();
}

Value * BinaryExpression::getOperand1() {
    return _instruction->getOperand(0);
}

Value * BinaryExpression::getOperand2() {
    return _instruction->getOperand(1);
}

string BinaryExpression::operand1Name() {
    return getNameOf(getOperand1());
}

string BinaryExpression::operand2Name() {
    return getNameOf(getOperand2());
}

/*
bool Expression::isEqualTo(Expression * expression) {
    if (_instruction->isCommutative() && _instruction->getOpcode() == expression->_instruction->getOpcode()) {
        if ((operand1Name() == expression->operand1Name() &&
             operand2Name() == expression->operand2Name()) ||
            (operand1Name() == expression->operand2Name() &&
             operand2Name() == expression->operand1Name())) {
        }
        else {
            return operand1Name() < expression->operand1Name();
        }
    }

    return operand1Name() > expression->operand1Name();
}
*/
