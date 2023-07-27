#include <Windows.h>

#include <mmsystem.h>

#include <algorithm>

#include "MyGameEngine.h"

#include "MyAccessHub.h"

#include <D3D12Helper.h>
#include <DXSampleHelper.h>

#include "d3dx12.h"

#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "d3d12.lib")           
#pragma comment(lib, "dxgi.lib")            

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")

using namespace DirectX;

HRESULT MyGameEngine::InitMyGameEngine(HINSTANCE hInst, HWND hwnd)
{
    MyAccessHub::setMyGameEnegine(this);

    HRESULT hr = S_OK;  

    RECT rc;
    GetClientRect(hwnd, &rc);   
    UINT width = rc.right - rc.left;    
    UINT height = rc.bottom - rc.top;

    UINT dxgiFactoryFlags = 0;      

#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();

            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

            ComPtr<ID3D12Debug1> spDebugController1;
            debugController->QueryInterface(IID_PPV_ARGS(spDebugController1.GetAddressOf()));
            spDebugController1->SetEnableGPUBasedValidation(true);

        }
    }

#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,          
        D3D_FEATURE_LEVEL_12_1,      
        D3D_FEATURE_LEVEL_12_0,      

        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf())));

    {
        ComPtr<IDXGIAdapter1> adapter;

        ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
        {
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference( 
                    adapterIndex,
                    DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,    
                    IID_PPV_ARGS(adapter.GetAddressOf())));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    
                {
                    continue;
                }

                hr = E_FAIL;

                for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
                {
                    m_featureLevel = featureLevels[featureLevel];
                    if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), &m_pd3dDevice)))
                    {
                        break;
                    }

                }

                if (SUCCEEDED(hr))
                    break;
            }
        }

        if (m_pd3dDevice.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    
                {
                    continue;
                }

                hr = E_FAIL;

                for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
                {
                    m_featureLevel = featureLevels[featureLevel];
                    if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), &m_pd3dDevice)))
                    {
                        break;
                    }

                }

                if (SUCCEEDED(hr))
                    break;
            }

        }

    }

    ThrowIfFailed(hr);

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_pd3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCommandQueue.GetAddressOf())));


    ComPtr<IDXGISwapChain1> swapChain;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FRAME_COUNT;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   
    swapChainDesc.SampleDesc.Count = 1;

    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_pCommandQueue.Get(),                      
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    ThrowIfFailed(swapChain.As(&m_pSwapChain)); 
    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();   

    ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));


    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FRAME_COUNT;
    rtvHeapDesc.NodeMask = 0;                               
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;      
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;    
    ThrowIfFailed(m_pd3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())));

    m_rtvDescriptorSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());   

        for (UINT n = 0; n < FRAME_COUNT; n++)
        {
            ThrowIfFailed(m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(m_renderTargets[n].GetAddressOf())));   
            m_pd3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle); 
            rtvHandle.Offset(1, m_rtvDescriptorSize);                                   
            NAME_D3D12_OBJECT_INDEXED(m_renderTargets, n);                              

            ThrowIfFailed(m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocators[n].GetAddressOf())));
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};    
        dsvHeapDesc.NumDescriptors = 1;  
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(m_pd3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));
    }

    {
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        const CD3DX12_HEAP_PROPERTIES depthStencilHeapProps(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC depthStencilTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        ThrowIfFailed(m_pd3dDevice->CreateCommittedResource(
            &depthStencilHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilTextureDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(m_pDepthStencil.GetAddressOf())
        ));

        NAME_D3D12_OBJECT(m_pDepthStencil); 

        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }






    m_viewport.Width = (FLOAT)width;
    m_viewport.Height = (FLOAT)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;

    m_scissorRect.top = 0;
    m_scissorRect.bottom = height;
    m_scissorRect.left = 0;
    m_scissorRect.right = width;

    for (int i = 0; i < FRAME_COUNT; i++)
    {
        ThrowIfFailed(m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[i].Get(), nullptr, IID_PPV_ARGS(m_initCommand.GetAddressOf())));
        ThrowIfFailed(m_initCommand->Close());
    }

    {
        ThrowIfFailed(m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
        for (int i = 0; i < FRAME_COUNT; i++)
        {
            m_fenceValues[i] = 1;
        }

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return hr;
    m_pTextureMng = make_unique<TextureManager>();
    hr = m_pTextureMng->InitTextureManager();
    if (FAILED(hr))
        return hr;
    m_meshMng = make_unique<MeshManager>();
    m_pipelineMng = make_unique<PipeLineManager>();

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return hr;
    }

    m_soundMng = make_unique<SoundManager>();
    hr = m_soundMng->initSoundManager();
    if (FAILED(hr))
    {
        DWORD r = GetLastError();
        return hr;
    }

    if (!QueryPerformanceFrequency(&m_timerFreq))
    {
        return S_FALSE;
    }

#ifdef DIRECT_INPUT_ACTIVE
    m_inputMng = make_unique<InputManager>();
    hr = m_inputMng->initDirectInput(hInst);
    if (FAILED(hr))
    {
        DWORD r = GetLastError();
        return hr;
    }
#endif

    m_hitMng = make_unique<HitManager>();

    hr = m_sceneCont->initSceneController();
    if (FAILED(hr))
    {
        DWORD r = GetLastError();
        return hr;
    }

    return S_OK;
}

