
#define _WIN32_DCOM

#include "LWMICoreManager.h"

#include <assert.h>
#include <stdlib.h>

#include <Windows.h>

#include <WbemIdl.h>
#include <comdef.h>
#pragma comment(lib, "WbemUuid.lib")

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")


#define LSAFE_RELEASE(p) do{if (p){p->Release(); p = 0;}}while(0)
#define LSAFE_DELARRAY(p) do{if (p){delete[] p; p = 0;}}while(0)
#define LSAFE_DELETE(p) do{if (p){delete p; p = 0;}}while(0)



namespace LWMI
{
    void CIMDateTimeToInt64(const wstring& strTime, __int64& time)
    {
        ISWbemDateTime* pDateTime = NULL;
        HRESULT hr = CoCreateInstance(
            __uuidof(SWbemDateTime), 
            0,
            CLSCTX_INPROC_SERVER, 
            IID_PPV_ARGS(&pDateTime));
        if (SUCCEEDED(hr)) 
        {
            BSTR bstr = SysAllocString(strTime.c_str());
            if (bstr) 
            {
                hr = pDateTime->put_Value(bstr);
                if (SUCCEEDED(hr)) 
                {
                    BSTR bstrFT;
                    hr = pDateTime->GetFileTime(VARIANT_FALSE, &bstrFT);
                    if (SUCCEEDED(hr)) 
                    {
                        time = _wtoi64(bstrFT);
                        SysFreeString(bstrFT);
                    }
                }
                SysFreeString(bstr);
            }
            pDateTime->Release();
        }
    }


    /// @brief COM初始化类
    /// 只支持单线程
    class LInitCom
    {
    public:
        LInitCom()
        {
            this->bSuccess = false;

            // CoInitialize调用后返回S_FALSE或S_OK后都需调用CoUninitialize
            // CoInitialize调用后返回RPC_E_CHANGE_MODE, 表明当前线程已被初始化(且和当前模式不同),不需要调用CoUninitialize
            HRESULT hr = CoInitialize(NULL);
            if (hr == S_OK || hr == S_FALSE)
            {
                this->bSuccess = true;
            }
            else
            {
                this->bSuccess = false;
            }
        }

        ~LInitCom()
        {
            if (this->bSuccess)
            {
                CoUninitialize();
            }
        }
    private:
        bool bSuccess; ///< 标识是否初始化成功
    };

