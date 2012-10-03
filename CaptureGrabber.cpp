#include "CaptureGrabber.h"
#include "HUD.h"


CaptureGrabber::CaptureGrabber()
    : m_w(0), m_h(0)
{
}

CaptureGrabber::~CaptureGrabber()
{
}

HRESULT CaptureGrabber::Initialize()
{
    if(m_graphBuilder){
        return S_OK;
    }

    // filter graph
    if (FAILED(m_graphBuilder.CoCreateInstance(CLSID_FilterGraph))){
        return E_FAIL;
    }

    // output
    m_dstFilter=CreateDstFilter();
    if(!m_dstFilter){
        return E_FAIL;
    }
    if(FAILED(m_graphBuilder->AddFilter(m_dstFilter, L"Sample Grabber"))){
        return E_FAIL;
    }

    // input
    //m_srcFilter=CreateSrcFilter(L"Microsoft LifeCam Cinema");
	m_srcFilter=CreateSrcFilter(L"PS3Eye Camera");
    if(!m_srcFilter){
        return E_FAIL;
    }
    if(FAILED(m_graphBuilder->AddFilter(m_srcFilter, L"Video Input Device"))){
        return E_FAIL;
    }

    // connect input to output
    if(FAILED(m_graphBuilder->Connect(
                    GetPin(m_srcFilter, PINDIR_OUTPUT), 
                    GetPin(m_dstFilter, PINDIR_INPUT)))){
        return E_FAIL;
    }

    ShowFilters();

    // 画像サイズ
    AM_MEDIA_TYPE amt;
    if(FAILED(m_sampleGrabber->GetConnectedMediaType(&amt))){
        return E_FAIL;
    }
    VIDEOINFOHEADER *pVideoInfoHeader =
        (VIDEOINFOHEADER *)amt.pbFormat;

    m_w=pVideoInfoHeader->bmiHeader.biWidth;
    m_h=pVideoInfoHeader->bmiHeader.biHeight;
    m_buffer.resize(amt.lSampleSize);

    // 開始
    m_sampleGrabber->SetBufferSamples(TRUE);
    m_sampleGrabber->SetOneShot(FALSE);
    m_graphBuilder.QueryInterface(&m_mediaControl);
    if(!m_mediaControl){
        return E_FAIL;
    }
    if(FAILED(m_mediaControl->Run())){
        return E_FAIL;
    }

    return S_OK;
}

CComPtr<IPin> CaptureGrabber::GetPin(CComPtr<IBaseFilter> filter, PIN_DIRECTION dir)
{
    CComPtr<IPin> foundPin;
    CEnumComPtr<IEnumPins, IPin> enumPins;
    if(FAILED(filter->EnumPins(&enumPins))){
        return foundPin;
    }
    while(auto pin=enumPins.Next()){
        PIN_INFO info;
        pin->QueryPinInfo(&info);
        if (info.pFilter != NULL)
        {
            info.pFilter->Release();
        }
        if(info.dir==dir){
            foundPin=pin;
            break;
        }
    }
    return foundPin;
}

void CaptureGrabber::ShowFilters()
{
    // フィルタを列挙する
    HUD::Println(L"");
    CEnumComPtr<IEnumFilters, IBaseFilter> enumFilters;
    if(FAILED(m_graphBuilder->EnumFilters(&enumFilters))){
        return;
    }
    while(auto filter=enumFilters.Next()){
        FILTER_INFO info;
        if (FAILED(filter->QueryFilterInfo(&info)))
        {
            MessageBox(NULL, TEXT("Could not get the filter info"),
                    TEXT("Error"), MB_OK | MB_ICONERROR);
            continue;  // 次は正しく動く可能性がある。
        }
        // FILTER_INFO 構造体はフィルタ グラフ マネージャへのポインタを保持する。
        // その参照カウントは解放しなければならない。
        if (info.pGraph != NULL)
        {
            info.pGraph->Release();
        }
        HUD::Println(info.achName);
    }
}

CComPtr<IBaseFilter> CaptureGrabber::CreateDstFilter()
{

    // add sample grabber
    CComPtr<IBaseFilter> filter;
    if(FAILED(filter.CoCreateInstance(CLSID_SampleGrabber))){
        return NULL;
    }

    filter.QueryInterface(&m_sampleGrabber);
    if(!m_sampleGrabber){
        return NULL;
    }

    AM_MEDIA_TYPE amt;
    ZeroMemory(&amt, sizeof(amt));
    amt.majortype  = MEDIATYPE_Video;
    amt.subtype	   = MEDIASUBTYPE_RGB32;
    amt.formattype = FORMAT_VideoInfo; 
    m_sampleGrabber->SetMediaType(&amt);

    return filter;
}

CComPtr<IBaseFilter> CaptureGrabber::CreateSrcFilter(const std::wstring &friendlyName)
{
    // デバイスを列挙するためのCreateDevEnumを生成
    CComPtr<ICreateDevEnum> createDevEnum;
    if(FAILED(createDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum))){
        return NULL;
    }

    CEnumComPtr<IEnumMoniker, IMoniker> enumMoniker;
    createDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
    if(!enumMoniker){
        // no device
        return NULL;
    }

    CComPtr<IBaseFilter> filter;
    while(auto moniker=enumMoniker.Next()){
        CComPtr<IPropertyBag> property;
        if(FAILED(moniker->BindToStorage(
                        0,
                        0,
                        IID_IPropertyBag,
                        (void **)&property))){
            continue;
        }

        VARIANT varName;
        VariantInit(&varName);
        if(FAILED(property->Read(L"FriendlyName", &varName, 0))){
            continue;
        }
        std::wstring bstrVal(varName.bstrVal);
        VariantClear(&varName);

        if(friendlyName==bstrVal){
            HUD::Println(varName.bstrVal);
            moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter);
            break;
        }

    }

    return filter;
}

HRESULT CaptureGrabber::Update()
{
	if(m_buffer.empty()){
		return E_FAIL;
	}
    // 現在表示されている映像を静止画として取得
    long nBufferSize=m_buffer.size();
    HRESULT hr=m_sampleGrabber->GetCurrentBuffer(&nBufferSize, (long*)&m_buffer[0]);
    if(SUCCEEDED(hr) && nBufferSize==m_buffer.size()){
        return S_OK;
    }
    return E_FAIL;
}

