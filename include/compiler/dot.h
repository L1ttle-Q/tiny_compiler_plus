/*
output Graphviz(DOT)file
Input: Output stream, "root node" of the syntax tree
Output: Graphviz (DOT language) file at the specified path
*/

#ifndef _DOT_H_
#define _DOT_H_

#include "globals.h"

void conv_DOT(TreeNode *);

#endif