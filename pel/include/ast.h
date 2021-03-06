/*
 * File: ast.h
 * Provides syntax tree's representation for parser.
 */

#ifndef PEL_AST_H
#define PEL_AST_H

#include <cstdio>
#include "vector.h"

namespace pel{

class ASTNode;
class ASTPathExpr;
class Executor; // exe.h
class Token;   // scanner.h


//------------------------------------------------------------------------
// AST.
// All tree nodes and tokens are allocated by parser and managed in ASTree.
// Destroy tree will release all nodes and tokens.
// And don't delete or free any tree nodes and tokens outside of tree.
//------------------------------------------------------------------------
class ASTree{
public:
  ASTree(): root(0){}
  ~ASTree();

  void Print(FILE *stream=stdout) const;

  const ASTPathExpr* Root() const{ return root; }

private:
  ASTPathExpr *root;

  // Managed nodes and tokens.
  // Uses these managed objects to record allocation of memory and
  // release them after using.
  Vector<ASTNode*> nodes;
  Vector<Token*> tokens;
  
private:
  ASTree(const ASTree&); // copy is disabled
  ASTree& operator=(const ASTree&);

  // ASTree is generated by Parser and ParserImpl.
  friend class Parser;
  friend class ParserImpl;

  // Used by parser.
  // Don't delete new nodes outside of ASTree.
  template<class T> T* newNode(){
    T* node = new T();
    nodes.Push(node);
    return node;
  }

  // Used by parser, and don't delete new tokens outside of ASTree.
  Token *newToken();
};

//-----------------------
// base class of tree node
//-----------------------
class ASTNode{
public:
  enum ClassID{
    ASTNode_ = 0,  // IDs consist of class name and _
    ASTPathExpr_ ,
    ASTPathElemExpr_ ,
    ASTModelElemExpr_ ,
    ASTypeCastExpr_, 
    ASTNodeLabelExpr_ ,
    ASTNodeCastExpr_ ,
    ASTPathDecl_ ,
    ASTCondExpr_ ,
    ASTOrExpr_ ,
    ASTAndExpr_ ,
    ASTCondTerm_ ,
    ASTBaseTerm_ ,
    ASTRelOp_ ,
    ASTFuncExpr_ ,
    ASTRecurPathExpr_ ,
    ASTConstExpr_ ,
    ASTComplexLit_ ,
    ASTValLit_ ,
    ASTListLit_ ,
    ASTInterLit_ ,
    ASTolerLit_ ,
    ASTVarExpr_ ,
    ASTMetaVarExpr_ ,
    ASTIndexOp_ ,
    ClassIDSize, // indicates size of class ids
  };

  // return class id
  ClassID ID() const{ return id; }

  // determine instance by id
  static bool classof(const ASTNode* p){
    return p->ID() == ASTNode_;
  }

  // The only way to destroy derived classes by base pointers.
  void Destroy();

protected:
  // default
  ASTNode(): id(ASTNode_){}

  // used by subclasses
  explicit ASTNode(ClassID sid): id(sid){}

  ~ASTNode(){} // nothing to release

private:
  ClassID id;

private:
  // disable copy ?
  ASTNode(const ASTNode&);
  ASTNode& operator=(const ASTNode&);
};


// forward decl
class ASTPathExpr;
class ASTPathElemExpr;
class ASTModelElemExpr;
class ASTypeCastExpr;
class ASTNodeLabelExpr;
class ASTNodeCastExpr;
class ASTPathDecl;
class ASTCondExpr;
class ASTOrExpr;
class ASTAndExpr;
class ASTCondTerm;
class ASTBaseTerm;
class ASTRelOp;
class ASTFuncExpr;
class ASTRecurPathExpr;
class ASTConstExpr;
class ASTComplexLit;
class ASTValLit;
class ASTListLit;
class ASTInterLit;
class ASTolerLit;
class ASTVarExpr;
class ASTMetaVarExpr;
class ASTIndexOp;


class ASTPathExpr: public ASTNode{
public:
  ASTPathExpr(): ASTNode(ASTPathExpr_){
  }

