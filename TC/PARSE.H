/****************************************************/
/* File: parse.h                                    */
/* The parser interface for the TINY compiler       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _PARSE_H_
#define _PARSE_H_

#include "globals.h"

/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void);

struct Search_Tree {
  struct Node {
    TreeNode * attr;
    Node *nxt;
    Node(){}
    Node(TreeNode *a, Node *b) {attr = a; nxt = b;}
  }*Head;

  void del()
  {
    while (Head != NULL)
    {
      Node *p = Head;
      Head = Head->nxt;
      delete p;
    }
  }
};

#endif