void MyGameEngine::SetSceneController(SceneController* pSceneCont)
{
    m_sceneCont.reset(pSceneCont);
}


HitManager* MyGameEngine::GetHitManager()
{
    return m_hitMng.get();
}

void MyGameEngine::AddGameObject(GameObject* obj)
{
    m_gameObjects.push_back(obj);
    obj->init();                                        
}

void MyGameEngine::RemoveGameObject(GameObject* obj)
{
    obj->cleanupGameObject();                           
    m_gameObjects.remove(obj);
}

void MyGameEngine::SetMainRenderTarget(ID3D12GraphicsCommandList* cmdList)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    cmdList->RSSetViewports(1, &m_viewport);
    cmdList->RSSetScissorRects(1, &m_scissorRect);

}


void MyGameEngine::CleanupDevice()
{
    std::for_each(m_gameObjects.begin(), m_gameObjects.end(), [this](GameObject* obj)
        {
            obj->cleanupGameObject();
        }
    );

    m_gameObjects.clear();
    m_pTextureMng->ReleaseAllTextures();

    WaitForGpu();   

    CoUninitialize();
}


void MyGameEngine::FrameUpdate()
{

    LARGE_INTEGER   nowTimer;
    float           frame;
    const float     FRAME_TIME = 1.0f / 60.0f;
    
    QueryPerformanceCounter(&nowTimer);
    frame = static_cast<float>(nowTimer.QuadPart - m_preTimer.QuadPart) / static_cast<float>(m_timerFreq.QuadPart);

    if (frame >= FRAME_TIME)
    {
        list<GameObject*> deleteObjects;    
        deleteObjects.clear();              

        m_meshMng->resetMesh();

        m_preTimer = nowTimer;  
        m_inputMng->update();

        m_hitMng->refreshHitSystem();

        std::for_each(m_gameObjects.begin(), m_gameObjects.end(),   
            [this, &deleteObjects](GameObject* obj) {
                if (!obj->action()) {                           
                    deleteObjects.push_back(obj);               
                }
            }
        );

        m_hitMng->hitFrameAction();

        m_inputMng->refreshBuffer();

        if (!deleteObjects.empty())
        {
            std::for_each(deleteObjects.begin(), deleteObjects.end(),   
                [this](GameObject* obj) {
                    RemoveGameObject(obj);                              
                    delete(obj);                                        
                }
            );

            deleteObjects.clear();                                      
        }

        m_soundMng->audioUpdate();

        Render();

        MoveToNextFrame();  @
    }

}

void MyGameEngine::WaitForGpu()
{
    ThrowIfFailed(m_pCommandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    m_fenceValues[m_frameIndex]++;
}

void MyGameEngine::MoveToNextFrame()
{
    const UINT64 currentFenceVal = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_pCommandQueue->Signal(m_fence.Get(), currentFenceVal));

    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    m_fenceValues[m_frameIndex] = currentFenceVal + 1;

}

HRESULT MyGameEngine::CreateVertexBuffer(ID3D12Resource** pVertexBuffer, const void* initBuff, UINT vertexSize, UINT vertexCount)
{
    HRESULT hr;

    size_t totalSize = vertexSize * vertexCount;

    CD3DX12_HEAP_PROPERTIES upHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    CD3DX12_RESOURCE_DESC sourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

    if (SUCCEEDED( hr = m_pd3dDevice->CreateCommittedResource(
        &upHeapProp,   
        D3D12_HEAP_FLAG_NONE,
        &sourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(pVertexBuffer)) ) )

    {
        uint8_t* memory = nullptr;
        (*pVertexBuffer)->Map(0, nullptr, reinterpret_cast<void**>(&memory));
        std::memcpy(memory, initBuff, totalSize);
        (*pVertexBuffer)->Unmap(0, nullptr);
    }

    return hr;
}

