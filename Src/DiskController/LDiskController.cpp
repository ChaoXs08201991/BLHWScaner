
#include "LDiskController.h"


#include <string>

#include <Windows.h>
#include <ntddscsi.h>

// #include <nvme.h> // Win10 DDK中才包含该头文件


#ifndef NVME_MAX_LOG_SIZE
#define NVME_MAX_LOG_SIZE           4096
#endif


#pragma pack (1) // 取消内存对齐
/// @brief ATA8 ID结构
/// 共512字节(256个WORD)，这里仅定义了一些感兴趣的项(基本上依据ATA8-ACS)
/// 注意:该结构中的字符串字符两两颠倒 原因是ATA/ATAPI中的WORD，与Windows采用的字节顺序相反
struct SATA8IdentifyData
{
    USHORT GeneralConfig; // WORD 0: 基本信息字
    USHORT Obsolete1; // WORD 1: 废弃
    USHORT SpecConfig; // WORD 2: 具体配置
    USHORT Obsolete3; // WORD 3: 废弃
    USHORT Obsolete4; // WORD 4: 废弃
    USHORT Obsolete5; // WORD 5: 废弃
    USHORT Obsolete6; // WORD 6: 废弃
    USHORT CompactFlashReserved[2]; // WORD 7-8: 保留
    USHORT Obsolete9; // WORD 9: 废弃
    CHAR SerialNumber[20]; // WORD 10-19:序列号
    USHORT Obsolete20[3]; // WORD 20-22: 废弃
    CHAR FirmwareRev[8]; // WORD 23-26: 固件版本
    CHAR ModelNumber[40]; // WORD 27-46: 型号
    USHORT Reserved47; // WORD 47: 保留
    USHORT Reserved48; // WORD 48: 保留
    struct 
    {
        USHORT Obsolete0:8; // bit 0-7: 废弃
        USHORT DMASupport:1; // bit 8: 1=支持DMA
        USHORT LBASupport:1; // bit 9: 1=支持LBA
        USHORT IORDYDisabled:1; // bit 10: 1=IORDY可以被禁用
        USHORT IORDYSupport:1; // bit 11: 1=支持IORDY
        USHORT Reserved12:4; // bit 12-15: 保留
    }Capabilities; // WORD 49: 一般能力
    USHORT Reserved50; // WORD 50: 保留
    USHORT Obsolete51; // WORD 51: 废弃
    USHORT Obsolete52; // WORD 52: 废弃
    USHORT Reserved53; // WORD 53: 保留
    USHORT Obsolete54[5]; // WORD 54-58: 废弃
    USHORT Reserved59; // WORD 59: 保留
    ULONG LBATotalSectors; // WORD 60-61: LBA可寻址的扇区数
    USHORT Obsolete62; // WORD 62: 废弃
    struct 
    {
        USHORT Mode0:1; // bit 0: 1=支持模式0 (4.17Mb/s)
        USHORT Mode1:1; // bit 1: 1=支持模式1 (13.3Mb/s)
        USHORT Mode2:1; // bit 2: 1=支持模式2 (16.7Mb/s)
        USHORT Reserved5:5; // bit 3-7: 保留
        USHORT Mode0Sel:1; // bit8: 1=已选择模式0
        USHORT Mode1Sel:1; // bit9: 1=已选择模式1
        USHORT Mode2Sel:1; // bit10: 1=已选择模式2
        USHORT Reserved11:5; // bit 11-15: 保留
    } MultiWordDMA; // WORD 63: 多字节DMA支持能力
    struct 
    {
        USHORT AdvPOIModes:8; // bit 0-7: 支持高级POI模式数
        USHORT Reserved8:8; // bit 8-15: 保留
    } PIOCapacity; // WORD 64: 高级PIO支持能力
    USHORT MinMultiWordDMACycle; // WORD 65: 多字节DMA传输周期的最小值
    USHORT RecMultiWordDMACycle; // WORD 66: 多字节DMA传输周期的建议值
    USHORT MinPIONoFlowCycle; // WORD 67: 无流控制时PIO传输周期的最小值
    USHORT MinPIOFlowCycle; // WORD 68: 有流控制时PIO传输周期的最小值
    USHORT Reserved69[7]; // WORD 69-75: 保留
    struct
    {
        USHORT Reserved0:1; // bit 0: 保留
        USHORT SATAGen1:1; // bit1: 1=支持SATA Gen1(1.5Gb/s)
        USHORT SATAGen2:1; // bit2: 1=支持SATA Gen2(3.0Gb/s)
        USHORT SATAGen3:1; // bit3: 1=支持SATA Gen3(6.0Gb/s)
        USHORT Reserved4:12; // bit4-15: 保留
    }SATACapabilities; // WORD 76: SATA能力
    USHORT Reserved77; // WORD 77: 保留
    struct
    {
        USHORT Reserved0: 1; // bit0: 应该为0
        USHORT NoneZeroBufferOffsets: 1; // bit1: 1=设备支持非0缓冲偏移
        USHORT DMASetupAutoActivation: 1; // bit2:
        USHORT InitiatePowerManagement: 1; // bit3: 1=设备支持发起电源管理
        USHORT InorderDataDelivery: 1; // bit4:
        USHORT Reserved11: 11; // bit5-15: 保留
    }SATAFeaturesSupported; // WORD 78: SATA特征支持
    struct
    {
        USHORT Reserved0: 1; // bit0: 应该为0
        USHORT NoneZeroBufferOffsets: 1; // bit1: 1=非0缓冲偏移开启
        USHORT DMASetupAutoActivation: 1; // bit2:
        USHORT InitiatePowerManagement: 1; // bit3: 1=发起电源管理开启
        USHORT InorderDataDelivery: 1; // bit4:
        USHORT Reserved11: 11; // bit5-15: 保留
    }SATAFeaturesEnabled; // WORD 79: SATA特征开启
    struct 
    {
        USHORT Reserved0:1; // bit0: 保留
        USHORT Obsolete1:3; // bit1-3: 废弃
        USHORT ATA4:1; // bit4: 1=支持ATA/ATAPI-4
        USHORT ATA5:1; // bit5: 1=支持ATA/ATAPI-5
        USHORT ATA6:1; // bit6: 1=支持ATA/ATAPI-6
        USHORT ATA7:1; // bit7: 1=支持ATA/ATAPI-7
        USHORT ATA8:1; // bit8: 1=支持ATA8-ACS
        USHORT Reserved9:7; // bit9-15: 保留
    } MajorVersion; // WORD 80: 主版本
    USHORT MinorVersion; // WORD 81: 副版本
    USHORT Reserved82;// WORD 82: 保留
    struct 
    {
        USHORT Reserved0:3; // bit0-2: 保留
        USHORT AdvancedPowerManagementFeatureSetSupported:1; // bit3: 1=高级电源管理特征集支持
        USHORT Reserved4:12; // bit4-15: 保留
    }CommandSetsSupported; // WORD 83: 命令集支持
    USHORT Reserved84[2]; // WORD 84-85: 保留
    struct 
    {
        USHORT Reserved0:3; // bit0-2: 保留
        USHORT AdvancedPowerManagementFeatureSetEnabled:1; // bit3: 1=高级电源管理特征集开启
        USHORT Reserved4:12; // bit4-15: 保留
    }CommandSetFeatureEnabledSupported;  // WORD 86: 命令集或特征开启或支持
    USHORT Reserved87; // WORD 87: 保留
    struct 
    {
        USHORT Mode0:1;                // 1=支持模式0 (16.7Mb/s)
        USHORT Mode1:1;                // 1=支持模式1 (25Mb/s)
        USHORT Mode2:1;                // 1=支持模式2 (33Mb/s)
        USHORT Mode3:1;                // 1=支持模式3 (44Mb/s)
        USHORT Mode4:1;                // 1=支持模式4 (66Mb/s)
        USHORT Mode5:1;                // 1=支持模式5 (100Mb/s)
        USHORT Mode6:1;                // 1=支持模式6 (133Mb/s)
        USHORT Reserved7:1;          // 保留
        USHORT Mode0Sel:1;             // 1=已选择模式0
        USHORT Mode1Sel:1;             // 1=已选择模式1
        USHORT Mode2Sel:1;             // 1=已选择模式2
        USHORT Mode3Sel:1;             // 1=已选择模式3
        USHORT Mode4Sel:1;             // 1=已选择模式4
        USHORT Mode5Sel:1;             // 1=已选择模式5
        USHORT Mode6Sel:1;             // 1=已选择模式6
        USHORT Reserved15:1;          // 保留
    } UltraDMA; // WORD 88:  Ultra DMA支持能力
    USHORT Reserved89[2];         // WORD 89-90: 保留
    struct
    {
        BYTE LevelValue; // 高级电源管理级别值
        BYTE Reserved;
    }AdvancePowerManagementLevel; // WORD 91: 高级电源管理级别
    USHORT Reserved92[125];         // WORD 92-216
    USHORT NominalMediaRotationRate; //  WORD 217 标定转速(RPM), 如果值为1表示为SSD或者其他
    USHORT Reserved218[38]; // WORD 218-255 保留
};
#pragma pack () // 恢复内存对齐

