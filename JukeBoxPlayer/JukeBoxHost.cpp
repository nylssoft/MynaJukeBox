#include "StdAfx.h"
#include "JukeBoxHost.h"
#include "resource.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

LRESULT JukeBoxHost::OnCreate(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& /* bHandled */)
{
    AtlAxWinInit();
    CComPtr<IAxWinHostWindow>           spHost;
    HRESULT                             hr;
    RECT                                rcClient;
    m_dwAdviseCookie = 0;
    // create window
    GetClientRect(&rcClient);
    m_wndView.Create(m_hWnd, rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    if (NULL == m_wndView.m_hWnd)
        goto FAILURE;
    // load OCX in window
    hr = m_wndView.QueryHost(&spHost);
    if (hr != S_OK)
        goto FAILURE;
    hr = spHost->CreateControl(CComBSTR(L"{6BF52A52-394A-11d3-B153-00C04F79FAA6}"), m_wndView, 0);
    if (hr != S_OK)
        goto FAILURE;
    hr = m_wndView.QueryControl(&m_spWMPPlayer);
    if (hr != S_OK)
        goto FAILURE;
	// initialize media database
    return 0;
FAILURE:
    ::PostQuitMessage(0);
    return 0;
}

LRESULT JukeBoxHost::OnDestroy(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& bHandled )
{
    // close the OCX
    if (m_spWMPPlayer)
    {
        m_spWMPPlayer->close();
        m_spWMPPlayer.Release();
    }
    bHandled = FALSE;
    return 0;
}
