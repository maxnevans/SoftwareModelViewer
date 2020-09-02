#pragma once
#include "pch.h"

// Defining user message type to recieve notification
// from application logic started on the other thread
class ModelViewerWindowMessage
{
public:
    static constexpr const UINT WM_MODELVIEWER = WM_USER + 0x001;
    static constexpr const WPARAM WPARAM_REDRAW = 0x1;
    static constexpr const WPARAM WPARAM_EXCEPTION = 0x2;
};