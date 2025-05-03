#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#define FILE2_A "../built_src/file2.hpp"
#define FILE2_B "../built_src/file2.txt"
#define TABLE2 "transitionTable2"
#define RN2 KEYWORDS_RN

#define FILE3_A "../built_src/file3.hpp"
#define FILE3_B "../built_src/file3.txt"
#define TABLE3 "transitionTable3"
#define RN3 IDENTIFIERS_RN

#define FILE4_A "../built_src/file4.hpp"
#define FILE4_B "../built_src/file4.txt"
#define TABLE4 "transitionTable4"
#define RN4 UNSIGNEDVALUES_RN


//#define MAX_STATES 128
#define MAX_STATES 1024
//#define MAX_SYMBOLS 26
#define SYMBOL_NUMBER 256
#define EPSILON -1

#define ND -1

struct Transition {
    int from;
    int to;
    int symbolCode; // -1 = EPSILON
};

Transition transitions[1024];
int transition_count = 0;

int finit_states__NEW[1024]; // MAX_STATES
int finit_states_count__NEW = 0;
//int dead_state = 0;

struct NFA {
    int start;
    int end;
};

void add_transition(int from, int to, int symbolCode) {
    transitions[transition_count++] = { from, to, symbolCode };
}

int transition_table[MAX_STATES][SYMBOL_NUMBER];

void epsilon_closure(bool closure[], int state) {
    if (closure[state]) return;
    closure[state] = true;
    for (int i = 0; i < transition_count; ++i) {
        if (transitions[i].from == state && transitions[i].symbolCode == EPSILON)
            epsilon_closure(closure, transitions[i].to);
    }
}

void generate_transition_table(int state_counter) {
    memset(transition_table, -1, sizeof(transition_table));
    for (int s = 0; s < state_counter; ++s) {
        for (int sym = 0; sym < SYMBOL_NUMBER; ++sym) {
            bool closure[MAX_STATES] = { 0 };
            epsilon_closure(closure, s);
            for (int i = 0; i < transition_count; ++i) {
                if (closure[transitions[i].from] && transitions[i].symbolCode == sym) {
                    transition_table[s][sym] = transitions[i].to;
                }
            }
        }
    }
}

void print_transition_table(int state_counter, int dead_state) {
    if (dead_state != -1) {
        for (int stateIndex = 0; stateIndex < state_counter; ++stateIndex) {
            printf("#define Q%03d %d\n", stateIndex, stateIndex);
        }
    }
    printf("int transition_table[%d][%d] = {\n", SYMBOL_NUMBER, state_counter);
    printf("//          ");

    for (int i = 0; i < state_counter; ++i) {
        printf(" Q%03d  ", i);
    }

    printf("\n");
    for (int j = 0; j < SYMBOL_NUMBER; ++j) {
        if (j >= 32 && j <= 126) {
            printf("/*  '%c' */ {", j);
        }
        else {
            printf("/* \\x%02X */ {", j);
        }

        for (int i = 0; i < state_counter; ++i) {
            if (transition_table[i][j] != ND) {
                printf(" Q%03d", transition_table[i][j]);
            }
            else if (dead_state == -1) {
                printf("  ND ");
            }
            else {
                printf(" Q%03d", dead_state);
            }
            if (i < state_counter - 1) {
                printf(", ");
            }

        }
        printf(" }%s\n", j < SYMBOL_NUMBER - 1 ? "," : "");
    }
    printf("};\n");
}

void print_transition_table_to_file(char* fileName, char* tableName, int state_counter, int dead_state) {
    FILE* f = fopen(fileName, "w");
    if (!f) {
        perror("fopen");
        return;
    }

    fprintf(f, "#define _CRT_SECURE_NO_WARNINGS\n\n");
    fprintf(f, "#define SYMBOL_NUMBER 256\n");
    //fprintf(f, "#define STATE_NUMBER %d\n\n", state_counter);
    fprintf(f, "#define MAX_STATES 1024\n");
    fprintf(f, "#define MAX_FINIT_STATES 1024\n");

    fprintf(f, "\n");

    if (dead_state != -1) {
        for (int stateIndex = 0; stateIndex < state_counter; ++stateIndex) {
            fprintf(f, "#define Q%03d %d\n", stateIndex, stateIndex);
        }
    }

    fprintf(f, "\n");

    fprintf(f, "int %s[SYMBOL_NUMBER][MAX_STATES] = {\n", tableName);
    fprintf(f, "//          ");

    for (int i = 0; i < state_counter; ++i) {
        fprintf(f, " Q%03d  ", i);
    }

    fprintf(f, "\n");
    for (int j = 0; j < SYMBOL_NUMBER; ++j) {
        if (j >= 32 && j <= 126) {
            fprintf(f, "/*  '%c' */ {", j);
        }
        else {
            fprintf(f, "/* \\x%02X */ {", j);
        }

        for (int i = 0; i < state_counter; ++i) {
            if (transition_table[i][j] != ND) {
                fprintf(f, " Q%03d", transition_table[i][j]);
            }
            else if (dead_state == -1) {
                fprintf(f, "  ND ");
            }
            else {
                fprintf(f, " Q%03d", dead_state);
            }
            if (i < state_counter - 1) {
                fprintf(f, ", ");
            }
        }
        fprintf(f, " }%s\n", j < SYMBOL_NUMBER - 1 ? "," : "");
    }
    fprintf(f, "};\n");

    fprintf(f, "\n");

    fprintf(f, "int %sFinitStates[MAX_FINIT_STATES] = { ", tableName);
    for (int finitStatesIndex = 0; finitStatesIndex < finit_states_count__NEW; ++finitStatesIndex) {
        fprintf(f, "Q%03d%s", finit_states__NEW[finitStatesIndex], finitStatesIndex + 1 == finit_states_count__NEW ? " " : ", ");
    }
    fprintf(f, "};\n");

    fclose(f);
}

