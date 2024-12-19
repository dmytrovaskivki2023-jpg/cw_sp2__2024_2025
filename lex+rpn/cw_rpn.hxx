#define _CRT_SECURE_NO_WARNINGS
#define IDENTIFIER_LEXEME_TYPE 2
#define VALUE_LEXEME_TYPE 4
#define VALUE_SIZE 4

#ifndef __cplusplus
#define bool int
#define false 0
#define true 1
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_MODE_BY_ASSEMBLY

#define MAX_TEXT_SIZE 8192
#define MAX_GENERATED_TEXT_SIZE (MAX_TEXT_SIZE * 6)
#define GENERATED_TEXT_SIZE_ 32768
#define GENERATED_TEXT_SIZE (MAX_TEXT_SIZE % MAX_GENERATED_TEXT_SIZE)


#define SUCCESS_STATE 0


#define CODEGEN_DATA_TYPE int

#define START_DATA_OFFSET 512
#define OUT_DATA_OFFSET (START_DATA_OFFSET + 512)

#define M1 1024
#define M2 1024


#define MAX_OUTTEXT_SIZE (8*8192*1024)
unsigned char outText[MAX_OUTTEXT_SIZE] = ""; // !!!
#define MAX_TEXT_SIZE 8192
#define MAX_WORD_COUNT (MAX_TEXT_SIZE / 5)
#define MAX_LEXEM_SIZE 1024

//int data2[M1][M2];

//int data[M1];

unsigned long long int dataOffsetMinusCodeOffset = 0x00003000;

struct LabelOffsetInfo {
	char labelStr[MAX_LEXEM_SIZE];
	unsigned char * labelBytePtr;
	// TODO: ...
};
struct LabelOffsetInfo labelsOffsetInfoTable[MAX_WORD_COUNT] = { { "", NULL/*, 0, 0*/ } };
struct LabelOffsetInfo* lastLabelOffsetInfoInTable = labelsOffsetInfoTable; // first for begin

struct GotoPositionInfo { // TODO: by Index
	char labelStr[MAX_LEXEM_SIZE];
	unsigned char * gotoInstructionPositionPtr;
	// TODO: ...
};
struct GotoPositionInfo gotoPositionsInfoTable[MAX_WORD_COUNT] = { { "", NULL/*, 0, 0*/ } }; // TODO: by Index
struct GotoPositionInfo* lastGotoPositionInfoInTable = gotoPositionsInfoTable; // first for begin

////////////////////////////////

#define MAX_TOKEN_STRUCT_ELEMENT_COUNT 64
#define MAX_TOKEN_STRUCT_ELEMENT_PART_COUNT 4

enum TokenStructName {
	MULTI_TOKEN_NOT,

	MULTI_TOKEN_ADD,
	MULTI_TOKEN_SUB,
	MULTI_TOKEN_MUL,
	MULTI_TOKEN_DIV,
	MULTI_TOKEN_MOD,

	MULTI_TOKEN_BIND_RIGHT_TO_LEFT,
	MULTI_TOKEN_BIND_LEFT_TO_RIGHT,

	MULTI_TOKEN_IF,
	MULTI_TOKEN_THEN,
	MULTI_TOKEN_ELSE,

	MULTI_TOKEN_FOR,
	MULTI_TOKEN_TO,
	MULTI_TOKEN_DOWNTO,
	MULTI_TOKEN_DO,

	MULTI_TOKEN_SEMICOLON,

	MULTI_TOKEN_BEGIN,
	MULTI_TOKEN_END
};
char(*a12345_ptr)[123];// = { '\0', '\0' };
char a12345[123][123] = { '\0' };
char* tokenStruct[MAX_TOKEN_STRUCT_ELEMENT_COUNT][MAX_TOKEN_STRUCT_ELEMENT_PART_COUNT] = { NULL };
void intitTokenStruct() {

	a12345_ptr = a12345;

	tokenStruct[MULTI_TOKEN_NOT][0] = (char*)"not";

	tokenStruct[MULTI_TOKEN_ADD][0] = (char*)"++";
	tokenStruct[MULTI_TOKEN_SUB][0] = (char*)"--";
	tokenStruct[MULTI_TOKEN_MUL][0] = (char*)"**";
	tokenStruct[MULTI_TOKEN_DIV][0] = (char*)"div";
	tokenStruct[MULTI_TOKEN_MOD][0] = (char*)"mod";

	tokenStruct[MULTI_TOKEN_BIND_RIGHT_TO_LEFT][0] = (char*)"<<";
	tokenStruct[MULTI_TOKEN_BIND_LEFT_TO_RIGHT][0] = (char*)">>";

	tokenStruct[MULTI_TOKEN_IF][0] = (char*)"if"; tokenStruct[MULTI_TOKEN_IF][1] = (char*)"(";
	tokenStruct[MULTI_TOKEN_THEN][0] = (char*)")"; 
	tokenStruct[MULTI_TOKEN_ELSE][0] = (char*)"else";


	tokenStruct[MULTI_TOKEN_FOR][0] = (char*)"for";
	tokenStruct[MULTI_TOKEN_TO][0] = (char*)"to";
	tokenStruct[MULTI_TOKEN_DOWNTO][0] = (char*)"downto";
	tokenStruct[MULTI_TOKEN_DO][0] = (char*)"do"; // tokenStruct[MULTI_TOKEN_DO][1] = (char*)":";

	tokenStruct[MULTI_TOKEN_SEMICOLON][0] = (char*)";";

	tokenStruct[MULTI_TOKEN_BEGIN][0] = (char*)"begin";
	tokenStruct[MULTI_TOKEN_END][0] = (char*)"end";

	//return 0;
}
char a_ = (intitTokenStruct(), 0);

