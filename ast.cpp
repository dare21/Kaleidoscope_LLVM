#include "ast.hpp"
#include <iostream>

//language uses floating point values, hence the use of ConstantFP

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
Module* TheModule;
map<string, AllocaInst*> NamedValues;
legacy::FunctionPassManager* TheFPM;


//generating a num constant
Value* NumberExprAST::codegen() const {
  return ConstantFP::get(TheContext, APFloat(Val));
}

//generating a variable
Value* VariableExprAST::codegen() const {
  AllocaInst* V = NamedValues[Name];
  if (!V) {
    cerr << "Nepoznata promenljiva " << Name << endl;
    return nullptr;
  }
  return V;
}

//destructor for binary expressions
BinaryExprAST::~BinaryExprAST() {
  delete LHS;
  delete RHS;
}

//generating an "add" expression
Value* AddExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateFAdd(L, R, "addtmp");
}

//generating a "sub" expression
Value* SubExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateFSub(L, R, "subtmp");
}

//generating a "mul" expression
Value* MulExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateFMul(L, R, "multmp");
}

//generating a "div" expression
Value* DivExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateFDiv(L, R, "divtmp");
}

//generating a "lower then" expression
Value* LtExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateUIToFP(Builder.CreateFCmpOLT(L, R, "lttmp"), Type::getDoubleTy(TheContext), "booltmp");
}

//generating a "greater then" expression
Value* GtExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateUIToFP(Builder.CreateFCmpOGT(L, R, "gttmp"), Type::getDoubleTy(TheContext), "booltmp");
}

//generating code for a "seq" expression
Value* SeqExprAST::codegen() const {
  Value* L = LHS->codegen();
  Value* R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  return R;
}

//destructor for function call
CallExprAST::~CallExprAST() {
  for (vector<ExprAST*>::iterator i = Args.begin(); i != Args.end(); i++)
    delete *i;
}

//generating code for a function call
Value* CallExprAST::codegen() const {
  Function *f = TheModule->getFunction(Callee);
  if (!f) {
    cerr << "Called an unidentified function: " << Callee << endl;
    return nullptr;
  }

  if (Args.size() != f->arg_size()) {
    cerr << "Function " << Callee << " has been called with incorrect number of arguments!" << endl;
    return nullptr;
  }

  vector<Value*> ArgsV;
  for (auto e: Args) {
    Value *tmp = e->codegen();
    if (!tmp)
      return nullptr;
      
    ArgsV.push_back(tmp);
  }

  return Builder.CreateCall(f, ArgsV, "calltmp");
}

//generating code for an "if" expression
Value* IfExprAST::codegen() const {
  Value *CondV = Cond->codegen();
  if (!CondV)
    return nullptr;

  Value *Tmp = Builder.CreateFCmpONE(CondV, ConstantFP::get(TheContext, APFloat(0.0)), "ifcond");

  Function *f = Builder.GetInsertBlock()->getParent();
  
  BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", f);
  BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else");
  BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");
  
  Builder.CreateCondBr(Tmp, ThenBB, ElseBB);
 
  Builder.SetInsertPoint(ThenBB);
  Value *ThenV = Then->codegen();
  if (!ThenV)
    return nullptr;
  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();

  f->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);
  Value *ElseV = Else->codegen();
  if (!ElseV)
    return nullptr;
  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();

  f->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  PHINode *PHI = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");
  PHI->addIncoming(ThenV, ThenBB);
  PHI->addIncoming(ElseV, ElseBB);

  return PHI;
}

//destrcutor for an "if" expression
IfExprAST::~IfExprAST() {
  delete Cond;
  delete Then;
  delete Else;
}

//generating code for a "for" loop
Value* ForExprAST::codegen() const {
  Value *StartV = Start->codegen();
  if (!StartV)
    return nullptr;

  Function *f = Builder.GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", f);

  AllocaInst *Alloca = CreateEntryBlockAlloca(f, VarName);
  Builder.CreateStore(StartV, Alloca);
  
  Builder.CreateBr(LoopBB);

  Builder.SetInsertPoint(LoopBB);
  

  AllocaInst *OldVal = NamedValues[VarName];
  NamedValues[VarName] = Alloca;

  Value* EndV = End->codegen();
  if (!EndV)
    return nullptr;
  Value *Tmp = Builder.CreateFCmpONE(EndV, ConstantFP::get(TheContext, APFloat(0.0)), "loopcond");
  BasicBlock *Loop1BB = BasicBlock::Create(TheContext, "loop1", f);
  BasicBlock *AfterLoopBB = BasicBlock::Create(TheContext, "afterloop");
  Builder.CreateCondBr(Tmp, Loop1BB, AfterLoopBB);
  
  Builder.SetInsertPoint(Loop1BB);
  Value* BodyV = Body->codegen();
  if (!BodyV)
    return nullptr;

  Value* StepV = Step->codegen();
  if (!StepV)
    return nullptr;
  Value *CurrVal = Builder.CreateLoad(Alloca, VarName);
  Value *NextVar = Builder.CreateFAdd(CurrVal, StepV, "nextvar");
  Builder.CreateStore(NextVar, Alloca);
  Builder.CreateBr(LoopBB);

  if (OldVal)
    NamedValues[VarName] = OldVal;
  else
    NamedValues.erase(VarName);

  f->getBasicBlockList().push_back(AfterLoopBB);
  Builder.SetInsertPoint(AfterLoopBB);

  return ConstantFP::get(TheContext, APFloat(0.0));
}