char* process_alternation__NEW(char* inputStr, int baseState, int* lastFreeState);

char* process_term__NEW(char* inputStr, int startState, int* nextFreeState) {
    while ((inputStr[0] != ')' || inputStr[1] == ')')
        && (inputStr[0] != '|' || inputStr[1] == '|')
        && (inputStr[-1] == '|' || inputStr[0] != '|' || inputStr[1] != '|' || inputStr[2] != '|') // ! functionally incomplete implementation
        && inputStr[0] != '\0') {

        if (inputStr[0] == '~' && inputStr[1] != '~' ||
            inputStr[0] == '^' && inputStr[1] != '^') {
            ++inputStr;
            continue;
        }

        if (inputStr[0] == '(' && inputStr[1] != '(') {
            inputStr = process_alternation__NEW(++inputStr, startState, nextFreeState);
        }
        else {
            add_transition(startState, *nextFreeState, *inputStr);
            if (inputStr[0] == '(' && inputStr[1] == '(' ||
                inputStr[0] == ')' && inputStr[1] == ')' ||
                inputStr[0] == '|' && inputStr[1] == '|' ||
                inputStr[0] == '~' && inputStr[1] == '~' ||
                inputStr[0] == '^' && inputStr[1] == '^') {
                ++inputStr;
            }
            ++inputStr;
            ++* nextFreeState;
        }

        startState = *nextFreeState - 1;

    }

    if (inputStr[0] == '|' && inputStr[1] != '|' ||
        inputStr[-1] != '|' && inputStr[0] == '|' && inputStr[1] == '|' && inputStr[2] == '|' // ! functionally incomplete implementation
        ) {
        ++inputStr;
    }

    return inputStr;
}

char* process_alternation__NEW(char* inputStr, int baseState, int* nextFreeState) {
    int alternation_outs_counter = 0;
    int alternation_outs[1024] = { 0 };
    int alternation_outs_pass_and_iteration_and_finit[1024] = { 0 };

    while ((inputStr[0] != ')' || inputStr[1] == ')')
        && inputStr[0] != '\0') {
        if (inputStr[0] == '~' && inputStr[1] != '~') {
            ++inputStr;
            continue;
        }
        if (inputStr[0] == '^' && inputStr[1] == '|') {
            finit_states__NEW[finit_states_count__NEW++] = baseState;
            inputStr += 2;
            continue;
        }

        inputStr = process_term__NEW(inputStr, baseState, nextFreeState);

        if (inputStr[0] != '\0' && inputStr[0] == ')' && inputStr[1] == '~' && inputStr[2] != '~') {
            alternation_outs_pass_and_iteration_and_finit[alternation_outs_counter] = 1;
        }
        alternation_outs[alternation_outs_counter++] = transition_count - 1;
    }

    for (int index = 0; index < alternation_outs_counter; ++index) {
        int enclosedState = transitions[alternation_outs[index] % 1024].to;
        int notFinitEnclosedState = 1;
        for (int finitStatesIndex = 0; finitStatesIndex < finit_states_count__NEW; ++finitStatesIndex) {
            if (finit_states__NEW[finitStatesIndex] == enclosedState) {
                notFinitEnclosedState = 0;
                break;
            }
        }

        if (notFinitEnclosedState) {
            transitions[alternation_outs[index] % 1024].to = *nextFreeState;

            for (int transitionIndex = 0; transitionIndex < transition_count; ++transitionIndex) {
                if (transitions[transitionIndex].to == enclosedState) {
                    transitions[transitionIndex].to = *nextFreeState;
                }
            }

            if (alternation_outs_pass_and_iteration_and_finit[alternation_outs_counter - 1] == 1) {
                add_transition(*nextFreeState, *nextFreeState, transitions[alternation_outs[index] % 1024].symbolCode);
            }
        }

#if 0
        for (int transitionIndex = 0; false && notFinitEnclosedState && transitionIndex < transition_count; ++transitionIndex) {
            if (transitions[transitionIndex].to == enclosedState) {
                if (alternation_outs_pass_and_iteration_and_finit[index] != 1) {
                    add_transition(transitions[transitionIndex].to, transitions[transitionIndex].to, transitions[transitionIndex].symbolCode);
                }
            }
        }
#endif

        if (alternation_outs_pass_and_iteration_and_finit[index] == 1) { // !!!!!!!!
            finit_states__NEW[finit_states_count__NEW++] = *nextFreeState;
        }
    }

    //    for (int index = 0; false && index < alternation_outs_counter; ++index) {
    //        if (alternation_outs_pass_and_iteration_and_finit[index]) {
    //            for (int transitionIndex = 0; alternation_outs_pass_and_iteration_and_finit[index] && transitionIndex < transition_count; ++transitionIndex) {
    //                if (transitions[transitionIndex].to == transitions[alternation_outs[index] % 1024].to) {
    //
    //                    transitions[transitionIndex].to = *nextFreeState;                         
    //                }
    //            }
    //        }
    //    }

    ++* nextFreeState;

    if (inputStr[0] == ')' && inputStr[1] != ')') {
        ++inputStr;
    }
    if (inputStr[0] == '~' && inputStr[1] != '~') {
        ++inputStr;
    }

    return inputStr;
}

