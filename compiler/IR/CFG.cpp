#include "CFG.h"

using namespace std;

CFG::CFG()
{
    current_bb = createBB();
    add_finalBB();
    current_bb->setExitTrue(final_bb);
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
    return bb;
}

void CFG::add_bb(BasicBlock* bb){
    bbs.push_back(bb);
    current_bb = bb;
}

void CFG::add_finalBB()
{
    string nameBB = ".finalBB";
    final_bb = new BasicBlock(this, nameBB);
    bbs.push_back(final_bb);
}

BasicBlock *CFG::createTestWhileBB()
{
    nbWhile++;
    string nameBB = ".testWhileBB" + to_string(nbWhile);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createWhileBB()
{
    string nameBB = ".whileBB" + to_string(nbWhile);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createAfterWhileBB()
{
    string nameBB = ".afterWhileBB" + to_string(nbWhile);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createTestIfBB()
{
    nbIf++;
    string nameBB = ".testIfBB" + to_string(nbIf);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createThenBB()
{
    string nameBB = ".thenBB" + to_string(nbIf);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createElseBB()
{
    string nameBB = ".elseBB" + to_string(nbIf);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

BasicBlock *CFG::createAfterIfBB()
{
    string nameBB = ".afterIfBB" + to_string(nbIf);
    BasicBlock *bb = new BasicBlock(this, nameBB);
    bbs.push_back(bb);
    return bb;
}

void CFG::gen_asm(ostream& o){
    o << ".globl	main\n";
    o << " main: \n";
    for(BasicBlock* bb : bbs){
        bb->gen_asm(o);
    }
}