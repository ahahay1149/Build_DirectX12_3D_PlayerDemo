#pragma once

#include <windows.h>
#include <string>
#include <list>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <wrl/client.h>             

#include "GameObject.h"
#include "TextureManager.h"
#include "PipeLineManager.h"
#include "d3dx12.h"
#include "HitManager.h"
#include "InputManager.h"
#include "MeshManager.h"

#include "SoundManager.h"

#include "SceneController.h"

#define FRAME_COUNT (2)

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class PipeLineManager;
class HitManager;
class GameObject;
class CharacterData;

class MyGameEngine
{
private:
    D3D_DRIVER_TYPE                     m_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL                   m_featureLevel = D3D_FEATURE_LEVEL_12_1;
    ComPtr<ID3D12Device4> m_pd3dDevice = nullptr;

    ComPtr<ID3D12CommandQueue> m_pCommandQueue = nullptr;

    ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr;

    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FRAME_COUNT];
    ComPtr<ID3D12Resource> m_renderTargets[FRAME_COUNT];
    ComPtr<ID3D12GraphicsCommandList>   m_initCommand;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;      
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;      

    UINT                    m_rtvDescriptorSize;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    ComPtr<ID3D12Resource> m_pDepthStencil = nullptr;

    std::wstring    m_title;
    UINT            m_windowWidth;
    UINT            m_windowHeight;

    LARGE_INTEGER   m_timerFreq;
    LARGE_INTEGER   m_preTimer;

    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValues[FRAME_COUNT];
    unique_ptr<TextureManager>  m_pTextureMng;

    unique_ptr<PipeLineManager> m_pipelineMng;

    unique_ptr<MeshManager>     m_meshMng;

    unique_ptr<InputManager>    m_inputMng;

    unique_ptr<HitManager>      m_hitMng;

    unique_ptr<SoundManager>    m_soundMng;

    unique_ptr<SceneController>         m_sceneCont;
    
    std::list<GameObject*>              m_gameObjects;    

    CharacterData* m_cameraData;

    void Render();

public:
    MyGameEngine(UINT width, UINT height, std::wstring title);

    const WCHAR* GetTitle() { return m_title.c_str(); }

    UINT GetWidth() const { return m_windowWidth; }
    UINT GetHeight() const { return m_windowHeight; }

    HRESULT InitMyGameEngine(HINSTANCE, HWND);

    void SetSceneController(SceneController* pSceneCont);

    void CleanupDevice();
    void FrameUpdate();

    void WaitForGpu();
    void MoveToNextFrame();
    HRESULT CreateVertexBuffer(ID3D12Resource** pVertexBuffer, const void* initBuff, UINT vertexSize, UINT vertexCount);
    HRESULT CreateIndexBuffer(ID3D12Resource** pIndexBuffer, const void* indexBuff, UINT valueSize, UINT indexCount);

    HRESULT CreateConstantBuffer(ID3D12Resource** pConstBuffer, const void* initBuff, UINT buffSize);

    ID3D12Device4* GetDirect3DDevice()
    {
        return m_pd3dDevice.Get();
    }

    ID3D12CommandAllocator* GetCurrentCommandAllocator()
    {
        return m_commandAllocators[m_frameIndex].Get();
    }

    ID3D12CommandAllocator* GetCommandAllocator(int frameIndex)
    {
        return m_commandAllocators[frameIndex].Get();
    }

    SceneController* GetSceneController()
    {
        return m_sceneCont.get();
    }

    TextureManager* GetTextureManager()
    {
        return m_pTextureMng.get();
    }

    PipeLineManager* GetPipelineManager()
    {
        return m_pipelineMng.get();
    }

    MeshManager* GetMeshManager()
    {
        return m_meshMng.get();
    }

    InputManager* GetInputManager()
    {
        return m_inputMng.get();
    }

    HitManager* GetHitManager();

    SoundManager* GetSoundManager()
    {
        return m_soundMng.get();
    }

    ID3D12GraphicsCommandList* GetInitCommandList()
    {
        return m_initCommand.Get();
    }

    UINT GetCurrentFrameIndex()
    {
        return m_frameIndex;
    }

    ID3D12Resource* GetRenderTarget(UINT index)
    {
        return m_renderTargets[index].Get();
    }

    ID3D12Resource* GetDepthStencil()
    {
        return m_pDepthStencil.Get();
    }

    void AddGameObject(GameObject* obj);                     

    void RemoveGameObject(GameObject* obj);                  

    void SetCameraData(CharacterData* cameraData)
    {
        m_cameraData = cameraData;
    }

    CharacterData* GetCameraData()
    {
        return m_cameraData;
    }

    void SetMainRenderTarget(ID3D12GraphicsCommandList* cmdList);

    void InitCameraConstantBuffer(CharacterData* chData);
    void UpdateCameraMatrixForComponent(float fov, XMVECTOR Eye, XMVECTOR At, XMVECTOR Up, float width, float height, float nearZ, float farZ);

    HRESULT UpdateShaderResourceOnGPU(ID3D12Resource* resource, const void* res, size_t buffSize);

    HRESULT UploadCreatedTextures();
};