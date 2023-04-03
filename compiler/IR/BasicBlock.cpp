#include "BasicBlock.h"

using namespace std;

BasicBlock::BasicBlock(CFG *cfg, string entry_label) : cfg(cfg), label(entry_label) {}

BasicBlock::~BasicBlock()
{
    for (IRInstr *instr : instrs)
    {
        delete instr;
    }
}

void BasicBlock::gen_asm(ostream &o){
    o << label << ":" << endl;
    for(IRInstr* instr: instrs) {
        instr->gen_asm(o);
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, vector<string> params, map<string, pair<string,int>> *variables){
    IRInstr* instr = new IRInstr(this, op, params, variables);
    instrs.push_back(instr);
}
