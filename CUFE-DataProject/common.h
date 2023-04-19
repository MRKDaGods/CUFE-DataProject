#pragma once

#define _UTIL ::utils::
#define _UI ::ui::
#define _COLLECTION ::collections::
#define _CORE ::core::

#ifndef _STD
#define _STD ::std::
#endif


//typedefs
typedef unsigned int uint32_t;

#define APP_NAME "Process Scheduler"
#define APP_SIZE_WIDTH 150
#define APP_SIZE_HEIGHT 60

#define LOG_WIDTH 60

//required by swprintf
#define _CRT_NON_CONFORMING_SWPRINTFS

#define BOOL_TO_WSTR(b) (b ? L"TRUE" : L"FALSE")