

#ifndef _LDISKCONTROLLER_H_
#define _LDISKCONTROLLER_H_


#include <string>
using std::string;
using std::wstring;


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef SMART_DATA_LENGTH
#define SMART_DATA_LENGTH                 362                   // SMART数据长度
#endif

#define BUS_TYPE_UNKNOWN                  0x00
#define BUS_TYPE_SCSI                     0x01
#define BUS_TYPE_ATAPI                    0x02
#define BUS_TYPE_ATA                      0x03
#define BUS_TYPE_1394                     0x04
#define BUS_TYPE_SSA                      0x05
#define BUS_TYPE_FIBRE                    0x06
#define BUS_TYPE_USB                      0x07
#define BUS_TYPE_RAID                     0x08
#define BUS_TYPE_ISCSI                    0x09
#define BUS_TYPE_SAS                      0x0A
#define BUS_TYPE_SATA                     0x0B
#define BUS_TYPE_SD                       0x0C
#define BUS_TYPE_MMC                      0x0D
#define BUS_TYPE_VIRTUAL                  0x0E
#define BUS_TYPE_FILEBACKEDVIRTUAL        0x0F
#define BUS_TYPE_SPACES                   0x10
#define BUS_TYPE_NVME                     0x11
#define BUS_TYPE_SCM                      0x12
#define BUS_TYPE_UFS                      0x13
#define BUS_TYPE_MAX                      0x14
#define BUS_TYPE_MAXRESERVED              0x7F

#ifndef NVME_TEMP_COUNT_MAX
#define NVME_TEMP_COUNT_MAX               16                    // 最大温度数量
#endif

/// @brief 存储设备属性
struct LStorageDeviceProperty
{
    bool RemovableMedia;                // 是否为可移动媒体
    unsigned char DeviceType;           // 设备类型, 由SCSI规范定义, 为0表示没有该值
    unsigned char DeviceTypeModifier;   // 设备类型修饰, 由SCSI规范定义, 为0表示没有该值
    unsigned long BusType;              // 总线类型
    string VendorId;                    // 厂商ID
    string ProductId;                   // 产品ID
    string ProductRevision;             // 产品修订
    string SerialNumber;                // 序列号
};

/// @brief 磁盘几何结构
struct LDiskGeometry 
{
    long long Cylinders;                // 柱面数量
    unsigned long TracksPerCylinder;    // 每个柱面的磁道数
    unsigned long SectorsPerTrack;      // 每个磁道的扇区数
    unsigned long BytesPerSector;       // 每个扇区的字节数
    unsigned long MediaType;            // 媒体类型
};

/// @brief NVMe温度信息结构
struct LNVMeTempInfo
{
    unsigned int TempCount;             // 温度数量
    int Temp[NVME_TEMP_COUNT_MAX];      // 存储温度值, 单位摄氏度
};

/// @brief NVMe身份数据
struct LNVMeIdentifyData
{
    unsigned short VID;                 // Vendor ID: 厂商ID
    unsigned short SSVID;               // Subsystem Vendor ID: 子系统厂商ID
    string SerialNumber;                // 序列号
    string ModelNumber;                 // 模型号
    string FirmwareRevision;            // 固件版本
};

/// @brief NVMe健康日志
struct LNVMeHealthLog
{
    bool IsAvailableSpaceLow;                   // 标识可用空间是否低于阈值
    bool IsTemperatureThreshold;                // 标识温度是否高于最高阈值或低于最低阈值
    bool IsReliabilityDegraded;                 // 标识可靠度是否降低
    bool IsReadOnly;                            // 标识是否只读
    bool IsVolatileMemoryBackupDeviceFailed;    // 标识是否易失型内存备份设备错误

    unsigned char AvailableSpare;               // 可用空间百分比
    unsigned char AvailableSpareThreshold;      // 可用空间百分比阈值

    unsigned long long DataRead;                // 数据总读取, 单位G
    unsigned long long DataWritten;             // 数据总写入, 单位G

