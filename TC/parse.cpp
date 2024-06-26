/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"
#include "scan.h"
#include "parse.h"
#define max(a, b) (a > b ? a : b)

static TokenType token; /* holds current token */

Search_Tree Tr;

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(char *name);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static void Create_Search_Tree(void);
static void Delete_Search_Tree(void);
static bool Cmp_Tree(TreeNode*, TreeNode*);
static void Tree_Merge(TreeNode*&);
static TreeNode * exp(void);
static TreeNode * _exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * declare_stmt(ExpType);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode * statement(void)
{ TreeNode * t = NULL;
  switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_stmt(NULL); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
    case DecINT : t = declare_stmt(Integer); break;
    case DecFLOAT : t = declare_stmt(Float); break;
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}

TreeNode * if_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  if (t!=NULL) t->child[0] = exp();
  match(THEN);
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  match(END);
  return t;
}

TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t!=NULL) t->child[1] = exp();
  return t;
}

TreeNode * assign_stmt(char* name)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
  {
    t->attr.attr.name = copyString(tokenString);
    t->attr.type = Id;
  }
  if ((t!=NULL) && (name != NULL))
  {
    t->attr.attr.name = name;
    t->attr.type = Id;
  }
  if (name == NULL) match(ID);
  match(ASSIGN);
  if (t!=NULL) t->child[0] = exp();
  t->type = t->child[0]->type;
  return t;
}

TreeNode * read_stmt(void)
{ TreeNode * t = newStmtNode(ReadK);
  match(READ);
  if ((t!=NULL) && (token==ID))
  {
    t->attr.attr.name = copyString(tokenString);
    t->attr.type = Id;
  }
  match(ID);
  return t;
}

TreeNode * write_stmt(void)
{ TreeNode * t = newStmtNode(WriteK);
  match(WRITE);
  if (t!=NULL) t->child[0] = exp();
  return t;
}


TreeNode * declare_stmt(ExpType type)
{ TreeNode *t = newStmtNode(DeclareK);
  if (type == Integer) {match(DecINT); t->type = Integer;}
  else {match(DecFLOAT); t->type = Float;}
  t->attr.type = Id;
  t->attr.attr.name = copyString(tokenString);
  match(ID);
  if (token==ASSIGN)
  {
    t->child[0] = assign_stmt(t->attr.attr.name);
    if (t->child[0]->type == Float && t->type == Integer) syntaxError("float value assign to int");
  }
  TreeNode *now = t;
  while ((now!=NULL) && (token==COMMA))
  {
    match(COMMA);
    if (token != ID) {syntaxError("unexpected token -> "); printToken(token,tokenString); fprintf(listing,"      ");}
    now->child[1] = newStmtNode(DeclareK);
    now->child[1]->attr.type = now->attr.type;
    now->child[1]->type = now->type;
    now = now->child[1];
    now->attr.attr.name = copyString(tokenString);
    match(ID);
    if (token==ASSIGN)
    {
      now->child[0] = assign_stmt(now->attr.attr.name);
      if (now->child[0]->type == Float && now->type == Integer) syntaxError("float value assign to int");
    }
  }
  return t;
}

void Create_Search_Tree(void)
{
  Tr.Head = NULL;
}

void Delete_Search_Tree(void)
{
  Tr.del();
}

bool Cmp_Tree(TreeNode* a, TreeNode *b)
{
  if (a == NULL)
  {
    if (b == NULL) return true;
    else return false;
  }
  if (b == NULL) return false;

  if (a->type != b->type) return false;
  if (a->kind.exp != b->kind.exp) return false;
  if (a->attr.type != b->attr.type) return false;
  if (a->attr.type == Id) return !strcmp(a->attr.attr.name, b->attr.attr.name);
  else if (a->attr.attr.valfloat != b->attr.attr.valfloat) return false;
  return Cmp_Tree(a->child[0], b->child[0]) && Cmp_Tree(a->child[1], b->child[1]);
}

