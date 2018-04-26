
#ifndef _LWMI_SYSTEM_CLASSES_H_
#define _LWMI_SYSTEM_CLASSES_H_


#include <string>
using std::wstring;
#include <vector>
using std::vector;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

namespace LWMI
{
    class LWMICoreManager;

    /// @brief 计算机系统管理者类(非操作系统概念上应该包含软硬件)
    class LComputerSystemManager
    {
    public:
        LComputerSystemManager();
        ~LComputerSystemManager();

        /// @brief 获取计算机类型
        enum LCOMPUTER_SYSTEM_TYPE
        {
            COMPUTER_SYSTEM_UNKNOWN = 0, ///< 未知
            COMPUTER_SYSTEM_DESKTOP = 1, ///< 台式机
            COMPUTER_SYSTEM_NOTE_BOOK = 2, ///< 笔记本
            COMPUTER_SYSTEM_TABLET = 3 ///< 平板电脑
        };

        /// @brief 获取计算机系统数量
        /// @return 计算机系统数量(应该为1)
        int GetComputerSystemCount();

        /// @brief 获取计算机系统型号
        /// @param[in] index 索引
        /// @param[out] model 存储型号
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemModel(IN int index, OUT wstring& model);

        /// @brief 获取计算机系统制造商
        /// @param[in] index 索引
        /// @param[out] manufacturer 存储制造商
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取计算机类型
        /// @param[in] index 索引
        /// @param[out] type 存储类型
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemPCType(IN int index, OUT LCOMPUTER_SYSTEM_TYPE& type);

        /// @brief 获取计算机系统类型
        ///
        /// 如: "x64-based PC"
        /// @param[in] index 索引
        /// @param[out] type 存储类型
        /// @return 成功返回true, 失败返回false
        bool GetComputerSystemType(IN int index, OUT wstring& type);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LComputerSystemManager(const LComputerSystemManager&);
        LComputerSystemManager& operator = (const LComputerSystemManager&);
    };

    /// @brief 操作系统管理者类
    class LOperatingSystemManager
    {
    public:
        LOperatingSystemManager();
        ~LOperatingSystemManager();

        /// @brief 获取操作系统数量
        /// @return 操作系统数量(应该为1)
        int GetOSCount();

        /// @brief 获取操作系统标题
        /// @param[in] index 索引
        /// @param[out] caption 标题
        /// @return 成功返回true, 失败返回false
        bool GetOSCaption(IN int index, OUT wstring& caption);

        /// @brief 获取操作系统架构
        /// @param[in] index 索引
        /// @param[out] architecture 操作系统架构
        /// @return 成功返回true, 失败返回false
        bool GetOSArchitecture(IN int index, OUT wstring& architecture);

        /// @brief 获取操作系统版本
        /// @param[in] index 索引
        /// @param[out] version 版本
        /// @return 成功返回true, 失败返回false
        bool GetOSVersion(IN int index, OUT wstring& version);

        /// @brief 获取操作系统系统盘符
        /// @param[in] index 索引
        /// @param[out] drive 系统盘符
        /// @return 成功返回true, 失败返回false
        bool GetOSSystemDrive(IN int index, OUT wstring& drive);

        /// @brief 获取操作系统当前日期
        /// @param[in] index 索引
        /// @param[out] time 日期, 此值表示自1601年1月1日开始的100纳秒为单位的时间
        /// @return 成功返回true, 失败返回false
        bool GetOSLocalDateTime(IN int index, OUT __int64& time);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LOperatingSystemManager(const LOperatingSystemManager&);
        LOperatingSystemManager& operator = (const LOperatingSystemManager&);
    };

    /// @brief 进程管理者类
    class LProcessManager
    {
    public:
        /// @brief 默认构造函数
        LProcessManager();

        /// @brief 构造函数
        /// @param[in] caption 进程标题
        explicit LProcessManager(IN const wstring& caption);

        /// @brief 构造函数
        /// @param[in] caption 进程ID
        explicit LProcessManager(IN unsigned long id);

        ~LProcessManager();

