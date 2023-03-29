#include "CFG.h"

using namespace std;

CFG::CFG()
{
    current_bb = createBB();
}

CFG::~CFG()
{
    for(BasicBlock* bb: bbs){
        delete bb;
    }
}


BasicBlock* CFG::createBB(){
    string nameBB = ".bb" + to_string(bbs.size());
    BasicBlock* bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    current_bb = bb;
    return bb;
}

void CFG::add_bb(BasicBlock* bb){
    bbs.push_back(bb);
    current_bb = bb;
}

void CFG::gen_asm(ostream& o){
    gen_asm_prologue(o);
    for(BasicBlock* bb : bbs){
        bb->gen_asm(o);
    }
    gen_asm_epilogue(o);
}

void CFG::gen_asm_prologue(ostream& o){
    o << ".globl	_main\n";
    o << " _main: \n";
    o << " 	# prologue \n";
    o << " 	pushq %rbp # save %rbp on the stack \n";
    o << " 	movq %rsp, %rbp # define %rbp for the current function \n";
}

void CFG::gen_asm_epilogue(ostream& o){
    o << " 	# epilogue \n";
    o << " 	popq %rbp # restore %rbp from the stack \n";
    o << " 	ret\n";
}