#define TOKENS_RE         ";|:=|=:|\\+|-|\\*|,|==|!=|:|\\[|\\]|\\(|\\)|\\{|\\}|<=|>=|[_0-9A-Za-z]+|[^ \t\r\f\v\n]"
#define KEYWORDS_RE       ";|:=|=:|\\+|-|\\*|,|==|!=|:|\\[|\\]|\\(|\\)|\\{|\\}|NAME|DATA|BODY|END|BREAK|CONTINUE|GET|PUT|IF|ELSE|FOR|TO|DOWNTO|DO|WHILE|REPEAT|UNTIL|GOTO|DIV|MOD|<=|>=|NOT|AND|OR|INTEGER16"
#define IDENTIFIERS_RE    "_[A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z]"
#define UNSIGNEDVALUES_RE "0|[1-9][0-9]*"

// RN_SPEC (, ), |, ~, ^

#define KEYWORDS_RN___    "("\
                          ";"\
                          "|:="\
                          "|=:"\
                          "|\\+"\
                          "|-|"\
                          "\\*"\
                          "|,"\
                          "|=="\
                          "|!="\
                          "|:"\
                          "|\\["\
                          "|\\]"\
                          "|\\("\
                          "|\\)"\
                          "|\\{"\
                          "|\\}"\
                          "|NAME"\
                          "|DATA"\
                          "|BODY"\
                          "|END"\
                          "|BREAK"\
                          "|CONTINUE"\
                          "|GET"\
                          "|PUT"\
                          "|IF"\
                          "|ELSE"\
                          "|FOR"\
                          "|TO"\
                          "|DOWNTO"\
                          "|DO"\
                          "|WHILE"\
                          "|REPEAT"\
                          "|UNTIL"\
                          "|GOTO"\
                          "|DIV"\
                          "|MOD"\
                          "|<="\
                          "|>="\
                          "|NOT"\
                          "|AND"\
                          "|OR"\
                          "|INTEGER16"\
                          //\0

#define KEYWORDS_RN__     "("\
                          ";"\
                          "|:="\
                          "|=:"\
                          "|+"\
                          "|-"\
                          "|*"\
                          "|,"\
                          "|=="\
                          "|!="\
                          "|:"\
                          "|["\
                          "|]"\
                          "|(("\
                          "|))"\
                          "|{"\
                          "|}"\
                          "|NAME"\
                          "|DATA"\
                          "|BODY"\
                          "|END"\
                          "|BREAK"\
                          "|CONTINUE"\
                          "|GET"\
                          "|PUT"\
                          "|IF"\
                          "|ELSE"\
                          "|FOR"\
                          "|TO"\
                          "|DOWNTO"\
                          "|DO"\
                          "|WHILE"\
                          "|REPEAT"\
                          "|UNTIL"\
                          "|GOTO"\
                          "|DIV"\
                          "|MOD"\
                          "|<="\
                          "|>="\
                          "|NOT"\
                          "|AND"\
                          "|OR"\
                          "|INTEGER16"\
                          //\0