        /// @brief 获取进程数量
        /// @return 进程数量
        int GetProcessCount();

        /// @brief 获取进程被创建的日期
        /// @param[in] index 索引
        /// @param[out] time 日期, 此值表示自1601年1月1日开始的100纳秒为单位的时间
        /// @return 成功返回true, 失败返回false
        bool GetCreationDate(IN int index, OUT __int64& time);

        /// @brief 获取进程名称
        /// @param[in] index 索引
        /// @param[out] name 存储进程名称
        /// @return 成功返回true, 失败返回false
        bool GetName(IN int index, OUT wstring& name);

        /// @brief 获取进程用户名
        /// 非管理员权限只能获取普通用户名
        /// @param[in] index 索引
        /// @param[out] userName 存储进程用户
        /// @return 成功返回true, 失败返回false
        bool GetOwner(IN int index, OUT wstring& userName);

        /// @brief 获取进程ID
        /// @param[in] index 索引
        /// @param[out] id 存储进程ID
        /// @return 成功返回true, 失败返回false
        bool GetProcessId(IN int index, OUT unsigned long& id);

        /// @brief 获取执行文件路径
        /// @param[in] index 索引
        /// @param[out] exePath 存储执行文件路径
        /// @return 成功返回true, 失败返回false
        bool GetExecutablePath(IN int index, OUT wstring& exePath);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LProcessManager(const LProcessManager&);
        LProcessManager& operator = (const LProcessManager&);
    };

    /// @brief 系统信息管理者类
    class LMS_SystemInformationManager
    {
    public:
        LMS_SystemInformationManager();
        ~LMS_SystemInformationManager();

        /// @brief 获取系统信息数量
        /// @return 系统信息数量(应该为1)
        int GetSystemInforCount();

        /// @brief 获取主板制造商
        /// @param[in] index
        /// @param[out] manufacturer 存储制造商信息
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取主板产品名称
        /// @param[in] index
        /// @param[out] product 存储产品名称
        /// @return 成功返回true, 失败返回false
        bool GetBaseBoardProductName(IN int index, OUT wstring& product);

        /// @brief 获取BIOS发布日期
        /// @param[in] index
        /// @param[out] releaseDate 存储发布日期
        /// @return 成功返回true, 失败返回false
        bool GetBIOSReleaseDate(IN int index, OUT wstring& releaseDate);

        /// @brief 获取BIOS厂商信息
        /// @param[in] index
        /// @param[out] vendor 存储厂商信息 
        /// @return 成功返回true, 失败返回false
        bool GetBIOSVendor(IN int index, OUT wstring& vendor);

        /// @brief 获取BIOS版本
        /// @param[in] index 
        /// @param[out] version 存储BIOS版本
        /// @return 成功返回true, 失败返回false
        bool GetBIOSVersion(IN int index, OUT wstring& version);

        /// @brief 获取系统家族
        /// @param[in] index
        /// @param[out] family 存储系统家族
        /// @return 成功返回true, 失败返回false
        bool GetSystemFamily(IN int index, OUT wstring& family);

        /// @brief 获取系统制造商信息
        /// @param[in] index
        /// @param[in] manufacturer 存储制造商信息
        /// @return 成功返回true, 失败返回false
        bool GetSystemManufacturer(IN int index, OUT wstring& manufacturer);

        /// @brief 获取系统产品名称
        /// @param[in] index
        /// @param[out] productName 存储产品名称
        /// @return 成功返回true, 失败返回false
        bool GetSystemProductName(IN int index, OUT wstring& productName);

        /// @brief 获取系统SKU信息
        ///
        /// 如: ASUS-NotebookSKU
        /// @param[in] index
        /// @param[in] sku 存储SKU信息
        /// @return 成功返回true, 失败返回false
        bool GetSystemSKU(IN int index, OUT wstring& sku);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMS_SystemInformationManager(const LMS_SystemInformationManager&);
        LMS_SystemInformationManager& operator = (const LMS_SystemInformationManager&);
    };

    /// @brief SMBIOS二进制数据表管理者类
    class LMSSmBios_RawSMBiosTablesManager
    {
    public:
        LMSSmBios_RawSMBiosTablesManager();
        ~LMSSmBios_RawSMBiosTablesManager();