    unsigned long long PowerCycle;              // 通电次数
    unsigned long long PowerOnHours;            // 通电时间   
    unsigned long long UnsafeShutdowns;         // 异常断电数
};

/// @brief 获取逻辑分区剩余空间
/// @param[in] logicalDrive 逻辑分区名称
/// @param[out] pFreeSpace 存储剩余空间, 单位字节
/// @return 成功返回true, 失败返回false 
bool GetLogicalDriveFreeSpace(IN const wstring& logicalDrive, unsigned long long* pFreeSpace);


class CGenStorController;

/// @brief 通用存储控制器
class LGenStorController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: 
    /// 硬盘逻辑分区: C:, D:, E:, ...
    /// 物理驱动器: PhysicalDrive0, PhysicalDrive1, ...
    /// CD-ROM, DVD/ROM: CDROM0, CDROM1, ...
    explicit LGenStorController(IN const wstring& devicePath);

    /// @brief 析构函数
    virtual ~LGenStorController();

    /// @brief 判断设备是否存在
    /// @return 存在返回true, 不存在返回false
    bool DeviceExist();

    /// @brief 重置设备路径
    /// @param[in] devicePath 设备路径
    void ResetDeviceId(IN const wstring& devicePath);

    /// @brief 获取设备属性
    /// @param[out] devicePrperty 存储设备属性
    /// @return 成功返回true, 失败返回false
    bool GetDeviceProperty(OUT LStorageDeviceProperty& devicePrperty);

protected:
    /// @brief 默认构造函数
    LGenStorController();

    CGenStorController* m_pGenStorController; // 通用存储控制器
};

/// @brief 磁盘控制器
class LDiskController : public LGenStorController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    explicit LDiskController(IN const wstring& devicePath);

    /// @brief 析构函数
    virtual ~LDiskController();

    /// @brief 获取磁盘几何信息
    /// @param[in] geometry 存储几何信息
    /// @return 成功返回true, 失败返回false
    bool GetGeometry(OUT LDiskGeometry& geometry);

protected:
    /// @brief 默认构造函数
    LDiskController();
};

/// @brief IDE(ATA)磁盘控制器
/// 使用该类需要管理员权限
class LIDEDiskController : public LDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"\\\\.\\DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    explicit LIDEDiskController(IN const wstring& devicePath);

    /// @brief 析构函数
    virtual ~LIDEDiskController();

    /// @brief 获取标定转速
    /// @return 标定转速(单位RPM), 发生错误返回0, 返回值为1表示硬盘为SSD
    unsigned long GetRotationRate();

    /// @brief 获取SATA类型
    /// @return SATA接口类型, 1(SATA1.0 1.5Gb/s), 2(SATA2.0 3.0Gb/s), 3(SATA3.0 6.0Gb/s), 0(获取失败)
    unsigned long GetSATAType();

    /// @brief 获取SMART数据
    /// @param[out] smartData[362] 存储362个字节的SMART数据
    /// @return 成功返回true, 失败返回false
    bool GetSMARTData(OUT unsigned char smartData[SMART_DATA_LENGTH]);
};


/// @brief NVMe磁盘控制器
/// 使用该类需要管理员权限
class LNVMeDiskController : public LDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"\\\\.\\DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    explicit LNVMeDiskController(IN const wstring& devicePath);

    /// @brief 析构函数
    virtual ~LNVMeDiskController();

    /// @brief 获取身份数据
    /// @param[out] identify 存储身份数据
    /// @return 成功返回true, 失败返回false
    bool GetIdentify(OUT LNVMeIdentifyData& identify);

    /// @brief 获取温度信息
    /// @param[out] tempInfo 存储温度信息
    /// @return 成功返回true, 失败返回false
    bool GetTemperature(OUT LNVMeTempInfo& tempInfo);

    /// @brief 获取健康日志
    /// @param[out] healthLog 存储健康日志
    /// @return 成功返回true, 失败返回false
    bool GetHealthLog(OUT LNVMeHealthLog& healthLog);
};

#endif