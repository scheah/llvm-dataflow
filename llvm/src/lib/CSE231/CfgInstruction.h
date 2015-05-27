#include "llvm/IR/Instruction.h"

using namespace llvm;



class CfgInstruction {
public:
    CfgInstruction(Instruction * instruction);

    unsigned getOpcode();


private:

    unsigned _opcode;
    const char * _opcodeName;
};

