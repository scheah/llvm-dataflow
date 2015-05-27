#include "CfgInstruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"

CfgInstruction::CfgInstruction(Instruction * instruction) {
    _opcode = instruction->getOpcode();
    _opcodeName = instruction->getOpcodeName();

    if (StoreInst::classof(instruction)) {
        StoreInst * storeInst = (StoreInst *) &(*instruction);

        if (ConstantInt* CI = dyn_cast<ConstantInt>(storeInst->getValueOperand())) {
            if (CI->getBitWidth() <= 32) {
                _valueOperand = CI->getSExtValue();
            }
        }

        //_valueOperand = storeInst->getValueOperand()->getValueID();
        _pointerOperand = storeInst->getPointerOperand()->getName();
        instruction->dump();
        errs() << "Operands\t" << storeInst->isSimple() << "\t" << _valueOperand << "\t" << _pointerOperand << "\n";
    }
}

unsigned CfgInstruction::getOpcode() {
    return _opcode;
}