void Tree_Merge(TreeNode*& a)
{
  if (NoMerge) {return ;}

  Search_Tree::Node *p = Tr.Head;
  bool suc = false;
  while (p)
  {
    if (a == p->attr)
    {
      suc = true;
      break;
    }
    if (Cmp_Tree(a, p->attr))
    {
      delete a;
      a = p->attr;
      suc = true;
      fprintf(listing, "Merge:(%d, %d)\n", a->attr.type, a->attr.attr);
      break;
    }
    p = p->nxt;
  }
  if (!suc)
  {
    Tr.Head = new Search_Tree::Node(a, Tr.Head);
  }
}

static int tmp_cnt = 1;

char * Build_tmp(int argc = -1)
{
  if (~argc)
  {
    char* s = new char[45];
    sprintf(s, "tmp%d", argc);
    return s;
  }

  static char s[45];
  sprintf(s, "tmp%d", tmp_cnt++);
  return s;
}

void TmpVarMerge(TreeNode *x)
{
  if (!x || !x->attr.opid) return ;

  for (int i = 0; i < MAXCHILDREN; i++)
  {
    // TmpVarMerge(x->child[i]);
                                //maximum of depth is 2
    if (!x->child[i] || !x->child[i]->attr.opid) continue;
    
    TreeNode* tmp = newExpNode(IdK);
    tmp->attr.type = Id;
    tmp->attr.attr.name = Build_tmp(x->child[i]->attr.opid);
    tmp->type = (ExpType)((int)tmp->type | (int)x->child[i]->type);
    x->child[i] = tmp;
  }
}

TreeNode * TmpVarBuild(TreeNode *x)
{
  Search_Tree Tr_rev; // Search_Tree is a stack, but we need to enumerate each element sequentially
  Tr_rev.Head = NULL;
  Search_Tree::Node *p = Tr.Head;
  while (p)
  {
    Tr_rev.Head = new Search_Tree::Node(p->attr, Tr_rev.Head);
    p = p->nxt;
  }

  p = Tr_rev.Head;
  TreeNode *res = NULL;
  while (p)
  {
    if (p->attr->attr.type != Op)
    {
      Search_Tree::Node *_p = p;
      p = p->nxt;
      delete _p;
      continue;
    }
    TreeNode *t = newStmtNode(DeclareK);
    p->attr->attr.opid = atoi(Build_tmp() + 3);

    t->child[1] = newStmtNode(AssignK);
    fprintf(listing, "Build tmpvar: %d (%d)\n", p->attr->attr.opid, p->attr->attr.attr.op);
    t->child[1]->attr.attr.name = Build_tmp(p->attr->attr.opid);
    t->child[1]->attr.type = Id;

    t->attr.type = Id;
    t->attr.attr.name = t->child[1]->attr.attr.name;

    TmpVarMerge(p->attr);
    t->child[1]->child[0] = p->attr;
    t->type = t->child[1]->type = t->child[1]->child[0]->type;
    t->attr.type = t->child[1]->attr.type = t->child[1]->child[0]->attr.type;

    Search_Tree::Node *_p = p;
    p = p->nxt;
    delete _p;

    t->child[0] = res;
    res = t;
  }
  if (!res) return x;
  return res;
}

TreeNode * exp(void)
{
  Create_Search_Tree();
  TreeNode *res = _exp();
  if (TmpVarOptimize) {res = TmpVarBuild(res);}
  Delete_Search_Tree();
  return res;
}

