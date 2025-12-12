#pragma once

/* 
 * 동적 연결 라이브러리 dll파일로 만들도록 컴파일러에게 지시.
*/

#ifdef _WIN32
    #ifdef GNEngine_EXPORTS
        #define GNEngine_API __declspec(dllexport)
    #else
        #define GNEngine_API __declspec(dllimport)
    #endif
#else
    #define GNEngine_API
#endif

