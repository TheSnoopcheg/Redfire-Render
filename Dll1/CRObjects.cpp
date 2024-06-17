#include "CRObjects.h"

int objectId[7] = { 16878, 15978, 954, 1276, 1279, 19624, 14967 };
const char* objectName[7] = { "Container", "Corpse", "Horseshoe", "Totem", "Loot", "Metarials", "Horse" };

std::string to_utf8(const char* str)
{
    std::string res;
    WCHAR* ures = NULL;
    char* cres = NULL;

    int result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
    if (result_u != 0)
    {
        ures = new WCHAR[result_u];
        if (MultiByteToWideChar(1251, 0, str, -1, ures, result_u))
        {
            int result_c = WideCharToMultiByte(CP_UTF8, 0, ures, -1, 0, 0, 0, 0);
            if (result_c != 0)
            {
                cres = new char[result_c];
                if (WideCharToMultiByte(CP_UTF8, 0, ures, -1, cres, result_c, 0, 0))
                {
                    res = cres;
                }
            }
        }
    }

    delete[] ures;
    delete[] cres;

    return res;
}


std::string toStringWithPrecision(double d, int pr) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << d;
    return stream.str();
}

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

bool CRObjects::IsLabelOnScreen(samp::TextLabel label) {
    float x, y, z;

    const D3DXMATRIX m{ reinterpret_cast<float*>(0xB6FA2C) };
    unsigned long dwLenX = *reinterpret_cast<unsigned long*>(0xC17044);
    unsigned long dwLenY = *reinterpret_cast<unsigned long*>(0xC17048);

    x = label.m_position.x;
    y = label.m_position.y;
    z = label.m_position.z;
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

double CRObjects::GetDistanceBetweenCoords3d(D3DXVECTOR3 _f, D3DXVECTOR3 _s) {
    return sqrt(pow(_s.x - _f.x, 2) + pow(_s.y - _f.y, 2) + pow(_s.z - _f.z, 2));
}

D3DXVECTOR3 CVectorToD3dxVector(sampapi::CVector vector) {
    return { vector.x, vector.y, vector.z };
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
                                    if (lineRenderStatus)
                                        drawlist->AddLine(ImVec2(pedScreenPos.x, pedScreenPos.y), ImVec2(objScreenPos.x, objScreenPos.y), 0xFFFFFFFF);
                                    std::string r_text = objectName[i];
                                    r_text += " [" + toStringWithPrecision(GetDistanceBetweenCoords3d(pedPos, objPos), 1) + "]";
                                    drawlist->AddText(ImVec2(objScreenPos.x + 1, objScreenPos.y + 1), 0xFF000000, r_text.c_str());
                                    drawlist->AddText(ImVec2(objScreenPos.x, objScreenPos.y), 0xFFFFFFFF, r_text.c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
        if (textRenderStatus) {
            for (int i = 0; i < 2048; i++) {
                if (samp::RefNetGame() != nullptr) {
                    if (samp::RefNetGame()->m_pPools->m_pLabel->m_bNotEmpty[i]) {
                        sampapi::CVector vec = samp::RefNetGame()->m_pPools->m_pLabel->m_object[i].m_position;
                        std::string text = samp::RefNetGame()->m_pPools->m_pLabel->m_object[i].m_pText;
                        std::string str = std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z) + text;
                        samp::RefChat()->AddMessage(-1, str.c_str());
                        samp::TextLabel label = samp::RefNetGame()->m_pPools->m_pLabel->m_object[i];
                        if (IsLabelOnScreen(label)) {
                            const char* text = label.m_pText;
                            D3DXVECTOR3 textPos = CVectorToD3dxVector(label.m_position);
                            D3DXVECTOR2 textScreenPos = CalcScreenCoords(textPos);
                            drawlist->AddText(ImVec2(textScreenPos.x, textScreenPos.y), 0xFFFFFFFF, to_utf8(text).c_str());
                        }
                    }
                }
            }
        }
    }
}