unsigned char detectMultiToken(struct LexemInfo* lexemInfoTable, enum TokenStructName tokenStructName) {
	if (lexemInfoTable == NULL) {
		return false;
	}

	if (!strncmp(lexemInfoTable[0].lexemStr, tokenStruct[tokenStructName][0], MAX_LEXEM_SIZE)
		&& (!tokenStruct[tokenStructName][1] || !strncmp(lexemInfoTable[1].lexemStr, tokenStruct[tokenStructName][1], MAX_LEXEM_SIZE))
		&& (!tokenStruct[tokenStructName][2] || !strncmp(lexemInfoTable[2].lexemStr, tokenStruct[tokenStructName][2], MAX_LEXEM_SIZE))
		&& (!tokenStruct[tokenStructName][3] || !strncmp(lexemInfoTable[3].lexemStr, tokenStruct[tokenStructName][3], MAX_LEXEM_SIZE))) {
	
		return !!tokenStruct[tokenStructName][0]
			+ !!tokenStruct[tokenStructName][1]
			+ !!tokenStruct[tokenStructName][2]
			+ !!tokenStruct[tokenStructName][3]
			;
	}
	else {
		return 0;
	}
}


#define MAX_ACCESSORY_STACK_SIZE 128
struct LexemInfo lexemInfoTransformationTempStack[MAX_ACCESSORY_STACK_SIZE];
unsigned long long int lexemInfoTransformationTempStackSize = 0;

//


unsigned long long int getVariableOffset(char* identifierStr) {
	for (unsigned long long int index = 0; identifierIdsTable[index][0] != '\0'; ++index) {
		if (!strncmp(identifierIdsTable[index], identifierStr, MAX_LEXEM_SIZE)) {
			return START_DATA_OFFSET + sizeof(CODEGEN_DATA_TYPE) * index;
		}
	}
	
	return OUT_DATA_OFFSET;
}
//

	//0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 , 0x20 ,
unsigned char* outBytes2Code(unsigned char* currBytePtr, unsigned char* fragmentFirstBytePtr, unsigned long long int bytesCout) {
	for (; bytesCout--; *currBytePtr++ = *fragmentFirstBytePtr++);
	return currBytePtr;
}

unsigned char* makeEndProgramCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
		const unsigned char code__imul_ebp_4[]       = { 0x6B, 0xED, 0x04 };
		const unsigned char code__sub_stackTop_eax[] = { 0x03, 0xE5 };
		const unsigned char code__xor_ebp_ebp[]      = { 0x33, 0xED };

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__imul_ebp_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_stackTop_eax, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__xor_ebp_ebp, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf("\r\n");
	//printf("imul ebp, 4\r\n");
	//printf("add esp, ebp\r\n");
	//printf("xor ebp, ebp;\r\n");

	printf("    push 0\r\n");
	printf("    call ExitProcess\r\n");

	printf("\r\n\r\n");

	printf("    putProc PROC\r\n");		
	printf("        push eax\r\n");			
	printf("        push offset valueTemp_fmt\r\n");				
	printf("        push offset valueTemp_msg\r\n");					
	printf("        call wsprintfA\r\n");						
	printf("        add esp, 12\r\n");																	
	printf("\r\n");							
	printf("        ;push 40h\r\n");								
	printf("        ;push offset title_msg\r\n");									
	printf("        ;push offset valueTemp_msg;\r\n");										
	printf("        ;push 0\r\n");												
	printf("        ;call MessageBoxA\r\n");												
	printf("\r\n");
	printf("        push 0\r\n");
	printf("        push 0; offset NumberOfCharsWritten\r\n");
	printf("        push eax; NumberOfCharsToWrite\r\n");
	printf("        push offset valueTemp_msg\r\n");
	printf("        push hConsoleOutput\r\n");
	printf("        call WriteConsoleA\r\n");
	printf("\r\n");
	printf("        ret\r\n");												
	printf("    putProc ENDP\r\n");

	printf("\r\n\r\n");

	//printf("    getProc PROC\r\n");
	//printf("        push eax\r\n");
	//printf("        push offset valueTemp_fmt\r\n");
	//printf("        push offset valueTemp_msg\r\n");
	//printf("        call wsprintfA\r\n");
	//printf("        add esp, 12\r\n");
	//printf("\r\n");
	//printf("        push 40h\r\n");
	//printf("        push offset title_msg\r\n");
	//printf("        push offset valueTemp_msg;\r\n");
	//printf("        push 0\r\n");
	//printf("        call MessageBoxA\r\n");
	//printf("\r\n");
	//printf("        ret\r\n");
	//printf("    getProc ENDP\r\n");

	printf("    getProc PROC\r\n");		
	printf("        push ebp\r\n");			
	printf("        mov ebp, esp\r\n");				
	printf("\r\n");				
	printf("        push 0\r\n");					
	printf("        push offset readOutCount\r\n");						
	printf("        push 15\r\n");							
	printf("        push offset buffer + 1\r\n");								
	printf("        push hConsoleInput\r\n");									
	printf("        call ReadConsoleA\r\n");										
	printf("\r\n");										
	printf("        lea esi, offset buffer\r\n");											
	printf("        add esi, readOutCount\r\n");												
	printf("        sub esi, 2\r\n");													
	printf("        call string_to_int\r\n");														
	printf("\r\n");														
	printf("        mov esp, ebp\r\n");															
	printf("        pop ebp\r\n");																
	printf("        ret\r\n");																																	
	printf("    getProc ENDP\r\n");

	printf("\r\n\r\n");

	printf("    string_to_int PROC\r\n");
	printf("    ;  input: ESI - string\r\n");
	printf("    ; output: EAX - value\r\n");
	printf("        xor eax, eax\r\n");
	printf("        mov ebx, 1\r\n");
	printf("        xor ecx, ecx\r\n");
	printf("\r\n");
	printf("convert_loop :\r\n");
	printf("        movzx ecx, byte ptr[esi]\r\n");
	printf("        test ecx, ecx\r\n");
	printf("        jz done\r\n");
	printf("        sub ecx, '0'\r\n");
	printf("        imul ecx, ebx\r\n");
	printf("        add eax, ecx\r\n");
	printf("        imul ebx, ebx, 10\r\n");
	printf("        dec esi\r\n");
	printf("	    jmp convert_loop\r\n");
	printf("\r\n");
	printf("done:\r\n");
	printf("        ret\r\n");
	printf("    string_to_int ENDP\r\n");

	printf("\r\n\r\n");

	printf("end start\r\n");

	printf("\r\n\r\n");

