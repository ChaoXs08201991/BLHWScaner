
#include "LWMINotifyEvent.h"
#include "LWMICoreManager.h"

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")


namespace LWMI
{
    class MonitorBrightEvent : public IWbemObjectSink
    {
        LONG m_lRef;
        bool bDone;
        MonitorBrightNotifyCallback m_pCallBackFunc;

    public:
        MonitorBrightEvent()
        {
            m_lRef = 0;
            m_pCallBackFunc = NULL;
        }

        ~MonitorBrightEvent()
        {
            bDone = true;
        }

        virtual ULONG STDMETHODCALLTYPE AddRef()
        {
            return InterlockedIncrement(&m_lRef);
        }

        virtual ULONG STDMETHODCALLTYPE Release()
        {
            LONG lRef = InterlockedDecrement(&m_lRef);
            if (lRef == 0)
                delete this;
            return lRef;
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
        {
            if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
            {
                *ppv = (IWbemObjectSink *) this;
                AddRef();
                return WBEM_S_NO_ERROR;
            }
            else return E_NOINTERFACE;
        }

        virtual HRESULT STDMETHODCALLTYPE Indicate(
            LONG lObjectCount,
            IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        )
        {
            HRESULT hres = S_OK;

            for (int i = 0; i < lObjectCount; i++)
            {
                VARIANT vtProp;
                VariantInit(&vtProp);
                HRESULT hr = apObjArray[i]->Get(L"Brightness", 0, &vtProp, 0, 0);
                if (hr != WBEM_S_NO_ERROR)
                {
                    VariantClear(&vtProp);
                    continue;
                }

                if (vtProp.vt == VT_EMPTY || vtProp.vt == VT_NULL)
                {
                    VariantClear(&vtProp);
                    continue;
                }
                if (m_pCallBackFunc != NULL)
                {
                    m_pCallBackFunc(vtProp.bVal);
                }
                VariantClear(&vtProp);
            }

            return WBEM_S_NO_ERROR;
        }

        virtual HRESULT STDMETHODCALLTYPE SetStatus(
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        )
        {
            if (lFlags == WBEM_STATUS_COMPLETE)
            {
            }
            else if (lFlags == WBEM_STATUS_PROGRESS)
            {
            }

            return WBEM_S_NO_ERROR;
        }

        /// @brief 设置回调函数
        void SetCallback(MonitorBrightNotifyCallback pFunc)
        {
            m_pCallBackFunc = pFunc;
        }
    };

    LMonitorBrightNotify::LMonitorBrightNotify()
    {
        m_pCoreNotify = new LWMICoreNotify();
    }

    LMonitorBrightNotify::~LMonitorBrightNotify()
    {
        m_pCoreNotify->CancelQueryAsync();
        delete m_pCoreNotify;
    }

    bool LMonitorBrightNotify::StartReceive(MonitorBrightNotifyCallback pFunc)
    {
        if (pFunc == NULL)
            return false;
        MonitorBrightEvent* pBrightEvent = new MonitorBrightEvent();
        pBrightEvent->AddRef();
        pBrightEvent->SetCallback(pFunc);

        m_pCoreNotify->BaseInit(NAMESPACE_ROOT_WMI, pBrightEvent);

        // 显示器亮度事件对象已被注册给核心通知对象, 它的引用计数会增加
        // 所以下面调用Release方法也不会真正析构对象, 核心通知对象被析构时, 显示器亮度事件对象才会真正被析构
        pBrightEvent->Release();
        pBrightEvent = NULL;

        return m_pCoreNotify->QueryAsync(L"SELECT * FROM WMIMonitorBrightnessEvent");
    }

    bool LMonitorBrightNotify::CancelReceive()
    {
        return m_pCoreNotify->CancelQueryAsync();
    }
}