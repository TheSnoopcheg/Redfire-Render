#pragma once
#include "sampapi/CNetGame.h"
#include "sampapi/CGame.h"
#include "CMenu.h"
#include "d3dx9.h"

namespace samp = sampapi::v037r1;

class CRObjects {
private:
    D3DXVECTOR2 CalcScreenCoords(D3DXVECTOR3 vecWorld);
    D3DXVECTOR3 GetPedCoordinates();
public:
    void Render();
    bool IsObjOnScreen(samp::CObject* object);
    D3DXVECTOR3 GetObjectCoordinates(samp::CObject* object);
};