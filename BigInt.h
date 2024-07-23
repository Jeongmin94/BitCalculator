#pragma once

#include <stdlib.h>
#include <string.h>

#include "Adder.h"
#include "func.h"

typedef struct _bigint {
	int size;
	int* data;
} BigInt;

BigInt* createBigInt();
void deleteBigInt(BigInt* bi);
void printBigIntBinary(const BigInt* bi);

BigInt* toBigInt(const char* num);
long long toLLInt(const BigInt* bi);

// add, sub
BigInt* bi_add(const BigInt* bi1, const BigInt* bi2);
BigInt* bi_sub(const BigInt* bi1, const BigInt* bi2);
BigInt* bi_comp(BigInt* bi);

// 테스트 및 문자열 <-> 숫자 변환의 용이성을 위해 64bit으로 제한
// !TODO: 64bit를 넘는 범위의 큰 정수에 대한 변환 추가 필요
BigInt* createBigInt() {
	BigInt* bi = (BigInt*)malloc(sizeof(BigInt));

	if (bi == NULL) return NULL;

	bi->size = 2; // 4byte * 2 = 8byte
	bi->data = (int*)malloc(sizeof(int) * bi->size);
	memset(bi->data, 0, sizeof(int) * bi->size);

	return bi;
}
void deleteBigInt(BigInt* bi) {
	free(bi->data);
	free(bi);
}

void printBigInt(const BigInt* bi) {
	long long sum = 0;

	int count = 0;
	for (int i = 0; i < bi->size; i++) {
		for (int j = 0; j < 32; j++) {
			long long mask = (1LL << count);

			sum += (bi->data[i] & mask);
			count++;
		}
	}

	printf("%lld\n", sum);
}

void printBigIntBinary(const BigInt* bi) {
	for (int i = bi->size - 1; i >= 0; i--) {
		for (int j = 31; j >= 0; j--) {
			int mask = (1 << j);

			if (BIT_EXT(bi->data[i], mask) != 0) {
				printf("%d", 1);
			}
			else {
				printf("%d", 0);
			}
		}
	}
	printf("\n");
}

BigInt* toBigInt(const char* num) {
	BigInt* bi = createBigInt();

	int idx = 0;
	bool bit[64];
	memset(bit, 0, sizeof(bit));

	long long n = atoll(num);
	for (int i = 0; i < 64; i++) {
		long long mask = (1LL << i);

		bit[i] = BIT_EXT(n, mask);
	}
	//for (int i = 63; i >= 0; i--) {
	//	printf("%d", bit[i]);
	//}
	//printf("\n");

	idx = 63;
	int count = 0;
	int bIdx = bi->size - 1;
	while (idx >= 0) {
		int mask = (1 << idx);

		bi->data[bIdx] = (bi->data[bIdx] | (bit[idx] << idx));

		idx--;
		count++;
		if (count % 32 == 0) {
			bIdx--;
			count = 0;
		}
	}

	return bi;
}

long long toLLInt(const BigInt* bi) {
	long long sum = 0;
	int count = 0;

	for (int i = 0; i < bi->size; i++) {
		for (int j = 0; j < 32; j++) {
			long long mask = (1LL << count);

			sum += (bi->data[i] & mask);
			count++;
		}
	}
	return sum;
}

BigInt* bi_add(const BigInt* bi1, const BigInt* bi2) {
	if (bi1 == NULL || bi2 == NULL) return NULL;

	int sz = (bi1->size > bi2->size) ? bi1->size : bi2->size;

	BigInt* nb = createBigInt();
	//nb->size = sz;
	//nb->data = (int*)malloc(sizeof(int) * sz);

	bool c = 0;
	for (int i = 0; i < sz; i++) {

		for (int j = 0; j < 32; j++) {
			int mask = (1 << j);

			bool carry = 0;
			bool sum = 0;
			int b1 = (bi1->size > i) ? bi1->data[i] : 0;
			int b2 = (bi2->size > i) ? bi2->data[i] : 0;
			fullAdder(BIT_EXT(b1, mask), BIT_EXT(b2, mask), c, &carry, &sum);

			c = carry;
			nb->data[i] = (nb->data[i] | (sum << j));
		}
	}

	return nb;
}

BigInt* bi_sub(const BigInt* bi1, const BigInt* bi2) {
	BigInt* comp = bi_comp(bi2);
	BigInt* ret = bi_add(bi1, comp);

	deleteBigInt(comp);
	return ret;
}

BigInt* bi_comp(BigInt* bi) {
	for (int i = 0; i < bi->size; i++) {
		bi->data[i] = ~bi->data[i];
	}
	BigInt* one = toBigInt("1");
	BigInt* ret = bi_add(bi, one);

	deleteBigInt(one);
	return ret;
}
