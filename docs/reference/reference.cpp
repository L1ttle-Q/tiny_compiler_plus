/*
用于递归输出Graphviz(DOT语言)文件
输入：输出流、语法树“根节点”
输出：指定路径的Graphviz(DOT语言)文件
*/
void GrammarChecker::_outputGraphvizFormat(std::ofstream& ofs, GrammarTreeNode* rootNode)
{
    while (rootNode) {
        // 如果是非终结符，先输出它的所有子孩子，再向下层（孩子）递归
        if (nonterminals.find(rootNode->value) != nonterminals.end()) {
            // 如果这个根节点有孩子，则输出它的孩子们
            if (rootNode->leftChild) {
                // 输出父dot声明，使指针成为唯一id，避免重名
                ofs << "\"" << rootNode << "\" [ label = \"" << &rootNode << " " << rootNode->value << "\"];\n";
                // 输出子dot声明
                for (auto child = rootNode->leftChild; child != NULL; child = child->rightNode)
                    ofs << "\"" << child << "\" [ label = \"" << &child << " " << child->value << "\"];\n";
                // 输出父子连接
                ofs << "\"" << rootNode << "\" -> { ";
                for (auto child = rootNode->leftChild; child != NULL; child = child->rightNode)
                    ofs << " \"" << child << "\" ";
                ofs << "};\n";
            }
            // 然后在孩子层继续遍历
            _outputGraphvizFormat(ofs, rootNode->leftChild);
        }
        // 如果是终结符，则跳过（已经被上一层打印过了）
        rootNode = rootNode->rightNode;
    }
}

/*
输出Graphviz(DOT语言)文件
输入：输出文件路径
输出：指定路径的Graphviz(DOT语言)文件
*/
void GrammarChecker::outputGraphvizFormat(const char* outputFilePath)
{
    std::ofstream ofs(outputFilePath);
    if (!ofs.is_open()) throw std::runtime_error("Graphviz格式输出流打开失败");
    ofs << "digraph SyntaxTree {\n";
    _outputGraphvizFormat(ofs, syntaxTree, 1);
    ofs << "}\n";
}