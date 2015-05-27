#include "CfgInstruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"

CfgInstruction::CfgInstruction(Instruction * instruction) {
    _opcode = instruction->getOpcode();
    _opcodeName = instruction->getOpcodeName();

    if (StoreInst::classof(instruction)) {
        StoreInst * storeInst = (StoreInst *) &(*instruction);
        instruction->dump();
        errs() << "OpcodeType" << _opcodeName << "\n";
    }
}

unsigned CfgInstruction::getOpcode() {
    return _opcode;
}
