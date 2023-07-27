#include "StandardFbxPipeline.h"
#include "DXSampleHelper.h" 
#include "d3dx12.h"
#include "FBXCharacterData.h"

#include <MyAccessHub.h>

HRESULT StandardFbxPipeline::InitPipeLineStateObject(ID3D12Device2* d3dDev)
{
    HRESULT hr = E_FAIL;

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(d3dDev->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;    
    }

    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);  

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    CD3DX12_ROOT_PARAMETER1 rootParameters[5];


    if (m_animationMode)
    {
        rootParameters[0].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);   
        rootParameters[2].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[3].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[4].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);  
        rootSignatureDesc.Init_1_1(5, rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT); 
    }
    else
    {
        rootParameters[0].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[2].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

        rootParameters[3].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

        m_animationOffset = 3;

        rootSignatureDesc.Init_1_1(4, rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT); 
    }

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, signature.GetAddressOf(), error.GetAddressOf()));

    ThrowIfFailed(d3dDev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));

    struct
    {
        byte* data;
        uint32_t size;
    } meshShader, pixelShader;	

    if (m_animationMode)
    {
        ReadDataFromFile(L"Resources/shaders/FbxSkinAnimeVertexShader.cso", &meshShader.data, &meshShader.size);
    }
    else
    {
        ReadDataFromFile(L"Resources/shaders/FbxVertexShader.cso", &meshShader.data, &meshShader.size);
    }

    ReadDataFromFile(L"Resources/shaders/FbxPixelShader.cso", &pixelShader.data, &pixelShader.size);
    D3D12_INPUT_ELEMENT_DESC layout[] =  
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        {"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"BLENDINDICES", 1, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

    if (m_animationMode)
    {
        psoDesc.InputLayout = { layout, _countof(layout) };
    }
    else
    {
        psoDesc.InputLayout = { layout, 4 };    
    }
    psoDesc.pRootSignature = m_rootSignature.Get();

    psoDesc.VS.BytecodeLength = meshShader.size;
    psoDesc.PS.BytecodeLength = pixelShader.size;
    psoDesc.VS.pShaderBytecode = meshShader.data;
    psoDesc.PS.pShaderBytecode = pixelShader.data;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; 
    psoDesc.SampleDesc.Count = 1;   

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);          

    D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = psoDesc.BlendState.RenderTarget[0];
    defaultRenderTargetBlendDesc.BlendEnable = FALSE;   

    defaultRenderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    defaultRenderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    defaultRenderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    defaultRenderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    defaultRenderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    defaultRenderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

    psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;  
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.StencilEnable = TRUE;     

    psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK; 
    psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = 
            { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP,
            D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    psoDesc.DepthStencilState.FrontFace = defaultStencilOp;
    psoDesc.DepthStencilState.BackFace = defaultStencilOp;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    ThrowIfFailed(d3dDev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipeLineState.GetAddressOf())));
    {
        m_cmdLists.resize(FRAME_COUNT);
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        for (int i = 0; i < FRAME_COUNT; i++)
        {
            ID3D12CommandAllocator* cmdAL = engine->GetCommandAllocator(i);
            ThrowIfFailed(engine->GetDirect3DDevice()->CreateCommandList(0,
                D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAL, m_pipeLineState.Get(),
                IID_PPV_ARGS(m_cmdLists[i].GetAddressOf())));
            m_cmdLists[i]->Close();
        }
        engine->WaitForGpu();
    }

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

    if (m_animationMode)
    {
        cbvHeapDesc.NumDescriptors = 24;          
    }
    else
    {
        cbvHeapDesc.NumDescriptors = 23;         
    }

    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;      
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  
    hr = d3dDev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(m_srvHeap.GetAddressOf()));
    return hr;
}

