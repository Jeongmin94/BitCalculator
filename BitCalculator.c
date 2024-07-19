#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "func.h"
#include "Adder.h"

#define MX 200

int main()
{
	char buf[MX];
	char tokens[3][MX];
	int mx = sizeof(tokens) / sizeof(tokens[0]);

	while (true)
	{
		memset(buf, 0, sizeof(buf));
		gets(buf);
		if (strcmp(buf, "quit") == 0)
			break;
		if (strcmp(buf, "") == 0) {
			printf("식을 입력해주세요.\n");
			continue;
		}

		memset(tokens, 0, sizeof(tokens));

		int idx = 0;
		int tokCount = 0;
		char* ptr = strtok(buf, " ");
		while (ptr != NULL && idx < mx)
		{
			strcpy(tokens[idx++], ptr);
			ptr = strtok(NULL, " ");
			++tokCount;
		}

		if (tokCount != 3) {
			printf("정상적인 식을 입력해주세요. (정수) (연산자) (정수)\n");
			continue;
		}

		if (!(isNumber(tokens[0]) && isNumber(tokens[2]))) {
			printf("정수를 입력해주세요. 현재 입력된 값 {%s}, {%s}\n", tokens[0], tokens[2]);
			continue;
		}

		if (!isOperator(tokens[1])) {
			printf("올바른 연산자(+-*/)를 입력해주세요.\n");
			continue;
		}

		int num1 = atoi(tokens[0]);
		int num2 = atoi(tokens[2]);

#define OP tokens[1][0]
		switch (OP)
		{
		case '+':
		{
			printf("%d\n\n", add(num1, num2));
		}
		break;
		case '-':
		{
			printf("%d\n\n", sub(num1, num2));
		}
		break;
		case '*':
		{
			printf("%d\n\n", mul(num1, num2));
		}
		break;
		case '/':
		{
			div2(num1, num2);
		}
		break;
		}
	}

	return 0;
}

