#pragma once
#include "sampapi/CNetGame.h"
#include "sampapi/CGame.h"
#include <sampapi/CChat.h>
#include "CMenu.h"
#include <string>
#include <iomanip>
#include <sstream>
#include "d3dx9.h"

namespace samp = sampapi::v037r1;

class CRObjects {
private:
    D3DXVECTOR2 CalcScreenCoords(D3DXVECTOR3 vecWorld);
    D3DXVECTOR3 GetPedCoordinates();
    double GetDistanceBetweenCoords3d(D3DXVECTOR3 _f, D3DXVECTOR3 _s);
public:
    void Render();
    bool IsObjOnScreen(samp::CObject* object);
    bool IsLabelOnScreen(samp::TextLabel label);
    D3DXVECTOR3 GetObjectCoordinates(samp::CObject* object);
};