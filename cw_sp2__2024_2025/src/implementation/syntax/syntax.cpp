#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: syntax.cpp                  *
*                                                  (draft!) *
*************************************************************/

#include "../../include/def.h"
#include "../../include/config.h"
#include "../../include/syntax/syntax.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
//#include <unordered_map>
#include <string>
#include <set>

using namespace std;

Grammar grammar = {
    CONFIGURABLE_GRAMMAR
};

#if 0
Grammar originalGrammar = {
    ORIGINAL_GRAMMAR
};
#endif

#define DEBUG_STATES

#define MAX_LEXEMS 256
//#define MAX_RULES 128
#define MAX_SYMBOLS 64

typedef struct {
    char symbols[MAX_SYMBOLS][MAX_TOKEN_SIZE];
    int count;
} SymbolSet;

typedef SymbolSet ParseInfoTable[MAX_LEXEMS][MAX_LEXEMS];

bool insertIntoSymbolSet(SymbolSet* set, const char* symbol) {
    for (int i = 0; i < set->count; ++i) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            // symbol already exists
            return false;
        }
    }
    strncpy(set->symbols[set->count], symbol, MAX_TOKEN_SIZE);
    set->symbols[set->count][MAX_TOKEN_SIZE - 1] = '\0';
    ++set->count;
    return true;
}

bool containsSymbolSet(const SymbolSet* set, const char* symbol) {
    for (int i = 0; i < set->count; ++i) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            return true;
        }
    }
    return false;
}

// initialize with empty SymbolSets
ParseInfoTable parseInfoTable = { { { 0 } } };

struct ASTNode {
    char isPartialSuccess;
    //
    std::string value;
    bool isTerminal;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& val, bool isTerminal) : isTerminal(isTerminal), value(val) {}
    ~ASTNode() {
        for (ASTNode* child : children) {
            delete child;
        }
    }
};

ASTNode* buildASTByCPPMap(const std::map<int, std::map<int, std::set<std::string>>>& parseInfoTable,
    Grammar* grammar,
    int start,
    int end,
    const std::string& symbol) {
    if (start > end) return nullptr;

    ASTNode* node = new ASTNode(symbol, false);

    for (const Rule& rule : grammar->rules) {
        if (rule.lhs != symbol) continue;

        if (rule.rhs_count == 1) {
            //if (parseInfoTable.at(start).at(end).count(rule.rhs[0])) {
            node->children.push_back(new ASTNode(rule.rhs[0], true));
            return node;
            //}
        }
        else if (rule.rhs_count == 2) {
            for (int split = start; split < end; ++split) {
                if (parseInfoTable.at(start).at(split).count(rule.rhs[0]) &&
                    parseInfoTable.at(split + 1).at(end).count(rule.rhs[1])) {
                    node->children.push_back(buildASTByCPPMap(parseInfoTable, grammar, start, split, rule.rhs[0]));
                    node->children.push_back(buildASTByCPPMap(parseInfoTable, grammar, split + 1, end, rule.rhs[1]));
                    return node;
                }
            }
        }
    }

    return nullptr;
}

ASTNode* buildAST(//const std::map<int, std::map<int, std::set<std::string>>>& parseInfoTable,
    ParseInfoTable& parseInfoTable,
    Grammar* grammar,
    int start,
    int end,
    const std::string& symbol) {
    if (start > end) return nullptr;

    ASTNode* node = new ASTNode(symbol, false);

    for (const Rule& rule : grammar->rules) {
        if (rule.lhs != symbol) continue;

        if (rule.rhs_count == 1) {
            //if (parseInfoTable.at(start).at(end).count(rule.rhs[0])) {
            node->children.push_back(new ASTNode(rule.rhs[0], true));
            return node;
            //}
        }
        else if (rule.rhs_count == 2) {
            for (int split = start; split < end; ++split) {
                if (containsSymbolSet(&parseInfoTable[start][split], rule.rhs[0]) && 
                    containsSymbolSet(&parseInfoTable[split + 1][end], rule.rhs[1])) {
                    node->children.push_back(buildAST(parseInfoTable, grammar, start, split, rule.rhs[0]));
                    node->children.push_back(buildAST(parseInfoTable, grammar, split + 1, end, rule.rhs[1]));
                    return node;
                }
            }
        }
    }

    return nullptr;
}