#endif

	return currBytePtr;
}

unsigned char* makeTitle(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {

#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf(".686\r\n");
	printf(".model flat, stdcall\r\n");
	printf("option casemap : none\r\n");
#endif

	return currBytePtr;
}

unsigned char* makeDependenciesDeclaration(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {

#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf("\r\n");
	printf("GetStdHandle proto STDCALL, nStdHandle : DWORD\r\n");
	printf("ExitProcess proto STDCALL, uExitCode : DWORD\r\n");
	printf("ReadConsoleA proto STDCALL, hConsoleInput : DWORD, lpBuffer : DWORD, nNumberOfCharsToRead : DWORD, lpNumberOfCharsRead : DWORD, lpReserved : DWORD\r\n");
	printf("WriteConsoleA proto STDCALL, hConsoleOutput : DWORD, lpBuffert : DWORD, nNumberOfCharsToWrite : DWORD, lpNumberOfCharsWritten : DWORD, lpReserved : DWORD\r\n");
	printf("wsprintfA PROTO C : VARARG\r\n");
	printf("\r\n");
	printf("GetConsoleMode PROTO STDCALL, hConsoleHandle:DWORD, lpMode : DWORD\r\n");
	printf("\r\n");
	printf("SetConsoleMode PROTO STDCALL, hConsoleHandle:DWORD, dwMode : DWORD\r\n");
	printf("\r\n");
	printf("ENABLE_LINE_INPUT EQU 0002h\r\n");
	printf("ENABLE_ECHO_INPUT EQU 0004h\r\n");
#endif
		
	return currBytePtr;
}

unsigned char* makeDataSection(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf("\r\n");
	printf(".data\r\n");
	printf("    data_start db 8192 dup (0)\r\n");
	printf("    ;title_msg db \"Output:\", 0\r\n");
	printf("    valueTemp_msg db 256 dup(0)\r\n");
	printf("    valueTemp_fmt db \"%%d\", 10, 13, 0\r\n");
	printf("    ;NumberOfCharsWritten dd 0\r\n");
	printf("    hConsoleInput dd 0\r\n");
	printf("    hConsoleOutput dd 0\r\n");
	printf("    buffer db 128 dup(0)\r\n");
	printf("    readOutCount dd ?\r\n");
#endif

	return currBytePtr;
}

unsigned char* makeBeginProgramCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf("\r\n");
	printf(".code\r\n");
	printf("start:\r\n");
#endif

	return currBytePtr;
}

unsigned char* makeInitCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	unsigned char code__call_NexInstructionLabel[]          = { 0xE8, 0x00, 0x00, 0x00, 0x00 };      
	                                                                                                 
	unsigned char code__pop_esi[]                           = { 0x5E };                              
	unsigned char code__sub_esi_5[]                         = { 0x83, 0xEE, 0x05 };                  
	unsigned char code__mov_edi_esi[]                       = { 0x8B, 0xFE };                        
	unsigned char code__add_edi_dataOffsetMinusCodeOffset[] = { 0xE8, 0xC7, 0x00, 0x00, 0x00, 0x00 }; 
	//unsigned char code__xor_ebp_ebp[]                       = { 0x33, 0xED };                     
	unsigned char code__mov_ecx_edi[]                       = { 0x8B, 0xCF };                       
	unsigned char code__add_ecx_512[]                       = { 0x81, 0xC1, 0x00, 0x02, 0x00, 0x00 };
	const unsigned char* dataOffsetMinusCodeOffsetValueParts = (const unsigned char*)&dataOffsetMinusCodeOffset;
	//code__add_edi_dataOffsetMinusCodeOffset[2] = dataOffsetMinusCodeOffsetValueParts[0];
	//code__add_edi_dataOffsetMinusCodeOffset[3] = dataOffsetMinusCodeOffsetValueParts[1];
	//code__add_edi_dataOffsetMinusCodeOffset[4] = dataOffsetMinusCodeOffsetValueParts[2];
	//code__add_edi_dataOffsetMinusCodeOffset[5] = dataOffsetMinusCodeOffsetValueParts[3];

	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__call_NexInstructionLabel, 5);
	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__pop_esi, 1);
	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_esi_5, 3);
	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_edi_esi, 2);
	//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_edi_dataOffsetMinusCodeOffset, 6);
	//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__xor_ebp_ebp, 2);
	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_ecx_edi, 2);
	currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_ecx_512, 6);

