/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: def.h                       *
*                                                  (draft!) *
*************************************************************/

#define SUCCESS_STATE 0

#define LEXICAL_ANALYZE_MODE 1 // lexicalAnalyze
#define MAKE_LEXEMES_SEQUENSE 2 // ADD MODE
#define SYNTAX_ANALYZE_MODE 4
#define MAKE_AST 8 // ADD MODE
#define SEMANTIX_ANALYZE_MODE 16 // ADD MODE
#define MAKE_PREPARE 32 // ADD MODE
#define MAKE_C 64 // ADD MODE
#define MAKE_ASSEMBLY 128 // ADD MODE
#define MAKE_OBJECT 256 // ADD MODE
#define MAKE_BINARY 512 // ADD MODE
#define RUN_BINARY 1024 // ADD MODE

#define UNDEFINED_MODE 16384

#define INTERACTIVE_MODE 32768

#define FULL_COMPILER_MODE 2048 // ?

#define DEBUG_MODE 4096

//#define DECLENUM(NAME, ...) typedef enum {__VA_ARGS__, size##NAME} NAME;
#define DECLENUM(NAME, ...) enum NAME {__VA_ARGS__, size##NAME};
#define GET_ENUM_SIZE(NAME) size##NAME
#define SET_QUADRUPLE_STR_MACRO_IN_ARRAY(ARRAY, NAME)\
ARRAY[MULTI_TOKEN_##NAME][0] = (char*)T_##NAME##_0;\
ARRAY[MULTI_TOKEN_##NAME][1] = (char*)T_##NAME##_1;\
ARRAY[MULTI_TOKEN_##NAME][2] = (char*)T_##NAME##_2;\
ARRAY[MULTI_TOKEN_##NAME][3] = (char*)T_##NAME##_3;


//#define EXPAND_MACRO(...) __VA_ARGS__ // Проміжний макрос для розгортання
//
//#define SET_QUADRUPLE_STR_MACRO_IN_ARRAY_(ARRAY, QUADRUPLE_ELEMENT_INDEX, ...)\
//SET_QUADRUPLE_STR_MACRO_IN_ARRAY(ARRAY, QUADRUPLE_ELEMENT_INDEX, EXPAND_MACRO(__VA_ARGS__) )
//
//#define TOKENS_FOR_MULTI_TOKEN_BITWISE_NOT "~", "", "", ""
//
//SET_QUADRUPLE_STR_MACRO_IN_ARRAY_(tokenStruct, MULTI_TOKEN_BITWISE_NOT, TOKENS_FOR_MULTI_TOKEN_BITWISE_NOT)

//#define MAX_TEXT_SIZE 8192
//#define MAX_WORD_COUNT (MAX_TEXT_SIZE / 5)
//#define MAX_LEXEM_SIZE 1024
//#define MAX_VARIABLES_COUNT 256
//#define MAX_KEYWORD_COUNT 64
//
//#define KEYWORD_LEXEME_TYPE 1
//#define IDENTIFIER_LEXEME_TYPE 2 // #define LABEL_LEXEME_TYPE 8
//#define VALUE_LEXEME_TYPE 4
//#define UNEXPEXTED_LEXEME_TYPE 127
//
//#define LEXICAL_ANALISIS_MODE 1
//#define SEMANTIC_ANALISIS_MODE 2
//#define FULL_COMPILER_MODE 4
//
//#define DEBUG_MODE 512
//
//+!///#define MAX_PARAMETERS_SIZE 4096
//+!///#define PARAMETERS_COUNT 4
//+!///#define INPUT_FILENAME_PARAMETER 0
//
//#define DEFAULT_MODE (LEXICAL_ANALISIS_MODE | DEBUG_MODE)