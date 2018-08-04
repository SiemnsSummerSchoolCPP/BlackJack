#pragma once

#include <string>
#include <iomanip>
#include <sstream>

#define STR(stuff) (std::string() + stuff)
#define SS(stuff) (std::stringstream() << std::fixed << stuff)
