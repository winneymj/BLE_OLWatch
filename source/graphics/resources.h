#ifndef __RESOURCES_H
#define __RESOURCES_H
#include <mbed.h>

#define PROGMEM

// #define BUFFSIZE_STR_DAYS	4
// #define BUFFSIZE_STR_MONTHS	4

// #define BUFFSIZE_DATE_FORMAT		((BUFFSIZE_STR_DAYS - 1) + (BUFFSIZE_STR_MONTHS - 1) + 12)

// // Days
// // Also see BUFFSIZE_STR_DAYS
// #define STR_MON			"Mon"
// #define STR_TUE			"Tue"
// #define STR_WED			"Wed"
// #define STR_THU			"Thu"
// #define STR_FRI			"Fri"
// #define STR_SAT			"Sat"
// #define STR_SUN			"Sun"

// // Months
// // Also see BUFFSIZE_STR_MONTHS
// #define STR_JAN			"Jan"
// #define STR_FEB			"Feb"
// #define STR_MAR			"Mar"
// #define STR_APR			"Apr"
// #define STR_MAY			"May"
// #define STR_JUN			"Jun"
// #define STR_JUL			"Jul"
// #define STR_AUG			"Aug"
// #define STR_SEP			"Sep"
// #define STR_OCT			"Oct"
// #define STR_NOV			"Nov"
// #define STR_DEC			"Dec"

// const char days[7][BUFFSIZE_STR_DAYS] PROGMEM = {
// 	STR_MON,
// 	STR_TUE,
// 	STR_WED,
// 	STR_THU,
// 	STR_FRI,
// 	STR_SAT,
// 	STR_SUN
// };

// const char months[12][BUFFSIZE_STR_MONTHS] PROGMEM = {
// 	STR_JAN,
// 	STR_FEB,
// 	STR_MAR,
// 	STR_APR,
// 	STR_MAY,
// 	STR_JUN,
// 	STR_JUL,
// 	STR_AUG,
// 	STR_SEP,
// 	STR_OCT,
// 	STR_NOV,
// 	STR_DEC
// };

// String formats
#define DATE_FORMAT ("%s %02hhu %s 20%02hhu")
#define TIME_FORMAT_SMALL ("%02hhu:%02hhu%c")

#endif