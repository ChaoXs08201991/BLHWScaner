
#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 未知模式
#ifndef BIOS_UNKNOWN
#define BIOS_UNKNOWN 0
#endif

/// @brief 传统模式
#ifndef BIOS_LEGACY
#define BIOS_LEGACY 1
#endif

/// @brief UEFI模式
#ifndef BIOS_UEFI
#define BIOS_UEFI 2
#endif


/// @brief 获取BIOS模式
/// @return 未知模式(BIOS_UNKNOWN 0), 传统模式(BIOS_LEGACY 1), UEFI模式(BIOS_UEFI 2)
int GetBIOSMode();

/// @brief 判断是否支持触摸屏
/// @return true(支持)，false(不支持)
bool IsTouchScreenSupported();


#endif