ID3D12GraphicsCommandList* StandardFbxPipeline::ExecuteRender()
{

    if (m_renderList.size() < 1) return nullptr;

    UINT strides = sizeof(FbxVertex);
    UINT offsets = 0;

    MyGameEngine* engine = MyAccessHub::getMyGameEngine();

    CharacterData* camChar = engine->GetCameraData();
    if (camChar == nullptr) return nullptr; 
    
    ID3D12Resource* p_viewMtx = camChar->GetConstantBuffer(0);
    ID3D12Resource* p_prjMtx = camChar->GetConstantBuffer(1);

    MeshManager* pMeshMng = engine->GetMeshManager();
    TextureManager* pTextureMng = engine->GetTextureManager();

    ID3D12CommandAllocator* cmdAl = engine->GetCurrentCommandAllocator();

    UINT frameIndex = engine->GetCurrentFrameIndex();

    ID3D12GraphicsCommandList* cmdList = m_cmdLists[frameIndex].Get();

    ThrowIfFailed(cmdList->Reset(cmdAl, m_pipeLineState.Get()));
    engine->SetMainRenderTarget(cmdList);

    CD3DX12_RESOURCE_BARRIER tra =
        CD3DX12_RESOURCE_BARRIER::Transition(engine->GetRenderTarget(frameIndex),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmdList->ResourceBarrier(1, &tra);  
    cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
    cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    if (m_animationMode)
    {
        cmdList->SetGraphicsRootConstantBufferView(2, p_viewMtx->GetGPUVirtualAddress());   
        cmdList->SetGraphicsRootConstantBufferView(3, p_prjMtx->GetGPUVirtualAddress());    
    }
    else
    {
        cmdList->SetGraphicsRootConstantBufferView(1, p_viewMtx ->GetGPUVirtualAddress());  
        cmdList->SetGraphicsRootConstantBufferView(2, p_prjMtx ->GetGPUVirtualAddress());   
    }

    for (auto gameobj : m_renderList)
    {
        FBXCharacterData* p_fbxChara = static_cast<FBXCharacterData*>(gameobj->getCharacterData());

        FBXDataContainer* mainFbx = p_fbxChara->GetMainFbx();

        ID3D12Resource* p_WorldMtx = p_fbxChara->GetConstantBuffer(0);

        cmdList->SetGraphicsRootConstantBufferView(0, p_WorldMtx->GetGPUVirtualAddress());  

        XMFLOAT3 position = p_fbxChara->getPosition();
        XMFLOAT3 rotation = p_fbxChara->getRotation();
        XMFLOAT3 scale = p_fbxChara->getScale();

        XMMATRIX model_matrix;
        XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
        XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(rotation.x));
        XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(rotation.y));
        XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(rotation.z));
        XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
        model_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

        XMMATRIX storeMatrix = XMMatrixTranspose(model_matrix);

        engine->UpdateShaderResourceOnGPU(p_WorldMtx, &storeMatrix, sizeof(XMMATRIX));
        MeshContainer* mesh = nullptr;

        if (m_animationMode)
        {
            ID3D12Resource* resource = p_fbxChara->GetConstantBuffer(mainFbx->GetCBuffIndex());
            cmdList->SetGraphicsRootConstantBufferView(1, resource->GetGPUVirtualAddress());    
            engine->UpdateShaderResourceOnGPU(resource, mainFbx->GetAnimatedMatrix(), sizeof(XMFLOAT4X4) * mainFbx->GetClusterCount());
        }
        for (int meshIndex = 0; (mesh = mainFbx->GetMeshContainer(meshIndex)) != nullptr; meshIndex++)
        {
            pMeshMng->SetVertexBuffer(cmdList, mesh->m_MeshId);
            pMeshMng->SetIndexBuffer(cmdList, mesh->m_MeshId);
            if (mesh->m_MaterialId != L"")
            {
                MaterialContainer* matCon = mainFbx->GetMaterialContainer(mesh->m_MaterialId);

                int textureLength = matCon->m_diffuseTextures.size();
                
                auto gpuHeap = m_srvHeap->GetGPUDescriptorHandleForHeapStart();

                for (int i = 0; i < textureLength; i++)
                {
                    std::wstring texId = matCon->m_diffuseTextures[i];

                    if (m_srvTexList.find(texId) == m_srvTexList.end())
                    {
                        pTextureMng->CreateTextureSRV(engine->GetDirect3DDevice(), m_srvHeap.Get(),
                                                        m_animationOffset + m_numOfTex, texId);
                        m_srvTexList[texId] = m_numOfTex;
                        m_numOfTex++;   
                    }

                    gpuHeap.ptr +=
                        engine->GetDirect3DDevice()->GetDescriptorHandleIncrementSize
                        (D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (m_animationOffset + m_srvTexList[texId]);

                    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(gpuHeap);
                    cmdList->SetGraphicsRootDescriptorTable(m_animationOffset + i, cbvSrvGpuHandle);
                }
            }

            cmdList->DrawIndexedInstanced
                ((UINT)mesh->m_indexData.size(),    
                    1, 0, 0, 0);        
        }

    }

    tra = CD3DX12_RESOURCE_BARRIER::Transition(engine->GetRenderTarget(frameIndex),
                D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    cmdList->ResourceBarrier(1, &tra);
    m_renderList.clear();
    cmdList->Close();

    return cmdList;
}
