#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: null_statement.cpp          *
*                                                  (draft!) *
*************************************************************/

#include "../../include/def.h"
#include "../../include/generator/generator.h"
#include "../../include/lexica/lexica.h"
#include "stdio.h"

unsigned char* makeNullStatementAfterNonContextCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr, unsigned char generatorMode) {
	unsigned char multitokenSize = detectMultiToken(*lastLexemInfoInTable, MULTI_TOKEN_NULL_STATEMENT);
	if (multitokenSize) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;null statement (non-context)\r\n");
#endif

		return *lastLexemInfoInTable += multitokenSize, currBytePtr;
	}

	return currBytePtr;
}