  ~ASTPathExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTPathExpr_;
  }

  // Recursively print AST to console.
  // @trunk Trunk representation of ASTree, consists of
  // ' ', '|', '-', and '`'.
  // More to see AST::Print().
  void Print(Vector<char>& trunk)const;

  // Recursively prepare execution units according to AST.
  // @exe Executor to be prepared.
  void Prepare(Executor& exe)const;
  
  //
  Vector<const ASTPathElemExpr*> elems;
};


class ASTPathElemExpr: public ASTNode{
public:
  ASTPathElemExpr(): ASTNode(ASTPathElemExpr_){
    sel.model = 0;
    index = 0;
  }

  ~ASTPathElemExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTPathElemExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  union{
    const ASTNode          *ptr;
    const ASTModelElemExpr *model;
    const ASTFuncExpr      *func;
    const ASTRecurPathExpr *path;
    const ASTConstExpr     *constt;
  }sel;

  const ASTIndexOp *index; // optional
};


class ASTModelElemExpr: public ASTNode{
public:
  ASTModelElemExpr(): ASTNode(ASTModelElemExpr_){
    ident = 0;
    type = 0;
    label = 0;
    node = 0;
  }
  
  ~ASTModelElemExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTModelElemExpr_;
  }

  void Print(Vector<char>& trunk)const;
  
  void Prepare(Executor& exe)const;
  
  const Token             *ident;  // term
  const ASTypeCastExpr    *type;
  const ASTNodeLabelExpr  *label;
  const ASTNodeCastExpr   *node;
};


class ASTypeCastExpr: public ASTNode{
public:
  ASTypeCastExpr():ASTNode(ASTypeCastExpr_){
    ident = 0;
  }

  ~ASTypeCastExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTypeCastExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *ident; // term
};


class ASTNodeLabelExpr: public ASTNode{
public:
  ASTNodeLabelExpr():ASTNode(ASTNodeLabelExpr_){
    ident = 0;
  }

  ~ASTNodeLabelExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTNodeLabelExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *ident; // term
};


class ASTNodeCastExpr: public ASTNode{
public:
  ASTNodeCastExpr():ASTNode(ASTNodeCastExpr_){
    cond = 0;
  }

  ~ASTNodeCastExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTNodeCastExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  Vector<const ASTPathDecl*> paths;
  const ASTCondExpr         *cond;
};


class ASTPathDecl: public ASTNode{
public:
  ASTPathDecl():ASTNode(ASTPathDecl_){
    ident = 0;
    path = 0;
  }

  ~ASTPathDecl(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTPathDecl_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token       *ident;
  const ASTPathExpr *path;
};


class ASTCondExpr: public ASTNode{
public:
  ASTCondExpr():ASTNode(ASTCondExpr_){
    or_ = 0;
  }
  
  ~ASTCondExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTCondExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const ASTOrExpr *or_;
};


class ASTOrExpr: public ASTNode{
public:
  ASTOrExpr():ASTNode(ASTOrExpr_){
  }

  ~ASTOrExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTOrExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  Vector<const ASTAndExpr*> ands;
};


class ASTAndExpr: public ASTNode{
public:
  ASTAndExpr():ASTNode(ASTAndExpr_){
  }

  ~ASTAndExpr(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTAndExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  Vector<const ASTCondTerm*> terms;
};


class ASTCondTerm: public ASTNode{
public:
  ASTCondTerm(): ASTNode(ASTCondTerm_){
    sel.base = 0;
  }

  ~ASTCondTerm(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTCondTerm_;
  }
  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  union{
    const ASTNode     *ptr;
    const ASTBaseTerm *base;
    const ASTCondExpr *cond;
  }sel;
};


class ASTBaseTerm: public ASTNode{
public:
  ASTBaseTerm(): ASTNode(ASTBaseTerm_){
    lpath = 0;
    op = 0;
    rpath = 0;
  }