//
// 本结构从Win10 DDK nvme.h文件中复制而来
//
typedef struct {
    USHORT  MP;                 // bit 0:15.    Maximum  Power (MP)

    UCHAR   Reserved0;          // bit 16:23

    UCHAR   MPS : 1;    // bit 24: Max Power Scale (MPS)
    UCHAR   NOPS : 1;    // bit 25: Non-Operational State (NOPS)
    UCHAR   Reserved1 : 6;    // bit 26:31

    ULONG   ENLAT;              // bit 32:63.   Entry Latency (ENLAT)
    ULONG   EXLAT;              // bit 64:95.   Exit Latency (EXLAT)

    UCHAR   RRT : 5;    // bit 96:100.  Relative Read Throughput (RRT)
    UCHAR   Reserved2 : 3;    // bit 101:103

    UCHAR   RRL : 5;    // bit 104:108  Relative Read Latency (RRL)
    UCHAR   Reserved3 : 3;    // bit 109:111

    UCHAR   RWT : 5;    // bit 112:116  Relative Write Throughput (RWT)
    UCHAR   Reserved4 : 3;    // bit 117:119

    UCHAR   RWL : 5;    // bit 120:124  Relative Write Latency (RWL)
    UCHAR   Reserved5 : 3;    // bit 125:127

    USHORT  IDLP;               // bit 128:143  Idle Power (IDLP)

    UCHAR   Reserved6 : 6;    // bit 144:149
    UCHAR   IPS : 2;    // bit 150:151  Idle Power Scale (IPS)

    UCHAR   Reserved7;          // bit 152:159

    USHORT  ACTP;               // bit 160:175  Active Power (ACTP)

    UCHAR   APW : 3;    // bit 176:178  Active Power Workload (APW)
    UCHAR   Reserved8 : 3;    // bit 179:181
    UCHAR   APS : 2;    // bit 182:183  Active Power Scale (APS)


    UCHAR   Reserved9[9];       // bit 184:255.

} NVME_POWER_STATE_DESC, *PNVME_POWER_STATE_DESC;

