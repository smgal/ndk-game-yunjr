
#include "sm_util.h"

namespace
{
	class Random
	{
	public:
		Random(unsigned long seed)
			: m_seed(seed)
		{
		}
		void setSeed(unsigned long seed)
		{
			m_seed = seed;
		}
		unsigned long generate(void)
		{
			unsigned long temp;

			m_seed = m_seed * 214013L + 2531011L;
			temp   = ((m_seed) >> 16) & 0xFFFF;

			m_seed = m_seed * 134775813L + 1L;

			return temp | (m_seed & 0xFFFF0000);
		}

	private:
		unsigned long m_seed;
	};

	Random s_random(1037);
}

void smutil::randomize(unsigned long seed)
{
	s_random.setSeed(seed);
}

int smutil::random(unsigned long max)
{
	return (max > 0) ? (s_random.generate() % max) & 0x7FFFFFFF : 0;
}

int smutil::expected(unsigned long seed, unsigned long max, unsigned long iteration)
{
	Random random(seed);

	while (iteration-- > 0)
		random.generate();

	return int(random.generate() % max);
}