//destructor for a "for" loop
ForExprAST::~ForExprAST() {
  delete Start;
  delete End;
  delete Step;
  delete Body;
}

//generating code for a function prototype
Function* PrototypeAST::codegen() const {
  vector<Type*> tmp;
  for (unsigned i = 0; i < Args.size(); i++)
    tmp.push_back(Type::getDoubleTy(TheContext));
  
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(TheContext), tmp, false);
  
  Function* f = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);

  unsigned i = 0;
  for (auto &a : f->args())
    a.setName(Args[i++]);

  return f;
}

//destructor for functions
FunctionAST::~FunctionAST() {
  delete Proto;
  delete Body;
}

//generating code of a function
Value* FunctionAST::codegen() const {
  Function *f = TheModule->getFunction(Proto->getName());

  if (!f)
    f = Proto->codegen();
  if (!f)
    return nullptr;

  if (!f->empty()) {
    cerr << "Funkcija " << Proto->getName() << " ne moze da se redefinise" << endl;
    return nullptr;
  }

  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", f);
  Builder.SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &a : f->args()) {
    AllocaInst *Alloca = CreateEntryBlockAlloca(f, string(a.getName()));
    NamedValues[string(a.getName())] = Alloca;
    Builder.CreateStore(&a, Alloca);
  }

  Value* tmp = Body->codegen();
  if (tmp != nullptr) {
    Builder.CreateRet(tmp);

    verifyFunction(*f);

    TheFPM->run(*f);
    
    return f;
  }

  f->eraseFromParent();
  
  return nullptr;
}

Value* AssignExprAST::codegen() const {
  Value *r = Expression->codegen();
  if (!r)
    return nullptr;

  Builder.CreateStore(r, NamedValues[Name]);

  return r;
}

AssignExprAST::~AssignExprAST() {
  delete Expression;
}

Value* VarExprAST::codegen() const {

  Function *f = Builder.GetInsertBlock()->getParent();
  
  vector<AllocaInst*> oldAllocas;
  for (unsigned i = 0; i < VarNames.size(); i++)
    oldAllocas.push_back(NamedValues[VarNames[i].first]);
  
  for (unsigned i = 0; i < VarNames.size(); i++) {
    AllocaInst *Alloca = CreateEntryBlockAlloca(f, VarNames[i].first);
    NamedValues[VarNames[i].first] = Alloca;
    Value *tmp = VarNames[i].second->codegen();
    if (!tmp)
      return nullptr;
    Builder.CreateStore(tmp, Alloca);
  }

  Value *b = Body->codegen();
  if (!b)
    return nullptr;

  for (unsigned i = 0; i < oldAllocas.size(); i++) {
    if (oldAllocas[i])
      NamedValues[VarNames[i].first] = oldAllocas[i];
    else
      NamedValues.erase(VarNames[i].first);
  }
  
  return b;
}

VarExprAST::~VarExprAST() {
  for (unsigned i = 0; i < VarNames.size(); i++)
    delete VarNames[i].second;
  delete Body;
}

//module and pass manager initialization
void InitializeModuleAndPassManager() {
  TheModule = new Module("My module", TheContext);
  
  TheFPM = new legacy::FunctionPassManager(TheModule);
  
  TheFPM->add(createInstructionCombiningPass());
  TheFPM->add(createReassociatePass());
  TheFPM->add(createGVNPass());
  TheFPM->add(createCFGSimplificationPass());
  TheFPM->add(createPromoteMemoryToRegisterPass());

  TheFPM->doInitialization();
}

AllocaInst* CreateEntryBlockAlloca(Function *f, string s) {
  IRBuilder<> TmpBuilder(&(f->getEntryBlock()), f->getEntryBlock().begin());
  return TmpBuilder.CreateAlloca(Type::getDoubleTy(TheContext), 0, s);
}