#ifdef DEBUG_MODE_BY_ASSEMBLY
	printf("\r\n");
	printf("    db 0E8h, 00h, 00h, 00h, 00h; call NexInstruction\r\n");
	printf(";NexInstruction:\r\n");
	printf("    pop esi\r\n");
	printf("    sub esi, 5\r\n");
	printf("    mov edi, esi\r\n");//printf("    mov edi, offset data_start\r\n");
	printf("    add edi, 0%08Xh\r\n", (int)dataOffsetMinusCodeOffset);
	//printf("    xor ebp, ebp\r\n");
	printf("    mov ecx, edi\r\n");
	printf("    add ecx, 512\r\n");
	printf("    push -10\r\n");
	printf("    call GetStdHandle\r\n");
	printf("    mov hConsoleInput, eax\r\n");
	printf("    push -11\r\n");
	printf("    call GetStdHandle\r\n");
	printf("    mov hConsoleOutput, eax\r\n");
	printf("    \r\n");
	printf("    ;push ecx\r\n");		
	printf("    ;push ebx\r\n");			
	printf("    ;push esi\r\n");				
	printf("    ;push edi\r\n");
	printf("    ;push offset mode\r\n");
	printf("    ;push hConsoleInput\r\n");
	printf("    ;call GetConsoleMode\r\n");
	printf("    ;mov ebx, eax\r\n");
	printf("    ;or ebx, ENABLE_LINE_INPUT \r\n");
	printf("    ;or ebx, ENABLE_ECHO_INPUT\r\n");
	printf("    ;push ebx\r\n");
	printf("    ;push hConsoleInput\r\n");
	printf("    ;call SetConsoleMode\r\n");
	printf("    ;pop edi\r\n");
	printf("    ;pop esi\r\n");
	printf("    ;pop ebx\r\n");
	printf("    ;pop ecx\r\n");

#endif

	return currBytePtr;
}

unsigned char* makeValueCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if ((*lastLexemInfoInTable)->tokenType == VALUE_LEXEME_TYPE) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%d\"\r\n", (*lastLexemInfoInTable)->ifvalue);
#endif
		const unsigned char code__add_ecx_4[] = { 0x83, 0xC1, 0xC4 };
		unsigned char code__mov_toAddrFromECX_value[]     = { 0xC7, 0x01, 0x00, 0x00, 0x00, 0x00 };
		const unsigned char * valueParts = (const unsigned char*)&(*lastLexemInfoInTable)->ifvalue;
		code__mov_toAddrFromECX_value[2] = valueParts[0];
		code__mov_toAddrFromECX_value[3] = valueParts[1];
		code__mov_toAddrFromECX_value[4] = valueParts[2];
		code__mov_toAddrFromECX_value[5] = valueParts[3];

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_toAddrFromECX_value, 6);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    add ecx, 4\r\n");
		printf("    mov dword ptr [ecx], 0%08Xh\r\n", (int)(*lastLexemInfoInTable)->ifvalue);
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeIdentifierCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if ((*lastLexemInfoInTable)->tokenType == IDENTIFIER_LEXEME_TYPE) {
		bool findComplete = false;
		unsigned long long int variableIndex = 0;
		for (; identifierIdsTable[variableIndex][0] != '\0'; ++variableIndex) {
			if (!strncmp((*lastLexemInfoInTable)->lexemStr, identifierIdsTable[variableIndex], MAX_LEXEM_SIZE)) {
				findComplete = true;
				break;
			}
		}
		if (!findComplete) {
			printf("\r\nError!\r\n");
			exit(0);
		}
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", (*lastLexemInfoInTable)->lexemStr);
#endif

		variableIndex *= VALUE_SIZE;

		unsigned char code__mov_eax_edi[]                         = { 0x8B, 0xC7 };                   
		unsigned char code__add_eax_variableOffsetInDataSection[] = { 0x05, 0x00, 0x00, 0x00, 0x00 };
		const unsigned char code__mov_eax_valueByAdrressInEAX[]   = { 0x8B, 0x00 }; 
		const unsigned char code__add_ecx_4[]                     = { 0x83, 0xC1, 0xC4 }; 
		const unsigned char code__mov_toAddrFromECX_eax[]         = { 0x89, 0x01 };
		const unsigned char* variableIndexValueParts = (const unsigned char*)&variableIndex;
		code__add_eax_variableOffsetInDataSection[1] = variableIndexValueParts[0];
		code__add_eax_variableOffsetInDataSection[2] = variableIndexValueParts[1];
		code__add_eax_variableOffsetInDataSection[3] = variableIndexValueParts[2];
		code__add_eax_variableOffsetInDataSection[4] = variableIndexValueParts[3];

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_edi, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_eax_variableOffsetInDataSection, 5);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_valueByAdrressInEAX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_toAddrFromECX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, edi\r\n");
		printf("    add eax, 0%08Xh\r\n", (int)variableIndex);
		printf("    mov eax, dword ptr[eax]\r\n");
		printf("    add ecx, 4\r\n");
		printf("    mov dword ptr [ecx], eax\r\n");
