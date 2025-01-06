#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024 // lex  +  rpn  +  MACHINECODEGEN!   *
*                         file: goto_lable.cpp              *
*                                                  (draft!) *
*************************************************************/

#include <string>
#include <map>

#include "../../include/def.h"
#include "../../include/generator/generator.h"
#include "../../include/lexica/lexica.h"
#include "stdio.h"

std::map<std::string, unsigned long long int> labelInfoTable;

unsigned char* makeLabelCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr, unsigned char generatorMode) {
	unsigned char multitokenSize, multitokenSize_ = detectMultiToken(*lastLexemInfoInTable + 1, MULTI_TOKEN_NULL_STATEMENT);
	multitokenSize = detectMultiToken(*lastLexemInfoInTable + multitokenSize_ + 1, MULTI_TOKEN_COLON);
	if (multitokenSize) {
		multitokenSize += multitokenSize_;
	}
	if ((*lastLexemInfoInTable)->tokenType != IDENTIFIER_LEXEME_TYPE){
		return currBytePtr;
	}
	if (multitokenSize++) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;ident \"%s\"(as label) previous \"%s\"\r\n", (*lastLexemInfoInTable)->lexemStr, tokenStruct[MULTI_TOKEN_COLON][0]);
#endif

		labelInfoTable[(*lastLexemInfoInTable)->lexemStr] = (unsigned long long int)currBytePtr;

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    LABEL@%016llX:\r\n", (unsigned long long int)lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize - 1].lexemStr);
#endif

		return *lastLexemInfoInTable += multitokenSize, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeGotoLabelCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr, unsigned char generatorMode) {
	unsigned char multitokenSize = detectMultiToken(*lastLexemInfoInTable, MULTI_TOKEN_GOTO);
	if (multitokenSize++) {
		if ((*lastLexemInfoInTable + 1)->tokenType != IDENTIFIER_LEXEME_TYPE) {
			return currBytePtr;
		}
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\" previous ident \"%s\"(as label)\r\n", tokenStruct[MULTI_TOKEN_GOTO][0], (*lastLexemInfoInTable)[1].lexemStr);
#endif

		const unsigned char code__jmp_offset[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__jmp_offset, 5);
		*(unsigned int*)(currBytePtr - 4) = (unsigned int)((unsigned char*)labelInfoTable[(*lastLexemInfoInTable)->lexemStr] - currBytePtr);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    jmp LABEL@%016llX\r\n", (unsigned long long int)lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize - 1].lexemStr);
#endif

		return *lastLexemInfoInTable += multitokenSize, currBytePtr;
	}

	return currBytePtr;
}