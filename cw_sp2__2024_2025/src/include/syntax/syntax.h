#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024 // lex  +  rpn  +  MACHINECODEGEN!   *
*                         file: syntax.h                    *
*                                                  (draft!) *
*************************************************************/

#include "../../include/def.h"
#include "../../include/generator/generator.h"
#include "../../include/lexica/lexica.h"

#define SYNTAX_ANALYZE_BY_CYK_ALGORITHM 0
#define SYNTAX_ANALYZE_BY_RECURSIVE_DESCENT 1

#define DEFAULT_SYNTAX_ANAlYZE_MODE SYNTAX_ANALYZE_BY_CYK_ALGORITHM

using namespace std;

#define MAX_RULES 356

#define MAX_TOKEN_SIZE 128
#define MAX_RTOKEN_COUNT 2 // 3

typedef struct {
	char lhs[MAX_TOKEN_SIZE];
	int rhs_count;
	char rhs[MAX_RTOKEN_COUNT][MAX_TOKEN_SIZE];
} Rule;

typedef struct {
	Rule rules[MAX_RULES];
	int rule_count;
	char start_symbol[MAX_TOKEN_SIZE] ;
} Grammar;

extern Grammar grammar;

#define DEBUG_STATES

//bool cykAlgorithmImplementation(struct LexemInfo* lexemInfoTable, Grammar* grammar);
bool syntaxAnalyze(LexemInfo* lexemInfoTable, Grammar* grammar, char syntaxlAnalyzeMode);