#endif
		
		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeGetCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "get", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "get");
		printf("    push ecx\r\n");
		printf("    push ebx\r\n");
		printf("    push esi\r\n");
		printf("    push edi\r\n");
		printf("    call getProc\r\n");
		printf("    pop edi\r\n");
		printf("    pop esi\r\n");
		printf("    pop ebx\r\n");
		printf("    pop ecx\r\n");

#endif

		// TODO: ...

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov ebx, dword ptr[ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    add ebx, edi\r\n");
		printf("    mov dword ptr [ebx], eax\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makePutCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "put", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "put");
#endif

		//const unsigned char code__not_stackTopByECX[]     = { 0xF7, 0x11 }; 
		const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 }; 

		//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__not_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 3);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx]\r\n");
		printf("    call putProc\r\n");
		printf("    ;db 0FFh, 15h\r\n");	
		printf("    ;dd 00831113h\r\n"); // add adress
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeNotCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "not", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "not");
#endif

		const unsigned char code__not_stackTopByECX[]     = { 0xF7, 0x11 }; 
		const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 }; 

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__not_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 3);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    not dword ptr[ecx]\r\n");
		printf("    mov eax, dword ptr[ecx]\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeAddCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "++", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "++");
#endif

		const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 };       
		const unsigned char code__sub_ecx_4[]             = { 0x83, 0xED, 0x04 };
		const unsigned char code__add_stackTopByECX_eax[] = { 0x01, 0x01};       
		//const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 };       

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_stackTopByECX_eax, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    add dword ptr[ecx], eax\r\n");
		printf("    mov eax, dword ptr[ecx]\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeSubCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "--", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "--");
#endif

		const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 };
		const unsigned char code__sub_ecx_4[]             = { 0x83, 0xED, 0x04 };
		const unsigned char code__sub_stackTopByECX_eax[] = { 0x29, 0x01 };
		//const unsigned char code__mov_eax_stackTopByECX[] = { 0x8B, 0x01 };

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_stackTopByECX_eax, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    sub dword ptr[ecx], eax\r\n");
		printf("    mov eax, dword ptr[ecx]\r\n");
#endif
		
		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeMulCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "**", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "**");
#endif

		const unsigned char code__mov_eax_stackTopByECXMinus4[] = { 0x8B, 0x41, 0xFC };
		const unsigned char code__imul_stackTopByECX[]          = { 0xF7, 0x29 };      
		const unsigned char code__sub_ecx_4[]                   = { 0x83, 0xED, 0x04 };
		const unsigned char code__mov_toAddrFromECX_eax[]       = { 0x89, 0x01 };      

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECXMinus4, 3);
		//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__cdq, 1);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__imul_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_4, 3);
		//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__push_eax, 1);
		//currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__dec_ebp, 1);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_toAddrFromECX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx - 4]\r\n");
		printf("    cdq\r\n");
		printf("    imul dword ptr [ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    mov dword ptr [ecx], eax\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeDivCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "div", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "div");
#endif

		const unsigned char code__mov_eax_stackTopByECXMinus4[] = { 0x8B, 0x41, 0xFC };
		const unsigned char code__cdq[]                         = { 0x99 };            
		const unsigned char code__idiv_stackTopByECX[]          = { 0xF7, 0x39 };      
		const unsigned char code__sub_ecx_4[]                   = { 0x83, 0xED, 0x04 };
		const unsigned char code__mov_toAddrFromECX_eax[]       = { 0x89, 0x01 };      

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECXMinus4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__cdq, 1);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__idiv_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_toAddrFromECX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx - 4]\r\n");
		printf("    cdq\r\n");
		printf("    idiv dword ptr [ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    mov dword ptr [ecx], eax\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeModCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) { // task
	unsigned char multitokenSize = detectMultiToken(*lastLexemInfoInTable, MULTI_TOKEN_MOD);
	if (multitokenSize) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", "mod");
#endif

		const unsigned char code__mov_eax_stackTopByECXMinus4[] = { 0x8B, 0x41, 0xFC }; // mov eax, dword ptr[ecx - 4]
		const unsigned char code__cdq[]                         = { 0x99 };             // cdq
		const unsigned char code__idiv_stackTopByECX[]          = { 0xF7, 0x39 };       // idiv dword ptr [ecx]
		const unsigned char code__sub_ecx_4[]                   = { 0x83, 0xED, 0x04 }; // sub ecx, 4
		const unsigned char code__mov_eax_edx[]                 = { 0x8B, 0xC2 };       // mov eax, edx
		const unsigned char code__mov_toAddrFromECX_eax[]       = { 0x89, 0x01 };       // mov dword ptr [ecx], eax

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECXMinus4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__cdq, 1);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__idiv_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_edx, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_toAddrFromECX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx - 4]\r\n");
		printf("    cdq\r\n");
		printf("    idiv dword ptr [ecx]\r\n");
		printf("    sub ecx, 4\r\n");
		printf("    mov eax, edx\r\n");
		printf("    mov dword ptr [ecx], eax\r\n");
