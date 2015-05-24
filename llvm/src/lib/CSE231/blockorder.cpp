//===- test.cpp - 				    ---------------===//
//
// Sample pass to understand basic block behavior
// 
// 		
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "BlockOrder"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/User.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Format.h"

#include <stdio.h>

using namespace llvm;
using namespace std;

namespace {
	struct BlockOrder : public BasicBlockPass {
		static char ID; // Pass identification, replacement for typeid
		
		BlockOrder() : BasicBlockPass(ID) {}

		virtual bool runOnBasicBlock(BasicBlock &B) {
			errs() << "Begin block with name: ";
			errs().write_escaped(B.getName()) << "\n";
			for (BasicBlock::iterator I = B.begin(); I != B.end(); ++I) { 
				if (I->isBinaryOp() || StoreInst::classof(I) || I->isShift()) { //filter out instructions that we can compute dataflow facts from
					I->dump(); //this prints to console immediately
					
					
					if(StoreInst::classof(I) ) {
						StoreInst * storeInst = (StoreInst*) I; //this cast is fucking broken
						errs() << "\toperand: \n";
						errs() << "\t\t";
						storeInst->getValueOperand()->dump();
						
						errs() << "\tdest: \n";
						storeInst->getPointerOperand()->dump();
					}
					else {
						errs() << "\toperands: \n";
						for (unsigned int i = 0; i < I->getNumOperands(); i++) {
							Value * operand = I->getOperand(i);
							errs() << "\t\t";
							operand->dump();
						}
					}
					

				}
			}
			return false;
		}
	};
}

char BlockOrder::ID = 0;
static RegisterPass<BlockOrder> X("block_order", "BlockOrder Pass");