  ~ASTBaseTerm(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTBaseTerm_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const ASTPathExpr *lpath;
  const ASTRelOp    *op;
  const ASTPathExpr *rpath;
};


class ASTRelOp: public ASTNode{
public:
  ASTRelOp(): ASTNode(ASTRelOp_){
    op = 0;
  }

  ~ASTRelOp(){
  }

  static bool classof(const ASTNode* p){
    return p->ID() == ASTRelOp_;
  }

  void Print(Vector<char>& trunk)const;
  
  void Prepare(Executor& exe)const;
  
  const Token  *op; // term
};


class ASTFuncExpr: public ASTNode{
public:
  ASTFuncExpr(): ASTNode(ASTFuncExpr_){
    ident = 0;
  }

  ~ASTFuncExpr(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTFuncExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token               *ident; // func name
  Vector<const ASTPathExpr*> paths; // params
};


class ASTRecurPathExpr: public ASTNode{
public:
  ASTRecurPathExpr(): ASTNode(ASTRecurPathExpr_){
    path = 0;
    plusplus = false;
  }

  ~ASTRecurPathExpr(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTRecurPathExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const ASTPathExpr *path;
  bool  plusplus; // ?
};


class ASTConstExpr: public ASTNode{
public:
  ASTConstExpr(): ASTNode(ASTConstExpr_){
    sel.var = 0;
  }

  ~ASTConstExpr(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTConstExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  union{
    const ASTNode         *ptr;
    const ASTComplexLit   *complex_;
    const ASTVarExpr      *var;
    const ASTMetaVarExpr  *meta;
  }sel;
};


class ASTComplexLit: public ASTNode{
public:
  ASTComplexLit(): ASTNode(ASTComplexLit_){
    sel.val = 0;
  }

  ~ASTComplexLit(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTComplexLit_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  union{
    const ASTNode     *ptr;
    const ASTValLit   *val;
    const ASTListLit  *list;
    const ASTInterLit *inter;
    const ASTolerLit  *toler;
  }sel;
};


class ASTValLit: public ASTNode{
public:
  ASTValLit(): ASTNode(ASTValLit_){
    val = 0;
  }

  ~ASTValLit(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTValLit_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *val;
};


class ASTListLit: public ASTNode{
public:
  ASTListLit(): ASTNode(ASTListLit_){
  }

  ~ASTListLit(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTListLit_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  Vector<const ASTValLit*> vals;
};


class ASTInterLit: public ASTNode{
public:
  ASTInterLit(): ASTNode(ASTInterLit_){
    l = r = 0;
  }

  ~ASTInterLit(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTInterLit_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const ASTValLit *l, *r;
};


class ASTolerLit: public ASTNode{
public:
  ASTolerLit(): ASTNode(ASTolerLit_){
    l = r = 0;
  }

  ~ASTolerLit(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTolerLit_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const ASTValLit *l, *r;
};


class ASTVarExpr: public ASTNode{
public:
  ASTVarExpr():ASTNode(ASTVarExpr_){
    ident = 0;
  }

  ~ASTVarExpr(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTVarExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *ident; //term
};


class ASTMetaVarExpr: public ASTNode{
public:
  ASTMetaVarExpr(): ASTNode(ASTMetaVarExpr_){
    ident = 0;
  }

  ~ASTMetaVarExpr(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTMetaVarExpr_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *ident; //term
};


class ASTIndexOp: public ASTNode{
public:
  ASTIndexOp(): ASTNode(ASTIndexOp_){
    intv = 0;
  }

  ~ASTIndexOp(){}

  static bool classof(const ASTNode* p){
    return p->ID() == ASTIndexOp_;
  }

  void Print(Vector<char>& trunk)const;

  void Prepare(Executor& exe)const;
  
  const Token *intv;
};


} // ~pel


#endif