//
// 本结构从Win10 DDK nvme.h文件中复制而来
//
typedef struct {
    //
    // byte 0 : 255, Controller Capabilities and Features
    //
    USHORT  VID;                // byte 0:1.    M - PCI Vendor ID (VID)
    USHORT  SSVID;              // byte 2:3.    M - PCI Subsystem Vendor ID (SSVID)
    UCHAR   SN[20];             // byte 4: 23.  M - Serial Number (SN)
    UCHAR   MN[40];             // byte 24:63.  M - Model Number (MN)
    UCHAR   FR[8];              // byte 64:71.  M - Firmware Revision (FR)
    UCHAR   RAB;                // byte 72.     M - Recommended Arbitration Burst (RAB)
    UCHAR   IEEE[3];            // byte 73:75.  M - IEEE OUI Identifier (IEEE). Controller Vendor code.

    struct {
        UCHAR   MultiPCIePorts : 1;
        UCHAR   MultiControllers : 1;
        UCHAR   SRIOV : 1;
        UCHAR   Reserved : 5;
    } CMIC;                     // byte 76.     O - Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC)

    UCHAR   MDTS;               // byte 77.     M - Maximum Data Transfer Size (MDTS)
    USHORT  CNTLID;             // byte 78:79.   M - Controller ID (CNTLID)
    ULONG   VER;                // byte 80:83.   M - Version (VER)
    ULONG   RTD3R;              // byte 84:87.   M - RTD3 Resume Latency (RTD3R)
    ULONG   RTD3E;              // byte 88:91.   M - RTD3 Entry Latency (RTD3E)

    struct {
        ULONG   Reserved0 : 8;
        ULONG   NamespaceAttributeChanged : 1;
        ULONG   Reserved1 : 23;
    } OAES;                     // byte 92:95.   M - Optional Asynchronous Events Supported (OAES)

    UCHAR   Reserved0[144];     // byte 96:239.
    UCHAR   ReservedForManagement[16];     // byte 240:255.  Refer to the NVMe Management Interface Specification for definition.

                                           //
                                           // byte 256 : 511, Admin Command Set Attributes
                                           //
    struct {
        USHORT  SecurityCommands : 1;
        USHORT  FormatNVM : 1;
        USHORT  FirmwareCommands : 1;
        USHORT  NamespaceCommands : 1;
        USHORT  DeviceSelfTest : 1;
        USHORT  Directives : 1;
        USHORT  Reserved : 10;
    } OACS;                     // byte 256:257. M - Optional Admin Command Support (OACS)

    UCHAR   ACL;                // byte 258.    M - Abort Command Limit (ACL)
    UCHAR   AERL;               // byte 259.    M - Asynchronous Event Request Limit (AERL)

    struct {
        UCHAR   Slot1ReadOnly : 1;
        UCHAR   SlotCount : 3;
        UCHAR   ActivationWithoutReset : 1;
        UCHAR   Reserved : 3;
    } FRMW;                     // byte 260.    M - Firmware Updates (FRMW)

    struct {
        UCHAR   SmartPagePerNamespace : 1;
        UCHAR   CommandEffectsLog : 1;
        UCHAR   Reserved : 6;
    } LPA;                      // byte 261.    M - Log Page Attributes (LPA)

    UCHAR   ELPE;               // byte 262.    M - Error Log Page Entries (ELPE)
    UCHAR   NPSS;               // byte 263.    M - Number of Power States Support (NPSS)

    struct {
        UCHAR   CommandFormatInSpec : 1;
        UCHAR   Reserved : 7;
    } AVSCC;                    // byte 264.    M - Admin Vendor Specific Command Configuration (AVSCC)

    struct {
        UCHAR   Supported : 1;
        UCHAR   Reserved : 7;
    } APSTA;                    // byte 265.     O - Autonomous Power State Transition Attributes (APSTA)

    USHORT  WCTEMP;             // byte 266:267. M - Warning Composite Temperature Threshold (WCTEMP)
    USHORT  CCTEMP;             // byte 268:269. M - Critical Composite Temperature Threshold (CCTEMP)
    USHORT  MTFA;               // byte 270:271. O - Maximum Time for Firmware Activation (MTFA)
    ULONG   HMPRE;              // byte 272:275. O - Host Memory Buffer Preferred Size (HMPRE)
    ULONG   HMMIN;              // byte 276:279. O - Host Memory Buffer Minimum Size (HMMIN)
    UCHAR   TNVMCAP[16];        // byte 280:295. O - Total NVM Capacity (TNVMCAP)
    UCHAR   UNVMCAP[16];        // byte 296:311. O - Unallocated NVM Capacity (UNVMCAP)

    struct {
        ULONG   RPMBUnitCount : 3;    // Number of RPMB Units
        ULONG   AuthenticationMethod : 3;    // Authentication Method
        ULONG   Reserved0 : 10;
        ULONG   TotalSize : 8;    // Total Size: in 128KB units.
        ULONG   AccessSize : 8;    // Access Size: in 512B units.
    } RPMBS;                    // byte 312:315. O - Replay Protected Memory Block Support (RPMBS)

    UCHAR   Reserved1[196];     // byte 316:511.

                                //
                                // byte 512 : 703, NVM Command Set Attributes
                                //
    struct {
        UCHAR   RequiredEntrySize : 4;    // The value is in bytes and is reported as a power of two (2^n).
        UCHAR   MaxEntrySize : 4;    // This value is larger than or equal to the required SQ entry size.  The value is in bytes and is reported as a power of two (2^n).
    } SQES;                     // byte 512.    M - Submission Queue Entry Size (SQES)

    struct {
        UCHAR   RequiredEntrySize : 4;    // The value is in bytes and is reported as a power of two (2^n).
        UCHAR   MaxEntrySize : 4;    // This value is larger than or equal to the required CQ entry size. The value is in bytes and is reported as a power of two (2^n).
    } CQES;                     // byte 513.    M - Completion Queue Entry Size (CQES)

    UCHAR   Reserved2[2];       // byte 514:515.

    ULONG   NN;                 // byte 516:519. M - Number of Namespaces (NN)

    struct {
        USHORT  Compare : 1;
        USHORT  WriteUncorrectable : 1;
        USHORT  DatasetManagement : 1;
        USHORT  WriteZeroes : 1;
        USHORT  FeatureField : 1;
        USHORT  Reservations : 1;

        USHORT  Reserved : 10;
    } ONCS;                     // byte 520:521. M - Optional NVM Command Support (ONCS)

    struct {
        USHORT  CompareAndWrite : 1;
        USHORT  Reserved : 15;
    } FUSES;                    // byte 522:523. M - Fused Operation Support (FUSES)

    struct {
        UCHAR   FormatApplyToAll : 1;
        UCHAR   SecureEraseApplyToAll : 1;
        UCHAR   CryptographicEraseSupported : 1;
        UCHAR   Reserved : 5;
    } FNA;                      // byte 524.     M - Format NVM Attributes (FNA)

    struct {
        UCHAR   Present : 1;
        UCHAR   Reserved : 7;
    } VWC;                      // byte 525.     M - Volatile Write Cache (VWC)

    USHORT  AWUN;               // byte 526:527. M - Atomic Write Unit Normal (AWUN)
    USHORT  AWUPF;              // byte 528:529. M - Atomic Write Unit Power Fail (AWUPF)

    struct {
        UCHAR   CommandFormatInSpec : 1;
        UCHAR   Reserved : 7;
    } NVSCC;                    // byte 530.     M - NVM Vendor Specific Command Configuration (NVSCC)

    UCHAR   Reserved3;          // byte 531.

    USHORT  ACWU;               // byte 532:533  O - Atomic Compare & Write Unit (ACWU)

    UCHAR   Reserved4[2];       // byte 534:535.

    struct {
        ULONG   SGLSupported : 1;
        ULONG   Reserved0 : 15;
        ULONG   BitBucketDescrSupported : 1;
        ULONG   ByteAlignedContiguousPhysicalBuffer : 1;
        ULONG   SGLLengthLargerThanDataLength : 1;
        ULONG   Reserved1 : 13;
    } SGLS;                     // byte 536:539. O - SGL Support (SGLS)

    UCHAR   Reserved5[164];     // byte 540:703.

                                //
                                // byte 704 : 2047, I/O Command Set Attributes
                                //
    UCHAR   Reserved6[1344];    // byte 704:2047.

                                //
                                // byte 2048 : 3071, Power State Descriptors
                                //
    NVME_POWER_STATE_DESC   PDS[32];    // byte 2048:2079. M - Power State 0 Descriptor (PSD0):  This field indicates the characteristics of power state 0
                                        // byte 2080:2111. O - Power State 1 Descriptor (PSD1):  This field indicates the characteristics of power state 1
                                        // byte 2112:2143. O - Power State 2 Descriptor (PSD1):  This field indicates the characteristics of power state 2
                                        // byte 2144:2175. O - Power State 3 Descriptor (PSD1):  This field indicates the characteristics of power state 3
                                        // byte 2176:2207. O - Power State 4 Descriptor (PSD1):  This field indicates the characteristics of power state 4
                                        // byte 2208:2239. O - Power State 5 Descriptor (PSD1):  This field indicates the characteristics of power state 5
                                        // byte 2240:2271. O - Power State 6 Descriptor (PSD1):  This field indicates the characteristics of power state 6
                                        // byte 2272:2303. O - Power State 7 Descriptor (PSD1):  This field indicates the characteristics of power state 7
                                        // byte 2304:2335. O - Power State 8 Descriptor (PSD1):  This field indicates the characteristics of power state 8
                                        // byte 2336:2367. O - Power State 9 Descriptor (PSD1):  This field indicates the characteristics of power state 9
                                        // byte 2368:2399. O - Power State 10 Descriptor (PSD1):  This field indicates the characteristics of power state 10
                                        // byte 2400:2431. O - Power State 11 Descriptor (PSD1):  This field indicates the characteristics of power state 11
                                        // byte 2432:2463. O - Power State 12 Descriptor (PSD1):  This field indicates the characteristics of power state 12
                                        // byte 2464:2495. O - Power State 13 Descriptor (PSD1):  This field indicates the characteristics of power state 13
                                        // byte 2496:2527. O - Power State 14 Descriptor (PSD1):  This field indicates the characteristics of power state 14
                                        // byte 2528:2559. O - Power State 15 Descriptor (PSD1):  This field indicates the characteristics of power state 15
                                        // byte 2560:2591. O - Power State 16 Descriptor (PSD1):  This field indicates the characteristics of power state 16
                                        // byte 2592:2623. O - Power State 17 Descriptor (PSD1):  This field indicates the characteristics of power state 17
                                        // byte 2624:2655. O - Power State 18 Descriptor (PSD1):  This field indicates the characteristics of power state 18
                                        // byte 2656:2687. O - Power State 19 Descriptor (PSD1):  This field indicates the characteristics of power state 19
                                        // byte 2688:2719. O - Power State 20 Descriptor (PSD1):  This field indicates the characteristics of power state 20
                                        // byte 2720:2751. O - Power State 21 Descriptor (PSD1):  This field indicates the characteristics of power state 21
                                        // byte 2752:2783. O - Power State 22 Descriptor (PSD1):  This field indicates the characteristics of power state 22
                                        // byte 2784:2815. O - Power State 23 Descriptor (PSD1):  This field indicates the characteristics of power state 23
                                        // byte 2816:2847. O - Power State 24 Descriptor (PSD1):  This field indicates the characteristics of power state 24
                                        // byte 2848:2879. O - Power State 25 Descriptor (PSD1):  This field indicates the characteristics of power state 25
                                        // byte 2880:2911. O - Power State 26 Descriptor (PSD1):  This field indicates the characteristics of power state 26
                                        // byte 2912:2943. O - Power State 27 Descriptor (PSD1):  This field indicates the characteristics of power state 27
                                        // byte 2944:2975. O - Power State 28 Descriptor (PSD1):  This field indicates the characteristics of power state 28
                                        // byte 2976:3007. O - Power State 29 Descriptor (PSD1):  This field indicates the characteristics of power state 29
                                        // byte 3008:3039. O - Power State 30 Descriptor (PSD1):  This field indicates the characteristics of power state 30
                                        // byte 3040:3071. O - Power State 31 Descriptor (PSD1):  This field indicates the characteristics of power state 31

                                        //
                                        // byte 3072 : 4095, Vendor Specific
                                        //
    UCHAR   VS[1024];           // byte 3072 : 4095.

} NVME_IDENTIFY_CONTROLLER_DATA, *PNVME_IDENTIFY_CONTROLLER_DATA;


