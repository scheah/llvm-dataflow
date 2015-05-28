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

#include <llvm/ADT/StringRef.h>
#include "llvm/Support/CFG.h"
#include "ConstantPropAnalysis.h"

#include <stdio.h>

using namespace llvm;
using namespace std;

namespace {
	struct BlockOrder : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		map<StringRef, BasicBlock*> visitedBlocks; //for use in detecting loop
		vector<BasicBlock*> blockOrder; // for use in remembering order of traversed blocks
        ConstantPropAnalysis analysis;

		BlockOrder() : FunctionPass(ID) {}

		virtual bool runOnFunction(Function &F) {
			errs() << "BEGIN FUNCTION: ";
			errs().write_escaped(F.getName()) << "\n";
			for (Function::iterator B = F.begin(); B != F.end(); ++B) { 
				errs() << "Begin block with name: ";
				errs().write_escaped(B->getName()) << "\n";
				// check predecessors to perform merge
				errs() << "\tPredecessors:\n";
				for (pred_iterator PI = pred_begin(B); PI != pred_end(B); ++PI) {
  					BasicBlock *pred = *PI;
					errs() << "\t\t";
					errs().write_escaped(pred->getName());
					errs() << "\n";

				}
				visitedBlocks[B->getName()] = &(*B);
				blockOrder.push_back(&(*B));
				// collect dataflow facts
				errs() << "\tInstructions:\n";
				for (BasicBlock::iterator I = B->begin(); I != B->end(); ++I) { 
					errs() << "\t\t";
                    
                    analysis.applyFlowFunction(I);

					//if (I->isBinaryOp() || StoreInst::classof(I) || I->isShift()) { //filter out instructions that we can compute dataflow facts from?
						I->dump(); //this prints to console immediately
						/*if(StoreInst::classof(I) ) {
							StoreInst * storeInst = (StoreInst *) &(*I); 
							errs() << "\toperand: \n";
							errs() << "\t\t";
							storeInst->getValueOperand()->dump();
					
							errs() << "\tdest: \n";
							errs() << "\t\t";
							storeInst->getPointerOperand()->dump();
						}
						else {
							errs() << "\toperands: \n";
							for (unsigned int i = 0; i < I->getNumOperands(); i++) {
								Value * operand = I->getOperand(i);
								errs() << "\t\t";
								operand->dump();
							}
							errs() << "\tdest: \n"; //In LLVM, the instruction *is* the same as it's result
							errs() << "\t\t";
							I->dump();
						}
					}*/
				}

                errs() << "Dump map\n";
                analysis.dump();

				// check successors for presence of a loop
				errs() << "\tSucessors:\n";
				//TerminatorInst * terminator = B->getTerminator();
				//for (unsigned int i = 0; i < terminator->getNumSuccessors(); i++) {
					//BasicBlock *successor = terminator->getSuccessor(i);
				for (succ_iterator SI = succ_begin(B); SI != succ_end(B); ++SI) {
					BasicBlock *successor = *SI;
					errs() << "\t\t";
					errs().write_escaped(successor->getName());
					BasicBlock* blockIfVisited = visitedBlocks[successor->getName()];
					if (blockIfVisited) {
						errs() << " !!!VISITED BEFORE!!!\n";
						blockLoop(blockIfVisited, &(*B));
					}
					else
						errs() << "\n";
				}
			}
			return false;
		}

		void blockLoop(BasicBlock *start, BasicBlock *end) {
			int startIndex;
			int endIndex;
			for (unsigned int i = 0; i < blockOrder.size(); i++) {
				if (blockOrder[i] == start) 
					startIndex = i;
				if (blockOrder[i] == end)
					endIndex = i;
			}
			errs() << "\t\t\tstartIndex: " << startIndex << "\n";
			errs() << "\t\t\tendIndex: " << endIndex << "\n";
		}
	};
}

char BlockOrder::ID = 0;
static RegisterPass<BlockOrder> X("block_order", "BlockOrder Pass");
