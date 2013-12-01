#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* lexemes[20];
int noOfLexemes = 0;
char buff1[400];

FILE *fp, *fp_out, *fp_out2;

char ch;
int c, i = 0, i1, start_text = 0, end_text = 0, lineNo = 1, lexemeNo = 0;
void* v;
int state = 0, flag = 0, text = 0, nbt = 0, endParse = 0, errorLine = 0;
char token[100];

typedef struct {
	char rule[100];
	int ruleNum;
} cell;
cell parseTable[4][8];

typedef struct Node node;
struct Node {
	char string[100];
	node *next;
};

node *link, *head;
void initialize() {

	head = (node*) malloc(sizeof(node));
	strcpy(head->string, "$");
	head->next = NULL;

	link = head;
	head = (node*) malloc(sizeof(node));
	strcpy(head->string, "TABLE");
	head->next = link;
	return;
}

void initParseTable() {
	strcpy(parseTable[0][0].rule, "TABLE-> <table ATTRIBUTES> ROWS</table>");
	parseTable[0][0].ruleNum = 1;

	strcpy(parseTable[1][3].rule, "ROWS-> <tr ATTRIBUTES> CELLS ROWS");
	parseTable[1][3].ruleNum = 2;

	strcpy(parseTable[1][2].rule, "ROWS-> e");
	parseTable[1][2].ruleNum = 3;

	strcpy(parseTable[2][0].rule, "CELLS-> TABLE CELLS");
	parseTable[2][0].ruleNum = 5;

	strcpy(parseTable[2][4].rule, "CELLS-> <td> TK_TEXT</td> CELLS");
	parseTable[2][4].ruleNum = 4;

	strcpy(parseTable[2][5].rule, "CELLS-> </tr>");
	parseTable[2][5].ruleNum = 6;

	strcpy(parseTable[3][1].rule, "ATTRIBUTES-> e");
	parseTable[3][1].ruleNum = 8;

	strcpy(parseTable[3][6].rule,
			"ATTRIBUTES-> TK_TEXT=\"TK_TEXT\" ATTRIBUTES");
	parseTable[3][6].ruleNum = 7;
}

void storeLexemes(int start, int end) {
	lexemeNo++;
	int i1, j;
	char lexeme[50];
	for (i1 = start, j = 0; i1 <= end; i1++, j++) {
		lexeme[j] = buff1[i1];
	}
	errorLine = 0;
	if (j > 1) {
		while (lexeme[j - 1] == '\t' || lexeme[j - 1] == ' '
				|| lexeme[j - 1] == '\r' || lexeme[j - 1] == '\n') {
			if (lexeme[j - 1] == '\r')
				errorLine++;

			--j;
		}
	}
	lexeme[j] = '\0';
	fprintf(fp_out2, "Lexeme No %d: %s\n", lexemeNo, lexeme);
	lexemes[noOfLexemes] = lexeme;
	noOfLexemes++;
}

int nextToken() {

	if (i == strlen(buff1) - 1) {
		if (text) {
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);
		}
		parser("END");
		return 1;
	}

	ch = buff1[i];

	if (ch == '\r') {
		lineNo++;
	}

	if (!flag && !text
			&& (ch == '\r' || ch == '\t' || ch == ' ' || ch == '\n')) {

		start_text = i + 1;
		flag = 0;

		return 0;

	}

	switch (state) {
	case 0:
		switch (ch) {
		case '<':
			end_text = i;
			state = 1;
			break;
		case '>':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");
				strcpy(token, "TK_TEXT"); // storing token in an array called token[100]
				parser(token); //passing token to a parser
				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG"); // storing token in an array called token[100]
			parser(token); //passing token to a parser

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:

			end_text = i;

			text = 0;

			state = 19;
			break;
		}
		break;

	case 1:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;

		case 't':
		case 'T':
			state = 2;
			break;
		case '/':
			state = 9;
			break;

		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 2:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'r':
		case 'R':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_TR_OP\n");
			strcpy(token, "TK_TR_OP");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'a':
		case 'A':
			state = 4;
			break;
		case 'd':
		case 'D':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_TD_OP\n");

			strcpy(token, "TK_TD_OP");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;

		}
		break;

	case 4:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'b':
		case 'B':
			state = 5;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 5:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'l':
		case 'L':
			state = 6;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 6:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'e':
		case 'E':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}

			fprintf(fp_out, "TK_TABLE_OP\n");

			strcpy(token, "TK_TABLE_OP");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 9:
		switch (ch)

		{
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");
			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 't':
		case 'T':
			state = 20;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 10:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '>':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_TR_CL\n");

			strcpy(token, "TK_TR_CL");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 11:
		switch (ch) {

		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'b':
		case 'B':
			state = 12;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			break;
			state = 19;
			break;
		}
		break;

	case 12:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'l':
		case 'L':
			state = 13;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 13:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'e':
		case 'E':
			state = 14;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 14:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;

		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '>':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_TABLE_CL\n");

			strcpy(token, "TK_TABLE_CL");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 15:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;

		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '>':
			if (flag) {
				storeLexemes(start_text, end_text);
				fprintf(fp_out, "TK_TEXT\n");

				strcpy(token, "TK_TEXT");
				parser(token);

				flag = 0;
				text = 0;
			}
			fprintf(fp_out, "TK_TD_CL\n");

			strcpy(token, "TK_TD_CL");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	case 19:
		switch (ch) {
		case '<':
			end_text = i - 1;
			flag = 1;
			//text=1;
			state = 1;
			break;

		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);
			start_text = i + 1;
			state = 0;
			break;

		default:
			end_text = i;
			text = 1;
			break;
		}
		break;

	case 20:
		switch (ch) {
		case '<':
			end_text = i;
			flag = 1;
			state = 1;
			break;
		case '>':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_RT_ANG\n");

			strcpy(token, "TK_RT_ANG");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '=':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_EQ\n");

			strcpy(token, "TK_EQ");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case '"':
			storeLexemes(start_text, end_text);
			fprintf(fp_out, "TK_TEXT\n");

			strcpy(token, "TK_TEXT");
			parser(token);

			start_text = i + 1;
			flag = 0;
			text = 0;
			fprintf(fp_out, "TK_DOUBLE_QUOTES\n");

			strcpy(token, "TK_DOUBLE_QUOTES");
			parser(token);

			start_text = i + 1;
			state = 0;
			break;
		case 'r':
			state = 10;
			break;
		case 'a':
		case 'A':
			state = 11;
			break;
		case 'd':
		case 'D':
			state = 15;
			break;
		default:
			if (flag) {
				end_text = i;
				text = 0;
			}
			state = 19;
			break;
		}
		break;

	}
	return 0;
}