//
// Information of log: NVME_LOG_PAGE_HEALTH_INFO. Size: 512 bytes
// 本结构从Win10 DDK nvme.h文件中复制而来
//
typedef struct {

    union {

        struct {
            UCHAR   AvailableSpaceLow : 1;                    // If set to 1, then the available spare space has fallen below the threshold.
            UCHAR   TemperatureThreshold : 1;                   // If set to 1, then a temperature is above an over temperature threshold or below an under temperature threshold.
            UCHAR   ReliabilityDegraded : 1;                    // If set to 1, then the device reliability has been degraded due to significant media related  errors or any internal error that degrades device reliability.
            UCHAR   ReadOnly : 1;                    // If set to 1, then the media has been placed in read only mode
            UCHAR   VolatileMemoryBackupDeviceFailed : 1;    // If set to 1, then the volatile memory backup device has failed. This field is only valid if the controller has a volatile memory backup solution.
            UCHAR   Reserved : 3;
        } DUMMYSTRUCTNAME;

        UCHAR AsUchar;

    } CriticalWarning;    // This field indicates critical warnings for the state of the  controller. Each bit corresponds to a critical warning type; multiple bits may be set.

    UCHAR   Temperature[2];                 // Temperature: Contains the temperature of the overall device (controller and NVM included) in units of Kelvin. If the temperature exceeds the temperature threshold, refer to section 5.12.1.4, then an asynchronous event completion may occur
    UCHAR   AvailableSpare;                 // Available Spare:  Contains a normalized percentage (0 to 100%) of the remaining spare capacity available
    UCHAR   AvailableSpareThreshold;        // Available Spare Threshold:  When the Available Spare falls below the threshold indicated in this field, an asynchronous event  completion may occur. The value is indicated as a normalized percentage (0 to 100%).
    UCHAR   PercentageUsed;                 // Percentage Used
    UCHAR   Reserved0[26];

    UCHAR   DataUnitRead[16];               // Data Units Read:  Contains the number of 512 byte data units the host has read from the controller; this value does not include metadata. This value is reported in thousands (i.e., a value of 1 corresponds to 1000 units of 512 bytes read)  and is rounded up.  When the LBA size is a value other than 512 bytes, the controller shall convert the amount of data read to 512 byte units. For the NVM command set, logical blocks read as part of Compare and Read operations shall be included in this value
    UCHAR   DataUnitWritten[16];            // Data Units Written: Contains the number of 512 byte data units the host has written to the controller; this value does not include metadata. This value is reported in thousands (i.e., a value of 1 corresponds to 1000 units of 512 bytes written)  and is rounded up.  When the LBA size is a value other than 512 bytes, the controller shall convert the amount of data written to 512 byte units. For the NVM command set, logical blocks written as part of Write operations shall be included in this value. Write Uncorrectable commands shall not impact this value.
    UCHAR   HostReadCommands[16];           // Host Read Commands:  Contains the number of read commands  completed by  the controller. For the NVM command set, this is the number of Compare and Read commands. 
    UCHAR   HostWrittenCommands[16];        // Host Write Commands:  Contains the number of write commands  completed by  the controller. For the NVM command set, this is the number of Write commands.
    UCHAR   ControllerBusyTime[16];         // Controller Busy Time:  Contains the amount of time the controller is busy with I/O commands. The controller is busy when there is a command outstanding to an I/O Queue (specifically, a command was issued via an I/O Submission Queue Tail doorbell write and the corresponding  completion queue entry  has not been posted yet to the associated I/O Completion Queue). This value is reported in minutes.
    UCHAR   PowerCycle[16];                 // Power Cycles: Contains the number of power cycles.
    UCHAR   PowerOnHours[16];               // Power On Hours: Contains the number of power-on hours. This does not include time that the controller was powered and in a low power state condition.
    UCHAR   UnsafeShutdowns[16];            // Unsafe Shutdowns: Contains the number of unsafe shutdowns. This count is incremented when a shutdown notification (CC.SHN) is not received prior to loss of power.
    UCHAR   MediaErrors[16];                // Media Errors:  Contains the number of occurrences where the controller detected an unrecovered data integrity error. Errors such as uncorrectable ECC, CRC checksum failure, or LBA tag mismatch are included in this field.
    UCHAR   ErrorInfoLogEntryCount[16];     // Number of Error Information Log Entries:  Contains the number of Error Information log entries over the life of the controller
    ULONG   WarningCompositeTemperatureTime;     // Warning Composite Temperature Time: Contains the amount of time in minutes that the controller is operational and the Composite Temperature is greater than or equal to the Warning Composite Temperature Threshold (WCTEMP) field and less than the Critical Composite Temperature Threshold (CCTEMP) field in the Identify Controller data structure
    ULONG   CriticalCompositeTemperatureTime;    // Critical Composite Temperature Time: Contains the amount of time in minutes that the controller is operational and the Composite Temperature is greater the Critical Composite Temperature Threshold (CCTEMP) field in the Identify Controller data structure
    USHORT  TemperatureSensor1;          // Contains the current temperature reported by temperature sensor 1.
    USHORT  TemperatureSensor2;          // Contains the current temperature reported by temperature sensor 2.
    USHORT  TemperatureSensor3;          // Contains the current temperature reported by temperature sensor 3.
    USHORT  TemperatureSensor4;          // Contains the current temperature reported by temperature sensor 4.
    USHORT  TemperatureSensor5;          // Contains the current temperature reported by temperature sensor 5.
    USHORT  TemperatureSensor6;          // Contains the current temperature reported by temperature sensor 6.
    USHORT  TemperatureSensor7;          // Contains the current temperature reported by temperature sensor 7.
    USHORT  TemperatureSensor8;          // Contains the current temperature reported by temperature sensor 8.
    UCHAR   Reserved1[296];

} NVME_HEALTH_INFO_LOG, *PNVME_HEALTH_INFO_LOG;


/// <SUMMARY>
/// 字符串裁剪
/// 去除字符串开始和结束的空白字符, 如: 空格, 换行, 回车, 制表符
/// </SUMMARY>
static string& StringTrimmed(string& str)
{
    if (str.empty())
    {
        return str;
    }

    unsigned int i = 0;
    while (isspace(str[i]) != 0)
    {
        i++;
    }
    str.erase(0, i);

    if (str.empty())
    {
        return str;
    }

    unsigned int j = str.length() - 1;
    while (isspace(str[j]) != 0)
    {
        if (j <= 0)
            break;

        j--;
    }

    str.erase(j + 1);

    return str;
}


/// @brief 通用存储控制器
///https://msdn.microsoft.com/en-us/library/windows/hardware/ff553891(v=vs.85).aspx
class CGenStorController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: 
    /// 硬盘逻辑分区: C:, D:, E:, ...
    /// 物理驱动器: PhysicalDrive0, PhysicalDrive1, ...
    /// CD-ROM, DVD/ROM: CDROM0, CDROM1, ...
    explicit CGenStorController(IN const wstring& devicePath)
        : m_devicePath(devicePath)
    {
    }

    virtual ~CGenStorController()
    {
    }

    /// @brief 判断设备是否存在
    /// @return 存在返回true, 不存在返回false
    bool DeviceExist()
    {
        HANDLE hDevice = this->OpenDeviceHandle();

        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            return false;
        }

        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;

            return true;
        }

        return true;
    }

    /// @brief 重置设备路径
    /// @param[in] devicePath 设备路径
    void ResetDeviceId(IN const wstring& devicePath)
    {
        m_devicePath = devicePath;
    }

    /// @brief 获取设备属性
    /// @param[out] devicePrperty 存储设备属性
    /// @return 成功返回true, 失败返回false
    bool GetDeviceProperty(OUT LStorageDeviceProperty& devicePrperty)
    {
        BOOL nRet = FALSE; // 系统API返回值
        bool bRet = false; // 函数返回值
        HANDLE hDevice = NULL; // 设备句柄
        ULONG nBytes; // 存储返回的字节数
        STORAGE_PROPERTY_QUERY propertyQuery; // 查询的属性
        STORAGE_DEVICE_DESCRIPTOR deviceDescriptor; // 属性描述结构
        STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = NULL; // 存储查询到的属性

                                                             // 打开设备
        hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 填充需要查询的属性
        ZeroMemory(&propertyQuery, sizeof(propertyQuery));
        propertyQuery.PropertyId = StorageDeviceProperty;
        propertyQuery.QueryType = PropertyStandardQuery;

        // 第一次查询, 获取属性需要的字节数
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &propertyQuery,
            sizeof(propertyQuery),
            &deviceDescriptor,
            sizeof(deviceDescriptor),
            &nBytes,
            NULL);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)malloc(deviceDescriptor.Size);

        // 第二次查询, 获取属性
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &propertyQuery,
            sizeof(propertyQuery),
            pDeviceDescriptor,
            deviceDescriptor.Size,
            &nBytes,
            NULL);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

        devicePrperty.BusType = pDeviceDescriptor->BusType;
        devicePrperty.DeviceType = pDeviceDescriptor->DeviceType;
        devicePrperty.DeviceTypeModifier = pDeviceDescriptor->DeviceTypeModifier;
        devicePrperty.RemovableMedia = pDeviceDescriptor->RemovableMedia == TRUE ? true : false;

        // 序列号以及其他属性需要根据偏移来取得, 如果偏移值为0, 则表示没有该值
        if (pDeviceDescriptor->SerialNumberOffset != 0)
        {
            devicePrperty.SerialNumber = (char*)pDeviceDescriptor + pDeviceDescriptor->SerialNumberOffset;
        }
        if (pDeviceDescriptor->VendorIdOffset != 0)
        {
            devicePrperty.VendorId = (char*)pDeviceDescriptor + pDeviceDescriptor->VendorIdOffset;
        }
        if (pDeviceDescriptor->ProductIdOffset != 0)
        {
            devicePrperty.ProductId = (char*)pDeviceDescriptor + pDeviceDescriptor->ProductIdOffset;
        }
        if (pDeviceDescriptor->ProductRevisionOffset != 0)
        {
            devicePrperty.ProductRevision = (char*)pDeviceDescriptor + pDeviceDescriptor->ProductRevisionOffset;
        }

    SAFE_EXIT:

        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }

        if (pDeviceDescriptor != NULL)
        {
            free(pDeviceDescriptor);
            pDeviceDescriptor = NULL;
        }

        return bRet;
    }

    /// @brief 获取媒体类型
    /// @param[in] mediaInfo 存储媒体信息
    /// @return 成功返回true, 失败返回false
    bool GetMediaType(OUT DEVICE_MEDIA_INFO& mediaInfo)
    {
        BOOL nRet = FALSE; // 系统API返回值
        bool bRet = false; // 函数返回值
        HANDLE hDevice = NULL; // 设备句柄
        ULONG nBytes = 0; // 存储返回的字节数
        ULONG requiredBytes = 0; // 存储需要的字节数

        GET_MEDIA_TYPES mediaTypes; // 媒体类型结构
        GET_MEDIA_TYPES* pMediaTypes = NULL; // 存储媒体类型

                                             // 打开设备
        hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 第一次查询需要的大小
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_GET_MEDIA_TYPES_EX,
            NULL,
            0,
            &mediaTypes,
            sizeof(mediaTypes),
            &nBytes,
            NULL);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        requiredBytes = sizeof(GET_MEDIA_TYPES) + mediaTypes.MediaInfoCount * sizeof(DEVICE_MEDIA_INFO);
        // 分配足够的空间
        pMediaTypes = (GET_MEDIA_TYPES*)malloc(requiredBytes);

        // 第二次查询, 获取类型
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_GET_MEDIA_TYPES_EX,
            NULL,
            0,
            pMediaTypes,
            requiredBytes,
            &nBytes,
            NULL);

        // 虽然可能获取到多组媒体信息, 但我们只取第一组, 以后如果有其他需求, 
        // 你可以重载本方法
        mediaInfo = *(pMediaTypes->MediaInfo);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

    SAFE_EXIT:

        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }

        if (pMediaTypes != NULL)
        {
            free(pMediaTypes);
            pMediaTypes = NULL;
        }

        return bRet;
    }

