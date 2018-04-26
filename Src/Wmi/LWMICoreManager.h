

#ifndef _LWMICOREMANAGER_H_
#define _LWMICOREMANAGER_H_



#include <string>
using std::wstring;
#include <vector>
using std::vector;

#ifndef NAMESPACE_ROOT_WMI
#define NAMESPACE_ROOT_WMI L"ROOT\\WMI"
#endif

#ifndef NAMESPACE_ROOT_CIMV2
#define NAMESPACE_ROOT_CIMV2 L"ROOT\\cimv2"
#endif


typedef unsigned char LBYTE;
typedef unsigned short LUINT16;
typedef unsigned int LUINT;
typedef unsigned __int64 LUINT64;
typedef bool LBOOL;

struct IWbemLocator;
struct IWbemServices;
struct IEnumWbemClassObject;
struct IWbemClassObject;
struct IWbemRefresher;
struct IWbemConfigureRefresher;

namespace LWMI
{
    void CIMDateTimeToInt64(const wstring& strTime, __int64& time);
    

    class LInitCom;


    /// @brief WMI方法输入参数对象
    class LWMIInParam
    {
    public:
        /// @brief 构造函数
        /// @param[in] pNamespace 需要连接的名字空间
        /// @param[in] pClassName 类名
        /// @param[in] pMethodName 方法名
        LWMIInParam(const wchar_t* pNamespace, const wchar_t* pClassName, const wchar_t* pMethodName);

        /// @brief 析构函数
        ~LWMIInParam();

        /// @brief 放置32位整数属性
        /// @param[in] pPrppertyName 属性名称
        /// @param[in] uintProperty 属性值
        /// @return 成功返回true, 失败返回false
        bool PutLUINTProperty(const wchar_t* pPropertyName, LUINT uintProperty);

        /// @brief 放置8位整数属性
        /// @param[in] pPrppertyName 属性名称
        /// @param[in] byteProperty 属性值
        /// @return 成功返回true, 失败返回false
        bool PutLBYTEProperty(const wchar_t* pPropertyName, LBYTE byteProperty);

    private:
        IWbemLocator* m_pWbemLocator;
        IWbemServices* m_pWbemServices;
        IWbemClassObject* m_pClassObject;
        IWbemClassObject* m_pInParamDef;
        IWbemClassObject* m_pInParam;

        LInitCom* m_pInitComObject;

        friend class LWMICoreManager;
    };


    /// @brief WMI方法输出参数对象
    class LWMIOutParam
    {    
    public:
        /// @brief 构造函数
        LWMIOutParam();

        /// @brief 析构函数
        ~LWMIOutParam();

        /// @brief 获取字符串属性
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] strProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetStringProperty(const wchar_t* pPropertyName, wstring& strProperty);

    private:
        IWbemClassObject* m_pOutParam; // 输出参数

        friend class LWMICoreManager;
    };


    /// @brief WMI核心对象
    class LWMICoreManager
    {
    public:
        LWMICoreManager();
        ~LWMICoreManager();

    public:
        /// @brief 初始化
        /// @param[in] pNamespace 需要连接的名字空间
        /// @return 成功返回true, 失败返回false
        bool BaseInit(const wchar_t* pNamespace);

        /// @brief WQL查询
        /// @param[in] pQuery 查询语句
        /// @return 成功返回true, 失败返回false
        bool WQLQuery(const wchar_t* pQuery);

        /// @brief 获取对象数量
        /// @return 对象的数量
        int GetObjectsCount();

        /// @brief 获取布尔属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] boolProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetBooleanProperty(int objectIndex, const wchar_t* pPropertyName, LBOOL& boolProperty);

        /// @brief 获取字符串属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] strProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetStringProperty(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty);

        /// @brief 获取字符串属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] strProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetStringPropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty);

        /// @brief 获取字符串数组属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] propertyArray 存储字符串数组属性
        /// @return 成功返回true, 失败返回false
        bool GetStringArrayProperty(int objectIndex, const wchar_t* pPropertyName, vector<wstring>& propertyArray);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui8Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT8Property(int objectIndex, const wchar_t* pPropertyName, LBYTE& ui8Property);

        /// @brief 获取字节数组属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPrppertyName 属性名称
        /// @param[out] arrayProperty 存储字节数组属性
        /// @return 成功返回true, 失败返回false
        bool GetUINT8ArrayProperty(int objectIndex, const wchar_t* pPropertyName, vector<LBYTE>& arrayProperty);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT16Property(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property);

        /// @brief 获取无符号整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT16PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property);

        /// @brief 获取无符号整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT32Property(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty);

        /// @brief 获取无符号整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] uiProperty 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT32PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty);

        /// @brief 获取无符号64位整数属性
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui64Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT64Property(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property);

        /// @brief 获取无符号64位整数属性(刷新后)
        /// @param[in] objectIndex 对象索引
        /// @param[in] pPropertyName 属性名称
        /// @param[out] ui64Property 存储属性值
        /// @return 成功返回true, 失败返回false
        bool GetUINT64PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property);

        /// @brief 执行方法
        /// @param[in] objectIndex 对象索引
        /// @param[in] pMethodName 方法名
        /// @param[out] pOutput 输出参数
        /// @return 成功返回true, 失败返回false
        bool ExecMethod(int objectIndex, const wchar_t* pMethodName, LWMIOutParam* pOutput);

        /// @brief 执行方法
        /// @param[in] objectIndex 对象索引
        /// @param[in] pMethodName 方法名
        /// @param[out] pInput 输入参数
        /// @return 成功返回true, 失败返回false
        bool ExecMethod(int objectIndex, const wchar_t* pMethodName, LWMIInParam* pInputParam);

    private:
        /// @brief 清理资源
        void BaseCleanUp();

    private:
        IWbemLocator* m_pWbemLocator;
        IWbemServices* m_pWbemServices;

        IEnumWbemClassObject* m_pEnumObject;
        IWbemClassObject** m_pObjectArray;
        int m_objectCount;

        IWbemRefresher* m_pWbemRefresher;
        IWbemConfigureRefresher* m_pWbemConfigRefresher;
        IWbemClassObject** m_pRefreshAbleObjectArray;

        LInitCom* m_pInitComObject;
    };


    

}


#endif

