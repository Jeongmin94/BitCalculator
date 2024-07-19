# 비트연산자를 이용한 계산기

### 요구사항

- 덧셈, 뺄셈, 곱셈, 나눗셈을 비트 연산자를 이용하여 구현
- 곱셈과 나눗셈은 `shift` 연산자를 반드시 활용
- 나눗셈은 몫과 나머지로 출력
- 입력 예:

```
> 3 + 4
> 7
> 5 / 2
> 몫:2, 나머지: 1
```

### 구현 제약 조건

- 32bit signed int에 대한 계산만 취급
- `quit`을 입력하면 프로그램 종료

# 1. 가산기(adder)

CPU 레벨에서의 연산을 구현하기 위해 2진수 덧셈을 수행하는 가산기를 구현해야 한다. 본 프로그램에서는 반가산기(half adder)로 구성된 전가산기(full adder)를 이용하여 덧셈 연산을 구현한다.

### 1.1 반가산기(half adder)

반가산기는 한 자릿수의 2진수 연산을 수행하는 논리회로이다. 반가산기의 진리표는 다음과 같다.

|A|B|Carry|Sum|
|:-:|:-:|:-----:|:---:|
|0|0|0|0|
|0|1|0|1|
|1|0|0|1|
|1|1|1|0|

입력으로 $A$, $B$가 들어오고, 이 값에 대한 덧셈 결과($Sum$)와 올림수($Carry$)를 표현한다. 따라서 반가산기의 논리식은 다음과 같다.

$$ Sum = A xor B $$
$$ Carry = A and B $$

이를 코드로 구현하면 다음과 같다.

```C
void halfAdder(bool a, bool b, bool* carry, bool* sum)
{
	*carry = (a & b);
	*sum = (a ^ b);
}
```

### 1.2 전가산기(full adder)

반가산기는 두 개의 입력을 받아 합과 올림수를 구하기 위한 논리회로이다. 그렇기 때문에 이전 연산에서 발생한 올림수를 포함한 계산은 할 수 없다.

전가산기는 이전 연산에서 발생한 올림수까지 포함하여 세 개의 입력을 받아 덧셈을 하는 논리회로이다. 그렇기 때문에 전가산기 여러 개를 이용하면 n 비트의 크기를 가지는 숫자에 대한 가산을 할 수 있게 된다.

우선 전가산기의 진리표를 확인하면 다음과 같다.

|X|Y|Z|Carry|Sum|
|:-:|:-:|:-:|:-:|:-:|
|0|0|0|0|0|
|0|0|1|0|1|
|0|1|0|0|1|
|0|1|1|1|0|
|1|0|0|0|1|
|1|0|1|1|0|
|1|1|0|1|0|
|1|1|1|1|1|

여기에서 $Z$를 이전 연산에서 발생한 올림수라고 생각한다면 전가산기는 다음과 같은 방식으로 계산할 수 있다.

```
1. 반가산기에 X, Y를 입력하여 C1, S1을 얻는다.
2. 반가산기에 S1, Z를 입력하여 C2, S2를 얻는다.
3. OR 게이트에 C1, C2를 입력하여 C3를 얻는다.
4. S2는 X, Y, Z의 덧셈 결과, C3는 올림수가 된다.
5. n bit 가산기를 만들고 싶으면 C3를 다음 전가산기의 Z로 입력해준다.
```

이를 코드로 표현하면 다음과 같다.

```C
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

# 2. 덧셈, 뺄셈 구현

덧셈은 전가산기에 각 비트에 저장된 값을 입력하여 결과를 누적시키면 된다. 뺄셈이라면 빼려고 하는 수의 1의보수 + 1를 구하여 덧셈으로 변경하여 전가산기에 입력해주면 된다.

> A - B = A + (B의 1의 보수 + 1)

```C
#define BIT_EXT(num, mask) num & mask
#define COMP(num) (add(~num, 1))

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
```

# 3. 곱셈

`<<` 연산을 수행하면 비트를 좌측으로 n칸 밀 수 있다. 또한, n칸 밀면 그 값은 $2^{n}$ 배만큼 커진다.

```
7 << 0 => 7
7 << 1 => 14
7 << 2 => 28
7 << 3 => 56
```

그리고 어떤 수의 곱셈은 다음과 같이 표현할 수 있다.

$$ 3 * 17 = \\ 3 * 16 + 3 * 1 = \\ 
3 * 2^{4} + 3 * 2^{0} $$

즉, $A$와 $B$의 곱셈에서, $B$를 2의 거듭제곱 형태로 분해할 수 있다는 것이다. 2의 거듭제곱 형태로 분해할 수 있다면 $A$에 해당 비트를 left shift하여 곱셈을 수행할 수 있게 된다.

```C
int mul(int num1, int num2) {
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
```

# 4. 나눗셈

나눗셈은 분자가 분모에 몇 번 곱해지는지 찾는 방식을 통해 그 결과를 구할 수 있다.

분자에 left shift 연산을 수행하여 $2^{n}$배로 만든 수가 분모보다 작거나 같은 경우에 몫으로 $2^{n}$을 누적시키는 방식으로 계산한다. $ 17 / 3 $에 대하여 적용하면 다음과 같이 진행된다.

1. $ 3 * 2^3 = 24 $ 는 17 보다 크다.
2. $ 3 * 2^2 = 12 $ 는 17 보다 작기 때문에 몫에 4를 누적시키고, 17 - 12 = 5를 분모로 사용한다.
3. $ 3 * 2^1 = 6 $ 는 5 보다 크다.
4. $ 3 * 2^0 = 3 $ 는 5 보다 작기 때문에 몫에 1을 누적시키고, 5 - 3 = 2를 분모로 사용한다.
5. 분모가 분자보다 작기 때문에 과정이 종료된다.
6. 분모에 남은 2가 나머지가 된다.

나눗셈을 수행할 때 나머지는 항상 0보다 크거나 같고, 분자의 절대값보다 작아야 한다. 따라서 음수에 대한 나눗셈은 다음과 같이 처리한다.

```
# case0: -27 / -4
-4를 방향 전환(부호 변환) 없이 7번 더하고, 1이 남는다.(몫: 7, 나머지: 1)

# case1: -27 / 4
4를 방향 전환하여 7번 더하고, 1이 남는다.(몫: -7, 나머지: 1)

# case2: 27 / -4
-4를 방향 전환하여 6번 더하고, 3이 남는다.(몫: 6, 나머지: 3)
```

```C
// TODO: INT_MIN이 입력으로 들어오면, 부호 전환을 하는 과정에서 오버플로우가 발생하여 정상적인 나눗셈을 진행할 수 없다.
int div2(int num1, int num2) {
	//printf("c res: %d %d\n", num1 / num2, num1 % num2);

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

	printf("몫: %d, 나머지: %d\n\n", res, remainder);

	return res;
}
```

> TODO: 위의 코드로 나눗셈을 할 때 `INT_MIN`을 사용하는 경우, 부호 전환을 하는 과정에서 오버플로우가 발생하여 정상적인 나눗셈을 진행할 수 없다.