HRESULT MyGameEngine::CreateIndexBuffer(ID3D12Resource** pIndexBuffer, const void* indexBuff, UINT valueSize, UINT indexCount)
{
    HRESULT hr;

    size_t indexSize = valueSize * indexCount;
    CD3DX12_HEAP_PROPERTIES upHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexSize);

    if (SUCCEEDED(hr = m_pd3dDevice->CreateCommittedResource(
        &upHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(pIndexBuffer))))
    {
        uint8_t* memory = nullptr;
        (*pIndexBuffer)->Map(0, nullptr, reinterpret_cast<void**>(&memory));
        std::memcpy(memory, indexBuff, indexSize);
        (*pIndexBuffer)->Unmap(0, nullptr);
    }

    return hr;
}

HRESULT MyGameEngine::CreateConstantBuffer(ID3D12Resource** pConstBuffer, const void* initBuff, UINT buffSize)
{
    CD3DX12_HEAP_PROPERTIES upheapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    UINT fixedSize = (buffSize + 255) & ~255;

    CD3DX12_RESOURCE_DESC sourceDesc = CD3DX12_RESOURCE_DESC::Buffer(fixedSize);

    HRESULT hr = m_pd3dDevice->CreateCommittedResource(
        &upheapProp,   
        D3D12_HEAP_FLAG_NONE,
        &sourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(pConstBuffer));

    if (SUCCEEDED(hr) && initBuff != nullptr)
    {
        uint8_t* memory = nullptr;
        (*pConstBuffer)->Map(0, nullptr, reinterpret_cast<void**>(&memory));
        std::memcpy(memory, initBuff, sizeof(buffSize));
        (*pConstBuffer)->Unmap(0, nullptr);
    }

    return hr;
}

void MyGameEngine::InitCameraConstantBuffer(CharacterData* chData)
{
    XMMATRIX mtr = {};

    chData->AddConstantBuffer(sizeof(XMMATRIX), &mtr);   
    chData->AddConstantBuffer(sizeof(XMMATRIX), &mtr);   
}

void MyGameEngine::UpdateCameraMatrixForComponent(float fov, XMVECTOR Eye, XMVECTOR At, XMVECTOR Up, float width, float height, float nearZ, float farZ)
{

    if (m_cameraData == nullptr) return;

    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakePerspectiveProjectionMatrix(fov, width, height, nearZ, farZ));

    uint8_t* memory = nullptr;
    ID3D12Resource* resource = m_cameraData->GetConstantBuffer(0);  
    resource->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    std::memcpy(memory, &view, sizeof(XMMATRIX));
    resource->Unmap(0, nullptr);

    resource = m_cameraData->GetConstantBuffer(1);  
    resource->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    std::memcpy(memory, &proj, sizeof(XMMATRIX));
    resource->Unmap(0, nullptr);

}

HRESULT MyGameEngine::UpdateShaderResourceOnGPU(ID3D12Resource* resource, const void* res, size_t buffSize)
{
    uint8_t* memory = nullptr;
    resource->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    std::memcpy(memory, res, buffSize);
    resource->Unmap(0, nullptr);

    return S_OK;
}

HRESULT MyGameEngine::UploadCreatedTextures()
{
    m_initCommand->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr);
    return m_pTextureMng->UploadCreatedTextures(m_pd3dDevice.Get(), m_initCommand.Get(), m_pCommandQueue.Get());
}

void MyGameEngine::Render()
{
    ThrowIfFailed(m_commandAllocators[m_frameIndex]->Reset()); 
    ThrowIfFailed(m_initCommand->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr));

    m_initCommand->RSSetViewports(1, &m_viewport);
    m_initCommand->RSSetScissorRects(1, &m_scissorRect);

    CD3DX12_RESOURCE_BARRIER tra = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_initCommand->ResourceBarrier(1, &tra);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    m_initCommand->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    m_initCommand->ClearRenderTargetView(rtvHandle, Colors::MidnightBlue, 0, nullptr);
    m_initCommand->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, &m_scissorRect);

    tra = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_initCommand->ResourceBarrier(1, &tra);

    ThrowIfFailed(m_initCommand->Close());

    int listCount = m_pipelineMng->Render();
    if (listCount > 0)
        m_pCommandQueue->ExecuteCommandLists(listCount, m_pipelineMng->GetCommandList());


    ThrowIfFailed(m_pSwapChain->Present(1, 0));

}

MyGameEngine::MyGameEngine(UINT width, UINT height, std::wstring title)
{
    m_title = title;
    m_windowWidth = width;
    m_windowHeight = height;
}
