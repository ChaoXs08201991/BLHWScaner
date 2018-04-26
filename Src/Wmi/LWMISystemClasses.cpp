#include "LWMISystemClasses.h"

#include "LWMICoreManager.h"

namespace LWMI
{
    LComputerSystemManager::LComputerSystemManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_ComputerSystem");
    }
    LComputerSystemManager::~LComputerSystemManager()
    {
        delete m_pWMICoreManager;
    }

    int LComputerSystemManager::GetComputerSystemCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LComputerSystemManager::GetComputerSystemModel(IN int index, OUT wstring& model)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Model", model);
    }

    bool LComputerSystemManager::GetComputerSystemManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Manufacturer", manufacturer);
    }

    bool LComputerSystemManager::GetComputerSystemPCType(IN int index, OUT LCOMPUTER_SYSTEM_TYPE& type)
    {
        type = COMPUTER_SYSTEM_UNKNOWN;

        LUINT16 u16Type;
        bool bRet = m_pWMICoreManager->GetUINT16Property(index, L"PCSystemType", u16Type);
        LUINT16 u16TypeEx;
        bool bRetEx = m_pWMICoreManager->GetUINT16Property(index, L"PCSystemTypeEx", u16TypeEx);

        if (bRet)
        {
            if (1 == u16Type)
                type = COMPUTER_SYSTEM_DESKTOP;

            if (2 == u16Type)
                type = COMPUTER_SYSTEM_NOTE_BOOK;
        }

        if (bRetEx)
        {
            if (8 == u16TypeEx)
                type = COMPUTER_SYSTEM_TABLET;
        }

        return bRet;
    }

    bool LComputerSystemManager::GetComputerSystemType(IN int index, OUT wstring& type)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemType", type);
    }

    LOperatingSystemManager::LOperatingSystemManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_OperatingSystem");
    }

    LOperatingSystemManager::~LOperatingSystemManager()
    {
        delete m_pWMICoreManager;
    }

    int LOperatingSystemManager::GetOSCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LOperatingSystemManager::GetOSCaption(IN int index, OUT wstring& caption)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Caption", caption);
    }

    bool LOperatingSystemManager::GetOSArchitecture(IN int index, OUT wstring& architecture)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"OSArchitecture", architecture);
    }

    bool LOperatingSystemManager::GetOSVersion(IN int index, OUT wstring& version)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Version", version);
    }

    bool LOperatingSystemManager::GetOSSystemDrive(IN int index, OUT wstring& drive)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemDrive", drive);
    }

    bool LOperatingSystemManager::GetOSLocalDateTime(IN int index, OUT __int64& time)
    {
        wstring strTime;
        bool bRet = m_pWMICoreManager->GetStringProperty(index, L"LocalDateTime", strTime);
        CIMDateTimeToInt64(strTime, time);
        return bRet;
    }

    LProcessManager::LProcessManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_Process");
    }

    LProcessManager::LProcessManager(IN const wstring& caption)
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);

        wstring queryStatement = L"SELECT * FROM Win32_Process ";
        queryStatement += L"WHERE Caption = \"";
        queryStatement += caption;
        queryStatement += L"\"";
        bRet = m_pWMICoreManager->WQLQuery(queryStatement.c_str());
    }

    LProcessManager::LProcessManager(IN unsigned long id)
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);

        wchar_t strID[16] = {0};
        swprintf(strID, 16, L"%u", id);

        wstring queryStatement = L"SELECT * FROM Win32_Process ";
        queryStatement += L"WHERE Handle = \"";
        queryStatement += strID;
        queryStatement += L"\"";
        bRet = m_pWMICoreManager->WQLQuery(queryStatement.c_str());
    }

    LProcessManager::~LProcessManager()
    {
        delete m_pWMICoreManager;
    }

    int LProcessManager::GetProcessCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LProcessManager::GetCreationDate(IN int index, OUT __int64& time)
    {
        wstring strTime;
        bool bRet = m_pWMICoreManager->GetStringProperty(index, L"CreationDate", strTime);
        CIMDateTimeToInt64(strTime, time);
        return bRet;
    }

    bool LProcessManager::GetName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LProcessManager::GetOwner(IN int index, OUT wstring& userName)
    {
        LWMIOutParam outParam;
        m_pWMICoreManager->ExecMethod(index, L"GetOwner", &outParam);
        return outParam.GetStringProperty(L"User", userName);
    }

    bool LProcessManager::GetProcessId(IN int index, OUT unsigned long& id)
    {
        LUINT uInt = 0;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"ProcessId", uInt);
        id = (unsigned long)uInt;
        return bRet;
    }

    bool LProcessManager::GetExecutablePath(IN int index, OUT wstring& exePath)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"ExecutablePath", exePath);
    }

    LMS_SystemInformationManager::LMS_SystemInformationManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MS_SystemInformation");
    }

    LMS_SystemInformationManager::~LMS_SystemInformationManager()
    {
        delete m_pWMICoreManager;
    }

    int LMS_SystemInformationManager::GetSystemInforCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMS_SystemInformationManager::GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BaseBoardManufacturer", manufacturer);
    }

    bool LMS_SystemInformationManager::GetBaseBoardProductName(IN int index, OUT wstring& product)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BaseBoardProduct", product);
    }

    bool LMS_SystemInformationManager::GetBIOSReleaseDate(IN int index, OUT wstring& releaseDate)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSReleaseDate", releaseDate);
    }

    bool LMS_SystemInformationManager::GetBIOSVendor(IN int index, OUT wstring& vendor)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSVendor", vendor);
    }

    bool LMS_SystemInformationManager::GetBIOSVersion(IN int index, OUT wstring& version)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"BIOSVersion", version);
    }

    bool LMS_SystemInformationManager::GetSystemFamily(IN int index, OUT wstring& family)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemFamily", family);
    }

    bool LMS_SystemInformationManager::GetSystemManufacturer(IN int index, OUT wstring& manufacturer)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemManufacturer", manufacturer);
    }

    bool LMS_SystemInformationManager::GetSystemProductName(IN int index, OUT wstring& productName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemProductName", productName);
    }

    bool LMS_SystemInformationManager::GetSystemSKU(IN int index, OUT wstring& sku)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"SystemSKU", sku);
    }

    LMSSmBios_RawSMBiosTablesManager::LMSSmBios_RawSMBiosTablesManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MSSmBios_RawSMBiosTables");
    }

    LMSSmBios_RawSMBiosTablesManager::~LMSSmBios_RawSMBiosTablesManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LMSSmBios_RawSMBiosTablesManager::GetRawSMBiosTablesCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosMajorVersion(IN int index, OUT unsigned char& version)
    {
        return m_pWMICoreManager->GetUINT8Property(index, L"SmbiosMajorVersion", version);
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosMinorVersion(IN int index, OUT unsigned char& version)
    {
        return m_pWMICoreManager->GetUINT8Property(index, L"SmbiosMinorVersion", version);
    }

    bool LMSSmBios_RawSMBiosTablesManager::GetSMBiosData(IN int index, OUT vector<unsigned char>& data)
    {
        return m_pWMICoreManager->GetUINT8ArrayProperty(index, L"SMBiosData", data);
    }

    LPerfRawData_PerfOS_MemoryManager::LPerfRawData_PerfOS_MemoryManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_PerfRawData_PerfOS_Memory");
    }

    LPerfRawData_PerfOS_MemoryManager::~LPerfRawData_PerfOS_MemoryManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LPerfRawData_PerfOS_MemoryManager::GetMemoryPerfDataCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPerfRawData_PerfOS_MemoryManager::GetMemoryAvailableMBytes(IN int index, OUT unsigned long& availableBytes)
    {
        LUINT64 temp;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"AvailableMBytes", temp);
        if (bRet)
            availableBytes = (unsigned long)temp;

        return bRet;
    }

    bool LPerfRawData_PerfOS_MemoryManager::GetMemoryUnusedMBytes(IN int index, OUT unsigned long& unusedBytes)
    {
        LUINT64 temp;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"FreeAndZeroPageListBytes", temp);
        if (bRet)
        {
            unusedBytes = (unsigned long)(temp/1024/1024);
        }

        return bRet;
    }

    LNetworkAdapterConfigManager::LNetworkAdapterConfigManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_NetworkAdapterConfiguration");
        
    }

    LNetworkAdapterConfigManager::LNetworkAdapterConfigManager(const wstring& settingId)
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        
        wstring queryStatement = L"SELECT * FROM Win32_NetworkAdapterConfiguration ";
        queryStatement += L"WHERE SettingID = \"";
        queryStatement += settingId;
        queryStatement += L"\"";

        bRet = m_pWMICoreManager->WQLQuery(queryStatement.c_str());
    }

    int LNetworkAdapterConfigManager::GetConfigCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LNetworkAdapterConfigManager::IsDHCPEnabled(IN int index, OUT bool& bEnabled)
    {
        return m_pWMICoreManager->GetBooleanProperty(index, L"DHCPEnabled", bEnabled);
    }

    bool LNetworkAdapterConfigManager::GetIPAddress(IN int index, OUT vector<wstring>& ipAddressVec)
    {
        return m_pWMICoreManager->GetStringArrayProperty(index, L"IPAddress", ipAddressVec);
    }

    LNetworkAdapterConfigManager::~LNetworkAdapterConfigManager()
    {
        delete m_pWMICoreManager;
    }

    LMSDisk_SmartDataManager::LMSDisk_SmartDataManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MSStorageDriver_FailurePredictData");
    }

    LMSDisk_SmartDataManager::~LMSDisk_SmartDataManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LMSDisk_SmartDataManager::GetCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMSDisk_SmartDataManager::GetInstanceName(IN int index, OUT wstring& instanceName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"InstanceName", instanceName);
    }

    bool LMSDisk_SmartDataManager::GetSmartData(IN int index, OUT unsigned char data[362])
    {
        vector<unsigned char> arrayData;
        bool bRet = m_pWMICoreManager->GetUINT8ArrayProperty(index, L"VendorSpecific", arrayData);
        for (unsigned int i = 0; i < 362 && i < arrayData.size(); i++)
        {
            data[i] = arrayData[i];
        }
        return bRet;
    }

    LMSDisk_SmartThresholdManager::LMSDisk_SmartThresholdManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM MSStorageDriver_FailurePredictThresholds");
    }

    LMSDisk_SmartThresholdManager::~LMSDisk_SmartThresholdManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LMSDisk_SmartThresholdManager::GetCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMSDisk_SmartThresholdManager::GetInstanceName(IN int index, OUT wstring& instanceName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"InstanceName", instanceName);
    }

    bool LMSDisk_SmartThresholdManager::GetThreshold(IN int index, OUT unsigned char threshold[362])
    {
        vector<unsigned char> arrayData;
        bool bRet = m_pWMICoreManager->GetUINT8ArrayProperty(index, L"VendorSpecific", arrayData);
        for (unsigned int i = 0; i < 362 && i < arrayData.size(); i++)
        {
            threshold[i] = arrayData[i];
        }
        return bRet;
    }

    LPerfFormattedData_ProcessManager::LPerfFormattedData_ProcessManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_PerfFormattedData_PerfProc_Process Where Name !=\"_Total\"");
    }

    LPerfFormattedData_ProcessManager::~LPerfFormattedData_ProcessManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LPerfFormattedData_ProcessManager::GetProcessCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPerfFormattedData_ProcessManager::GetProcessName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LPerfFormattedData_ProcessManager::GetProcessID(IN int index, OUT unsigned long& id)
    {
        LUINT data;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"IDProcess", data);
        id = (unsigned long)data;
        return bRet;
    }

    bool LPerfFormattedData_ProcessManager::GetIODataBytesPersec(IN int index, OUT unsigned long& ioData)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"IODataBytesPersec", data64);

        data64 = data64 / 1024;
        ioData = (unsigned long)data64;
        return bRet;
    }

    bool LPerfFormattedData_ProcessManager::GetPercentProcessorTime(IN int index, OUT unsigned long& percent)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"PercentProcessorTime", data64);
        percent = (unsigned long)data64;
        return bRet;
    }

    bool LPerfFormattedData_ProcessManager::GetWorkingSetPrivate(IN int index, OUT unsigned long& workingSet)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"WorkingSetPrivate", data64);
        data64 = data64 / 1024;
        workingSet = (unsigned long)data64;
        return bRet;
    }

    LPerfRawData_ProcessManager::LPerfRawData_ProcessManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM Win32_PerfRawData_PerfProc_Process Where Name !=\"_Total\"");
    }

    LPerfRawData_ProcessManager::~LPerfRawData_ProcessManager()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LPerfRawData_ProcessManager::GetProcessCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPerfRawData_ProcessManager::GetProcessName(IN int index, OUT wstring& name)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"Name", name);
    }

    bool LPerfRawData_ProcessManager::GetProcessID(IN int index, OUT unsigned long& id)
    {
        LUINT data;
        bool bRet = m_pWMICoreManager->GetUINT32Property(index, L"IDProcess", data);
        id = (unsigned long)data;
        return bRet;
    }

    bool LPerfRawData_ProcessManager::GetIODataBytesPersec(IN int index, OUT unsigned long& ioData)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"IODataBytesPersec", data64);

        data64 = data64 / 1024;
        ioData = (unsigned long)data64;
        return bRet;
    }

    bool LPerfRawData_ProcessManager::GetPercentProcessorTime(IN int index, OUT unsigned long long& time)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"PercentProcessorTime", data64);
        time = data64;
        return bRet;
    }

    bool LPerfRawData_ProcessManager::GetTimeStamp_Sys100NS(IN int index, OUT unsigned long long& timeStamp)
    {
        return m_pWMICoreManager->GetUINT64Property(index, L"TimeStamp_Sys100NS", timeStamp);
    }

    bool LPerfRawData_ProcessManager::GetWorkingSetPrivate(IN int index, OUT unsigned long& workingSet)
    {
        LUINT64 data64;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"WorkingSetPrivate", data64);
        data64 = data64 / 1024;
        workingSet = (unsigned long)data64;
        return bRet;
    }

    LPerfFormattedData_PerfDisk::LPerfFormattedData_PerfDisk(const wstring& diskName)
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_CIMV2);
        wstring queryStr = L"SELECT * FROM Win32_PerfFormattedData_PerfDisk_PhysicalDisk Where Name = \"";
        queryStr += diskName;
        queryStr += L"\"";
        bRet = m_pWMICoreManager->WQLQuery(queryStr.c_str());
    }

    LPerfFormattedData_PerfDisk::~LPerfFormattedData_PerfDisk()
    {
        if (m_pWMICoreManager != 0)
        {
            delete m_pWMICoreManager;
            m_pWMICoreManager = 0;
        }
    }

    int LPerfFormattedData_PerfDisk::GetDiskCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LPerfFormattedData_PerfDisk::GetPercentDiskTime(IN int index, OUT unsigned long& percent)
    {
        LUINT64 data64 = 0;
        bool bRet = m_pWMICoreManager->GetUINT64Property(index, L"PercentDiskTime", data64);
        percent = (unsigned long)data64;

        return bRet;
    }
	
	    LMonitorBrightnessManager::LMonitorBrightnessManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM WmiMonitorBrightness");
    }

    LMonitorBrightnessManager::~LMonitorBrightnessManager()
    {
        delete m_pWMICoreManager;
    }

    int LMonitorBrightnessManager::GetMonitorBrightnessCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMonitorBrightnessManager::GetInstanceName(IN int index, OUT wstring& instanceName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"InstanceName", instanceName);
    }

    bool LMonitorBrightnessManager::GetCurrentBrightness(IN int index, OUT unsigned char& brightness)
    {
        return m_pWMICoreManager->GetUINT8Property(index, L"CurrentBrightness", brightness);
    }

    LMonitorBrightnessMethodsManager::LMonitorBrightnessMethodsManager()
    {
        m_pWMICoreManager = 0;
        m_pWMICoreManager = new LWMICoreManager();
        bool bRet = m_pWMICoreManager->BaseInit(NAMESPACE_ROOT_WMI);
        bRet = m_pWMICoreManager->WQLQuery(L"SELECT * FROM WmiMonitorBrightnessMethods");
    }

    LMonitorBrightnessMethodsManager::~LMonitorBrightnessMethodsManager()
    {
        delete m_pWMICoreManager;
    }

    int LMonitorBrightnessMethodsManager::GetMonitorBrightnessMethodsCount()
    {
        return m_pWMICoreManager->GetObjectsCount();
    }

    bool LMonitorBrightnessMethodsManager::GetInstanceName(IN int index, OUT wstring& instanceName)
    {
        return m_pWMICoreManager->GetStringProperty(index, L"InstanceName", instanceName);
    }

    bool LMonitorBrightnessMethodsManager::SetBrightness(IN int index, IN unsigned int timeOut, IN unsigned char brightness)
    {
        LWMIInParam inparam(NAMESPACE_ROOT_WMI, L"WmiMonitorBrightnessMethods", L"WmiSetBrightness");
        if (!inparam.PutLUINTProperty(L"Timeout", timeOut))
            return false;

        if (!inparam.PutLBYTEProperty(L"Brightness", brightness))
            return false;

        return m_pWMICoreManager->ExecMethod(index, L"WmiSetBrightness", &inparam);
    }
}