#define KEYWORDS_RN_      "("\
                          ";"\
                          "|:="\
                          "|:"\
                          "|=:"\
                          "|=="\
                          "|+"\
                          "|-"\
                          "|*"\
                          "|,"\
                          "|!="\
                          "|["\
                          "|]"\
                          "|(("\
                          "|))"\
                          "|{"\
                          "|}"\
                          "|NAME"\
                          "|DATA"\
                          "|BODY"\
                          "|END"\
                          "|BREAK"\
                          "|CONTINUE"\
                          "|GET"\
                          "|PUT"\
                          "|IF"\
                          "|ELSE"\
                          "|FOR"\
                          "|TO"\
                          "|DOWNTO"\
                          "|DO"\
                          "|WHILE"\
                          "|REPEAT"\
                          "|UNTIL"\
                          "|GOTO"\
                          "|DIV"\
                          "|MOD"\
                          "|<="\
                          "|>="\
                          "|NOT"\
                          "|AND"\
                          "|OR"\
                          "|INTEGER16"\
                          //\0

#define KEYWORDS_RN       "("\
                          ";"\
                          "|:(^|=)"\
                          "|=(:|=)"\
                          "|+"\
                          "|-"\
                          "|*"\
                          "|,"\
                          "|!="\
                          "|["\
                          "|]"\
                          "|(("\
                          "|))"\
                          "|{"\
                          "|}"\
                          "|N(AME|OT)"\
                          "|D(ATA|O(^|WNTO)|IV)"\
                          "|B(ODY|REAK)"\
                          "|E(ND|LSE)"\
                          "|CONTINUE"\
                          "|G(ET|OTO)"\
                          "|PUT"\
                          "|I(F|NTEGER16)"\
                          "|FOR"\
                          "|TO"\
                          "|WHILE"\
                          "|REPEAT"\
                          "|UNTIL"\
                          "|MOD"\
                          "|<="\
                          "|>="\
                          "|AND"\
                          "|OR"\
                          //\0

#define IDENTIFIERS_RN    "("\
                          "_"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"\
                          //\0

#define UNSIGNEDVALUES_RN "("\
                          "0"\
                          "|"\
                          "(1|2|3|4|5|6|7|8|9)"\
                          "(^|0|1|2|3|4|5|6|7|8|9)~"\
                          //\0


void generatorB(char* rn, char * fileNameA, char* fileNameB, char* tableName) { // "C"
    printf("NOT FULLY IMPLEMENTED!\n\n");

    int dead_state = -1;
    int baseState = 0;
    int nextFreeState = baseState + 1;

    if (*rn != '\0' && *rn == '(') {
        rn = process_alternation__NEW(++rn, baseState, &nextFreeState);
        finit_states__NEW[finit_states_count__NEW++] = nextFreeState - 1;
        dead_state = nextFreeState++;
    }

    for (int transitionIndex = 0; transitionIndex < transition_count; ++transitionIndex) {
        printf(" %d ---('%c')--> %d \n", transitions[transitionIndex].from, transitions[transitionIndex].symbolCode, transitions[transitionIndex].to);
    }

    printf("Finit states: ");
    for (int finitStatesIndex = 0; finitStatesIndex < finit_states_count__NEW; ++finitStatesIndex) {
        printf("Q%03d%s", finit_states__NEW[finitStatesIndex], finitStatesIndex + 1 == finit_states_count__NEW ? " " : ", ");
    }
    printf(".\n");
    printf("  Dead state: Q%03d .\n", dead_state);

    int state_counter = nextFreeState;

    generate_transition_table(state_counter);

    if (state_counter <= 32) {
        print_transition_table(state_counter, -1);
    }
    else {
        printf("Print to file only. Display will take too long.\n");
    }

    print_transition_table_to_file(fileNameB, tableName, state_counter, -1);

    printf("\n");
    if (state_counter <= 32) {
        print_transition_table(state_counter, dead_state);
    }
    else {
        printf("Print to file only. Display will take too long.\n");
    }

    print_transition_table_to_file(fileNameA, tableName, state_counter, dead_state);

    if (true) for (int transitionIndex = 0; transitionIndex < transition_count; ++transitionIndex) {
        printf(" %d ---('%c')--> %d \n", transitions[transitionIndex].from, transitions[transitionIndex].symbolCode, transitions[transitionIndex].to);
    }
}

int main() {
    transition_count = 0;
    finit_states_count__NEW = 0;
    generatorB((char*)RN2, (char*)FILE2_A, (char*)FILE2_B, (char*)TABLE2);
    transition_count = 0;
    finit_states_count__NEW = 0;
    generatorB((char*)RN3, (char*)FILE3_A, (char*)FILE3_B, (char*)TABLE3);
    transition_count = 0;
    finit_states_count__NEW = 0;
    generatorB((char*)RN4, (char*)FILE4_A, (char*)FILE4_B, (char*)TABLE4);

    return 0;
}
