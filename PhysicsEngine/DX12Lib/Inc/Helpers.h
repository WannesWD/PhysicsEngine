#ifndef HELPER_H
#define HELPER_H

#define WIND32_LEAN_AND_MEAN
#include <Windows.h>
#include <exception>

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw std::exception();
}


#endif // !HELPER_H
