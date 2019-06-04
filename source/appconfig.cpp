
#include <mbed.h>

#include "appconfig.h"
#include "typedefs.h"

appconfig_s AppConfig::_appConfig = {true};

AppConfig::AppConfig() {
	// AppConfig::_appConfig.sleepTimeout = 1;
	// AppConfig::_appConfig.invert = false;
#ifdef COMPILE_ANIMATIONS
	// AppConfig::_appConfig.animations = true;
#endif
	// AppConfig::_appConfig.display180 = true;
	// AppConfig::_appConfig.showFPS = true;
	// AppConfig::_appConfig.timeMode = TIMEMODE_24HR;
	// AppConfig::_appConfig.volumes = 255;

	// appconfig_save();

	// alarm_reset();
}
