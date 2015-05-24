//===- test.cpp - 				    ---------------===//
//
// Sample pass to understand basic block behavior
// 
// 		
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "CountStaticInstructions"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Format.h"
#include <map>
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
				I->dump(); //this prints to console immediately
			}
			return false;
		}
	};
}

char BlockOrder::ID = 0;
static RegisterPass<BlockOrder> X("block_order", "BlockOrder Pass");
