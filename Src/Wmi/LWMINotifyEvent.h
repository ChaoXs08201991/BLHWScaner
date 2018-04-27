#pragma once

namespace LWMI
{
    typedef void(*MonitorBrightNotifyCallback)(unsigned int);

    class LWMICoreNotify;

    /// @brief 显示器亮度变化通知类
    /// 本类实例对象被析构时会自动呼叫CancelReceive
    class LMonitorBrightNotify
    {
    public:
        /// @brief 构造函数
        LMonitorBrightNotify();

        /// @brief 析构函数
        ~LMonitorBrightNotify();

        /// @brief 开始接受通知(异步)
        /// @param[in] pFunc 接受通知的回调函数
        /// @return 成功返回true, 失败返回false
        bool StartReceive(MonitorBrightNotifyCallback pFunc);

        /// @brief 取消接受通知
        /// @return 成功返回true, 失败返回false
        bool CancelReceive();

    private:
        LWMICoreNotify* m_pCoreNotify;
    };
}
