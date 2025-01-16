#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024-2025 // cw_sp2__2024_2025            *
*                         file: cw_lex.cpp                  *
*                                                  (draft!) *
*************************************************************/

#include "../../../src/include/cli/cli.h"

#include "../../../src/include/def.h"
#include "../../../src/include/config.h"
#include "../../../src/include/generator/generator.h"
#include "../../../src/include/lexica/lexica.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

unsigned long long int mode = 0;
char parameters[PARAMETERS_COUNT][MAX_PARAMETERS_SIZE] = { "" };

void comandLineParser(int argc, char* argv[], unsigned long long int* mode, char(*parameters)[MAX_PARAMETERS_SIZE]) {
	char tempTemp[PATH_NAME_LENGH] = { '\0' }, * tempPtrPrev, * tempPtrNext, nameTemp[PATH_NAME_LENGH] = { '\0' };
	char modesNotDefined = 1;
	*mode = 0;
	for (int index = 1; index < argc; ++index) {
		if (!strcmp(argv[index], "-lex")) {
			*mode |= LEXICAL_ANALYZE_MODE;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-stx")) {
			*mode |= SYNTAX_ANALYZE_MODE;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-smt")) {
			*mode |= SEMANTIX_ANALYZE_MODE;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-gen")) {
			*mode |= MAKE_ASSEMBLY | MAKE_BINARY;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-run")) {
			*mode |= RUN_BINARY;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-all")) {
			*mode |= LEXICAL_ANALYZE_MODE | SYNTAX_ANALYZE_MODE | SEMANTIX_ANALYZE_MODE | MAKE_ASSEMBLY | MAKE_BINARY | RUN_BINARY;
			modesNotDefined = 0;
			continue;
		}
		else if (!strcmp(argv[index], "-d")) {
			*mode |= DEBUG_MODE;
			modesNotDefined = 0;
			continue;
		}

		// other keys
		// TODO:...

		// input filename
		strncpy(parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER], argv[index], MAX_PARAMETERS_SIZE);
	}

	// default mode, if not entered manually
	if (modesNotDefined) {
		if (parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][0] != '\0') {
			*mode = LEXICAL_ANALYZE_MODE | SYNTAX_ANALYZE_MODE | SEMANTIX_ANALYZE_MODE | MAKE_ASSEMBLY | MAKE_BINARY;
		}
		else {
			*mode = UNDEFINED_MODE;// | INTERACTIVE_MODE | ;
			printf("Used interactive mode\r\n\r\n");
		}
	}

	if (*mode & UNDEFINED_MODE) {
		*mode |= INTERACTIVE_MODE;
		//*mode |= DEFAULT_MODE;
		*mode |= DEBUG_MODE;
	}

	// default input filename, if not entered manually
	if (parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		strcpy(parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER], DEFAULT_INPUT_FILENAME);
		//printf("Input filename not setted. Used defaule input filename \"%s\"\r\n\r\n", parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER]);
		char choice[2] = { parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][0], parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][1] };
		//std::cout << "Enter file name(Enter \"" << choice[0] << "\" to use default \"" DEFAULT_INPUT_FILE "\"):\n";
		printf("Input filename not setted. Enter file name(or enter '%c' to use default \"%s\"): ", parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][0], parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER]);
		//std::cin >> fileName;
		(void)scanf("%s", parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER]/*, MAX_PARAMETERS_SIZE*/);
		if (parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][0] == choice[0] && parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][1] == '\0') {
			parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER][1] = choice[1];
		}
		printf("\r\n");
	}


	strncpy(nameTemp, parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER], PATH_NAME_LENGH);
	nameTemp[PATH_NAME_LENGH - 1] = '\0';
	tempPtrPrev = nameTemp;
	tempPtrNext = NULL;
	for (; tempPtrNext = strstr(tempPtrPrev + 1, "."); tempPtrPrev = tempPtrNext);
	if (tempPtrPrev != nameTemp) {
		*tempPtrPrev = '\0';
	}
#if 0
	strncpy(tempTemp, parameters[INPUT_FILENAME_WITH_EXTENSION_PARAMETER], PATH_NAME_LENGH);
	tempPtrPrev = tempTemp;
	tempPtrPrev[0] == '\"' ? ++tempPtrPrev : 0;
	tempPtrNext = tempPtrPrev = strtok(tempPtrPrev, " .\\/:");
	while (tempPtrNext != NULL) {
		tempPtrNext = strtok(NULL, " .\\/:");
		if (tempPtrPrev && tempPtrNext) {
			strncpy(nameTemp, tempPtrPrev, PATH_NAME_LENGH);
		}
		tempPtrPrev = tempPtrNext;
	}
