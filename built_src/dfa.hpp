/*************************************************************************
* N.Kozak // Lviv'2020 // ACM // DFA for input string example (method 1) *
*    file: acmhw1_m1.c                                                   *
**************************************************************************/
#include "stdio.h"

#define SYMBOL_NUMBER 256 // dup
#define MAX_STATES 1024   // dup

typedef int INSTRUCTION;
typedef INSTRUCTION PROGRAM[SYMBOL_NUMBER][MAX_STATES];

typedef struct StructDFA{
	char * data;
	PROGRAM * program;
	void(*run)(struct StructDFA * dfa);
	int state;
} DFA;

void runner(DFA * dfa){
	for (; *dfa->data != '\0'; ++dfa->data){
		dfa->state = (*dfa->program)[*dfa->data][dfa->state];
	}
}

char tryToAccept(PROGRAM * program, int * finitStates, char * data){
	DFA dfa = { data, program, runner, 0/*Q000*/ };
	dfa.run(&dfa);

	for (; *finitStates; ++finitStates) {
		if (dfa.state == *finitStates) {
			printf("DFA: finit state\r\n");
			return 1;
		}
	}

	printf("DFA: no finit state\r\n");
	return 0;
}
