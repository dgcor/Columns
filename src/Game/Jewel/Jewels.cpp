#include "Jewels.h"
#include <algorithm>

void Jewels::rotate(bool reverse)
{
	if (reverse == false)
	{
		std::rotate(begin(), begin() + 1, end());
	}
	else
	{
		std::rotate(begin(), end() - 1, end());
	}
}
