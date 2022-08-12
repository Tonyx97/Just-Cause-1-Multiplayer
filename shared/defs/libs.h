#pragma once

#ifdef JC_CLIENT
#pragma comment(lib, "minhook.lib")
#pragma comment(lib, "d3dx9.lib")
#elif defined(JC_ASSET_CONV)
#ifdef JC_DBG
#pragma comment(lib, "assimp_debug.lib")
#else
#pragma comment(lib, "assimp.lib")
#endif
#endif