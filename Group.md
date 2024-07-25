# 큰 정수에 대한 덧셈, 뺄셈 계산기 구현

### 요구조건
- 64bit 최대 표현 범위를 넘어서는 숫자에 대한 덧셈, 뺄셈 연산을 구현
    - 2의 64제곱
    - 콤마 생략
# 1. 비트 단위 계산

### 구현 제약 사항
- 문자열 <-> 정수 변환에 대한 별도의 처리 로직을 구현하지 않았기 때문에 64bit long long 범위의 입력값이 주어진다고 가정
- 덧셈, 뺄셈 기능은 64bit 최대 표현 범위를 넘어서도 가능하도록 구현
### 1.1 BigInt 구조체 정의

- 64bit 범위를 넘어서는 정수를 관리하기 위한 `BigInt` 구조체 정의
- `data`는 낮은 자리수의 비트부터 시작

```C
typedef struct _bigint {
	int size;       // data 배열의 사이즈
	int* data;      // 이진수 형식으로 정수를 저장하기 위한 배열
} BigInt;
/*
ex)
size = 2이면, 해당 정수는 64bit의 크기를 가짐
| ------------------- data[0] ----------------  |
| 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 |
| ------------------- data[1] ----------------  |
| 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 |
*/
```

- `BigInt` 구조체에 대한 생성, 소멸 관련 함수

```C
BigInt* createBigInt(int size) {
	BigInt* bi = (BigInt*)malloc(sizeof(BigInt));

	if (bi == NULL) return NULL;

	bi->size = size; // 4byte * 2 = 8byte
	bi->data = (int*)malloc(sizeof(int) * bi->size);
	memset(bi->data, 0, sizeof(int) * bi->size);

	return bi;
}

void deleteBigInt(BigInt* bi) {
	free(bi->data);
	free(bi);
}
```

### 1.2 BigInt에 대한 덧셈

- 전가산기(full adder)를 이용하여 하위 비트부터 차례대로 가산 연산을 수행함
- 전가산기는 반가산기(half adder)를 사용하여 구현

```C
void halfAdder(bool a, bool b, bool* carry, bool* sum)
{
	*carry = (a & b);
	*sum = (a ^ b);
}

void fullAdder(bool a, bool b, bool c, bool* carry, bool* sum)
{
	bool tmpCarry;
	bool tmpSum;
	halfAdder(a, b, &tmpCarry, &tmpSum);

	bool tmpCarry2;

	halfAdder(tmpSum, c, &tmpCarry2, sum);
	*carry = (tmpCarry | tmpCarry2);
}
```

- `BigInt`에 대한 덧셈은 다음과 같이 구현

```C
BigInt* bi_add(const BigInt* bi1, const BigInt* bi2) {
	if (bi1 == NULL || bi2 == NULL) return NULL;

	int sz = (bi1->size > bi2->size) ? bi1->size : bi2->size;

	BigInt* nb = createBigInt(sz);
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
```

### 1.3 BigInt에 대한 뺄셈

- 정수의 뺄셈 연산은 다음과 같이 표현할 수 있음

$$ A - B = A + (-B) $$

- 컴퓨터에서 음수(부호 변환)를 구하는 방법은 1의 보수에 1을 더해주면 됨
    - 1의 보수 구하기 위해 `not(~)` 연산을 사용

```C
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
```

# 2. 문자열 계산

### 2.1 설계

1.    STRING_SIZE에 따라 정수 덧셈과 뺄셈의 결과에 대한 범위를 지정한다.
2.    연산 결과의 범위는 -(10^(STRING_SIZE - 1) - 1) ~ (2 * 10^(STRING_SIZE - 1) - 2)이다.
	a.    STRING_SIZE가 3이라면 ‘\0’를 제외한 두 자리 정수만을 입력으로 받을 수 있다. 연산 결과 범위는 -99 ~ 198이다.
3.    연산을 수행할 큰 정수 두 개((STRING_SIZE – 1)자리 정수)를 문자열로 각각 입력받는다.
	a.    입력은 피연산자와 연산자 사이에 공백이나 개행문자로 구분하여 입력 받는다.
	b.    음수를 입력받을 경우 부호를 제거하고 덧셈 또는 뺄셈 함수에 숫자만 해당하는 문자열 피연산자를 전달한다.
	c.    연산 결과가 음수일 경우 연산 마지막에 ‘-‘ 문자를 문자열 가장 앞에 추가할 것이다.
4.    두 정수의 자릿수가 다르다면 길이가 짧은 문자열에 0을 추가해서 자릿수가 큰 정수에 자릿수를 맞춘다.
	a.    예를 들어, 199+1을 한다고 했을 때, 199+001과 같이 자릿수가 적은 수에 0으로 자릿수를 채워준다.
5.    두 정수의 자릿수가 맞춰졌다면 각 자릿수를 연산한다.
	a.    덧셈
		i.    처음 자리올림수는 0으로 저장한다.
		ii.    1의 자리부터 연산을 하기 위해 반복문을 자릿수가 긴 문자열의 길이만큼의 인덱스부터 0까지 역순으로 돈다.
		iii.    반복문을 진행하면서 각 자릿수와 올림수를 더한 값이 10보다 크거나 같다면 자리올림수를 1로 설정한다. 그리고 1의 자리 숫자만 더해서 결과 문자열에 저장한다.(1의 자리 숫자만 더해주기 위해 10의 나머지 연산을 사용)
		iv.    각 자릿수와 올림수를 더한 값이 10보다 작다면 값을 결과 문자열에 저장하고 자리올림수를 0으로 설정한다.
		v.    반복문이 끝나고 자리올림수가 남아있다면 문자열 가장 왼쪽에 1을 삽입한다.
		vi.    덧셈 결과가 음수라면 문자열 가장 앞에 ‘-‘를 붙여준다.
b.    뺄셈
	i.    작은 수에서 큰 수를 뺄 때 음수 처리가 필요하다. a - b에서 b가 더 클 때, a와 b의 주소만 바꿔서 b – a가 되는 것처럼 한다. 그리고 마지막 결과에 단항 연산자 ‘-‘를 붙여준다.
	ii.    b가 더 크다는 것을 판별할 때 자릿수가 많은 경우, 자릿수는 같은데 값이 큰 경우를 나눠서 생각한다.
	iii.    덧셈과 마찬가지로 문자열을 역순으로 순회하면서 각 자릿수 별로 뺄셈을 진행한다.
	iv.    뺄셈은 받아내림이 필요하다. 뺄셈의 기준이 되는 수보다 빼려고 하는 수가 더 크다면 다음 자릿수에서 수를 빌려와서 빼야한다. 각 자릿수 즉, 한 자리씩 연산하고 있기 때문에 받아내림이 발생한다면 10-b+a를 결과 문자열에 저장한다. 그리고 빌림 수 1을 저장해놓는다.
	v.    직전 자리의 연산에서 다음 자리의 수를 빌려와서 뺐다면 현재 자리의 수를 1 감소시킨다. 그리고 빌림 수를 0으로 저장한다.
	vi.    뺄셈의 기준이 되는 수가 빼려고 하는 수보다 크거나 같으면 그대로 뺄셈을 진행해 결과 문자열에 저장한다.
	vii.    연산 결과 앞 쪽의 0이 있으면 문자열에서 0들을 제거한다.
	viii.    연산 결과가 음수라면 단항 연산자 ‘-‘를 문자열 가장 앞에 붙여준다.