    LWMIInParam::LWMIInParam(const wchar_t* pNamespace, const wchar_t* pClassName, const wchar_t* pMethodName)
    {
        m_pWbemLocator = NULL;
        m_pWbemServices = NULL;
        m_pClassObject = NULL;
        m_pInParamDef = NULL;
        m_pInParam = NULL;
        
        m_pInitComObject = new LInitCom();

        if (pNamespace == NULL || pClassName == NULL || pMethodName == NULL)
            return;

        HRESULT hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID*)&m_pWbemLocator);
        if (FAILED(hr))
            return;

        hr = m_pWbemLocator->ConnectServer(_bstr_t(pNamespace), NULL, NULL, NULL,
            0, NULL, NULL, &m_pWbemServices);
        if (FAILED(hr))
            return;

        hr = m_pWbemServices->GetObject(_bstr_t(pClassName), 0, NULL, &m_pClassObject, NULL);
        if (FAILED(hr))
            return;

        hr = m_pClassObject->GetMethod(pMethodName, 0, &m_pInParamDef, NULL);
        if (FAILED(hr))
            return;

        hr = m_pInParamDef->SpawnInstance(0, &m_pInParam);
        if (FAILED(hr))
            return;

    }

    bool LWMIInParam::PutLUINTProperty(const wchar_t* pPropertyName, LUINT uintProperty)
    {
        if (m_pInParam == NULL || pPropertyName == NULL)
            return false;

//         VARIANT var1;
//         VariantInit(&var1);
// 
//         V_VT(&var1) = VT_BSTR;
//         V_BSTR(&var1) = SysAllocString(L"0");
//         HRESULT hr = m_pInParam->Put(pPropertyName, 0, &var1, CIM_UINT32);

        VARIANT param;
        VariantInit(&param);
        param.vt = VT_I4;
        param.ullVal = uintProperty;
        HRESULT hr = m_pInParam->Put(pPropertyName, 0, &param, 0);
        VariantClear(&param);
        if (FAILED(hr))
            return false;

        return true;
    }

    bool LWMIInParam::PutLBYTEProperty(const wchar_t* pPropertyName, LBYTE byteProperty)
    {
        if (m_pInParam == NULL || pPropertyName == NULL)
            return false;

        if (m_pInParam == NULL || pPropertyName == NULL)
            return false;

        VARIANT param;
        VariantInit(&param);
        param.vt = VT_UI1;
        param.uiVal = byteProperty;
        HRESULT hr = m_pInParam->Put(pPropertyName, 0, &param, 0);
        VariantClear(&param);
        if (FAILED(hr))
            return false;

        return true;
    }

    LWMIInParam::~LWMIInParam()
    {
        LSAFE_RELEASE(m_pWbemLocator);
        LSAFE_RELEASE(m_pWbemServices);
        LSAFE_RELEASE(m_pClassObject);
        LSAFE_RELEASE(m_pInParamDef);
        LSAFE_RELEASE(m_pInParam);
        LSAFE_DELETE(m_pInitComObject);
    }

    LWMIOutParam::LWMIOutParam()
    {
        m_pOutParam = NULL;
    }

    LWMIOutParam::~LWMIOutParam()
    {
        LSAFE_RELEASE(m_pOutParam);
    }

    bool LWMIOutParam::GetStringProperty(const wchar_t* pPropertyName, wstring& strProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr = WBEM_S_NO_ERROR;

        if (m_pOutParam == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
            

        VariantInit(&vtProp);
        hr = m_pOutParam->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (vtProp.vt == VT_EMPTY || vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        strProperty.assign(vtProp.bstrVal);

    SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    LWMICoreManager::LWMICoreManager()
    {
        m_pWbemLocator = NULL;
        m_pWbemServices = NULL;
        m_pEnumObject = NULL;
        m_pObjectArray = NULL;
        m_objectCount = 0;

        m_pWbemRefresher = NULL;
        m_pWbemConfigRefresher = NULL;
        m_pRefreshAbleObjectArray = NULL;

        m_pInitComObject = new LInitCom();
    }

    LWMICoreManager::~LWMICoreManager()
    {
        this->BaseCleanUp();
        LSAFE_DELETE(m_pInitComObject);
    }

    bool LWMICoreManager::BaseInit(const wchar_t* pNamespace)
    {
        bool bRet = false;
        HRESULT hr;

        this->BaseCleanUp();

        if (pNamespace == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, 
            IID_IWbemLocator, (LPVOID*)&m_pWbemLocator);
        if (FAILED(hr)) 
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemLocator->ConnectServer(_bstr_t(pNamespace), NULL, NULL, NULL, 
            0, NULL, NULL, &m_pWbemServices);
        if (FAILED(hr)) 
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = CoSetProxyBlanket(m_pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        if (FAILED(hr)) 
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = CoCreateInstance(CLSID_WbemRefresher, NULL, CLSCTX_INPROC_SERVER,
            IID_IWbemRefresher, (LPVOID*)&m_pWbemRefresher);
        if (FAILED(hr)) 
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemRefresher->QueryInterface(IID_IWbemConfigureRefresher, (LPVOID*)&m_pWbemConfigRefresher);
        if (FAILED(hr)) 
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

SAFE_EXIT:
        if (!bRet)
        {
            this->BaseCleanUp();
        }
        return bRet;
    }

    bool LWMICoreManager::WQLQuery(const wchar_t* pQuery)
    {
        if (pQuery == NULL || m_pWbemServices == NULL)
            return false;

        for (int i = 0; i < m_objectCount; i ++)
        {
            LSAFE_RELEASE(m_pObjectArray[i]);
        }
        LSAFE_DELARRAY(m_pObjectArray);
        LSAFE_RELEASE(m_pEnumObject);
        m_objectCount = 0;

        HRESULT hr = m_pWbemServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(pQuery),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &m_pEnumObject);
        if (FAILED(hr)) return false;

        int maxSize = 5;
        m_pObjectArray = new IWbemClassObject*[maxSize];
        ZeroMemory(m_pObjectArray, maxSize * sizeof(IWbemClassObject*));

        ULONG uReturn = 0;
        while(true)
        {
            if (m_objectCount >= maxSize)
            {
                IWbemClassObject** pTempArray = new IWbemClassObject*[maxSize * 2];
                ZeroMemory(pTempArray, maxSize * 2 * sizeof(IWbemClassObject*));
                memcpy(pTempArray, m_pObjectArray, maxSize * sizeof(IWbemClassObject*));
                LSAFE_DELARRAY(m_pObjectArray);
                m_pObjectArray = pTempArray;
                pTempArray = NULL;
                maxSize = maxSize * 2;
            }
            hr = m_pEnumObject->Next(WBEM_INFINITE, 1, 
                &m_pObjectArray[m_objectCount], &uReturn);

            if(0 == uReturn)
            {
                break;
            }

            m_objectCount++;
        }

        if (m_objectCount > 0)
        {
            m_pRefreshAbleObjectArray = new IWbemClassObject*[m_objectCount];
        }

        for (int i = 0; i < m_objectCount; i++)
        {
            m_pRefreshAbleObjectArray[i] = NULL;

            long lid;
            HRESULT hr = m_pWbemConfigRefresher->AddObjectByTemplate(
                m_pWbemServices,
                m_pObjectArray[i],
                0,
                NULL,
                &(m_pRefreshAbleObjectArray[i]),
                &lid);
        }

        return true;
    }

    int LWMICoreManager::GetObjectsCount()
    {
        return m_objectCount;
    }

    bool LWMICoreManager::GetBooleanProperty(int objectIndex, const wchar_t* pPropertyName, LBOOL& boolProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if (objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (vtProp.vt == VT_EMPTY || vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        boolProperty = (vtProp.boolVal == VARIANT_TRUE) ? true : false;

    SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetStringProperty(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        strProperty.clear();

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        strProperty.assign(vtProp.bstrVal);


SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetStringPropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, wstring& strProperty)
    {
        strProperty.clear();

        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemRefresher == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemRefresher->Refresh(WBEM_FLAG_REFRESH_AUTO_RECONNECT);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pRefreshAbleObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pRefreshAbleObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        strProperty.assign(vtProp.bstrVal);


SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetStringArrayProperty(int objectIndex, const wchar_t* pPropertyName, vector<wstring>& propertyArray)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if (objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        propertyArray.clear();

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (vtProp.vt == VT_EMPTY || vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

        ULONG count = vtProp.parray->rgsabound->cElements;
        for (LONG i = 0; i < (long)count; i++)
        {
            BSTR bstr = NULL;
            SafeArrayGetElement(vtProp.parray, &i, &bstr);
            propertyArray.push_back(bstr);
        }


    SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT8Property(int objectIndex, const wchar_t* pPropertyName, LBYTE& ui8Property)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        ui8Property = vtProp.bVal;

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT8ArrayProperty(int objectIndex, const wchar_t* pPropertyName, vector<unsigned char>& arrayProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        
        arrayProperty.clear();
        arrayProperty.resize(vtProp.parray->rgsabound->cElements);
        for (long i = 0; i < (long)arrayProperty.size(); i++)
        {
            SafeArrayGetElement(vtProp.parray, &i, &arrayProperty[i]);
        }

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    

    bool LWMICoreManager::GetUINT16Property(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        ui16Property = vtProp.uiVal;

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT16PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT16& ui16Property)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemRefresher == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemRefresher->Refresh(WBEM_FLAG_REFRESH_AUTO_RECONNECT);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pRefreshAbleObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pRefreshAbleObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        ui16Property = vtProp.uiVal;

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT32Property(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        uiProperty = vtProp.uintVal;

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT32PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT& uiProperty)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemRefresher == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemRefresher->Refresh(WBEM_FLAG_REFRESH_AUTO_RECONNECT);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pRefreshAbleObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        VariantInit(&vtProp);
        hr = m_pRefreshAbleObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;
        uiProperty = vtProp.uintVal;

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT64Property(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (m_pObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        /*
        64位整数以字符串的方式保存在VARIANT中
        */
        bRet = true;
        ui64Property = _wcstoui64(vtProp.bstrVal, L'\0', 0);

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::GetUINT64PropertyRefreshed(int objectIndex, const wchar_t* pPropertyName, LUINT64& ui64Property)
    {
        bool bRet = false;
        VARIANT vtProp;
        HRESULT hr;

        if(objectIndex <0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pPropertyName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemRefresher == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        hr = m_pWbemRefresher->Refresh(WBEM_FLAG_REFRESH_AUTO_RECONNECT);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pRefreshAbleObjectArray[objectIndex] == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }


        VariantInit(&vtProp);
        hr = m_pRefreshAbleObjectArray[objectIndex]->Get(pPropertyName, 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if(vtProp.vt == VT_EMPTY||vtProp.vt == VT_NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        /*
        64位整数以字符串的方式保存在VARIANT中
        */
        bRet = true;
        ui64Property = _wcstoui64(vtProp.bstrVal, L'\0', 0);

SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::ExecMethod(
        int objectIndex, 
        const wchar_t* pMethodName,
        LWMIOutParam* pOutput)
    {
        bool bRet = false;
        HRESULT hr = WBEM_S_NO_ERROR;
        VARIANT vtProp;
        IWbemClassObject** ppOutput = NULL;

        if (objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pMethodName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemServices == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(L"__PATH", 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (pOutput == NULL)
            ppOutput = NULL;  
        else
            ppOutput = &(pOutput->m_pOutParam);
            
        
        hr = m_pWbemServices->ExecMethod(
            vtProp.bstrVal, 
            _bstr_t(pMethodName), 
            0, 
            NULL, 
            NULL, 
            ppOutput, 
            NULL);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

    SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    bool LWMICoreManager::ExecMethod(int objectIndex, const wchar_t* pMethodName, LWMIInParam* pInputParam)
    {
        bool bRet = false;
        HRESULT hr = WBEM_S_NO_ERROR;
        VARIANT vtProp;
        IWbemClassObject* pInput = NULL;

        if (objectIndex < 0 || objectIndex >= m_objectCount)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        if (pMethodName == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (m_pWbemServices == NULL)
        {
            bRet = false;
            goto SAFE_EXIT;
        }
        VariantInit(&vtProp);
        hr = m_pObjectArray[objectIndex]->Get(L"__PATH", 0, &vtProp, 0, 0);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        if (pInputParam != NULL && pInputParam->m_pInParam != NULL)
            pInput = pInputParam->m_pInParam;

        hr = m_pWbemServices->ExecMethod(
            vtProp.bstrVal,
            _bstr_t(pMethodName),
            0,
            NULL,
            pInput,
            NULL,
            NULL);
        if (hr != WBEM_S_NO_ERROR)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        bRet = true;

    SAFE_EXIT:
        VariantClear(&vtProp);
        return bRet;
    }

    void LWMICoreManager::BaseCleanUp()
    {
        for (int i = 0; i < m_objectCount; i ++)
        {
            LSAFE_RELEASE(m_pObjectArray[i]);
            LSAFE_RELEASE(m_pRefreshAbleObjectArray[i]);
        }

        m_objectCount = 0;

        LSAFE_DELARRAY(m_pObjectArray);
        LSAFE_DELARRAY(m_pRefreshAbleObjectArray);

        LSAFE_RELEASE(m_pEnumObject);
        LSAFE_RELEASE(m_pWbemServices);
        LSAFE_RELEASE(m_pWbemLocator);
        
        LSAFE_RELEASE(m_pWbemConfigRefresher);
        LSAFE_RELEASE(m_pWbemRefresher);


    }


}