#endif

	// default temp filename, if not entered manually
	if (*mode & (MAKE_LEXEMES_SEQUENSE | INTERACTIVE_MODE) && parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER], "_lexemes.txt", PATH_NAME_LENGH - strlen(parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out lexemes sequense filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & (MAKE_LEXEMES_SEQUENSE | INTERACTIVE_MODE) && parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], "_lexeme_error.txt", PATH_NAME_LENGH - strlen(parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out lexemes sequense filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_LEXEME_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & (MAKE_AST | INTERACTIVE_MODE) && parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER], ".ast", PATH_NAME_LENGH - strlen(parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out AST filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_AST_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & (MAKE_AST | INTERACTIVE_MODE) && parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], "_syntax_error.txt", PATH_NAME_LENGH - strlen(parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out AST filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_SYNTAX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & ((MAKE_C | MAKE_ASSEMBLY | MAKE_OBJECT | MAKE_BINARY) | INTERACTIVE_MODE) && parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER], "_semantix_error.txt", PATH_NAME_LENGH - strlen(parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out AST filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_SEMANTIX_ERROR_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & (MAKE_LEXEMES_SEQUENSE | INTERACTIVE_MODE) && parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER], "_prepared_lexemes.txt", PATH_NAME_LENGH - strlen(parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out lexemes sequense filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_PREPARED_LEXEMES_SEQUENSE_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if (*mode & (MAKE_C | INTERACTIVE_MODE) && parameters[OUT_C_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_C_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_C_FILENAME_WITH_EXTENSION_PARAMETER], ".c", PATH_NAME_LENGH - strlen(parameters[OUT_C_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out C filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_C_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
			//(void)getchar();
		}
	}

	// default temp filename, if not entered manually
	if ((*mode & (MAKE_ASSEMBLY | INTERACTIVE_MODE)) && parameters[OUT_ASSEMBLY_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_ASSEMBLY_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_ASSEMBLY_FILENAME_WITH_EXTENSION_PARAMETER], ".asm", PATH_NAME_LENGH - strlen(parameters[OUT_ASSEMBLY_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out assembly filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_ASSEMBLY_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
		}
	}

	// default input filename, if not entered manually
	if (*mode & (MAKE_OBJECT | INTERACTIVE_MODE) && parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}		
		strncpy(parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER], ".obj", PATH_NAME_LENGH - strlen(parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out object filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_OBJECT_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
		}
	}

	// default input filename, if not entered manually
	if (*mode & (MAKE_BINARY | INTERACTIVE_MODE) && parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER][0] == '\0') {
		if (*mode & INTERACTIVE_MODE) {
			system("CLS");
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);
		}
		strncpy(parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER], nameTemp, PATH_NAME_LENGH);
		strncat(parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER], ".exe", PATH_NAME_LENGH - strlen(parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER]));
		printf("Out binary filename not setted. Used defaule input filename \"%s\"\r\n", parameters[OUT_BINARY_FILENAME_WITH_EXTENSION_PARAMETER]);
		if (*mode & INTERACTIVE_MODE) {
			printf("Press Enter to next step");
			(void)getchar();
		}
	}

	return;
}

// after using this function use free(void *) function to release text buffer
size_t loadSource(char** text, char* fileName) {
	if (!fileName) {
		printf("No input file name\r\n");
		return 0;
	}

	FILE* file = fopen(fileName, "rb");

	if (file == NULL) {
		printf("File not loaded\r\n");
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long fileSize_ = ftell(file);
	if (fileSize_ >= MAX_TEXT_SIZE) {
		printf("the file(%ld bytes) is larger than %d bytes\r\n", fileSize_, MAX_TEXT_SIZE);
		fclose(file);
		exit(2); // TODO: ...
		//return 0;
	}
	size_t fileSize = fileSize_;
	rewind(file);

	if (!text) {
		printf("Load source error\r\n");
		return 0;
	}
	*text = (char*)malloc(sizeof(char) * (fileSize + 1));
	if (*text == NULL) {
		fputs("Memory error", stderr);
		fclose(file);
		exit(2); // TODO: ...
		//return 0;
	}

	size_t result = fread(*text, sizeof(char), fileSize, file);
	if (result != fileSize) {
		fputs("Reading error", stderr);
		fclose(file);
		exit(3); // TODO: ...
		//return 0;
	}
	(*text)[fileSize] = '\0';

	fclose(file);

	return fileSize;
}