protected:
    /// @brief 打开设备句柄
    /// @return 返回设备句柄
    HANDLE OpenDeviceHandle()
    {
        HANDLE hDevice = CreateFileW(
            m_devicePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, 
            OPEN_EXISTING,
            0,
            NULL);
        return hDevice;
    }

private:
    wstring m_devicePath; ///< 设备名称

private:
    CGenStorController(const CGenStorController&);
    CGenStorController& operator = (const CGenStorController&);
};

/// @brief 磁盘控制器
///https://msdn.microsoft.com/en-us/library/windows/hardware/ff552626(v=vs.85).aspx
class CDiskController : public CGenStorController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    CDiskController(IN const wstring& devicePath)
        : CGenStorController(devicePath)
    {
    }

    /// @brief 析构函数
    virtual ~CDiskController()
    {
    }

    /// @brief 获取磁盘几何信息
    /// @param[in] geometry 存储几何信息
    /// @return 成功返回true, 失败返回false
    bool GetGeometry(OUT DISK_GEOMETRY& geometry)
    {
        HANDLE hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
            return false;

        DWORD nBytes = 0;
        BOOL nRet = DeviceIoControl(
            hDevice,
            IOCTL_DISK_GET_DRIVE_GEOMETRY,
            NULL,
            0,
            &geometry,
            sizeof(geometry),
            &nBytes,
            NULL);

        bool bRet = false;
        if (nRet == FALSE)
        {
            bRet = false;
        }
        else
        {
            bRet = true;
        }


        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }

        return bRet;
    }

    /// @brief 获取版本信息
    ///
    /// 该方法要求磁盘支持SMART
    /// @param[in] versionParams 存储几何信息
    /// @return 成功返回true, 失败返回false
    bool GetVersionInfor(OUT GETVERSIONINPARAMS& versionParams)
    {
        HANDLE hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
            return false;

        DWORD nBytes = 0;
        BOOL nRet = DeviceIoControl(
            hDevice,
            SMART_GET_VERSION,
            NULL,
            0,
            &versionParams,
            sizeof(versionParams),
            &nBytes,
            NULL);

        bool bRet = false;
        if (nRet == FALSE)
        {
            bRet = false;
        }
        else
        {
            bRet = true;
        }


        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }

        return bRet;
    }
};

/// @brief IDE(ATA)磁盘控制器
///
/// https://msdn.microsoft.com/en-us/library/windows/hardware/ff559105(v=vs.85).aspx
/// ATA8-ACS
class CIDEDiskController : public CDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"////.//DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    CIDEDiskController(IN const wstring& devicePath);

    /// @brief 析构函数
    virtual ~CIDEDiskController();

    bool ATACmdCheckMediaCardType();

    /// @brief 检测电源模式
    /// @param[out] mode 存储模式的值
    /// mode 的值如下: (查阅ATA8 SPE)
    /// 0x00 Device is in Standby mode [省电模式]
    /// 0x40 Device is in NV Cache Power Mode and spindle is spun down or spinning down
    /// 0x41 Device is in NV Cache Power Mode and spindle is spun up or spinning up
    /// 0x80 Device is in Idle mode
    /// 0xFF Device is in Active mode or Idle mode
    /// @return 成功返回true, 失败返回false
    bool ATACmdCheckPowerMode(OUT BYTE& mode);

    /// @brief 直接设置电源模式为IDLE
    /// @return 成功返回true, 失败返回false
    bool ATACmdIDLEImmediate();

    /// @brief 直接设置电源模式为Standby
    /// @return 成功返回true, 失败返回false
    bool ATACmdStandbyImmediate();

    /// @brief 睡眠命令
    /// @return 成功返回true, 失败返回false
    bool ATACmdSleep();

    /// @brief 重置命令
    /// @return 成功返回true, 失败返回false
    bool ATACmdDeviceReset();

    /// @brief 诊断命令
    /// @param[out] state 存储状态值
    /// state 的值为0x01或0x81则表示设备是好的, 否则表示设备是坏的或者没有连接
    /// @return 成功返回true, 失败返回false
    bool ATACmdExecuteDeviceDiagnostic(OUT BYTE& state);

    /// @brief 检验读扇区扩展命令(没有返回读取的数据, For LBA48)
    /// @param[in] lbaAddress 需要读取的起始扇区开始地址
    /// @param[in] sectorCount 需要读取的扇区的数量
    /// @return 成功返回true, 失败返回false
    bool ATACmdReadVerifySectorExt(IN ULONGLONG lbaAddress, IN unsigned long sectorCount);

    /// @brief 获取ID信息命令
    /// @param[out] identifyData 存储ID信息
    /// @return 成功返回true, 失败返回false
    bool ATACmdIdentifyDevice(OUT SATA8IdentifyData& identifyData);

    /// @brief 获取SMART数据命令
    ///
    /// SMART属性定义: http://en.wikipedia.org/wiki/S.M.A.R.T.
    /// @param[out] smartData 存储SMART数据
    /// @return 成功返回true, 失败返回false
    bool ATACmdSMARTReadData(OUT unsigned char smartData[SMART_DATA_LENGTH]);