int getSyntaxError(struct LexemInfo* lexemInfoTable, const ASTNode* node, char &toError, int depth = 0) {
    static int lexemInfoTableIndexForPrintAST = 0; // ATTENTION: multithreading is not supported for this!

    if (!node) {
        return lexemInfoTableIndexForPrintAST;
    }
    if (!depth) {
        lexemInfoTableIndexForPrintAST = 0;
    }

    if (!node->isTerminal) {
        if (node->children.size() < 2) {
            if (toError == 1) {
                toError = 2;
            }
            else {
                toError = 1;
            }
        }
        else {
            toError = 0;
        }
    }

    if (node->isTerminal) {
        ++lexemInfoTableIndexForPrintAST;
        if (toError == 1) {
            toError = 0;
        }
        else if (toError == 2) {
            toError = 3;
            return lexemInfoTableIndexForPrintAST;
        }
    }

    for (const ASTNode* child : node->children) {
        getSyntaxError(lexemInfoTable, child, toError, depth + 1);
        if (toError == 3) {
            return lexemInfoTableIndexForPrintAST;
        }
    }

    return lexemInfoTableIndexForPrintAST;
}

void printAST(struct LexemInfo* lexemInfoTable, const ASTNode* node, int depth = 0) {
    static int lexemInfoTableIndexForPrintAST = 0; // ATTENTION: multithreading is not supported for this!
    if (!node) { 
        return; 
    }
    if (!depth) {
        lexemInfoTableIndexForPrintAST = 0;
    }

    for (unsigned int depthIndex = 0; depthIndex <= depth; ++depthIndex) {
        std::cout << "    " << "|";
    }

    std::cout << "--";
    if (node->isTerminal) {
        std::cout << "\"" << lexemInfoTable[lexemInfoTableIndexForPrintAST++].lexemStr << "\"";
    }   
    else {
        std::cout << node->value;
    }
    std::cout << "\n";

    for (const ASTNode* child : node->children) {
        printAST(lexemInfoTable, child, depth + 1);
    }
}

void printASTToFile(struct LexemInfo* lexemInfoTable, const ASTNode* node, std::ofstream& outFile, int depth = 0) {
    static int lexemInfoTableIndexForPrintAST = 0; // ATTENTION: multithreading is not supported for this!
    if (!node) {
        return;
    }
    if (!depth) {
        lexemInfoTableIndexForPrintAST = 0;
    }

    for (unsigned int depthIndex = 0; depthIndex <= depth; ++depthIndex) {
        outFile << "    |";
    }
    outFile << "--";

    if (node->isTerminal) {
        outFile << "\"" << lexemInfoTable[lexemInfoTableIndexForPrintAST++].lexemStr << "\"";
    }
    else {
        outFile << node->value;
    }
    outFile << "\n";

    for (const ASTNode* child : node->children) {
        printASTToFile(lexemInfoTable, child, outFile, depth + 1);
    }
}

void printAST__OLD_123(struct LexemInfo* lexemInfoTable, const ASTNode* node, int depth = 0) {
    static int lexemInfoTableIndexForPrintAST = 0; // ATTENTION: multithreading is not supported for this!
    if (!node) {
        return;
    }
    if (!depth) {
        lexemInfoTableIndexForPrintAST = 0;
    }
    for (unsigned int depthIndex = 0; depthIndex <= depth; ++depthIndex) {
        std::cout << "    " << "|";
    }
    std::cout << "--";
    if (node->isTerminal) {
        std::cout << "\"" << lexemInfoTable[lexemInfoTableIndexForPrintAST++].lexemStr << "\"";
    }
    else {
        std::cout << node->value;
    }
    std::cout << "\n";
    for (const ASTNode* child : node->children) {
        printAST(lexemInfoTable, child, depth + 1);
    }
}

void displayParseInfoTable(const map<int, map<int, set<string>>>& parseInfoTable) {
    constexpr int CELL_WIDTH = 128;

    cout << left << setw(CELL_WIDTH) << "[i\\j]";

    for (const auto& outerEntry : parseInfoTable) {
        cout << setw(CELL_WIDTH) << outerEntry.first;
    }
    cout << endl;

    for (const auto& outerEntry : parseInfoTable) {
        int i = outerEntry.first;
        cout << setw(CELL_WIDTH) << i;

        for (const auto& innerEntry : parseInfoTable) {
            int j = innerEntry.first;
            if (parseInfoTable.at(i).find(j) != parseInfoTable.at(i).end()) {
                const set<string>& rules = parseInfoTable.at(i).at(j);
                string cellContent;

                for (const string& rule : rules) {
                    cellContent += rule + ", ";
                }
                if (!cellContent.empty()) {
                    cellContent.pop_back();
                    cellContent.pop_back();
                }

                cout << setw(CELL_WIDTH) << cellContent;
            }
            else {
                cout << setw(CELL_WIDTH) << "-";
            }
        }
        cout << endl;
    }
}

