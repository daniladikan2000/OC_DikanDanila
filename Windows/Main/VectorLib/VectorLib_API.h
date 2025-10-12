#pragma once

#ifdef VECTORLIB_EXPORTS
#define VECTORLIB_API __declspec(dllexport)
#else
#define VECTORLIB_API __declspec(dllimport)
#endif