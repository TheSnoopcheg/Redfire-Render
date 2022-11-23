#include "CRObjects.h"

int objectId[7] = { 16878, 15978, 954, 1276, 1279, 19624, 14967 };
const char* objectName[7] = { "Container", "Corpse", "Horseshoe", "Totem", "Loot", "Metarials", "Horse" };

D3DXVECTOR2 CRObjects::CalcScreenCoords(D3DXVECTOR3 vecWorld)
{
    float x, y, z;
    const D3DXMATRIX m{ reinterpret_cast<float*>(0xB6FA2C) };
    unsigned long dwLenX = *reinterpret_cast<unsigned long*>(0xC17044);
    unsigned long dwLenY = *reinterpret_cast<unsigned long*>(0xC17048);

    x = (vecWorld.z * m._31) + (vecWorld.y * m._21) + (vecWorld.x * m._11) + m._41;
    y = (vecWorld.z * m._32) + (vecWorld.y * m._22) + (vecWorld.x * m._12) + m._42;
    z = (vecWorld.z * m._33) + (vecWorld.y * m._23) + (vecWorld.x * m._13) + m._43;

    double    fRecip = (double)1.0 / z;
    x *= (float)(fRecip * dwLenX);
    y *= (float)(fRecip * dwLenY);
    return { x, y };
}

bool CRObjects::IsObjOnScreen(samp::CObject* object) {
    float x, y, z;

    const D3DXMATRIX m{ reinterpret_cast<float*>(0xB6FA2C) };
    unsigned long dwLenX = *reinterpret_cast<unsigned long*>(0xC17044);
    unsigned long dwLenY = *reinterpret_cast<unsigned long*>(0xC17048);

    x = object->m_position.x;
    y = object->m_position.y;
    z = object->m_position.z;
    D3DXVECTOR2 screenPos = this->CalcScreenCoords({ x, y, z });

    z = (z * m._33) + (y * m._23) + (x * m._13) + m._43;
    if (screenPos.x <= dwLenX && screenPos.y <= dwLenY && screenPos.x >= 0 && screenPos.y >= 0 && z >= 1.0f)
        return true;
    return false;
}

D3DXVECTOR3 CRObjects::GetObjectCoordinates(samp::CObject* object) {
    float x, y, z;
    x = object->m_position.x;
    y = object->m_position.y;
    z = object->m_position.z;
    return { x, y, z };
}

D3DXVECTOR3 CRObjects::GetPedCoordinates() {
    float x, y, z;
    DWORD CPed = *reinterpret_cast<DWORD*>(0xB6F5F0);
    if (CPed != NULL) {
        DWORD CPed_stPos = *reinterpret_cast<DWORD*>(CPed + 0x14);
        x = *reinterpret_cast<float*>(CPed_stPos + 0x30);
        y = *reinterpret_cast<float*>(CPed_stPos + 0x34);
        z = *reinterpret_cast<float*>(CPed_stPos + 0x38);
        return { x, y, z };
    }
    return { 0.0f, 0.0f, 0.0f };
}

void CRObjects::Render() {
    auto drawlist = ImGui::GetBackgroundDrawList();
    if (!samp::RefGame()->IsMenuVisible()) {
        if (objectRenderStatus[0] || objectRenderStatus[1] || objectRenderStatus[2] || objectRenderStatus[3]
            || objectRenderStatus[4] || objectRenderStatus[5] || objectRenderStatus[6]) {
            for (int i = 0; i < 1000; i++) {
                if (samp::RefNetGame() != nullptr) {
                    if (samp::RefNetGame()->GetObjectPool()->m_pObject[i] != nullptr
                        && samp::RefNetGame()->GetObjectPool()->m_pObject[i]->m_pGameEntity != nullptr) {
                        samp::CObject* object = samp::RefNetGame()->GetObjectPool()->Get(i);
                        int model = object->m_nModel;
                        D3DXVECTOR3 pedPos = GetPedCoordinates();
                        D3DXVECTOR2 pedScreenPos = CalcScreenCoords(pedPos);
                        int size = sizeof(objectId) / sizeof(objectId[0]);
                        for (int i = 0; i <= size; i++) {
                            if (model == objectId[i] && objectRenderStatus[i]) {
                                if (IsObjOnScreen(object)) {
                                    D3DXVECTOR3 objPos = GetObjectCoordinates(object);
                                    D3DXVECTOR2 objScreenPos = CalcScreenCoords(objPos);
                                    drawlist->AddText(ImVec2(objScreenPos.x, objScreenPos.y), 0xFFFFFFFF, objectName[i]);
                                    if (lineRenderStatus)
                                        drawlist->AddLine(ImVec2(pedScreenPos.x, pedScreenPos.y), ImVec2(objScreenPos.x, objScreenPos.y), 0xFFFFFFFF);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}