#endif

		return * lastLexemInfoInTable += multitokenSize, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeSemicolonAfterNonContextCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	unsigned char multitokenSize = detectMultiToken(*lastLexemInfoInTable, MULTI_TOKEN_SEMICOLON);
	if (multitokenSize
		&&
		!lexemInfoTransformationTempStackSize // !!!!
		) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"%s\"\r\n", ";");
#endif

		* lastLexemInfoInTable += multitokenSize;
	}

	return currBytePtr;
}

unsigned char* makeRightToLeftBindCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "<<", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\"<<\"\r\n");
#endif

		const unsigned char code__mov_eax_stackTopByECX[]       = { 0x8B, 0x01 };       // mov eax, dword ptr[ecx]
		const unsigned char code__mov_ebx_stackTopByECXMinus4[] = { 0x8B, 0x59, 0xFC }; // mov ebx, dword ptr[ecx - 4]
		const unsigned char code__sub_ecx_8[]                   = { 0x83, 0xED, 0x08 }; // sub ecx, 8
		const unsigned char code__add_ebx_edi[]                 = { 0x03, 0xDF };       // add ebx, edi
		const unsigned char code__mov_addrFromEBX_eax[]         = { 0x89, 0x03 };       // mov dword ptr [ebx], eax	

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_ebx_stackTopByECXMinus4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_8, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_ebx_edi, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_addrFromEBX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov eax, dword ptr[ecx]\r\n");
		printf("    mov ebx, dword ptr[ecx - 4]\r\n");
		printf("    sub ecx, 8\r\n");
		printf("    add ebx, edi\r\n");
		printf("    mov dword ptr [ebx], eax\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

unsigned char* makeLeftToRightBindCode(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, ">>", MAX_LEXEM_SIZE)) {
#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("\r\n");
		printf("    ;\">>\"\r\n");
#endif

		const unsigned char code__mov_ebx_stackTopByECX[]       = { 0x8B, 0x19 };
		const unsigned char code__mov_eax_stackTopByECXMinus4[] = { 0x8B, 0x41, 0xFC };
		const unsigned char code__sub_ecx_8[]                   = { 0x83, 0xED, 0x08 };
		const unsigned char code__add_ebx_edi[]                 = { 0x03, 0xDF };
		const unsigned char code__mov_addrFromEBX_eax[]         = { 0x89, 0x03 };

		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_ebx_stackTopByECX, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_eax_stackTopByECXMinus4, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__sub_ecx_8, 3);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__add_ebx_edi, 2);
		currBytePtr = outBytes2Code(currBytePtr, (unsigned char*)code__mov_addrFromEBX_eax, 2);

#ifdef DEBUG_MODE_BY_ASSEMBLY
		printf("    mov ebx, dword ptr[ecx]\r\n");
		printf("    mov eax, dword ptr[ecx - 4]\r\n");
		printf("    sub ecx, 8\r\n");
		printf("    add ebx, edi\r\n");
		printf("    mov dword ptr [ebx], eax\r\n");
#endif

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

//////

void makePrepare4IdentifierOrValue(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable) { // 

	if ((*lastLexemInfoInTable)->tokenType == IDENTIFIER_LEXEME_TYPE || (*lastLexemInfoInTable)->tokenType == VALUE_LEXEME_TYPE) {
		if (!strncmp((*lastLexemInfoInTable)[1].lexemStr, "<<", MAX_LEXEM_SIZE)
			|| 
			!strncmp((*lastLexemInfoInTable)[-1].lexemStr, ">>", MAX_LEXEM_SIZE)
			||
			!strncmp((*lastLexemInfoInTable)[-1].lexemStr, "get", MAX_LEXEM_SIZE)
			||
			!strncmp((*lastLexemInfoInTable)[-2].lexemStr, "get", MAX_LEXEM_SIZE)
			) {
			bool findComplete = false;
			for (unsigned long long int index = 0; identifierIdsTable[index][0] != '\0'; ++index) {
				if (!strncmp((*lastLexemInfoInTable)->lexemStr, identifierIdsTable[index], MAX_LEXEM_SIZE)) {
					findComplete = true;
					(*lastTempLexemInfoInTable)->ifvalue = /*dataOffset + */VALUE_SIZE * index;
					_itoa((*lastTempLexemInfoInTable)->ifvalue, (*lastTempLexemInfoInTable)->lexemStr, 10);
					((*lastTempLexemInfoInTable)++)->tokenType = VALUE_LEXEME_TYPE; // ADDRESS_LEXEME_TYPE
					++* lastLexemInfoInTable;
				}
			}
			if (!findComplete) {
				printf("\r\nError!\r\n");
				exit(0);
			}
		}
		else {
			*(*lastTempLexemInfoInTable)++ = *(*lastLexemInfoInTable)++;	
		}
	}
}

