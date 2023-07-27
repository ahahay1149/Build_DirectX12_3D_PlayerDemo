#pragma once
#include "PipeLineManager.h"

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class SpriteRenderPipeline :
    public GraphicsPipeLineObjectBase
{
private:
    static const UINT MAX_SPRITES = 512;	
    uint8_t blendMode = 0;
    uint8_t samplerMode = 0;

    struct MixSettings
    {
        UINT colorMix;
        UINT alphaMix;
    };
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;      
    ComPtr<ID3D12Resource> m_pUvBuffer = nullptr;				
    ComPtr<ID3D12Resource> m_pSizeBuffer = nullptr;				

    ComPtr<ID3D12Resource> m_pAffineMatrix = nullptr;			
    ComPtr<ID3D12Resource> m_pColorBuffer = nullptr;				
    ComPtr<ID3D12Resource> m_pMixBuffer = nullptr;				
    XMFLOAT4 m_aUv[MAX_SPRITES];
    XMFLOAT2 m_aSize[MAX_SPRITES];
    XMMATRIX m_aAffin[MAX_SPRITES];
    XMFLOAT4 m_aColor[MAX_SPRITES];
    MixSettings m_aMix[MAX_SPRITES];
public:
    virtual HRESULT InitPipeLineStateObject(ID3D12Device2* d3dDev) override;
    virtual ID3D12GraphicsCommandList* ExecuteRender() override;

    void SetBlendMode(uint8_t mode)
    {
        blendMode = mode;
    }

    void SetSamplerMode(uint8_t mode)
    {
        samplerMode = mode;
    }
};

