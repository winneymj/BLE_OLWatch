
#ifndef APPCONFIG_H_
#define APPCONFIG_H_

#include <mbed.h>
#include "appconfig.h"
#include "typedefs.h"

// Could use bitfields for the bools to save a few bytes of RAM and EEPROM, but uses an extra ~82 bytes of flash
typedef struct{
	// //byte sleepMode;
	// //byte sleepBrightness;
	// uint8_t sleepTimeout;
	// //byte brightness;
	// bool invert;
#ifdef COMPILE_ANIMATIONS
	bool animations;
#endif
	// //byte clockface;
	// bool display180;
	// bool showFPS;
	// timemode_t timeMode;
	// union {
	// 	uint8_t volumes;
	// 	struct{ // get rid of these bitfields?
	// 		uint8_t volUI:2;
	// 		uint8_t volAlarm:2;
	// 		uint8_t volHour:2;
	// 		uint8_t brightness:2;
	// 		};
	// 	};
}appconfig_s;

class AppConfig {
public:
    AppConfig();
    static bool animations() { return AppConfig::_appConfig.animations; };

private:
    static appconfig_s _appConfig;
};
// void appconfig_init(void);
// void appconfig_save(void);
// void appconfig_reset(void);

#endif
