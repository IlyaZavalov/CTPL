#pragma once

#include <cassert>

#define CTPL_ASSERT(cond, message) assert(((message), (cond)))
#define CTPL_UNREACHABLE() CTPL_ASSERT(false, "shouldn't reach there")