void saveParseInfoTableToFile(const map<int, map<int, set<string>>>& parseInfoTable, const string& filename) {
    constexpr int CELL_WIDTH = 128;

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    file << left << setw(CELL_WIDTH) << "[i\\j]";

    for (const auto& outerEntry : parseInfoTable) {
        file << setw(CELL_WIDTH) << outerEntry.first;
    }
    file << endl;

    for (const auto& outerEntry : parseInfoTable) {
        int i = outerEntry.first;
        file << setw(CELL_WIDTH) << i;

        for (const auto& innerEntry : parseInfoTable) {
            int j = innerEntry.first;
            if (parseInfoTable.at(i).find(j) != parseInfoTable.at(i).end()) {
                const set<string>& rules = parseInfoTable.at(i).at(j);
                string cellContent;

                for (const string& rule : rules) {
                    cellContent += rule + ", ";
                }
                if (!cellContent.empty()) {
                    cellContent.pop_back();
                    cellContent.pop_back();
                }

                file << setw(CELL_WIDTH) << cellContent;
            }
            else {
                file << setw(CELL_WIDTH) << "-";
            }
        }
        file << endl;
    }

    file.close();
}

bool cykAlgorithmImplementation(struct LexemInfo* lexemInfoTable, Grammar* grammar, char * astFileName, bool viewAST) {
    if (lexemInfoTable == NULL || grammar == NULL) {
        return false;
    }

#if defined(_DEBUG)
    printf("ATTENTION: for better performance, use Release mode!\r\n");
#endif

#ifndef DEBUG_STATES
    cout << "cykParse in progress.....[please wait]";
#else
    cout << "cykParse in progress.....[please wait]: ";
#endif

//    ParseInfoTable parseInfoTable = { {{0}} }; // Initialize with empty SymbolSets

    int lexemIndex = 0;
    for (--lexemIndex; lexemInfoTable[++lexemIndex].lexemStr[0];) {
#ifdef DEBUG_STATES
        printf("\rcykParse in progress.....[please wait]: %02d %16s", lexemIndex, lexemInfoTable[lexemIndex].lexemStr);
#endif

        // Iterate over the rules
        for (int xIndex = 0; xIndex < grammar->rule_count; ++xIndex) {
            Rule& rule = grammar->rules[xIndex];
            // If a terminal is found
            if (rule.rhs_count == 1 && (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                )) {
                insertIntoSymbolSet(&parseInfoTable[lexemIndex][lexemIndex], rule.lhs);
            }
        }
        for (int iIndex = lexemIndex; iIndex >= 0; --iIndex) {
            for (int kIndex = iIndex; kIndex <= lexemIndex; ++kIndex) {
                for (int xIndex = 0; xIndex < grammar->rule_count; ++xIndex) {
                    Rule& rule = grammar->rules[xIndex];
                    if (rule.rhs_count == 2
                        && containsSymbolSet(&parseInfoTable[iIndex][kIndex], rule.rhs[0])
                        && containsSymbolSet(&parseInfoTable[kIndex + 1][lexemIndex], rule.rhs[1])
                        ) {
                        insertIntoSymbolSet(&parseInfoTable[iIndex][lexemIndex], rule.lhs);
                    }
                }
            }
        }
    }

    cout << "\r" << "cykParse complete........[     ok    ]\n";

    if (!containsSymbolSet(&parseInfoTable[0][lexemIndex - 1], grammar->start_symbol)) {
        return false;
    }

    ASTNode* astRoot = buildAST(parseInfoTable, grammar, 0, lexemIndex - 1, grammar->start_symbol);
    if (astRoot) {
        if (viewAST) {
            std::cout << "Abstract Syntax Tree:\n";
            printAST(lexemInfoTable, astRoot);
        }
        if (astFileName && astFileName[0] != '\0') {
            std::ofstream astOFStream(astFileName, std::ofstream::out);
            printASTToFile(lexemInfoTable, astRoot, astOFStream);
            astOFStream.close();
            printf("File \"%s\" saved.\n", astFileName);
        }
        delete astRoot;
    }
    else {
        std::cout << "Failed to build AST.\n";
    }

    return true;
}

#define MAX_STACK_DEPTH 256
#define MAX_AST_NODE_COUNT 65536

