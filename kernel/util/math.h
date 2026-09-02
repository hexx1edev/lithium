#pragma once

#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define ALIGN_UP(x, a)   ALIGN_DOWN((x) + (a) - 1, a)