#ifndef __AST_HPP__
#define __AST_HPP__ 1

#include <string>
#include <vector>
using namespace std;

class ExprAST {
public:
  virtual void codegen() const = 0;
  virtual ~ExprAST() {

  }
};

class NumberExprAST : public ExprAST {
public:
  NumberExprAST(double x)
    :Val(x)
  {}
  void codegen() const;
private:
  double Val;
};

class VariableExprAST : public ExprAST {
public:
  VariableExprAST(string s)
    :Name(s)
  {}
  void codegen() const;
private:
  string Name;
};

class BinaryExprAST : public ExprAST {
public:
  BinaryExprAST(ExprAST *l, ExprAST *r)
    :LHS(l), RHS(r)
  {}
  ~BinaryExprAST();
private:
  BinaryExprAST(const BinaryExprAST&);
  BinaryExprAST& operator=(const BinaryExprAST&);
protected:
  ExprAST *LHS, *RHS;
};

class AddExprAST : public BinaryExprAST {
public:
  AddExprAST(ExprAST *l, ExprAST *r)
    :BinaryExprAST(l, r)
  {}
  void codegen() const;
};

class SubExprAST : public BinaryExprAST {
public:
  SubExprAST(ExprAST *l, ExprAST *r)
    :BinaryExprAST(l, r)
  {}
  void codegen() const;
};

class MulExprAST : public BinaryExprAST {
public:
  MulExprAST(ExprAST *l, ExprAST *r)
    :BinaryExprAST(l, r)
  {}
  void codegen() const;
};

class DivExprAST : public BinaryExprAST {
public:
  DivExprAST(ExprAST *l, ExprAST *r)
    :BinaryExprAST(l, r)
  {}
  void codegen() const;
};

class CallExprAST : public ExprAST {
public:
  CallExprAST(string s, vector<ExprAST*> v)
    :Callee(s), Args(v)
  {}
  void codegen() const;
  ~CallExprAST();
private:
  CallExprAST(const CallExprAST&);
  CallExprAST& operator=(const CallExprAST&);
  string Callee;
  vector<ExprAST*> Args;
};

class PrototypeAST {
public:
  PrototypeAST(string s, vector<string> v)
    :Name(s), Args(v)
  {}
  void codegen() const;
private:
  string Name;
  vector<string> Args;
};

class FunctionAST {
public:
  FunctionAST(PrototypeAST *p, ExprAST *e)
    :Proto(p), Body(e)
  {}
  void codegen() const;
  ~FunctionAST();
private:
  FunctionAST(const FunctionAST&);
  FunctionAST& operator=(const FunctionAST&);
  PrototypeAST *Proto;
  ExprAST *Body;
};

#endif
