﻿
#include "SystemInfo.h"

#include <Windows.h>
#include <WinBase.h>
#include <WinNT.h>


int GetBIOSMode()
{
    FIRMWARE_TYPE firmWare;
    if (FALSE == GetFirmwareType(&firmWare))
        return BIOS_UNKNOWN;

    if (firmWare == FIRMWARE_TYPE::FirmwareTypeBios)
        return BIOS_LEGACY;

    if (firmWare == FIRMWARE_TYPE::FirmwareTypeUefi)
        return BIOS_UEFI;

    return BIOS_UNKNOWN;
}

bool IsTouchScreenSupported()
{
    int iRet = GetSystemMetrics(SM_DIGITIZER);

    if ((iRet&NID_INTEGRATED_TOUCH) == NID_INTEGRATED_TOUCH)
        return true;

    if ((iRet&NID_EXTERNAL_TOUCH) == NID_EXTERNAL_TOUCH)
        return true;

    return false;
}