        /// @brief 获取SMBios数据表数目(一般为1)
        /// @return SMBios数据表数目
        int GetRawSMBiosTablesCount();

        /// @brief 获取SMBios主版本
        /// @param[in] index 索引
        /// @param[out] version 存储版本
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosMajorVersion(IN int index, OUT unsigned char& version);

        /// @brief 获取SMBios次版本
        /// @param[in] index 索引
        /// @param[out] version 存储版本
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosMinorVersion(IN int index, OUT unsigned char& version);

        /// @brief 获取SMBios数据
        /// @param[in] index 索引
        /// @param[out] data 存储数据
        /// @return 成功返回true, 失败返回false
        bool GetSMBiosData(IN int index, OUT vector<unsigned char>& data);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMSSmBios_RawSMBiosTablesManager(const LMSSmBios_RawSMBiosTablesManager&);
        LMSSmBios_RawSMBiosTablesManager& operator = (const LMSSmBios_RawSMBiosTablesManager&);
    };

    /// @brief 系统信息管理者类
    class LPerfRawData_PerfOS_MemoryManager
    {
    public:
        LPerfRawData_PerfOS_MemoryManager();
        ~LPerfRawData_PerfOS_MemoryManager();

        /// @brief 获取内存性能数据数量
        /// @return 内存性能数据数量(应该为1)
        int GetMemoryPerfDataCount();

        /// @brief 获取内存可用大小
        /// @param[in] index 索引
        /// @param[out] availableBytes 可用内存大小, 单位M
        /// @return 成功返回true, 失败返回false
        bool GetMemoryAvailableMBytes(IN int index, OUT unsigned long& availableBytes);

        /// @brief 获取内存未使用大小
        /// @param[in] index 索引
        /// @param[out] unusedBytes 可用内存大小, 单位M
        /// @return 成功返回true, 失败返回false
        bool GetMemoryUnusedMBytes(IN int index, OUT unsigned long& unusedBytes);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LPerfRawData_PerfOS_MemoryManager(const LPerfRawData_PerfOS_MemoryManager&);
        LPerfRawData_PerfOS_MemoryManager& operator = (const LPerfRawData_PerfOS_MemoryManager&);
    };

    
    /// @brief 网络配置管理者类
    class LNetworkAdapterConfigManager
    {
    public:
        LNetworkAdapterConfigManager();
        explicit LNetworkAdapterConfigManager(const wstring& settingId);
        ~LNetworkAdapterConfigManager();

        int GetConfigCount();

        /// @brief 是否启用动态IP获取
        /// @param[in] index 索引
        /// @param[out] bEnabled true(启用动态IP), false(未启用动态IP)
        /// @return 成功返回true, 失败返回false
        bool IsDHCPEnabled(IN int index, OUT bool& bEnabled);

        /// @brief 获取IP地址
        /// @param[in] index 索引
        /// @param[out] ipAddressVec 存储IP地址
        /// @return 成功返回true, 失败返回false
        bool GetIPAddress(IN int index, OUT vector<wstring>& ipAddressVec);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LNetworkAdapterConfigManager(const LNetworkAdapterConfigManager&);
        LNetworkAdapterConfigManager& operator = (const LNetworkAdapterConfigManager&);
    };


    /// @brief 硬盘SMART数据管理者类
    /// 需要管理员权限
    class LMSDisk_SmartDataManager
    {
    public:
        LMSDisk_SmartDataManager();
        ~LMSDisk_SmartDataManager();

        /// @brief 获取SMART数据数目
        /// @return SMART数据数目
        int GetCount();

        /// @brief 获取硬盘实例名称
        /// @param[in] index
        /// @param[out] instanceName 存储实例名称
        /// @return 成功返回true, 失败返回false
        bool GetInstanceName(IN int index, OUT wstring& instanceName);

