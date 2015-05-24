# llvm-dataflow-analysis
Basic block  
they have basicblockids (KEY)  
Dealing with while loops (get successor, if you already encountered that block, it's a loop!), might have to chain basic blocks until hit the fixed point (repeat of dataflow fact)  
Dealing with merge blocks: get num of predecessors. Perform merge dataflow function if 2 (or more?) predecessors