private:
    /// @brief ATA命令
    enum ATA_COMMAND
    {
        ATA_DEVICE_RESET = 0x08,                // 重置命令
        ATA_READ_VERIFY_SECTOR_EXT = 0x42,      // 检验读扩展命令
        ATA_EXECUTE_DEVICE_DIAGNOSTIC = 0x90,   // 诊断命令
        ATA_CHECK_MEDIA_CARD_TYPE = 0xD1,       // 检测媒体类型命令
        ATA_STANDBY_IMMEDIATE = 0xE0,           // 立即设置电源模式为STANDBY命令
        ATA_IDELE_IMMEDIATE = 0xE1,             // 立即设置电源模式为IDELE命令
        ATA_IDENTIFY_DEVICE = 0xEC,             // 获取ID信息命令
        ATA_CHECK_POWER_MODE = 0xE5,            // 检测电源模式命令
        ATA_SLEEP = 0xE6,                       // 随眠命令
        ATA_SET_FEATURE = 0xFE                  // 设置特征命令
    };

    /// @brief ATA命令设置特征子命令
    enum SET_FEATURE_SUB_COMMAND
    {
        SET_FEATURE_ENABLE_ADVANCED_POWER_MANAGEMENT = 0x05, // 开启高级电源管理
        SET_FEATURE_DISABLE_ADVANCED_POWER_MANAGEMENT = 0x85 // 关闭高级电源管理
    };
};


CIDEDiskController::CIDEDiskController(IN const wstring& devicePath)
    : CDiskController(devicePath)
{

}

CIDEDiskController::~CIDEDiskController()
{

}

bool CIDEDiskController::ATACmdCheckMediaCardType()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    
    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_CHECK_MEDIA_CARD_TYPE; // 命令寄存器
   
    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }
   
    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdCheckPowerMode(OUT BYTE& mode)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_CHECK_POWER_MODE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 查阅ATA8 SPEC 可知, 执行命令后TaskFile中的第2个字节值为电源模式的值
    mode = pATACmd->CurrentTaskFile[1];

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdIDLEImmediate()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_IDELE_IMMEDIATE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdStandbyImmediate()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_STANDBY_IMMEDIATE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdSleep()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_SLEEP; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdDeviceReset()
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_DEVICE_RESET; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdExecuteDeviceDiagnostic(OUT BYTE& state)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_EXECUTE_DEVICE_DIAGNOSTIC; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 获取错误寄存器中的值
    // 执行命令后TaskFile中的第1个字节值为错误寄存器的值
    state = pATACmd->CurrentTaskFile[0];
    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdReadVerifySectorExt(IN ULONGLONG lbaAddress, IN unsigned long sectorCount)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数
    IDEREGS* pCurrentTaskFile = (IDEREGS*)pATACmd->CurrentTaskFile;
    IDEREGS* pPreviousTaskFile = (IDEREGS*)pATACmd->PreviousTaskFile;

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_48BIT_COMMAND; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)

    // 设置命令寄存器
    pCurrentTaskFile->bCommandReg = pPreviousTaskFile->bCommandReg = ATA_READ_VERIFY_SECTOR_EXT;

     /*	
    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |           |   \_____________________/
    |           |              |
    |           |              `------------ If L=0, Head Select.
    |           |                                   These four bits select the head number.
    |           |                                   HS0 is the least significant.
    |           |                            If L=1, HS0 through HS3 contain bit 24-27 of the LBA.
    |           `--------------------------- Drive. When DRV=0, drive 0 (master) is selected. 
    |                                               When DRV=1, drive 1 (slave) is selected.
    `--------------------------------------- LBA mode. This bit selects the mode of operation.
    When L=0, addressing is by 'CHS' mode.
    When L=1, addressing is by 'LBA' mode.
    */

    // 设置驱动器头寄存器
    pCurrentTaskFile->bDriveHeadReg = pPreviousTaskFile->bDriveHeadReg = 0xE0; // 驱动器头寄存器设置为0xE0表示使用LBA寻址方式
   

    // 设置读取扇区数目寄存器
    pCurrentTaskFile->bSectorCountReg = (BYTE)sectorCount;
    pPreviousTaskFile->bSectorCountReg = (BYTE)(sectorCount >> 8);

    // 设置起始寄存器LBA地址
    pCurrentTaskFile->bSectorNumberReg = (BYTE)lbaAddress;
    pCurrentTaskFile->bCylLowReg = (BYTE)(lbaAddress >> 8);
    pCurrentTaskFile->bCylHighReg = (BYTE)(lbaAddress >> 16);
    pPreviousTaskFile->bSectorNumberReg = (BYTE)(lbaAddress >> 24);
    pPreviousTaskFile->bCylLowReg = (BYTE)(lbaAddress >> 32);
    pPreviousTaskFile->bCylHighReg = (BYTE)(lbaAddress >> 40);

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdIdentifyDevice(OUT SATA8IdentifyData& identifyData)
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)
    pATACmd->CurrentTaskFile[6] = ATA_IDENTIFY_DEVICE; // 命令寄存器

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    memcpy(&identifyData, paramBuffer + sizeof(ATA_PASS_THROUGH_EX), DATA_BUFFER_LEN);

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

bool CIDEDiskController::ATACmdSMARTReadData(OUT unsigned char smartData[SMART_DATA_LENGTH])
{
    BOOL nRet = FALSE; // 系统API返回值
    bool bRet = false; // 函数返回值
    HANDLE hDevice = NULL; // 设备句柄
    ULONG nBytes = 0; // 存储返回的字节数

    const int DATA_BUFFER_LEN = 512; // 数据缓冲长度
    BYTE paramBuffer[sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN] = {0}; // 参数缓冲区, ATA命令+输出缓冲区
    ATA_PASS_THROUGH_EX* pATACmd = (ATA_PASS_THROUGH_EX*)paramBuffer; // ATA命令参数
    IDEREGS* pCurrentTaskFile = (IDEREGS*)pATACmd->CurrentTaskFile;

    hDevice = this->OpenDeviceHandle();
    if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
    {
        bRet = false;
        goto SAFE_EXIT;
    }


    pATACmd->Length = sizeof(ATA_PASS_THROUGH_EX);
    pATACmd->AtaFlags = ATA_FLAGS_DATA_IN; // 读取数据
    pATACmd->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX); // 数据缓冲区的偏移值
    pATACmd->DataTransferLength = DATA_BUFFER_LEN; // 数据缓冲区的长度
    pATACmd->TimeOutValue = 3; // 命令执行的超时时间(秒)

    // 设置命令寄存器
    pCurrentTaskFile->bCommandReg = SMART_CMD;

     /*	
    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    |           |   \_____________________/
    |           |              |
    |           |              `------------ If L=0, Head Select.
    |           |                                   These four bits select the head number.
    |           |                                   HS0 is the least significant.
    |           |                            If L=1, HS0 through HS3 contain bit 24-27 of the LBA.
    |           `--------------------------- Drive. When DRV=0, drive 0 (master) is selected. 
    |                                               When DRV=1, drive 1 (slave) is selected.
    `--------------------------------------- LBA mode. This bit selects the mode of operation.
    When L=0, addressing is by 'CHS' mode.
    When L=1, addressing is by 'LBA' mode.
    */

    // 设置驱动器头寄存器
    pCurrentTaskFile->bDriveHeadReg = 0xA0; // 驱动器头寄存器设置为0xA0表示使用CHS寻址方式
   

    // 设置特征寄存器
    pCurrentTaskFile->bFeaturesReg = READ_ATTRIBUTES;

    pCurrentTaskFile->bCylLowReg = SMART_CYL_LOW;
    pCurrentTaskFile->bCylHighReg = SMART_CYL_HI;

    nRet = DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH,
        pATACmd,
        sizeof(ATA_PASS_THROUGH_EX),
        paramBuffer,
        sizeof(ATA_PASS_THROUGH_EX) + DATA_BUFFER_LEN,
        &nBytes,
        NULL);

    if (nRet == FALSE)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 执行命令后TaskFile中的第7个字节值为状态寄存器的值, 检测该值可以知道命令的执行结果
    // 如果状态寄存器的值得0位为1, 表示发生了错误
    if (pATACmd->CurrentTaskFile[6] & 0x1)
    {
        bRet = false;
        goto SAFE_EXIT;
    }

    // 返回的数据中, 前362个数据为SMART属性数据
    memcpy(smartData, paramBuffer + sizeof(ATA_PASS_THROUGH_EX), SMART_DATA_LENGTH);

    bRet = true;

SAFE_EXIT:
    if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
    {
        CloseHandle(hDevice);
        hDevice = NULL;
    }

    return bRet;
}

