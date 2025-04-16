// clang-format off
#ifdef _WIN32
	// For Windows
	#define _CRT_SECURE_NO_WARNINGS
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif
// clang-format on

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "BigInt.h"
#include "Adder.h"

#define MX 200

// clang-format off
#ifdef _WIN32
	#define SAFE_STRCPY(dst, dstsize, src) strcpy_s(dst, dstsize, src)
	#define SAFE_GETS(buf, size) gets_s(buf, size)
#else
	#define SAFE_STRCPY(dst, dstsize, src)  \
		do                                  \
		{                                   \
			strncpy(dst, src, dstsize - 1); \
			dst[dstsize - 1] = '\0';        \
		} while (0)
	#define SAFE_GETS(buf, size)            \
		do                                  \
		{                                   \
			fgets(buf, size, stdin);        \
			buf[strcspn(buf, "\n")] = '\0'; \
		} while (0)
#endif
// clang-format on
bool isBig = false;

int main()
{

#ifdef _WIN32
	// _CrtSetBreakAlloc(89);
#endif

	char buf[MX];
	char tokens[3][MX];
	int mx = sizeof(tokens) / sizeof(tokens[0]);

	while (true)
	{
		memset(buf, 0, sizeof(buf));
		SAFE_GETS(buf, MX);

		if (strcmp(buf, "quit") == 0)
			break;
		if (strcmp(buf, "") == 0)
		{
			printf("Please enter a value.\n");
			continue;
		}
		if (strcmp(buf, "1") == 0)
		{
			printf("Int32 mode\n");
			isBig = false;
			continue;
		}

		if (strcmp(buf, "2") == 0)
		{
			printf("BigInt mode\n");
			isBig = true;
			continue;
		}

		memset(tokens, 0, sizeof(tokens));

		int idx = 0;
		int tokCount = 0;
		char *ptr = strtok(buf, " ");
		while (ptr != NULL && idx < mx)
		{
			strcpy(tokens[idx++], ptr);
			ptr = strtok(NULL, " ");
			++tokCount;
		}

		if (tokCount != 3)
		{
			printf("Please enter a valid input. (number) (operator) (number)\n");
			continue;
		}

		if (!(isNumber(tokens[0]) && isNumber(tokens[2])))
		{
			printf("Please enter numbers. Invalid inputs: {%s}, {%s}\n", tokens[0], tokens[2]);
			continue;
		}

		if (!isOperator(tokens[1]))
		{
			printf("Please enter a valid operator(+-*/).\n");
			continue;
		}

#define OP tokens[1][0]
		if (isBig == false)
		{
			int num1 = atoi(tokens[0]);
			int num2 = atoi(tokens[2]);
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
		else
		{
			BigInt *bi1 = toBigInt(tokens[0]);
			BigInt *bi2 = toBigInt(tokens[2]);

			switch (OP)
			{
			case '+':
			{
				BigInt *res = bi_add(bi1, bi2);
				printBigInt(res);
				printBigIntBinary(res);
				deleteBigInt(res);
			}
			break;
			case '-':
			{
				BigInt *res = bi_sub(bi1, bi2);
				printBigInt(res);
				printBigIntBinary(res);
				deleteBigInt(res);
			}
			break;
			}

			deleteBigInt(bi1);
			deleteBigInt(bi2);
		}
	}

#ifdef _WIN32
	// _CrtDumpMemoryLeaks();
#endif
	return 0;
}