bool recursiveDescentParserRuleWithDebug(const char* ruleName, int& lexemIndex, LexemInfo* lexemInfoTable, Grammar* grammar, int depth, const struct LexemInfo** unexpectedLexemfailedTerminal) {
    if (depth > MAX_STACK_DEPTH) {
        printf("Error: Maximum recursion depth reached.\n");
        return false;
    }
    char isError = false;
    for (int i = 0; i < grammar->rule_count; ++i) {
        Rule& rule = grammar->rules[i];
        if (strcmp(rule.lhs, ruleName) != 0) continue;

        int savedIndex = lexemIndex;
        if (rule.rhs_count == 1) {
            if (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                ) {
                ++lexemIndex;
                return true;
            }
            else {
                //if (unexpectedLexemfailedTerminal && *unexpectedLexemfailedTerminal && depth > (*unexpectedLexemfailedTerminal)->ifvalue) {
                //    *unexpectedLexemfailedTerminal = lexemInfoTable + lexemIndex;
                //    (*(LexemInfo**)unexpectedLexemfailedTerminal)->ifvalue = depth;
                //}
                *unexpectedLexemfailedTerminal = lexemInfoTable + lexemIndex;
                if (0)printf("<< \"%s\" >>\n", rule.rhs[0]);
            }
        }
        else if (rule.rhs_count == 2) {
            if (recursiveDescentParserRuleWithDebug(rule.rhs[0], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal) &&
                recursiveDescentParserRuleWithDebug(rule.rhs[1], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal)) {
                return true;
            }
        }
        lexemIndex = savedIndex;
    }

    return false;
}

const LexemInfo* recursiveDescentParserWithDebug_(const char* ruleName, int& lexemIndex, LexemInfo* lexemInfoTable, Grammar* grammar, int depth, const struct LexemInfo* unexpectedUnknownLexemfailedTerminal) {
    if (depth > MAX_STACK_DEPTH) {
        printf("Error: Maximum recursion depth reached.\n");
        return unexpectedUnknownLexemfailedTerminal;
    }
    char isError = false;
    const LexemInfo* currUnexpectedLexemfailedTerminalPtr = nullptr, * returnUnexpectedLexemfailedTerminalPtr = nullptr;
    for (int i = 0; i < grammar->rule_count; ++i) {
        Rule& rule = grammar->rules[i];
        if (strcmp(rule.lhs, ruleName) != 0) continue;

        int savedIndex = lexemIndex;
        if (rule.rhs_count == 1) {
            if (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                ) {
                ++lexemIndex;
                return nullptr;
            }
            else {              
                currUnexpectedLexemfailedTerminalPtr = lexemInfoTable + lexemIndex;
            }
        }
        else if (rule.rhs_count == 2) {
            if (nullptr == (returnUnexpectedLexemfailedTerminalPtr = recursiveDescentParserWithDebug_(rule.rhs[0], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedUnknownLexemfailedTerminal))  
                && nullptr == (returnUnexpectedLexemfailedTerminalPtr = recursiveDescentParserWithDebug_(rule.rhs[1], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedUnknownLexemfailedTerminal))) {
                return nullptr;
            }
        }
        lexemIndex = savedIndex;
    }

    if (returnUnexpectedLexemfailedTerminalPtr != nullptr && returnUnexpectedLexemfailedTerminalPtr != unexpectedUnknownLexemfailedTerminal
        &&( returnUnexpectedLexemfailedTerminalPtr->tokenType == IDENTIFIER_LEXEME_TYPE
            || returnUnexpectedLexemfailedTerminalPtr->tokenType == VALUE_LEXEME_TYPE
            || returnUnexpectedLexemfailedTerminalPtr->tokenType == KEYWORD_LEXEME_TYPE
        )) {
        return returnUnexpectedLexemfailedTerminalPtr;
    }

    if (currUnexpectedLexemfailedTerminalPtr != nullptr) {
        return currUnexpectedLexemfailedTerminalPtr;
    }

    if(returnUnexpectedLexemfailedTerminalPtr != nullptr){
        return returnUnexpectedLexemfailedTerminalPtr;
    }

    return unexpectedUnknownLexemfailedTerminal;
}

//