int parser(char token[]) {
	if (!endParse && strcmp(token, "END") == 0) {
		if (strcmp(head->string, "$") != 0) {
			printf("UNEXPECTED END OF FILE\n");
		}
		return -1;
	}

	if (endParse)
		return -1;
	if (strcmp(head->string, "$") == 0) {
		printf("Error at line no. %d\tUnexpected Token %s found\n",
				lineNo - errorLine, token);
		endParse = 1;
		return -1;
	} else if (strcmp(head->string, "TABLE") == 0) {
		if (strcmp(token, "TK_TABLE_OP") == 0) {

			strcpy(head->string, "TK_TABLE_CL");

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "ROWS");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_RT_ANG");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "ATTRIBUTES");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TABLE_OP");
			head->next = link;

			printf("%s \n", parseTable[0][0].rule);
		} else {
			printf("Error at line no. %d\tUnexpected Token %s found\n",
					lineNo - errorLine, token);
			endParse = 1;
			return -1; //error
		}
	} else if (strcmp(head->string, "ROWS") == 0) {
		if (strcmp(token, "TK_TABLE_CL") == 0) {

			head = head->next;

			printf("%s \n", parseTable[1][2].rule);
			//return -1;
		} else if (strcmp(token, "TK_TR_OP") == 0) {

			strcpy(head->string, "ROWS");

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "CELLS");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_RT_ANG");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "ATTRIBUTES");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TR_OP");
			head->next = link;

			printf("%s \n", parseTable[1][3].rule);
		} else {
			printf("Error at line no. %d\tUnexpected Token %s found\n",
					lineNo - errorLine, token);
			endParse = 1;
			return -1; //error
		}
	} else if (strcmp(head->string, "CELLS") == 0) {
		if (strcmp(token, "TK_TABLE_OP") == 0) {

			strcpy(head->string, "CELLS");

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TABLE");
			head->next = link;

			printf("%s \n", parseTable[2][0].rule);
		} else if (strcmp(token, "TK_TD_OP") == 0) {

			strcpy(head->string, "CELLS");

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TD_CL");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TEXT");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_RT_ANG");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TD_OP");
			head->next = link;

			printf("%s \n", parseTable[2][4].rule);
		} else if (strcmp(token, "TK_TR_CL") == 0) {

			strcpy(head->string, "TK_TR_CL");
			printf("%s \n", parseTable[2][5].rule);
		} else {
			printf("Error at line no. %d\tUnexpected Token %s found\n",
					lineNo - errorLine, token);
			endParse = 1;
			return -1; //error
		}
	} else if (strcmp(head->string, "ATTRIBUTES") == 0) {
		if (strcmp(token, "TK_RT_ANG") == 0) {

			head = head->next;

			printf("%s \n", parseTable[3][1].rule);
		} else if (strcmp(token, "TK_TEXT") == 0) {

			strcpy(head->string, "ATTRIBUTES");

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_DOUBLE_QUOTES");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TEXT");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_DOUBLE_QUOTES");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_EQ");
			head->next = link;

			link = head;
			head = (node*) malloc(sizeof(node));
			strcpy(head->string, "TK_TEXT");
			head->next = link;

			printf("%s \n", parseTable[3][6].rule);
		} else {
			printf("Error at line no. %d\tUnexpected Token %s found\n",
					lineNo - errorLine, token);
			endParse = 1;
			return -1; //error
		}
	} else {
		if (strcmp(head->string, token) == 0) {
			head = head->next;
			return 0;
		} else {
			printf("Error at line no. %d\tUnexpected Token %s found\n",
					lineNo - errorLine, token);
			endParse = 1;
			return -1; //error

		}
	}
	parser(token);
}

int main() {
	fp = fopen("input.txt", "r");
	fp_out = fopen("Tokens.txt", "w");
	fp_out2 = fopen("Text_Lexemes.txt", "w");
	v = (void*) malloc((401) * sizeof(char));
	c = fread(v, 1, 400, fp);
	for (i1 = 0; i1 < 400; i1++)
		buff1[i1] = '\0';
	strcpy(buff1, v);

	initParseTable();
	initialize();
	while (nextToken() == 0) {
		i++;
	}

	fclose(fp);
	fclose(fp_out);
	fclose(fp_out2);
}
