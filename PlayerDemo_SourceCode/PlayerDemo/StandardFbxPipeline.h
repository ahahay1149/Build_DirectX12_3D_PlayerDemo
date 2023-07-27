#pragma once
#include "PipeLineManager.h"
#include "FBXCharacterData.h"
class StandardFbxPipeline :
    public GraphicsPipeLineObjectBase
{
protected:

    bool m_animationMode;       
    int m_animationOffset;

    ComPtr<ID3D12DescriptorHeap> m_srvHeap;

public:
    virtual HRESULT InitPipeLineStateObject(ID3D12Device2* d3dDev) override;
    virtual ID3D12GraphicsCommandList* ExecuteRender() override;

    void SetAnimationMode(bool on)
    {
        m_animationMode = on;
        if (m_animationMode)
            m_animationOffset = 4;
        else
            m_animationOffset = 3;
    }
};