        /// @brief 获取Smart数据
        /// @param[in] index 索引
        /// @param[out] data 存储数据
        /// @return 成功返回true, 失败返回false
        bool GetSmartData(IN int index, OUT unsigned char data[362]);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMSDisk_SmartDataManager(const LMSDisk_SmartDataManager&);
        LMSDisk_SmartDataManager& operator = (const LMSDisk_SmartDataManager&);
    };

    /// @brief 硬盘SMART阈值管理者类
    /// 需要管理员权限
    class LMSDisk_SmartThresholdManager
    {
    public:
        LMSDisk_SmartThresholdManager();
        ~LMSDisk_SmartThresholdManager();

        /// @brief 获取SMART阈值数目
        /// @return SMART阈值数目
        int GetCount();

        /// @brief 获取硬盘实例名称
        /// @param[in] index
        /// @param[out] instanceName 存储实例名称
        /// @return 成功返回true, 失败返回false
        bool GetInstanceName(IN int index, OUT wstring& instanceName);

        /// @brief 获取阈值
        /// @param[in] index 索引
        /// @param[out] threshold 存储数据
        /// @return 成功返回true, 失败返回false
        bool GetThreshold(IN int index, OUT unsigned char threshold[362]);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LMSDisk_SmartThresholdManager(const LMSDisk_SmartThresholdManager&);
        LMSDisk_SmartThresholdManager& operator = (const LMSDisk_SmartThresholdManager&);
    };

    /// @brief 进程性能信息管理者类
    class LPerfFormattedData_ProcessManager
    {
    public:
        LPerfFormattedData_ProcessManager();
        ~LPerfFormattedData_ProcessManager();

        /// @brief 获取进程数量
        /// @return 进程数量
        int GetProcessCount();

        /// @brief 获取进程名称
        /// @param[in] index 索引
        /// @param[out] name 存储进程名称
        /// @return 成功返回true, 失败返回false
        bool GetProcessName(IN int index, OUT wstring& name);

        /// @brief 获取进程ID
        /// @param[in] index 索引
        /// @param[out] id 存储进程ID值
        /// @return 成功返回true, 失败返回false
        bool GetProcessID(IN int index, OUT unsigned long& id);

        /// @brief 获取读写数据
        /// @param[in] index 索引
        /// @param[out] ioData 存储读写数据, 单位KB/S
        /// @return 成功返回true, 失败返回false
        bool GetIODataBytesPersec(IN int index, OUT unsigned long& ioData);

        /// @brief 获取进程CPU使用率
        /// @param[in] index 索引
        /// @param[out] percent 存储CPU使用率百分比
        /// @return 成功返回true, 失败返回false
        bool GetPercentProcessorTime(IN int index, OUT unsigned long& percent);

        /// @brief 获取进程私有工作集
        /// @param[in] index 索引
        /// @param[out] workingSet 存储私有工作集, 单位KB
        /// @return 成功返回true, 失败返回false
        bool GetWorkingSetPrivate(IN int index, OUT unsigned long& workingSet);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LPerfFormattedData_ProcessManager(const LPerfFormattedData_ProcessManager&);
        LPerfFormattedData_ProcessManager& operator = (const LPerfFormattedData_ProcessManager&);
    };

    /// @brief 进程性能信息管理者类
    class LPerfRawData_ProcessManager
    {
    public:
        LPerfRawData_ProcessManager();
        ~LPerfRawData_ProcessManager();

        /// @brief 获取进程数量
        /// @return 进程数量
        int GetProcessCount();

        /// @brief 获取进程名称
        /// @param[in] index 索引
        /// @param[out] name 存储进程名称
        /// @return 成功返回true, 失败返回false
        bool GetProcessName(IN int index, OUT wstring& name);

        /// @brief 获取进程ID
        /// @param[in] index 索引
        /// @param[out] id 存储进程ID值
        /// @return 成功返回true, 失败返回false
        bool GetProcessID(IN int index, OUT unsigned long& id);

        /// @brief 获取读写数据
        /// @param[in] index 索引
        /// @param[out] ioData 存储读写数据, 单位KB/S
        /// @return 成功返回true, 失败返回false
        bool GetIODataBytesPersec(IN int index, OUT unsigned long& ioData);

