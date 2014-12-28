
#ifndef __SM_UTIL_H__
#define __SM_UTIL_H__

namespace smutil
{
	// random
	void randomize(unsigned long seed);
	int  random(unsigned long max);
	int  expected(unsigned long seed, unsigned long max, unsigned long iteration = 0);

	// math
	template <typename T>
	T abs(T a)
	{
		return (a >= 0) ? a : -a;
	}
};

#endif // #ifndef __SM_UTIL_H__
