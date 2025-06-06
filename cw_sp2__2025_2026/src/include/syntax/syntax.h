#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: syntax.h (2.0)              *
*                                                  (draft!) *
*************************************************************/

#include "../../include/def.h"
#include "../../include/generator/generator.h"
#include "../../include/lexica/lexica.h"

//#define SYNTAX_ANALYZE_BY_CYK_ALGORITHM 0
//#define SYNTAX_ANALYZE_BY_RECURSIVE_DESCENT 1
//
//#define DEFAULT_SYNTAX_ANAlYZE_MODE SYNTAX_ANALYZE_BY_CYK_ALGORITHM

//using namespace std;

#define MAX_RULES 356

#define MAX_TOKEN_SIZE 128
#define MAX_RTOKEN_COUNT 16

typedef struct {
	char invertedSecondMarks;
	char secondMarks[16][MAX_TOKEN_SIZE];
	int rhs_count;
	char rhs[MAX_RTOKEN_COUNT][MAX_TOKEN_SIZE];
} RHSConteiner;

typedef struct {
	char lhs[MAX_TOKEN_SIZE];
	RHSConteiner rhss[3];
} Rule;

typedef struct {
	char invertedFirstMarks;
	char firstMarks[16][MAX_TOKEN_SIZE];
	Rule rule;
} MarkedRule;

typedef struct {
	MarkedRule multiRules[MAX_RULES];
	int rule_count;
	char start_symbol[MAX_TOKEN_SIZE];
} Grammar;

extern Grammar grammar;

#define DEBUG_STATES

//bool recursiveDescentParserRuleWithDebug(const char* ruleName, int& lexemIndex, struct LexemInfo* lexemInfoTable, Grammar* grammar, int depth, const struct LexemInfo** unexpectedLexemfailedTerminal);
bool getIndexAfterFragmentSyntax(char* ruleName, int& lexemIndex, struct LexemInfo* lexemInfoTable, Grammar* grammar, int depth/* not used */);

//bool cykAlgorithmImplementation(struct LexemInfo* lexemInfoTable, Grammar* grammar);
//int syntaxAnalyze(       LexemInfo* lexemInfoTable, Grammar* grammar, char syntaxlAnalyzeMode, char* astFileName, char* errorMessagesPtrToLastBytePtr, bool viewAST);
int syntaxAnalyze(struct LexemInfo* lexemInfoTable, Grammar* grammar, char syntaxlAnalyzeMode/* not used */, char* astFileName, char* errorMessagesPtrToLastBytePtr, bool viewAST);