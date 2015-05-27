#include "llvm/IR/Instruction.h"

using namespace llvm;



class CfgInstruction {
public:
    CfgInstruction(Instruction * instruction);

    unsigned getOpcode();
    unsigned getValueOperand();
    StringRef getPointerOperand();

private:

    unsigned _opcode;
    const char * _opcodeName;
    unsigned _valueOperand;
    StringRef _pointerOperand;
};

