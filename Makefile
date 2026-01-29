TARGET := iphone:clang:latest:7.0

ARCHS = arm64
DEBUG = 0
FINALPACKAGE = 1
FOR_RELEASE = 1

include $(THEOS)/makefiles/common.mk

TWEAK_NAME = LastOfEarth

${TWEAK_NAME}_FRAMEWORKS = UIKit Foundation Security QuartzCore CoreGraphics CoreText AVFoundation Accelerate GLKit SystemConfiguration GameController
${TWEAK_NAME}_CFLAGS = -fobjc-arc -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-value -fvisibility=hidden -Wc++11-narrowing -Wno-narrowing -Wundefined-bool-conversion -Wreturn-stack-address -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions -w -s -Wno-error=format-security -fvisibility=hidden -Werror -fpermissive -Wall -fexceptions
${TWEAK_NAME}_CCFLAGS = -std=c++17 -fno-rtti -fno-exceptions -DNDEBUG -fvisibility=hidden -Wc++11-narrowing -Wno-narrowing -Wundefined-bool-conversion -Wreturn-stack-address -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions -w -s -Wno-error=format-security -fvisibility=hidden -Werror -fpermissive -Wall -fexceptions

${TWEAK_NAME}_FILES = Tweak.mm

include $(THEOS_MAKE_PATH)/tweak.mk