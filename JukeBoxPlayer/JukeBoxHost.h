#pragma once
#include "StdAfx.h"
#include <OleDlg.h>
#include "wmp.h"

class JukeBoxHost : public CWindowImpl<JukeBoxHost, CWindow, CWinTraits<WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE> >
{
public:
    DECLARE_WND_CLASS_EX(NULL, 0, 0)

    BEGIN_MSG_MAP(CWMPHost)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

    LRESULT OnDestroy(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& bHandled );
    LRESULT OnCreate(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& /* bHandled */);

    CAxWindow                   m_wndView;
    CComPtr<IWMPPlayer>         m_spWMPPlayer;
    DWORD                       m_dwAdviseCookie;
};
