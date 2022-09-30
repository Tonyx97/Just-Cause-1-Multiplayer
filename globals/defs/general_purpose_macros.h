#pragma once

#define DEFINE_ENUM(x, type)	using x = type; \
								enum _##x : x