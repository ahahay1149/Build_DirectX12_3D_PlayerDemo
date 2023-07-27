#include "PipeLineManager.h"
#include "DXSampleHelper.h" 
#include "d3dx12.h"

#include <MyAccessHub.h>

void PipeLineManager::AddPipeLineObject(const std::wstring labelName, GraphicsPipeLineObjectBase* obj)
{
    if (SUCCEEDED(obj->InitPipeLineStateObject(MyAccessHub::getMyGameEngine()->GetDirect3DDevice())))
    {
        m_pipeLineDB[labelName].reset(obj);
    }
}

UINT PipeLineManager::Render()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    UINT renderCount = 1;   

    m_renderCommandList.clear();
    m_renderCommandList.push_back(engine->GetInitCommandList());

    for (auto const& ppObjPair : m_pipeLineDB)
    {
        ID3D12GraphicsCommandList* resultCmd = ppObjPair.second.get()->ExecuteRender();
        if (resultCmd != nullptr)
        {
            m_renderCommandList.push_back(resultCmd);
            renderCount++;
        }

        ppObjPair.second.get()->ClearRenderList();
    }

    return renderCount;
}