/// @brief NVMe磁盘控制器
/// 使用该类需要管理员权限
/// https://msdn.microsoft.com/en-us/library/windows/desktop/mt718131(v=vs.85).aspx#temperature
class CNVMeDiskController : public CDiskController
{
public:
    /// @brief 构造函数
    /// @param[in] devicePath 设备路径
    /// 设备路径格式为(C语言)"\\\\.\\DeviceName"
    /// 设备名称如: PhysicalDrive0, PhysicalDrive1, ...
    explicit CNVMeDiskController(IN const wstring& devicePath)
        : CDiskController(devicePath)
    {
    }

    /// @brief 析构函数
    virtual ~CNVMeDiskController()
    {
    }

    /// @brief 获取身份数据
    /// @param[out] identify 存储身份数据
    /// @return 成功返回true, 失败返回false
    bool GetIdentify(OUT LNVMeIdentifyData& identify)
    {
        BOOL nRet = FALSE;                                          // 系统API返回值
        bool bRet = false;                                          // 函数返回值
        ULONG nBytes = 0;                                           // 存储返回的字节数
        HANDLE hDevice = NULL;                                      // 设备句柄

        ULONG bufferLength = 0;                                     // 缓冲区长度
        PVOID pBuffer = NULL;                                       // 缓冲区
        PSTORAGE_PROPERTY_QUERY pPropertyQuery = NULL;              // 属性查询
        PSTORAGE_PROTOCOL_SPECIFIC_DATA pProtocolData = NULL;       // 协议数据
        PSTORAGE_PROTOCOL_DATA_DESCRIPTOR pProtocolDataDesc = NULL; // 协议数据描述
        PNVME_IDENTIFY_CONTROLLER_DATA pIdentifyData = NULL;        // 身份数据
        char tempBuffer[64] = { 0 };                                // 临时缓冲区
        
        // 分配缓冲区
        bufferLength = FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
            sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
            NVME_MAX_LOG_SIZE;
        pBuffer = new char[bufferLength];
        ZeroMemory(pBuffer, bufferLength);

        pPropertyQuery = (PSTORAGE_PROPERTY_QUERY)pBuffer;
        pProtocolDataDesc = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)pBuffer;
        pProtocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)pPropertyQuery->AdditionalParameters;

        pPropertyQuery->PropertyId = StorageAdapterProtocolSpecificProperty;
        pPropertyQuery->QueryType = PropertyStandardQuery;

        pProtocolData->ProtocolType = ProtocolTypeNvme;
        pProtocolData->DataType = NVMeDataTypeIdentify;
        pProtocolData->ProtocolDataRequestValue = 1;
        pProtocolData->ProtocolDataRequestSubValue = 0;
        pProtocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
        pProtocolData->ProtocolDataLength = NVME_MAX_LOG_SIZE;

        // 打开设备
        hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 发送请求
        nRet = DeviceIoControl(hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            pBuffer,
            bufferLength,
            pBuffer,
            bufferLength,
            &nBytes,
            NULL);
        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        pIdentifyData = (PNVME_IDENTIFY_CONTROLLER_DATA)((PCHAR)pProtocolData + pProtocolData->ProtocolDataOffset);

        identify.VID = pIdentifyData->VID;
        identify.SSVID = pIdentifyData->SSVID;

        ZeroMemory(tempBuffer, 64);
        memcpy(tempBuffer, pIdentifyData->SN, 20);
        identify.SerialNumber = tempBuffer;
        StringTrimmed(identify.SerialNumber);

        ZeroMemory(tempBuffer, 64);
        memcpy(tempBuffer, pIdentifyData->MN, 40);
        identify.ModelNumber = tempBuffer;
        StringTrimmed(identify.ModelNumber);

        ZeroMemory(tempBuffer, 64);
        memcpy(tempBuffer, pIdentifyData->FR, 8);
        identify.FirmwareRevision = tempBuffer;
        StringTrimmed(identify.FirmwareRevision);

        bRet = true;

    SAFE_EXIT:
        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }
        if (pBuffer != NULL)
        {
            delete[] pBuffer;
            pBuffer = NULL;
        }

        return bRet;
    }

    /// @brief 获取温度信息
    /// @param[out] tempInfo 存储温度信息
    /// @return 成功返回true, 失败返回false
    bool GetTemperature(OUT LNVMeTempInfo& tempInfo)
    {
        BOOL nRet = FALSE;                                      // 系统API返回值
        bool bRet = false;                                      // 函数返回值
        HANDLE hDevice = NULL;                                  // 设备句柄
        ULONG nBytes;                                           // 存储返回的字节数
        STORAGE_PROPERTY_QUERY propertyQuery;                   // 查询的属性
        STORAGE_TEMPERATURE_DATA_DESCRIPTOR tempDesc;           // 温度描述
        STORAGE_TEMPERATURE_DATA_DESCRIPTOR* pTempDesc = NULL;  // 温度描述指针

        tempInfo.TempCount = 0;

        // 打开设备
        hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 填充需要查询的属性
        ZeroMemory(&propertyQuery, sizeof(propertyQuery));
        propertyQuery.PropertyId = StorageAdapterTemperatureProperty;
        propertyQuery.QueryType = PropertyStandardQuery;

        // 第一次查询, 获取属性需要的字节数
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &propertyQuery,
            sizeof(propertyQuery),
            &tempDesc,
            sizeof(tempDesc),
            &nBytes,
            NULL);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        pTempDesc = (STORAGE_TEMPERATURE_DATA_DESCRIPTOR*) new char[tempDesc.Size];

        // 第二次查询, 获取属性
        nRet = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &propertyQuery,
            sizeof(propertyQuery),
            pTempDesc,
            tempDesc.Size,
            &nBytes,
            NULL);

        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        tempInfo.TempCount = pTempDesc->InfoCount < NVME_TEMP_COUNT_MAX ? 
            pTempDesc->InfoCount : NVME_TEMP_COUNT_MAX;
        for (unsigned int i = 0; i < pTempDesc->InfoCount && i < NVME_TEMP_COUNT_MAX; i++)
        {
            tempInfo.Temp[i] = pTempDesc->TemperatureInfo[i].Temperature;
        }

        bRet = true;
    
    SAFE_EXIT:

        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }
        if (pTempDesc != NULL)
        {
            delete[] pTempDesc;
            pTempDesc = NULL;
        }

        return bRet;
    }

    /// @brief 获取健康日志
    /// @param[out] healthLog 存储健康日志
    /// @return 成功返回true, 失败返回false
    bool GetHealthLog(OUT LNVMeHealthLog& healthLog)
    {
        BOOL nRet = FALSE;                                          // 系统API返回值
        bool bRet = false;                                          // 函数返回值
        ULONG nBytes = 0;                                           // 存储返回的字节数
        HANDLE hDevice = NULL;                                      // 设备句柄

        ULONG bufferLength = 0;                                     // 缓冲区长度
        PVOID pBuffer = NULL;                                       // 缓冲区
        PSTORAGE_PROPERTY_QUERY pPropertyQuery = NULL;              // 属性查询
        PSTORAGE_PROTOCOL_SPECIFIC_DATA pProtocolData = NULL;       // 协议数据
        PSTORAGE_PROTOCOL_DATA_DESCRIPTOR pProtocolDataDesc = NULL; // 协议数据描述
        PNVME_HEALTH_INFO_LOG pHealthInfoLog = NULL;                // 健康信息

        // 分配缓冲区
        bufferLength = FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
            sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
            NVME_MAX_LOG_SIZE;
        pBuffer = new char[bufferLength];
        ZeroMemory(pBuffer, bufferLength);

        pPropertyQuery = (PSTORAGE_PROPERTY_QUERY)pBuffer;
        pProtocolDataDesc = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)pBuffer;
        pProtocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)pPropertyQuery->AdditionalParameters;

        pPropertyQuery->PropertyId = StorageDeviceProtocolSpecificProperty;
        pPropertyQuery->QueryType = PropertyStandardQuery;

        pProtocolData->ProtocolType = ProtocolTypeNvme;
        pProtocolData->DataType = NVMeDataTypeLogPage;
        pProtocolData->ProtocolDataRequestValue = 2;
        pProtocolData->ProtocolDataRequestSubValue = 0;
        pProtocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
        pProtocolData->ProtocolDataLength = NVME_MAX_LOG_SIZE;

        // 打开设备
        hDevice = this->OpenDeviceHandle();
        if (hDevice == INVALID_HANDLE_VALUE || hDevice == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        // 发送请求
        nRet = DeviceIoControl(hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            pBuffer,
            bufferLength,
            pBuffer,
            bufferLength,
            &nBytes,
            NULL);
        if (nRet == FALSE)
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        pHealthInfoLog = (PNVME_HEALTH_INFO_LOG)((PCHAR)pProtocolData + pProtocolData->ProtocolDataOffset);

        healthLog.IsAvailableSpaceLow = (pHealthInfoLog->CriticalWarning.AvailableSpaceLow == 1);
        healthLog.IsTemperatureThreshold = (pHealthInfoLog->CriticalWarning.TemperatureThreshold == 1);
        healthLog.IsReliabilityDegraded = (pHealthInfoLog->CriticalWarning.ReliabilityDegraded == 1);
        healthLog.IsReadOnly = (pHealthInfoLog->CriticalWarning.ReadOnly == 1);
        healthLog.IsVolatileMemoryBackupDeviceFailed =
            (pHealthInfoLog->CriticalWarning.VolatileMemoryBackupDeviceFailed == 1);

        healthLog.AvailableSpare = pHealthInfoLog->AvailableSpare;
        healthLog.AvailableSpareThreshold = pHealthInfoLog->AvailableSpareThreshold;

        memcpy(&healthLog.DataRead, pHealthInfoLog->DataUnitRead, sizeof(healthLog.DataRead));
        healthLog.DataRead *= 1000;
        healthLog.DataRead *= 512;
        healthLog.DataRead /= 1024;  // K
        healthLog.DataRead /= 1024;  // M
        healthLog.DataRead /= 1024;  // G

        memcpy(&healthLog.DataWritten, pHealthInfoLog->DataUnitWritten, sizeof(healthLog.DataWritten));
        healthLog.DataWritten *= 1000;
        healthLog.DataWritten *= 512;
        healthLog.DataWritten /= 1024;  // K
        healthLog.DataWritten /= 1024;  // M
        healthLog.DataWritten /= 1024;  // G

        memcpy(&healthLog.PowerCycle, pHealthInfoLog->PowerCycle, sizeof(healthLog.PowerCycle));
        memcpy(&healthLog.PowerOnHours, pHealthInfoLog->PowerOnHours, sizeof(healthLog.PowerOnHours));
        memcpy(&healthLog.UnsafeShutdowns, pHealthInfoLog->UnsafeShutdowns, sizeof(healthLog.UnsafeShutdowns));

        
        bRet = true;

    SAFE_EXIT:
        if (hDevice != INVALID_HANDLE_VALUE && hDevice != NULL)
        {
            CloseHandle(hDevice);
            hDevice = NULL;
        }
        if (pBuffer != NULL)
        {
            delete[] pBuffer;
            pBuffer = NULL;
        }

        return bRet;
    }
};