        /// @brief 获取进程CPU使用时间
        /// @param[in] index 索引
        /// @param[out] time 存储CPU使用时间
        /// @return 成功返回true, 失败返回false
        bool GetPercentProcessorTime(IN int index, OUT unsigned long long& time);

        /// @brief 获取时间戳
        /// @param[in] index 索引
        /// @param[out] timeStamp 存储时间戳
        /// @return 成功返回true, 失败返回false
        bool GetTimeStamp_Sys100NS(IN int index, OUT unsigned long long& timeStamp);

        /// @brief 获取进程私有工作集
        /// @param[in] index 索引
        /// @param[out] workingSet 存储私有工作集, 单位KB
        /// @return 成功返回true, 失败返回false
        bool GetWorkingSetPrivate(IN int index, OUT unsigned long& workingSet);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LPerfRawData_ProcessManager(const LPerfRawData_ProcessManager&);
        LPerfRawData_ProcessManager& operator = (const LPerfRawData_ProcessManager&);
    };

    /// @brief 硬盘性能信息管理者类
    class LPerfFormattedData_PerfDisk
    {
    public:
        explicit LPerfFormattedData_PerfDisk(const wstring& diskName);
        ~LPerfFormattedData_PerfDisk();

        /// @brief 获取硬盘数量
        /// @return 进程数量
        int GetDiskCount();

        /// @brief 获取硬盘使用率
        /// @param[in] index 索引
        /// @param[out] percent 存储硬盘使用率百分比
        /// @return 成功返回true, 失败返回false
        bool GetPercentDiskTime(IN int index, OUT unsigned long& percent);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者

    private:
        LPerfFormattedData_PerfDisk(const LPerfFormattedData_PerfDisk&);
        LPerfFormattedData_PerfDisk& operator = (const LPerfFormattedData_PerfDisk&);
    };
	
	    /// @brief 显示器亮度管理类
    class LMonitorBrightnessManager
    {
    public:
        LMonitorBrightnessManager();
        ~LMonitorBrightnessManager();

        /// @brief 获取显示器亮度实例数量
        /// @return 显示器亮度实例数量
        int GetMonitorBrightnessCount();

        /// @brief 获取显示器亮度实例名
        /// @param[in] index 索引
        /// @param[out] instance 存储实例名
        /// @return 成功返回true, 失败返回false
        bool GetInstanceName(IN int index, OUT wstring& instanceName);

        /// @brief 获取当前亮度
        /// @param[in] index 索引
        /// @param[out] brightness 存储亮度值
        /// @return 成功返回true, 失败返回false
        bool GetCurrentBrightness(IN int index, OUT unsigned char& brightness);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LMonitorBrightnessManager(const LMonitorBrightnessManager&);
        LMonitorBrightnessManager& operator = (const LMonitorBrightnessManager&);
    };

    /// @brief 显示器亮度方法管理类
    class LMonitorBrightnessMethodsManager
    {
    public:
        LMonitorBrightnessMethodsManager();
        ~LMonitorBrightnessMethodsManager();

        /// @brief 获取显示器亮度方法实例数量
        /// @return 显示器亮度方法实例数量
        int GetMonitorBrightnessMethodsCount();

        /// @brief 获取显示器亮度实例名
        /// @param[in] index 索引
        /// @param[out] instance 存储实例名
        /// @return 成功返回true, 失败返回false
        bool GetInstanceName(IN int index, OUT wstring& instanceName);

        /// @brief 设置亮度值
        /// @param[in] index 索引
        /// @param[in] timeOut 超时时间(秒)
        /// @param[in] bright 亮度值(0~100)
        /// @return 成功返回true, 失败返回false
        bool SetBrightness(IN int index, IN unsigned int timeOut, IN unsigned char brightness);

    private:
        LWMICoreManager* m_pWMICoreManager; ///< WMI核心管理者
    private:
        LMonitorBrightnessMethodsManager(const LMonitorBrightnessMethodsManager&);
        LMonitorBrightnessMethodsManager& operator = (const LMonitorBrightnessMethodsManager&);
    };
}

#endif