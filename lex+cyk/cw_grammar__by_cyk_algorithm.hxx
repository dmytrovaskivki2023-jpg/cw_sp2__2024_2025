#define MAX_RULES 100

#define MAX_TOKEN_SIZE 64
#define MAX_RTOKEN_COUNT 24

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

// (1)
//program = 
// tokenNAME 
// ///////// >> program_name 
//>> tokenSEMICOLON 
//>> tokenBODY 
//>> tokenDATA 
//>> tokenSEMICOLON 
//>> tokenEND;

// (2)
//program = 
// tokenNAME  
// ident  
//>> tokenSEMICOLON 
//>> tokenBODY 
//>> tokenDATA 
//>> tokenSEMICOLON 
//>> tokenEND;

// (3)
//program = 
// tokenNAME__ident
// >> tokenSEMICOLON 
//>> tokenBODY__tokenDATA 
//>> tokenSEMICOLON__tokenEND;

// (4)
//program = 
// tokenNAME__ident____tokenSEMICOLON 
//>> tokenBODY__tokenDATA____tokenSEMICOLON__tokenEND;

Grammar grammar = { 
	{
		{"program", 2, {"tokenNAME__ident____tokenSEMICOLON", "tokenBODY__tokenDATA____tokenSEMICOLON__tokenEND"}},
        {"tokenNAME__ident____tokenSEMICOLON", 2, {"tokenNAME__ident","tokenSEMICOLON"}},
        {"tokenNAME__ident", 2, {"tokenNAME","ident"}},
        {"tokenBODY__tokenDATA____tokenSEMICOLON__tokenEND", 2, {"tokenBODY__tokenDATA","tokenSEMICOLON__tokenEND"}},
        {"tokenBODY__tokenDATA", 2, {"tokenBODY", "tokenDATA"}},
        {"tokenSEMICOLON__tokenEND", 2, {"tokenSEMICOLON","tokenEND"}},
		{"tokenNAME", 1, {"name"}},
        {"tokenSEMICOLON", 1, {";"}},
        {"tokenBODY", 1, {"body"}},
        {"tokenDATA", 1, {"data"}},
		{"tokenEND", 1, {"end"}},
        //
        {"ident", 1, {"ident_token"}},
	},
	12,
	"program" 
};

bool cykAlgorithmImplementation(struct LexemInfo* lexemInfoTable, Grammar* grammar, int maxErrorToViewCount = 32) {
    int n = 0;
    while (strlen(lexemInfoTable[n].lexemStr) > 0) {
        n++;
    }

    if (n == 0) return false;

    size_t dp_size = MAX_WORD_COUNT * MAX_WORD_COUNT * MAX_RULES * sizeof(bool);
    size_t errors_size = MAX_WORD_COUNT * MAX_WORD_COUNT * sizeof(bool);

    void* memory_block = calloc(1, dp_size + errors_size);
    if (!memory_block) {
        perror("Memory allocation failed");
        return false;
    }

    bool(*dp)[MAX_WORD_COUNT][MAX_RULES] = (bool(*)[MAX_WORD_COUNT][MAX_RULES])memory_block;
    bool(*errors)[MAX_WORD_COUNT] = (bool(*)[MAX_WORD_COUNT])((char*)memory_block + dp_size);

    for (int i = 0; i < n; i++) {
        for (int r = 0; r < grammar->rule_count; r++) {
            Rule rule = grammar->rules[r];
            if (rule.rhs_count == 1 &&(
                    lexemInfoTable[i].tokenType == IDENTIFIER_LEXEME_TYPE && !strcmp(rule.rhs[0], "ident_token")
                    ||
                    lexemInfoTable[i].tokenType == VALUE_LEXEME_TYPE && !strcmp(rule.rhs[0], "value_token")
                    ||
                    !strcmp(rule.rhs[0], lexemInfoTable[i].lexemStr)
                    )   
                ) {
                dp[i][i][r] = true;
            }
        }
        if (!dp[i][i][0]) {
            errors[i][i] = true;
        }
    }

    for (int span = 2; span <= n; span++) {
        for (int start = 0; start <= n - span; start++) {
            int end = start + span - 1;

            for (int split = start; split < end; split++) {
                for (int r = 0; r < grammar->rule_count; r++) {
                    Rule rule = grammar->rules[r];
                    if (rule.rhs_count == 2) {
                        int leftIndex = -1, rightIndex = -1;
                        for (int l = 0; l < grammar->rule_count; l++) {
                            if (strcmp(grammar->rules[l].lhs, rule.rhs[0]) == 0) {
                                leftIndex = l;
                            }
                            if (strcmp(grammar->rules[l].lhs, rule.rhs[1]) == 0) {
                                rightIndex = l;
                            }
                        }

                        if (leftIndex != -1 && rightIndex != -1 && dp[start][split][leftIndex] && dp[split + 1][end][rightIndex]) {
                            dp[start][end][r] = true;
                        }
                    }
                }
            }
            bool matched = false;
            for (int r = 0; r < grammar->rule_count; r++) {
                if (dp[start][end][r]) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                errors[start][end] = true;
            }
        }
    }

    for (int r = 0; r < grammar->rule_count; r++) {
        if (strcmp(grammar->rules[r].lhs, grammar->start_symbol) == 0 && dp[0][n - 1][r]) {
            free(memory_block);
            return true;
        }
    }

    printf("Syntax error detected:\n");
    for (int start = 0; start < n; start++) {
        for (int end = start; end < n; end++) {
            if (errors[start][end]) {
                if (maxErrorToViewCount-- <= 0) {
                    free(memory_block);
                    return false;
                }
                printf("Error in span [%d, %d]:\n", start, end);
                for (int i = start; i <= end; i++) {
                    printf("  %s\n", lexemInfoTable[i].lexemStr);
                }
            }
        }
    }

    free(memory_block);
    return false;
}