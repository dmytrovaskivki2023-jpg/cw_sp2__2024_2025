#define _CRT_SECURE_NO_WARNINGS  // for using sscanf in VS
/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: cw.cpp                      *
*                                                           *
*************************************************************/
//#pragma comment(linker, "/STACK:516777216")

#include <windows.h>
//#include <winbase.h>
//#include <winuser.h>
//#include <shlobj.h>
//#include <shlwapi.h>
//#include <objbase.h>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "conio.h"

//#include "locale.h"

#include <direct.h>

#include <fstream>
#include <iostream>
#include <algorithm>//
#include <iterator>
#include <regex>

#include "src/include/def.h"
#include "src/include/config.h"

#include "src/include/cli/cli.h"
//bool reSetDefaultInputFileName_ = 
//strcpy(parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER], "../test_programs/file1.cwl")
//!= NULL;
#include "src/include/lexica/lexica.h"

#include "src/include/syntax/syntax.h"
#include "src/include/semantix/semantix.h"
#include "src/include/preparer/preparer.h"
#include "src/include/generator/generator.h"

struct LexemInfo lexemesInfoTableTemp[MAX_WORD_COUNT]; // = { { "", 0, 0, 0 } };
struct LexemInfo* lastLexemInfoInTableTemp = lexemesInfoTableTemp; // first for begin

unsigned char new_code[8 * 1024 * 1024] = { '\0' }; //

unsigned char tempCodeBuffer[8 * 1024 * 1024] = { '\0' };
unsigned char outCodeBuffer[8 * 1024 * 1024] = { '\0' };

unsigned char errorMessagesPtrToLastBytePtr[8 * 1024 * 1024] = { '\0' };