bool GetLogicalDriveFreeSpace(IN const wstring& logicalDrive, unsigned long long* pFreeSpace)
{
    if (pFreeSpace == 0)
        return false;

    if (logicalDrive.empty())
        return false;

    (*pFreeSpace) = 0;

    ULARGE_INTEGER freeSpace;
    BOOL iRet = GetDiskFreeSpaceExW(logicalDrive.c_str(), &freeSpace, 0, 0);
    if (iRet == FALSE)
        return false;

    (*pFreeSpace) = freeSpace.QuadPart;


    return true;
}


LGenStorController::LGenStorController(IN const wstring& devicePath)
{
    m_pGenStorController = NULL;
    m_pGenStorController = new CGenStorController(devicePath);
}

LGenStorController::LGenStorController()
{
    m_pGenStorController = NULL;
}

LGenStorController::~LGenStorController()
{
    if (m_pGenStorController != NULL)
    {
        delete m_pGenStorController;
        m_pGenStorController = NULL;
    }
}

bool LGenStorController::DeviceExist()
{
    return m_pGenStorController->DeviceExist();
}

bool LGenStorController::GetDeviceProperty(OUT LStorageDeviceProperty& devicePrperty)
{
    return m_pGenStorController->GetDeviceProperty(devicePrperty);
}

void LGenStorController::ResetDeviceId(IN const wstring& devicePath)
{
    m_pGenStorController->ResetDeviceId(devicePath);
}

LDiskController::LDiskController(IN const wstring& devicePath)
    : LGenStorController()
{
    m_pGenStorController = NULL;
    m_pGenStorController = new CDiskController(devicePath);
}

LDiskController::LDiskController()
    : LGenStorController()
{
    m_pGenStorController = NULL;
}

LDiskController::~LDiskController()
{
    if (m_pGenStorController != NULL)
    {
        delete m_pGenStorController;
        m_pGenStorController = NULL;
    }
}

bool LDiskController::GetGeometry(OUT LDiskGeometry& geometry)
{
    CDiskController* pDiskController = (CDiskController*)m_pGenStorController;

    DISK_GEOMETRY diskGemotry;
    bool bRet = pDiskController->GetGeometry(diskGemotry);

    geometry.BytesPerSector = diskGemotry.BytesPerSector;
    geometry.Cylinders = diskGemotry.Cylinders.QuadPart;
    geometry.MediaType = diskGemotry.MediaType;
    geometry.SectorsPerTrack = diskGemotry.SectorsPerTrack;
    geometry.TracksPerCylinder = diskGemotry.TracksPerCylinder;

    return bRet;
    
}


LIDEDiskController::LIDEDiskController(IN const wstring& devicePath)
    : LDiskController()
{
    m_pGenStorController = NULL;
    m_pGenStorController = new CIDEDiskController(devicePath);
}

LIDEDiskController::~LIDEDiskController()
{
    if (m_pGenStorController != NULL)
    {
        delete m_pGenStorController;
        m_pGenStorController = NULL;
    }
}

unsigned long LIDEDiskController::GetRotationRate()
{
    CIDEDiskController* pIDEDiskController = (CIDEDiskController*)m_pGenStorController;

    SATA8IdentifyData identifyData;
    bool bRet = pIDEDiskController->ATACmdIdentifyDevice(identifyData);
    if (!bRet)
    {
        return 0;
    }

    return identifyData.NominalMediaRotationRate;
}

unsigned long LIDEDiskController::GetSATAType()
{
    CIDEDiskController* pIDEDiskController = (CIDEDiskController*)m_pGenStorController;

    SATA8IdentifyData identifyData;
    bool bRet = pIDEDiskController->ATACmdIdentifyDevice(identifyData);
    if (!bRet)
    {
        return 0;
    }

    /*
    高版本的包含低版本的值所以需要从高到低判断
    */
    if (identifyData.SATACapabilities.SATAGen3)
    {
        return 3;
    }

    if (identifyData.SATACapabilities.SATAGen2)
    {
        return 2;
    }

    if (identifyData.SATACapabilities.SATAGen1)
    {
        return 1;
    }

    return 0;
}

bool LIDEDiskController::GetSMARTData(OUT unsigned char smartData[SMART_DATA_LENGTH])
{
    CIDEDiskController* pIDEDiskController = (CIDEDiskController*)m_pGenStorController;

    return pIDEDiskController->ATACmdSMARTReadData(smartData);
}

LNVMeDiskController::LNVMeDiskController(IN const wstring& devicePath)
    : LDiskController()
{
    m_pGenStorController = NULL;
    m_pGenStorController = new CNVMeDiskController(devicePath);
}

LNVMeDiskController::~LNVMeDiskController()
{
    if (m_pGenStorController != NULL)
    {
        delete m_pGenStorController;
        m_pGenStorController = NULL;
    }
}

bool LNVMeDiskController::GetIdentify(OUT LNVMeIdentifyData& identify)
{
    CNVMeDiskController* pNVMeController = (CNVMeDiskController*)m_pGenStorController;

    return pNVMeController->GetIdentify(identify);
}

bool LNVMeDiskController::GetTemperature(OUT LNVMeTempInfo& tempInfo)
{
    CNVMeDiskController* pNVMeController = (CNVMeDiskController*)m_pGenStorController;

    return pNVMeController->GetTemperature(tempInfo);
}

bool LNVMeDiskController::GetHealthLog(OUT LNVMeHealthLog& healthLog)
{
    CNVMeDiskController* pNVMeController = (CNVMeDiskController*)m_pGenStorController;

    return pNVMeController->GetHealthLog(healthLog);
}


