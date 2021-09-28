#include "ast.hpp"

void NumberExprAST::codegen() const {
  //TODO
}

void VariableExprAST::codegen() const {
  //TODO
}

BinaryExprAST::~BinaryExprAST() {
  delete LHS;
  delete RHS;
}

void AddExprAST::codegen() const {
  //TODO
}

void SubExprAST::codegen() const {
  //TODO
}

void MulExprAST::codegen() const {
  //TODO
}

void DivExprAST::codegen() const {
  //TODO
}

CallExprAST::~CallExprAST() {
  for (vector<ExprAST*>::iterator i = Args.begin(); i != Args.end(); i++)
    delete *i;
}

void CallExprAST::codegen() const {
  //TODO
}

void PrototypeAST::codegen() const {
  //TODO
}

FunctionAST::~FunctionAST() {
  delete Proto;
  delete Body;
}

void FunctionAST::codegen() const {
  //TODO
}