int main(int argc, char* argv[]) {
	PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 2, (UINT)LoadKeyboardLayoutA("00000409", KLF_ACTIVATE));

	char path[PATH_NAME_LENGH];
	char temp[2 * PATH_NAME_LENGH];
	char productionOut[MAX_TEXT_SIZE] = { 0 };

	comandLineParser(argc, argv, &mode, parameters);

	char* text;
	size_t sourceSize = loadSource(&text, parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER]);
	if (!sourceSize) {
		printf("Empty source . . .");
		printf("Press Enter to exit . . .");
		getchar();
		return 0;
	}

	if (!_getcwd(path, PATH_NAME_LENGH))
	{
		printf("getcwd error ...\r\n");
		printf("Press Enter to exit . . .");
		return -1;
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("ATTENTIOON: The next step is critical, if it is skipped the compilation process will be terminated!\r\n");
		printf("Enter 'y' to lexical analyze action(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & LEXICAL_ANALYZE_MODE) {

		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			printf("Original source:\r\n");
			printf("-------------------------------------------------------------------\r\n");
			printf("%s\r\n", text);
			printf("-------------------------------------------------------------------\r\n\r\n");
		}

		int commentRemoverResult = commentRemover(text, "#*", "*#");
		if (commentRemoverResult) {
			printf("Comment remover return %d\r\n", commentRemoverResult);
			printf("Press Enter to exit . . .");
			(void)getchar();
			return 0;
		}
		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			printf("Source after comment removing:\r\n");
			printf("-------------------------------------------------------------------\r\n");
			printf("%s\r\n", text);
			printf("-------------------------------------------------------------------\r\n\r\n");
		}

		struct LexemInfo ifBadLexemeInfo = tokenize(text, &lastLexemInfoInTable, identifierIdsTable, lexicalAnalyze);

		if (ifBadLexemeInfo.tokenType == UNEXPEXTED_LEXEME_TYPE) {
			UNEXPEXTED_LEXEME_TYPE;
			ifBadLexemeInfo.tokenType;
			printf("Lexical analysis detected unexpected lexeme\r\n");
			printLexemes(&ifBadLexemeInfo, 1);
			if (parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				printLexemesToFile(lexemesInfoTable, 1, parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]);
			}
			printf("Press Enter to exit . . .");
			(void)getchar();
			return 0;
		}
		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			printLexemes(lexemesInfoTable, 0);
			if (parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				printLexemesToFile(lexemesInfoTable, 0, parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]);
			}
			if (parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				writeBytesToFile(parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], (unsigned char*)"No error.", strlen("No error."));
			}
		}
		else {
			printf("Lexical analysis complete success\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("ATTENTIOON: The next step is critical, if it is skipped the compilation process will be terminated!\r\n");
		printf("Enter 'y' to syntax analyze action(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & SYNTAX_ANALYZE_MODE) {
//		if (!mode) { // !!!!!!!!!!!!!
//			printf("Enter 'y' to save AST(to pass action process enter 'n' or others key): ");
//		}
		errorMessagesPtrToLastBytePtr[0] = '\0';
		unsigned char* errorMessagesPtrToLastBytePtr_ = errorMessagesPtrToLastBytePtr;
		if (SUCCESS_STATE != syntaxAnalyze(lexemesInfoTable, &grammar, DEFAULT_SYNTAX_ANAlYZE_MODE, parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER], (char*)errorMessagesPtrToLastBytePtr)) { // add AST param	
			if (parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {			
				writeBytesToFile(parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], errorMessagesPtrToLastBytePtr, strlen((const char*)errorMessagesPtrToLastBytePtr));		
			}
			if (parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				writeBytesToFile(parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER], (unsigned char*)"AST build failed.", strlen("AST build failed."));
			}
			return 0;
		}

		if (parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
			writeBytesToFile(parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], (unsigned char*)"No error.", strlen("No error."));
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("\r\ncw terminated.");
		return 0;
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("ATTENTIOON: The next step is critical, if it is skipped the compilation process will be terminated!\r\n");
		printf("Enter 'y' to semantix analyze action(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & SEMANTIX_ANALYZE_MODE) {
		errorMessagesPtrToLastBytePtr[0] = '\0';
		unsigned char* errorMessagesPtrToLastBytePtr_ = errorMessagesPtrToLastBytePtr;
		if (SUCCESS_STATE != semantixAnalyze(lexemesInfoTable, &grammar, identifierIdsTable, (char **)&errorMessagesPtrToLastBytePtr)) {
			if (parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				writeBytesToFile(parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], errorMessagesPtrToLastBytePtr, strlen((const char*)errorMessagesPtrToLastBytePtr));
			}
			return 0;
		}

		if (parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
			writeBytesToFile(parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], (unsigned char*)"No error.", strlen("No error."));
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("\r\ncw terminated.");
		return 0;
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("ATTENTIOON: The next step is critical, if it is skipped the compilation process will be terminated!\r\n");
		printf("Enter 'y' for the action to prepare for the compilation process(to pass the action process, enter 'n' or another key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & MAKE_PREPARE) {
		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			printLexemes(lexemesInfoTable, 0);
		}

		lastLexemInfoInTable = lexemesInfoTable;
		makePrepare(lexemesInfoTable, &lastLexemInfoInTable, &lastLexemInfoInTableTemp);

		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			printLexemes(lexemesInfoTableTemp, 0);
			if (parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
				printLexemesToFile(lexemesInfoTableTemp, 0, parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]);
			}
		}
		else {
			printf("Make prepare(expressions separation + creating reverse Polish notation) complete success\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("\r\ncw terminated.");
		return 0;
	}

	if (false && mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n\r\n");
		printf("\r\n");
		printf("Enter 'y' to create C-code(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (false && (mode & MAKE_C || mode & INTERACTIVE_MODE && getchar() == 'y')) { // MAKE_ASSEMBLY_MODE

		if (mode & DEBUG_MODE) {
			printLexemes(lexemesInfoTable, 0);
		}

		unsigned long long int imageSize = 0;
		unsigned char* currBytePtr = 0;
		//		if (objectMode) {
		imageSize = buildTemplateForCodeObject(new_code);
		currBytePtr = getObjectCodeBytePtr(new_code);
		//		}
		//		else {
		//			imageSize = buildTemplateForCodeImage(new_code);
		//			currBytePtr = getImageCodeBytePtr(new_code);
		//		}

		lastLexemInfoInTableTemp = lexemesInfoTableTemp;
		//		lastLexemInfoInTableTemp = lexemesInfoTableTemp;
				//	struct LexemInfo* lastLexemInfoInTableTemp_ = lexemesInfoTableTemp; // first for begin
		if (true)makeCode(&lastLexemInfoInTableTemp, currBytePtr, C_CODER_MODE);
		//printf("\r\n;CODE:\r\n");
		//viewCode(outCode, 160/*GENERATED_TEXT_SIZE*/, 16);
		//printf("\r\n;ENDCODE;\r\n");

//		if (objectMode) {
//			writeBytesToFile("out.obj", new_code, imageSize);
//		}
//		else {
		writeBytesToFile("out.exe", new_code, imageSize);
		//		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}

	if (false && mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("\r\n");
		printf("Enter 'y' to create assembly(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (false && (mode & MAKE_ASSEMBLY || mode & INTERACTIVE_MODE && getchar() == 'y')) { // MAKE_ASSEMBLY_MODE

		if (mode & DEBUG_MODE) {
			printLexemes(lexemesInfoTable, 0);
		}

		unsigned long long int imageSize = 0;
		unsigned char* currBytePtr = 0;
		//		if (objectMode) {
		imageSize = buildTemplateForCodeObject(new_code);
		currBytePtr = getObjectCodeBytePtr(new_code);
		//		}
		//		else {
		//			imageSize = buildTemplateForCodeImage(new_code);
		//			currBytePtr = getImageCodeBytePtr(new_code);
		//		}

		lastLexemInfoInTableTemp = lexemesInfoTableTemp;
		//	struct LexemInfo* lastLexemInfoInTableTemp_ = lexemesInfoTableTemp; // first for begin
		if (true)makeCode(&lastLexemInfoInTableTemp, currBytePtr, ASSEMBLY_X86_WIN32_CODER_MODE);
		//printf("\r\n;CODE:\r\n");
		//viewCode(outCode, 160/*GENERATED_TEXT_SIZE*/, 16);
		//printf("\r\n;ENDCODE;\r\n");

//		if (objectMode) {
//			writeBytesToFile("out.obj", new_code, imageSize);
//		}
//		else {
		writeBytesToFile("out.exe", new_code, imageSize);
		//		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}

	unsigned long long int byteCountWritedToTempCodeBuffer = 0;
	if (mode & INTERACTIVE_MODE) { // BUILD NATIVE CODE
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("ATTENTIOON: The next step is critical, if it is skipped the compilation process will be terminated!\r\n");
		printf("Enter 'y' to create native code(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & MAKE_ASSEMBLY) {
		lastLexemInfoInTableTemp = lexemesInfoTableTemp;

		byteCountWritedToTempCodeBuffer = makeCode(&lastLexemInfoInTableTemp, tempCodeBuffer, MACHINE_X86_WIN32_CODER_MODE) - tempCodeBuffer;

		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			viewCode(tempCodeBuffer, byteCountWritedToTempCodeBuffer, 16);
		}
		else {
			printf("Native code created complete successfully.\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("\r\ncw terminated.");
		return 0;
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("\r\n");
		printf("Enter 'y' to create obj-file(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & MAKE_OBJECT) {
		unsigned long long int objectSize = buildTemplateForCodeObject(outCodeBuffer);
		unsigned char* currBytePtr = getObjectCodeBytePtr(outCodeBuffer);

		(void)outBytes2Code(currBytePtr, tempCodeBuffer, byteCountWritedToTempCodeBuffer);

		writeBytesToFile(parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER], outCodeBuffer, objectSize);

		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			viewCode(outCodeBuffer, objectSize, 16);
		}
		else {
			printf("Object code created complete successfully.\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else if (mode & INTERACTIVE_MODE) {
		(void)getchar();
	}

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("\r\n");
		printf("Enter 'y' to create exe-file(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & MAKE_BINARY) {
		unsigned long long int imageSize = buildTemplateForCodeImage(outCodeBuffer);
		unsigned char* currBytePtr = getImageCodeBytePtr(outCodeBuffer);

		(void)outBytes2Code(currBytePtr, tempCodeBuffer, byteCountWritedToTempCodeBuffer);

		//writeBytesToFile("out.exe", new_code, imageSize);
		writeBytesToFile(parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER], outCodeBuffer, imageSize);

		if (mode & (DEBUG_MODE | INTERACTIVE_MODE)) {
			viewCode(outCodeBuffer, imageSize, 16);
		}
		else {
			printf("Object code created complete successfully.\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("\r\nPress Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("\r\ncw terminated.");
		return 0;
	}
#if 0
	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("\r\n");
		printf("Enter 'y' to assembly program action(to pass action process enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & MAKE_ASSEMBLY || getchar() == 'y') { // MAKE_ASSEMBLY_MODE

		if (mode & DEBUG_MODE) {
			printLexemes(lexemesInfoTable, 0);
		}

		lastLexemInfoInTable = lexemesInfoTable;
		makePrepare(lexemesInfoTable, &lastLexemInfoInTable, &lastLexemInfoInTableTemp);
		//printLexemes(lexemesInfoTableTemp, 0);

		if (mode & DEBUG_MODE) {
			printLexemes(lexemesInfoTableTemp, 0);
		}
		else {
			printf("Make prepare(expressions separation + creating reverse Polish notation) complete success\r\n");
		}

		if (mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}
	else {
		printf("cw terminated.");
		return 0;
	}
#endif

	if (mode & INTERACTIVE_MODE) {
		system("CLS");
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		printf("No command line arguments are entered, so you are working in step-by-step interactive mode.\r\n");
		printf("\r\n");
		printf("Enter 'y' to run program action(to pass action process Enter 'n' or others key): ");
	}
	fflush(stdin);
	if (mode & INTERACTIVE_MODE && getchar() == 'y' || mode & RUN_BINARY) {
		printf("\r\n");
		//sprintf(temp, "START /b /wait \"\" /D \"%s\\masm32p\" %s.exe", path, parameters[OUT_BINARY_FILENAME_WITHOUT_EXTENSION_PARAMETER]);
		snprintf(temp, MAX_PARAMETERS_SIZE, "START /b /wait \"\" %s", parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER]);
		fflush(stdin);
		system((char*)temp);
		fflush(stdin);
	}
	else if (mode ^ RUN_BINARY) {
		printf("\r\n");
	}

	printf("\r\n\r\nPress Enter to exit . . .");
	getchar();

	return 0;
}