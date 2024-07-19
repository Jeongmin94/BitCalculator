#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Adder.h"

#define BIT_EXT(num, mask) num & mask
#define COMP(num) (add(~num, 1))

void printBit(int num);
bool isNumber(const char* str);
bool isOperator(const char* str);

int add(int num1, int num2);
int sub(int num1, int num2);
int mul(int num1, int num2);
int div2(int num1, int num2);

void printBit(int num) {
	printf("num: %d\n", num);
	for (int i = 31; i >= 0; i--) {
		int mask = (1 << i);
		int val = ((num & mask) >> i);
		if (val < 0) val = 1;
		printf("%d", val);
	}
	printf("\n");
}

bool isNumber(const char* str) {
	for (int i = 0; i < str[i] != '\0'; i++) {
		if ((str[i] == '+' || str[i] == '-') && i == 0) continue;
		if (str[i] < '0' || str[i] > '9') return false;
	}

	return true;
}

bool isOperator(const char* str)
{
	return strcmp(str, "+") == 0
		|| strcmp(str, "-") == 0
		|| strcmp(str, "*") == 0
		|| strcmp(str, "/") == 0;
}

int add(int num1, int num2) {
	int res = 0;
	bool c = 0;
	for (int i = 0; i < 32; i++) {
		int mask = (1 << i);

		bool a = BIT_EXT(num1, mask);
		bool b = BIT_EXT(num2, mask);

		bool carry;
		bool sum;
		fullAdder(a, b, c, &carry, &sum);

		c = carry;
		res = res | (sum << i);
	}

	return res;
}

int sub(int num1, int num2) {
	return add(num1, COMP(num2));
}

int mul(int num1, int num2) {
	printf("c res: %d\n", num1 * num2);

	int res = 0;
	int sign = 1;

	if (num2 < 0) {
		num2 = add(~num2, 1);
		sign = -1;
	}

	for (int i = 0; i < 32; i++) {
		int mask = (1 << i);
		if ((BIT_EXT(num2, mask)) != 0)
			res = add(res, (num1 << i));
	}

	if (sign == -1) {
		res = add(~res, 1);
	}

	return res;
}

int div2(int num1, int num2) {
	printf("c res: %d %d\n", num1 / num2, num1 % num2);

	int res = 0;
	bool flag = 0;
	bool floor = 0;

	if (num1 < 0) {
		num1 = add(~num1, 1);
		flag ^= 1;
		floor = 1;
	}

	if (num2 < 0) {
		num2 = add(~num2, 1);
		flag ^= 1;
	}

	for (int i = 31; i >= 0; i--) {
		int val = num2 << i;
		if (val <= 0) continue;

		if (num1 >= val) {
			num1 = sub(num1, val);
			res = add(res, (1 << i));
		}
	}

	int remainder = num1;
	if (floor && remainder != 0) {
		res = add(res, 1);
		remainder = sub(num2, remainder);
	}

	if (flag) {
		res = add(~res, 1);
	}

	printf("¸ò: %d, ³ª¸ÓÁö: %d\n\n", res, remainder);

	return res;
}