TreeNode * _exp(void)
{ TreeNode * t = simple_exp();
  if ((token==LT)||(token==EQ)||(token==GT)||(token==LET)||(token==GET)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.attr.op = token;
      p->attr.type = Op;
      // p->type = Boolean;
      p->type = Integer;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  if (t != NULL) Tree_Merge(t);
  return t;
}

TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS)||(token==XOR))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.attr.op = token;
      p->attr.type = Op;
      t = p;
      match(token);

      t->child[1] = term();
      t->type = (ExpType)((int)t->child[0]->type | (int)t->child[1]->type);

      if (ConstMerge)
      {
        int ty1 = t->child[0]->attr.type, ty2 = t->child[1]->attr.type;
        if (((ty1 & 1) ^ (ty1 >> 1)) &&
            ((ty2 & 1) ^ (ty2 >> 1)))
        {
          float t1, t2;
          if (t->child[0]->type == Float) t1 = t->child[0]->attr.attr.valfloat;
          else t1 = t->child[0]->attr.attr.valint;
          if (t->child[1]->type == Float) t2 = t->child[1]->attr.attr.valfloat;
          else t2 = t->child[1]->attr.attr.valint;

          t->kind.exp = ConstK;
          t->attr.type = (AttrType)max(ty1, ty2);
          if (t->attr.type == Int)
          {
            switch(p->attr.attr.op)
            {
              case PLUS: t->attr.attr.valint = (int)(t1 + t2); break;
              case MINUS: t->attr.attr.valint = (int)(t1 - t2); break;
              case XOR: t->attr.attr.valint = (int)t1 ^ (int)t2; break;
            }
          }
          else
          {
            switch(p->attr.attr.op)
            {
              case PLUS: t->attr.attr.valfloat = t1 + t2; break;
              case MINUS: t->attr.attr.valfloat = t1 - t2; break;
              case XOR: syntaxError("XOR applied to float"); break;
            }
          }

          // delete t->child[0];
          t->child[0] = NULL;
          // delete t->child[1];
          t->child[1] = NULL;

        }
      }

      Tree_Merge(t);
    }
  }
  if (t != NULL) Tree_Merge(t);
  return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==OVER))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.attr.op = token;
      p->attr.type = Op;
      t = p;
      match(token);

      p->child[1] = factor();
      t->type = (ExpType)((int)t->child[0]->type | (int)t->child[1]->type);

      if (ConstMerge)
      {
        int ty1 = t->child[0]->attr.type, ty2 = t->child[1]->attr.type;
        if (((ty1 & 1) ^ (ty1 >> 1)) &&
            ((ty2 & 1) ^ (ty2 >> 1)))
        {
          float t1, t2;
          if (t->child[0]->type == Float) t1 = t->child[0]->attr.attr.valfloat;
          else t1 = t->child[0]->attr.attr.valint;
          if (t->child[1]->type == Float) t2 = t->child[1]->attr.attr.valfloat;
          else t2 = t->child[1]->attr.attr.valint;

          t->kind.exp = ConstK;
          t->attr.type = (AttrType)max(ty1, ty2);
          if (t->attr.type == Int) t->attr.attr.valint = (int)(t->attr.attr.op == TIMES ? (t1 * t2) : (t1 / t2));
          else t->attr.attr.valfloat = (t->attr.attr.op == TIMES ? (t1 * t2) : (t1 / t2));

          // delete t->child[0];
          t->child[0] = NULL;
          // delete t->child[1];
          t->child[1] = NULL;

        }
      }

      Tree_Merge(t);
    }
  }
  if (t != NULL) Tree_Merge(t);
  return t;
}

TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case INT :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==INT))
      {
        t->attr.attr.valint = atoi(tokenString);
        t->attr.type = Int;
        t->type = Integer;
      }
      match(INT);
      break;
    case FLOAT :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==FLOAT))
      {
        sscanf(tokenString, "%f", &t->attr.attr.valfloat);
        t->attr.type = F;
        t->type = Float;
      }
      match(FLOAT);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
      {
        t->attr.attr.name = copyString(tokenString);
        t->attr.type = Id;
        t->type = Integer; // Default: regard id as int
      }
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = _exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  if (t != NULL) Tree_Merge(t);
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = stmt_sequence();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