int precedenceLevel(char* lexemStr) {
	if (!strncmp(lexemStr, "not", MAX_LEXEM_SIZE)) {
		return 5;
	}

	else if (!strncmp(lexemStr, "**", MAX_LEXEM_SIZE)) {
		return 4;
	}
	else if (!strncmp(lexemStr, "div", MAX_LEXEM_SIZE)) {
		return 4;
	}
	else if (!strncmp(lexemStr, "mod", MAX_LEXEM_SIZE)) {
		return 4;
	}

	else if (!strncmp(lexemStr, "++", MAX_LEXEM_SIZE)) {
		return 3;
	}
	else if (!strncmp(lexemStr, "--", MAX_LEXEM_SIZE)) {
		return 3;
	}

	else if (!strncmp(lexemStr, "<<", MAX_LEXEM_SIZE)) {
		return 2;
	}
	else if (!strncmp(lexemStr, ">>", MAX_LEXEM_SIZE)) {
		return 2;
	}

	else if (!strncmp(lexemStr, "get", MAX_LEXEM_SIZE)) {
		return 1;
	}
	else if (!strncmp(lexemStr, "put", MAX_LEXEM_SIZE)) {
		return 1;
	}

	return 0;

}
bool isLeftAssociative(char* lexemStr) {
	if (!strncmp(lexemStr, "**", MAX_LEXEM_SIZE)) {
		return true;
	}
	else if (!strncmp(lexemStr, "div", MAX_LEXEM_SIZE)) {
		return true;
	}
	else if (!strncmp(lexemStr, "mod", MAX_LEXEM_SIZE)) {
		return true;
	}
	else if (!strncmp(lexemStr, "++", MAX_LEXEM_SIZE)) {
		return true;
	}
	else if (!strncmp(lexemStr, "--", MAX_LEXEM_SIZE)) {
		return true;
	}
	else if (!strncmp(lexemStr, ">>", MAX_LEXEM_SIZE)) {
		return false;
	}

	else if (!strncmp(lexemStr, "<<", MAX_LEXEM_SIZE)) {
		return false;
	}
	else if (!strncmp(lexemStr, "not", MAX_LEXEM_SIZE)) {
		return false;
	}
	else if (!strncmp(lexemStr, "get", MAX_LEXEM_SIZE)) {
		return false;
	}
	else if (!strncmp(lexemStr, "put", MAX_LEXEM_SIZE)) {
		return false;
	}

	return false;
}
void makePrepare4Operators(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable) {
	if (precedenceLevel((*lastLexemInfoInTable)->lexemStr)) {
		while (lexemInfoTransformationTempStackSize > 0) {
			struct LexemInfo& currLexemInfo = lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize - 1];
			if (precedenceLevel(currLexemInfo.lexemStr) && (
				(isLeftAssociative((*lastLexemInfoInTable)->lexemStr)  && (precedenceLevel((*lastLexemInfoInTable)->lexemStr) <= precedenceLevel(currLexemInfo.lexemStr)))
				||					
				(!isLeftAssociative((*lastLexemInfoInTable)->lexemStr) && (precedenceLevel((*lastLexemInfoInTable)->lexemStr) <  precedenceLevel(currLexemInfo.lexemStr)))
				)) {

				**lastTempLexemInfoInTable = currLexemInfo; ++*lastTempLexemInfoInTable;
				--lexemInfoTransformationTempStackSize;
			}
			else {
				break;
			}
		}

		lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize++] = *((*lastLexemInfoInTable)++);
	}
}

void makePrepare4LeftParenthesis(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable){
	if ((*lastLexemInfoInTable)->lexemStr[0] == '(') {
		lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize++] = *((*lastLexemInfoInTable)++);
	}
}

void makePrepare4RightParenthesis(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable) {
	if ((*lastLexemInfoInTable)->lexemStr[0] == ')') {
		bool findLeftParenthesis = false;
		while (lexemInfoTransformationTempStackSize > 0) {
			struct LexemInfo& currLexemInfo = lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize - 1];
			if (currLexemInfo.lexemStr[0] == '(') {
				findLeftParenthesis = true;
				break;
			}
			else {
				**lastTempLexemInfoInTable = currLexemInfo; ++*lastTempLexemInfoInTable;
				lexemInfoTransformationTempStackSize--;
			}
		}
		if (!findLeftParenthesis) {
			printf("Warning: parentheses mismatched\n");

			**lastTempLexemInfoInTable = **lastLexemInfoInTable; ++*lastTempLexemInfoInTable;
		}
		else {
			--lexemInfoTransformationTempStackSize;
		}

		++*lastLexemInfoInTable;
	}
}

void makePrepareEnder(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable) {
	while (lexemInfoTransformationTempStackSize > 0) {
		struct LexemInfo& currLexemInfo = lexemInfoTransformationTempStack[lexemInfoTransformationTempStackSize - 1];
		if (currLexemInfo.lexemStr[0] == '(' || currLexemInfo.lexemStr[0] == ')') {
			printf("Error: parentheses mismatched\n");
			exit(0);
		}

		**lastTempLexemInfoInTable = currLexemInfo; ++(*lastTempLexemInfoInTable);
		--lexemInfoTransformationTempStackSize;
	}

	(*lastTempLexemInfoInTable)->lexemStr[0] = '\0';
}