bool recursiveDescentParserRuleWithDebugWithBuildAST(const char* ruleName, int& lexemIndex, LexemInfo* lexemInfoTable, Grammar* grammar, int depth, const struct LexemInfo** unexpectedLexemfailedTerminal) {
    if (depth > MAX_STACK_DEPTH) {
        printf("Error: Maximum recursion depth reached.\n");
        return false;
    }
    char isError = false;
    for (int i = 0; i < grammar->rule_count; ++i) {
        Rule& rule = grammar->rules[i];
        if (strcmp(rule.lhs, ruleName) != 0) continue;

        int savedIndex = lexemIndex;
        if (rule.rhs_count == 1) {
            if (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                ) {
                ++lexemIndex;
                return true;
            }
            else {
                //if (unexpectedLexemfailedTerminal && *unexpectedLexemfailedTerminal && depth > (*unexpectedLexemfailedTerminal)->ifvalue) {
                //    *unexpectedLexemfailedTerminal = lexemInfoTable + lexemIndex;
                //    (*(LexemInfo**)unexpectedLexemfailedTerminal)->ifvalue = depth;
                //}
                *unexpectedLexemfailedTerminal = lexemInfoTable + lexemIndex;
                if (0)printf("<< \"%s\" >>\n", rule.rhs[0]);
            }
        }
        else if (rule.rhs_count == 2) {
            if (recursiveDescentParserRuleWithDebug(rule.rhs[0], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal) &&
                recursiveDescentParserRuleWithDebug(rule.rhs[1], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal)) {
                return true;
            }
        }
        lexemIndex = savedIndex;
    }

    return false;
}

ASTNode* recursiveDescentParserRuleWithDebugWithBuildErrorAST(const char* ruleName, int& lexemIndex, LexemInfo* lexemInfoTable, Grammar* grammar, int depth, const struct LexemInfo** unexpectedLexemfailedTerminal) {
    static int astNodeCount = 0;
    
    if (depth > MAX_STACK_DEPTH) {
        //printf("Error: Maximum recursion depth reached.\n");
        return nullptr;
    }

    ASTNode* node = new ASTNode(ruleName, false);
    node->isPartialSuccess = 0;
    for (int i = 0; i < grammar->rule_count; ++i) {
        Rule& rule = grammar->rules[i];

        if (strcmp(rule.lhs, ruleName) != 0) { 
            continue; 
        }

        int savedIndex = lexemIndex;

        if (rule.rhs_count == 1) {
            if (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                ) {
                node->children.push_back(new ASTNode(lexemInfoTable[lexemIndex].lexemStr, true));
                ++lexemIndex;
                if (++astNodeCount >= MAX_AST_NODE_COUNT) {
                    delete node;
                    return nullptr;
                }
                node->isPartialSuccess = 0;
                return node;
            }
            else {
                *unexpectedLexemfailedTerminal = lexemInfoTable + lexemIndex;
            }
        }
        else if (rule.rhs_count == 2) {
            ASTNode* leftChild = recursiveDescentParserRuleWithDebugWithBuildErrorAST(rule.rhs[0], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal);
            ASTNode* rightChild = nullptr;
            if (leftChild) {
                rightChild = recursiveDescentParserRuleWithDebugWithBuildErrorAST(rule.rhs[1], lexemIndex, lexemInfoTable, grammar, depth + 1, unexpectedLexemfailedTerminal);
            }
            if (node->isPartialSuccess == 0 && leftChild != nullptr && rightChild != nullptr) {
                node->children.push_back(leftChild);
                node->children.push_back(rightChild);
                node->isPartialSuccess = leftChild->isPartialSuccess + rightChild->isPartialSuccess;
                if (!node->isPartialSuccess) {
                    if (++astNodeCount >= MAX_AST_NODE_COUNT) {
                        delete node;
                        return nullptr;
                    }
                    return node;
                }
                node->isPartialSuccess = 2;
            }
            else if (node->isPartialSuccess == 1 && leftChild != nullptr && rightChild != nullptr) {
                node->children.pop_back();
                node->children.push_back(leftChild);
                node->children.push_back(rightChild);
                node->isPartialSuccess = leftChild->isPartialSuccess + rightChild->isPartialSuccess;
                if (!node->isPartialSuccess) {
                    if (++astNodeCount >= MAX_AST_NODE_COUNT) {
                        delete node;
                        return nullptr;
                    }
                    return node;
                }
                node->isPartialSuccess = 2;
            }
            else if (node->isPartialSuccess >= 2 && leftChild != nullptr && rightChild != nullptr) {
                if (node->isPartialSuccess - 2 > leftChild->isPartialSuccess + rightChild->isPartialSuccess) {
                    node->children.pop_back();
                    node->children.pop_back();
                    node->children.push_back(leftChild);
                    node->children.push_back(rightChild);
                    node->isPartialSuccess = leftChild->isPartialSuccess + rightChild->isPartialSuccess;
                    if (!node->isPartialSuccess) {
                        if (++astNodeCount >= MAX_AST_NODE_COUNT) {
                            delete node;
                            return nullptr;
                        }
                        return node;
                    }
                    node->isPartialSuccess = 2;
                }
            }
            else if (node->isPartialSuccess == 0 && leftChild != nullptr) {
                node->children.push_back(leftChild);
                node->isPartialSuccess = 1;
            }
            else if (node->isPartialSuccess == 1 && leftChild != nullptr) { // NOP
                //node->children.pop_back();
                //node->children.push_back(leftChild);
                //node->isPartialSuccess = 1;
            }
            else if (node->isPartialSuccess >= 2 && leftChild != nullptr) { } // NOP

        }
        lexemIndex = savedIndex;
    }
    if (node->isPartialSuccess) {
        if (++astNodeCount >= MAX_AST_NODE_COUNT) {
            delete node;
            return nullptr;
        }
        return node;
    }
    delete node;
    return nullptr;
}

