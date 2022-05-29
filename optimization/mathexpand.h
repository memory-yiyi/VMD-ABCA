#pragma once
#include"rentype.h"

namespace math
{
	int factorial(rentype::byte x);
	template<typename type> type sum(const type* arr, int len);
	template<typename type> type mean(const type* arr, int len);
	template<typename type> type var(const type* arr, int len);
	template<typename type, rentype::byte size> type sum(const type* arr, int len);
	template<typename type, rentype::byte size> type mean(const type* arr, int len);
	template<typename type, rentype::byte size> type var(const type* arr, int len);
	template<typename type> int GetAddressOfArray(const type* first, const type* secend);
}

int math::factorial(rentype::byte x)
{
	if (!x)
		return 1;
	int ans = 1;
	for (rentype::byte i = x; i > 1; i--)
		ans *= i;
	if (x > 0)
		return ans;
	else
		return 0;
}

template<typename type>
type math::sum(const type* arr, int len)
{
	type ans = 0;
	for (int i = 0; i < len; i++)
		ans += arr[i];
	return ans;
}

template<typename type>
inline type math::mean(const type* arr, int len)
{
	return sum<type>(arr, len) / len;
}

template<typename type>
type math::var(const type* arr, int len)
{
	type ex = mean<type>(arr, len);
	type ans = 0;
	for (int i = 0; i < len; i++)
		ans += (arr[i] - ex) * (arr[i] - ex);
	return ans / (len - 1);
}

template<typename type, rentype::byte size>
type math::sum(const type* arr, int len)
{
	using rentype::byte;
	type ans = 0;
	const type* ap = arr;
	byte* cp = (byte*)ap;
	for (int i = 0; i < len; i++)
	{
		ans += *ap;
		cp += size;
		ap = (const type*)cp;
	}
	return ans;
}

template<typename type, rentype::byte size>
inline type math::mean(const type* arr, int len)
{
	return sum<type, size>(arr, len) / len;
}

template<typename type, rentype::byte size>
type math::var(const type* arr, int len)
{
	using rentype::byte;
	type ex = mean<type, size>(arr, len);
	type ans = 0;
	const type* ap = arr;
	byte* cp = (byte*)ap;
	for (int i = 0; i < len; i++)
	{
		ans += (*ap - ex) * (*ap - ex);
		cp += size;
		ap = (const type*)cp;
	}
	return ans / (len - 1);
}

template<typename type>
inline int math::GetAddressOfArray(const type* first, const type* secend)
{
	return ((size_t)secend - (size_t)first) / sizeof(type);
}