void makePrepare(struct LexemInfo** lastLexemInfoInTable, struct LexemInfo** lastTempLexemInfoInTable) {

	lexemInfoTransformationTempStackSize = 0;
	for (; (*lastLexemInfoInTable)->lexemStr[0] != '\0'; *(*lastTempLexemInfoInTable)++ = *(*lastLexemInfoInTable)++) {

		for (struct LexemInfo* lastLexemInfoInTable_ = NULL; lastLexemInfoInTable_ != *lastLexemInfoInTable;) {
			lastLexemInfoInTable_ = *lastLexemInfoInTable;
			makePrepare4IdentifierOrValue(lastLexemInfoInTable, lastTempLexemInfoInTable);
			makePrepare4Operators(lastLexemInfoInTable, lastTempLexemInfoInTable);
			makePrepare4LeftParenthesis(lastLexemInfoInTable, lastTempLexemInfoInTable);
			makePrepare4RightParenthesis(lastLexemInfoInTable, lastTempLexemInfoInTable);
		}

		makePrepareEnder(lastLexemInfoInTable, lastTempLexemInfoInTable);
	}
}

unsigned char* initMake(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	for (; (*lastLexemInfoInTable)->lexemStr[0] && strncmp((*lastLexemInfoInTable)->lexemStr, "body", MAX_LEXEM_SIZE); ++ * lastLexemInfoInTable);

	return currBytePtr;
}

unsigned char* noMake(struct LexemInfo** lastLexemInfoInTable, unsigned char* currBytePtr) {
	if (!strncmp((*lastLexemInfoInTable)->lexemStr, "name", MAX_LEXEM_SIZE)
		|| !strncmp((*lastLexemInfoInTable)->lexemStr, "data", MAX_LEXEM_SIZE)
		|| !strncmp((*lastLexemInfoInTable)->lexemStr, "long", MAX_LEXEM_SIZE)
		|| !strncmp((*lastLexemInfoInTable)->lexemStr, "int", MAX_LEXEM_SIZE)
		|| !strncmp((*lastLexemInfoInTable)->lexemStr, "body", MAX_LEXEM_SIZE)
		|| !strncmp((*lastLexemInfoInTable)->lexemStr, "end", MAX_LEXEM_SIZE)
		) {

		return ++ * lastLexemInfoInTable, currBytePtr;
	}

	return currBytePtr;
}

void makeCode(struct LexemInfo** lastLexemInfoInTable/*TODO:...*/, unsigned char* currBytePtr) { // TODO:...
	currBytePtr = makeTitle(lastLexemInfoInTable, currBytePtr);
	currBytePtr = makeDependenciesDeclaration(lastLexemInfoInTable, currBytePtr);

	currBytePtr = makeDataSection(lastLexemInfoInTable, currBytePtr);

	currBytePtr = makeBeginProgramCode(lastLexemInfoInTable, currBytePtr);
	lexemInfoTransformationTempStackSize = 0;
	currBytePtr = makeInitCode(lastLexemInfoInTable, currBytePtr);
	currBytePtr = initMake(lastLexemInfoInTable, currBytePtr);
	for (struct LexemInfo *lastLexemInfoInTable_; lastLexemInfoInTable_ = *lastLexemInfoInTable, (*lastLexemInfoInTable)->lexemStr[0] != '\0';) {
		currBytePtr = makeValueCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeIdentifierCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = makeNotCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = makeAddCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeSubCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeMulCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeDivCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeModCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = makeRightToLeftBindCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makeLeftToRightBindCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = makeGetCode(lastLexemInfoInTable, currBytePtr);
		currBytePtr = makePutCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = makeSemicolonAfterNonContextCode(lastLexemInfoInTable, currBytePtr);

		currBytePtr = noMake(lastLexemInfoInTable, currBytePtr);

		if (lastLexemInfoInTable_ == *lastLexemInfoInTable) {
			printf("\r\nError in the code generator! Unknown token!\r\n");
			exit(0);
		}

	}

	currBytePtr = makeEndProgramCode(lastLexemInfoInTable, currBytePtr);
}

unsigned char outCode[GENERATED_TEXT_SIZE] = { '\0' };
void viewCode(unsigned char* outCodePtr, size_t outCodePrintSize, unsigned char align) {
	printf("\r\n;            +0x0 +0x1 +0x2 +0x3 +0x4 +0x5 +0x6 +0x7 +0x8 +0x9 +0xA +0xB +0xC +0xD +0xE +0xF ");
	printf("\r\n;0x00000000: ");
	size_t outCodePrintIndex = outCodePrintSize - 1;
	for (size_t index = 0; index <= outCodePrintIndex;) {
		printf("0x%02X ", outCodePtr[index]);
		if (!(++index % align)) {
			size_t indexMinus16 = index - align;
			do {
				//printf("0x%02X ", outCodePtr[index]);
				if (outCodePtr[indexMinus16] >= 32 && outCodePtr[indexMinus16] <= 126) {
					printf("%c", outCodePtr[indexMinus16]);
				}
				else {
					printf(" ");
					//printf("%2c", 32);
				}
			} while (++indexMinus16 % align);

			printf("\r\n;0x%08X: ", (unsigned int)index);
		}
	}
}