//

int syntaxAnalyze(LexemInfo* lexemInfoTable, Grammar* grammar, char syntaxlAnalyzeMode, char* astFileName, char* errorMessagesPtrToLastBytePtr, bool viewAST) {
    bool cykAlgorithmImplementationReturnValue = false;
    if (syntaxlAnalyzeMode == SYNTAX_ANALYZE_BY_CYK_ALGORITHM) {
        cykAlgorithmImplementationReturnValue = cykAlgorithmImplementation(lexemesInfoTable, grammar, astFileName, viewAST);
        //printf("cykAlgorithmImplementation return \"%s\".\r\n", cykAlgorithmImplementationReturnValue ? "true" : "false");  
        if (cykAlgorithmImplementationReturnValue) {
            return SUCCESS_STATE;
        }
        else {
            writeBytesToFile(astFileName, (unsigned char*)"Error of AST build", strlen("Error of AST build"));
        }
    }
    else if (astFileName && astFileName[0] != '\0') {
        writeBytesToFile(astFileName, (unsigned char*)"AST build no support.", strlen("AST build no support."));
    }

    if (cykAlgorithmImplementationReturnValue == false || syntaxlAnalyzeMode == SYNTAX_ANALYZE_BY_RECURSIVE_DESCENT) {
        int lexemIndex = 0;
        const struct LexemInfo* unexpectedLexemfailedTerminal = nullptr;

        if (recursiveDescentParserRuleWithDebug(grammar->start_symbol, lexemIndex, lexemInfoTable, grammar, 0, &unexpectedLexemfailedTerminal)) {           
            if (lexemInfoTable[lexemIndex].lexemStr[0] == '\0') {
                printf("Parse successful.\n");
                printf("%d.\n", lexemIndex);
                exit(0);
                return SUCCESS_STATE;
            }
            else {
                printf("Parse failed: Extra tokens remain.\r\n");
                errorMessagesPtrToLastBytePtr += sprintf(errorMessagesPtrToLastBytePtr, "Parse failed: Extra tokens remain.\r\n");
                exit(0);
                return ~SUCCESS_STATE;
            }
        }
        else {
            ASTNode* ast = recursiveDescentParserRuleWithDebugWithBuildErrorAST(grammar->start_symbol, lexemIndex, lexemInfoTable, grammar, 0, &unexpectedLexemfailedTerminal);
            char errorMark = 0;
            int lexemInfoTableIndexForPrintAST = getSyntaxError(lexemInfoTable, ast, errorMark);
            delete ast;
            /*const struct LexemInfo* */unexpectedLexemfailedTerminal = lexemInfoTable + lexemInfoTableIndexForPrintAST;
            if (unexpectedLexemfailedTerminal->lexemStr[0] != '\0') {
                printf("Parse failed.\r\n");
                printf("    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"%s\" on line %lld at position %lld.)\r\n", unexpectedLexemfailedTerminal->lexemStr, unexpectedLexemfailedTerminal->row, unexpectedLexemfailedTerminal->col);
                errorMessagesPtrToLastBytePtr += sprintf(errorMessagesPtrToLastBytePtr, "Parse failed.\r\n");
                errorMessagesPtrToLastBytePtr += snprintf(errorMessagesPtrToLastBytePtr, MAX_LEXEM_SIZE + 128 + strlen("    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"#\" on line # at position #.)\r\n"), "    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"%s\" on line %lld at position %lld.)\r\n", unexpectedLexemfailedTerminal->lexemStr, unexpectedLexemfailedTerminal->row, unexpectedLexemfailedTerminal->col);
            }
            else {
                printf("Parse failed: unexpected terminal.\r\n");
                errorMessagesPtrToLastBytePtr += sprintf(errorMessagesPtrToLastBytePtr, "Parse failed: unexpected terminal.\r\n");
            }
            exit(0);
            return ~SUCCESS_STATE;
        }
    }
    exit(0);
    return ~SUCCESS_STATE;
}

bool syntaxlAnalyze_(LexemInfo* lexemInfoTable, Grammar* grammar, char syntaxlAnalyzeMode, char* astFileName, char* errorMessagesPtrToLastBytePtr, bool viewAST) {
    bool cykAlgorithmImplementationReturnValue = false;
    if (syntaxlAnalyzeMode == SYNTAX_ANALYZE_BY_CYK_ALGORITHM) {
        bool cykAlgorithmImplementationReturnValue = cykAlgorithmImplementation(lexemesInfoTable, grammar, astFileName, viewAST);

        printf("cykAlgorithmImplementation return \"%s\".\r\n", cykAlgorithmImplementationReturnValue ? "true" : "false");
    if(!cykAlgorithmImplementationReturnValue) {
        writeBytesToFile(astFileName, (unsigned char*)"Error of AST build", strlen("Error of AST build"));
    }
    }
    else if(astFileName && astFileName[0] != '\0') {
        writeBytesToFile(astFileName, (unsigned char*)"AST build no support.", strlen("AST build no support."));
    }

    if (cykAlgorithmImplementationReturnValue && syntaxlAnalyzeMode == SYNTAX_ANALYZE_BY_RECURSIVE_DESCENT) {
        int lexemIndex = 0;
        const struct LexemInfo unexpectedUnknownLexemfailedTerminal("unknown", 0, 0, 0, ~0, ~0); //
        const struct LexemInfo* returnUnexpectedLexemfailedTerminal = nullptr;

        if (nullptr == (returnUnexpectedLexemfailedTerminal = recursiveDescentParserWithDebug_(grammar->start_symbol, lexemIndex, lexemInfoTable, grammar, 0, &unexpectedUnknownLexemfailedTerminal))) {
            if (lexemInfoTable[lexemIndex].lexemStr[0] == '\0') {
                printf("Parse successful.\n");
                printf("%d.\n", lexemIndex);
                return true;
            }
            else {
                printf("Parse failed: Extra tokens remain.\n");
                return false;
            }
        }
        else {
            if (returnUnexpectedLexemfailedTerminal->lexemStr[1]) {
                printf("Parse failed.\r\n");
                printf("    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"%s\" on line %lld at position %lld\r\n    ..., but this is not certain.)\r\n", returnUnexpectedLexemfailedTerminal->lexemStr, returnUnexpectedLexemfailedTerminal->row, returnUnexpectedLexemfailedTerminal->col);
            }
            else {
                printf("Parse failed: unexpected terminal.\r\n");
            }
            return false;
        }
        return false;
    }

    return false;
}


// OLD //
bool cykAlgorithmImplementationByCPPMap(struct LexemInfo* lexemInfoTable, Grammar* grammar) {
    if (lexemInfoTable == NULL || grammar == NULL) {
        return false;
    }

#if defined(_DEBUG)
    printf("ATTENTION: for better performance, use Release mode!\r\n");
#endif

#ifndef DEBUG_STATES
    cout << "cykParse in progress.....[please wait]";
#else
    cout << "cykParse in progress.....[please wait]: ";
#endif

    map<int, map<int, set<string>>> parseInfoTable;

    int lexemIndex = 0;
    for (--lexemIndex; lexemInfoTable[++lexemIndex].lexemStr[0];) {
#ifdef DEBUG_STATES
        printf("\rcykParse in progress.....[please wait]: %02d %16s", lexemIndex, lexemInfoTable[lexemIndex].lexemStr);
#endif

        // Iterate over the rules
        for (int xIndex = 0; xIndex < grammar->rule_count; ++xIndex) {
            string&& lhs = grammar->rules[xIndex].lhs;
            Rule& rule = grammar->rules[xIndex];
            // If a terminal is found
            if (rule.rhs_count == 1 && (
                lexemInfoTable[lexemIndex].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_terminal")
                || lexemInfoTable[lexemIndex].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_terminal")
                || !strncmp(rule.rhs[0], lexemInfoTable[lexemIndex].lexemStr, MAX_LEXEM_SIZE)
                )) {
                parseInfoTable[lexemIndex][lexemIndex].insert(lhs);
            }
        }
        for (int iIndex = lexemIndex; iIndex >= 0; --iIndex) {
            for (int kIndex = iIndex; kIndex <= lexemIndex; ++kIndex) {
                for (int xIndex = 0; xIndex < grammar->rule_count; ++xIndex) {
                    string&& lhs = grammar->rules[xIndex].lhs;
                    Rule& rule = grammar->rules[xIndex];
                    if (rule.rhs_count == 2
                        && parseInfoTable[iIndex][kIndex].find(rule.rhs[0]) != parseInfoTable[iIndex][kIndex].end()
                        && parseInfoTable[kIndex + 1][lexemIndex].find(rule.rhs[1]) != parseInfoTable[kIndex + 1][lexemIndex].end()
                        ) {
                        parseInfoTable[iIndex][lexemIndex].insert(lhs);
                    }
                }
            }
        }
    }

    cout << "\r" << "cykParse complete........[     ok    ]\n";

    if (parseInfoTable[0][lexemIndex - 1].find(grammar->start_symbol) == parseInfoTable[0][lexemIndex - 1].end()) {
        return false;
    }

//    parseByRecursiveDescent_(lexemInfoTable, grammar);
    //    displayParseInfoTable(parseInfoTable);
    //    saveParseInfoTableToFile(parseInfoTable, "parseInfoTable.txt");

    ASTNode* astRoot = buildASTByCPPMap(parseInfoTable, grammar, 0, lexemIndex - 1, grammar->start_symbol);
    if (astRoot) {
        std::cout << "Abstract Syntax Tree:\n";
        printAST(lexemInfoTable, astRoot);
        delete astRoot; // Не забуваємо звільняти пам'ять
    }
    else {
        std::cout << "Failed to build AST.\n";
    }

    //return parseInfoTable[0][lexemIndex - 1].find(grammar->start_symbol) != parseInfoTable[0][lexemIndex - 1].end(); // return !!parseInfoTable[0][lexemIndex - 1].size();
    return true;
}

#if 0
bool parseByRecursiveDescent(LexemInfo* lexemInfoTable, Grammar* grammar) {
    int lexemIndex = 0;
    const struct LexemInfo* unexpectedLexemfailedTerminal = nullptr;

    if (recursiveDescentParserRuleWithDebug(grammar->start_symbol, lexemIndex, lexemInfoTable, grammar, 0, &unexpectedLexemfailedTerminal)) {
        if (lexemInfoTable[lexemIndex].lexemStr[0] == '\0') {
            printf("Parse successful.\n");
            printf("%d.\n", lexemIndex);
            return true;
        }
        else {
            printf("Parse failed: Extra tokens remain.\n");
            exit(0);
        }
    }
    else {
        if (unexpectedLexemfailedTerminal) {
            printf("Parse failed in line.\r\n");
            printf("    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"%s\" on line %lld at position %lld\r\n    ..., but this is not certain.)\r\n", unexpectedLexemfailedTerminal->lexemStr, unexpectedLexemfailedTerminal->row, unexpectedLexemfailedTerminal->col);
        }
        else {
            printf("Parse failed: unexpected terminal.\r\n");
        }
        exit(0);
    }
    return false;
}
bool parseByRecursiveDescent_(LexemInfo* lexemInfoTable, Grammar* grammar) {
    int lexemIndex = 0;
    const struct LexemInfo unexpectedUnknownLexemfailedTerminal("unknown", 0, 0, 0, ~0, ~0); //
    const struct LexemInfo* returnUnexpectedLexemfailedTerminal = nullptr;

    if (nullptr == (returnUnexpectedLexemfailedTerminal = recursiveDescentParserWithDebug_(grammar->start_symbol, lexemIndex, lexemInfoTable, grammar, 0, &unexpectedUnknownLexemfailedTerminal))) {
        if (lexemInfoTable[lexemIndex].lexemStr[0] == '\0') {
            printf("Parse successful.\n");
            printf("%d.\n", lexemIndex);
            return true;
        }
        else {
            printf("Parse failed: Extra tokens remain.\n");
            exit(0);
        }
    }
    else {
        if (returnUnexpectedLexemfailedTerminal->lexemStr[1]) {
            printf("Parse failed.\r\n");
            printf("    (The predicted terminal does not match the expected one.\r\n    Possible unexpected terminal \"%s\" on line %lld at position %lld\r\n    ..., but this is not certain.)\r\n", returnUnexpectedLexemfailedTerminal->lexemStr, returnUnexpectedLexemfailedTerminal->row, returnUnexpectedLexemfailedTerminal->col);
        }
        else {
            printf("Parse failed: unexpected terminal.\r\n");
        }
        exit(0);
    }
    return false;
}
#endif
