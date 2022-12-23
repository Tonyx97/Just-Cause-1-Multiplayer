#pragma once

// client-only libs

#if defined(JC_CLIENT)
#pragma comment(lib, "minhook.lib")
#pragma comment(lib, "d3dx9.lib")
#endif

// server-only libs

#if defined(JC_SERVER)
#pragma comment(lib, "librg_static.lib")
#endif

// client and server libs

#if defined(JC_CLIENT) || defined(JC_SERVER)
#pragma comment(lib, "lua544_static.lib")
#endif

// masterserver and server libs

#if defined(JC_MASTERSV) || defined(JC_SERVER)
#pragma comment(lib, "mariadbcpp.lib")
#endif

// asset converter libs

#if defined(JC_ASSET_CONV)
#pragma comment(lib, "assimp.lib")
#endif

// masterserver libs

#if defined(JC_MASTERSV)
#endif