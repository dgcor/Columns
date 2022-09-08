#pragma once

#include "Jewel.h"

class Jewels : public std::vector<std::shared_ptr<Jewel>>
{
public:
	void rotate(bool reverse);
};
