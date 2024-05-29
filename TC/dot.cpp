/*
output Graphviz(DOT)file
Input: Output stream, "root node" of the syntax tree
Output: Graphviz (DOT language) file at the specified path
*/

#include <stdio.h>
#include <stdbool.h>
#include "parse.h"
#include "dot.h"

char _TR(TokenType x)
{
    char res = '\0';
    switch(x)
    {
        case PLUS: res = '+'; break;
        case MINUS: res = '-'; break;
        case TIMES: res = '*'; break;
        case OVER: res = '/'; break;
        case LPAREN: res = '('; break;
        case RPAREN: res = ')'; break;
    }
    return res;
}

char * _TYPE(ExpType x)
{
    static char s[10];
    switch(x)
    {
        case Integer: sprintf(s, "Int"); break;
        case Float: sprintf(s, "Float"); break;
        case Boolean: sprintf(s, "Bool"); break;
        case Void: sprintf(s, "Void"); break;
    }
    return s;
}

char* trans(TreeNode* x)
{
    static char s[25];
    if (x->nodekind == ExpK)
    {
        Attr p = x->attr;
        switch (p.type)
        {
        case Op:
            sprintf(s, "%s %c(%s)", "Op", _TR(p.attr.op), _TYPE(x->type));
            break;
        case Int:
            sprintf(s, "%s %d", "Int", p.attr.valint);
            break;
        case F:
            sprintf(s, "%s %.2f", "Float", p.attr.valfloat);
            break;
        case Id:
            sprintf(s, "%s %s", "Id", p.attr.name);
            break;
        }
    }
    else
    {
        StmtKind p = x->kind.stmt;
        switch (p)
        {
        case IfK:
            sprintf(s, "IF");
            break;
        case RepeatK:
            sprintf(s, "Repeat");
            break;
        case AssignK:
            sprintf(s, "Assign %s", x->attr.attr.name);
            break;
        case ReadK:
            sprintf(s, "Read");
            break;
        case WriteK:
            sprintf(s, "Write");
            break;
        case DeclareK:
            sprintf(s, "Declare %s -> %s", x->attr.attr.name, _TYPE(x->type));
            break;
        }
    }
    return s;
}

Search_Tree fg;

bool Fg(TreeNode *x)
{
    Search_Tree::Node *p = fg.Head;
    while (p)
    {
        if (x == p->attr) return true;
        p = p->nxt;
    }
    fg.Head = new Search_Tree::Node(x, fg.Head);
    return false;
}

void Create_fg()
{
    fg.Head = NULL;
}

void Delete_fg()
{
    fg.del();
}

void PrintGraphviz(FILE* out, TreeNode* rt)
{
    if (!rt || Fg(rt)) return;
    fprintf(out, "\"%p\" [ label = \"%s\"];\n", rt, trans(rt));
    for (int i = 0; i < MAXCHILDREN; i++)
    {
        if (rt->child[i])
            PrintGraphviz(out, rt->child[i]);
    }

    for (int i = 0; i < MAXCHILDREN; i++)
        if (rt->child[i])
            fprintf(out, "\"%p\" -> \"%p\" [label = \" ch[%d] \"];\n", rt, rt->child[i], i);
    
    if (rt->sibling)
    {
        Delete_fg();
        Create_fg();
        PrintGraphviz(out, rt->sibling);
    }
}

/*
Outputs a Graphviz (DOT language) file
Input: Output file path
Output: Graphviz (DOT language) file at the specified path
*/

void conv_DOT(TreeNode * rt)
{
    FILE *out = fopen("syntax_tree.dot", "w");
    if (!out) {fprintf(listing, "Failed to open Graphviz format output stream"); return ;}
    fprintf(out, "digraph SyntaxTree {\n");
    Create_fg();
    PrintGraphviz(out, rt);
    Delete_fg();
    fprintf(out, "}\n");
}
