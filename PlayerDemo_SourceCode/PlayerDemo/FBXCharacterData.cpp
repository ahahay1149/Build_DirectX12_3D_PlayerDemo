#include <algorithm>

#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "FBXCharacterData.h"

using namespace fbxsdk;
using namespace DirectX;

void convertFbxAMatrixToXMFLOAT4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

FBX_TEXTURE_TYPE FBXDataContainer::GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable)
{
	std::string texStr = entryTable.GetSource();

	if (texStr == "Maya|DiffuseTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_DIFFUSE;
	}
	else if (texStr == "Maya|NormalTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_NORMAL;
	}
	else if (texStr == "Maya|SpecularTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_SPECUAR;
	}
	else if (texStr == "Maya|FalloffTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_FALLOFF;
	}
	else if (texStr == "Maya|ReflectionMapTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_REFLECTIONMAP;
	}

	return FBX_TEXTURE_TYPE::FBX_UNKNOWN;
}

void FinishFBXLoad(fbxsdk::FbxManager** man, fbxsdk::FbxImporter** imp, fbxsdk::FbxScene** sc)
{

	if ((*imp) != nullptr)
	{
		(*imp)->Destroy();
		*imp = nullptr;
	}

	if ((*sc) != nullptr)
	{
		(*sc)->Destroy();
		*sc = nullptr;
	}

	if ((*man) != nullptr)
	{
		(*man)->Destroy();
		*man = nullptr;
	}

}

HRESULT FBXDataContainer::ReadFbxToMeshContainer(const std::wstring id, FbxMesh * pMesh)
{
	HRESULT hr = S_OK;

	unique_ptr<MeshContainer> meshCont = make_unique<MeshContainer>();

	int nodecount = pMesh->GetNodeCount();
	
	meshCont->setFbxMesh(pMesh);

	FbxVector4* controllPoints = nullptr;

	int* indices = nullptr;

	int vertexCount;
	int contCount;

	controllPoints = pMesh->GetControlPoints();

	indices = pMesh->GetPolygonVertices();

	vertexCount = pMesh->GetPolygonVertexCount();

	contCount = pMesh->GetControlPointsCount();
	meshCont->m_vertexData.clear();
	meshCont->m_vertexData.resize(vertexCount);

	meshCont->m_vtxMin.x = FLT_MAX;
	meshCont->m_vtxMin.y = FLT_MAX;
	meshCont->m_vtxMin.z = FLT_MAX;

	meshCont->m_vtxMax.x = FLT_MIN;
	meshCont->m_vtxMax.y = FLT_MIN;
	meshCont->m_vtxMax.z = FLT_MIN;

	for (int i = 0; i < vertexCount; i++)
	{
		ZeroMemory(&meshCont->m_vertexData[i], sizeof(FbxVertex));

		int index = indices[i];

		meshCont->m_vertexData[i].position.x = (float)controllPoints[index][0];
		meshCont->m_vertexData[i].position.y = (float)controllPoints[index][1];
		meshCont->m_vertexData[i].position.z = (float)controllPoints[index][2];
		if (meshCont->m_vertexData[i].position.x < meshCont->m_vtxMin.x)
		{
			meshCont->m_vtxMin.x = meshCont->m_vertexData[i].position.x;
		}
		else if (meshCont->m_vertexData[i].position.x > meshCont->m_vtxMax.x)
		{
			meshCont->m_vtxMax.x = meshCont->m_vertexData[i].position.x;
		}

		if (meshCont->m_vertexData[i].position.y < meshCont->m_vtxMin.y)
		{
			meshCont->m_vtxMin.y = meshCont->m_vertexData[i].position.y;
		}
		else if (meshCont->m_vertexData[i].position.y > meshCont->m_vtxMax.y)
		{
			meshCont->m_vtxMax.y = meshCont->m_vertexData[i].position.y;
		}

		if (meshCont->m_vertexData[i].position.z < meshCont->m_vtxMin.z)
		{
			meshCont->m_vtxMin.z = meshCont->m_vertexData[i].position.z;
		}
		else if (meshCont->m_vertexData[i].position.z > meshCont->m_vtxMax.z)
		{
			meshCont->m_vtxMax.z = meshCont->m_vertexData[i].position.z;
		}
	}

	if (m_vtxTotalMax.x < meshCont->m_vtxMax.x) m_vtxTotalMax.x = meshCont->m_vtxMax.x;
	if (m_vtxTotalMax.y < meshCont->m_vtxMax.y) m_vtxTotalMax.y = meshCont->m_vtxMax.y;
	if (m_vtxTotalMax.z < meshCont->m_vtxMax.z) m_vtxTotalMax.z = meshCont->m_vtxMax.z;

	if (m_vtxTotalMin.x > meshCont->m_vtxMin.x) m_vtxTotalMin.x = meshCont->m_vtxMin.x;
	if (m_vtxTotalMin.y > meshCont->m_vtxMin.y) m_vtxTotalMin.y = meshCont->m_vtxMin.y;
	if (m_vtxTotalMin.z > meshCont->m_vtxMin.z) m_vtxTotalMin.z = meshCont->m_vtxMin.z;

	FbxStringList uvset_names;
	pMesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	pMesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		meshCont->m_vertexData[i].uv.x = (float)uv[0];
		meshCont->m_vertexData[i].uv.y = (float)(1.0 - uv[1]);	
	}


	meshCont->m_indexData.clear();
	meshCont->m_indexData.resize(vertexCount);

	int length = pMesh->GetPolygonCount();
	
	for (int i = 0; i < length; i++)
	{	
		int baseIndex = i * 3;	

		meshCont->m_indexData[baseIndex] = static_cast<ULONG>(baseIndex);
		meshCont->m_indexData[baseIndex + 1] = static_cast<ULONG>(baseIndex + 1);
		meshCont->m_indexData[baseIndex + 2] = static_cast<ULONG>(baseIndex + 2);
	}
	
	MyAccessHub::getMyGameEngine()->GetMeshManager()->AddIndexBuffer(id, meshCont->m_indexData.data(), sizeof(ULONG), vertexCount);

	FbxArray<FbxVector4> normals;
	pMesh->GetPolygonVertexNormals(normals);

	for (int i = 0; i < normals.Size(); i++)
	{
		meshCont->m_vertexData[i].normal.x = normals[i][0];
		meshCont->m_vertexData[i].normal.y = normals[i][1];
		meshCont->m_vertexData[i].normal.z = normals[i][2];
	}

	{

		int color_count = pMesh->GetElementVertexColorCount();

		FbxGeometryElementVertexColor* vertex_colors = pMesh->GetElementVertexColor(0);

		if (color_count == 0 || vertex_colors == nullptr)
		{
			for (int i = 0; i < vertexCount; i++)
			{
				meshCont->m_vertexData[i].color.x = 1.0f;
				meshCont->m_vertexData[i].color.y = 1.0f;
				meshCont->m_vertexData[i].color.z = 1.0f;
				meshCont->m_vertexData[i].color.w = 1.0f;
			}
		}
		else
		{
			FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
			FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();

			if (mapping_mode == FbxLayerElement::eByPolygonVertex)
			{
				if (reference_mode == FbxLayerElement::eIndexToDirect)
				{
					FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
					FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
					for (int i = 0; i < indeces.GetCount(); i++)
					{
						int id = indeces.GetAt(i);
						FbxColor color = colors.GetAt(id);
						meshCont->m_vertexData[i].color.x = (float)color.mAlpha;
						meshCont->m_vertexData[i].color.y = (float)color.mRed;
						meshCont->m_vertexData[i].color.z = (float)color.mGreen;
						meshCont->m_vertexData[i].color.w = (float)color.mBlue;
					}
				}
			}
		}

	}

	meshCont->m_MeshId = id;

	FbxLayerElementMaterial* elMat = pMesh->GetElementMaterial(0);
	int matIndex = elMat->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* srfMat = pMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(matIndex);
	if (srfMat != nullptr)
	{
		wchar_t namebuff[64] = {};
		auto mtname = srfMat->GetName();
		size_t conv = 0;
		mbstowcs_s(&conv, namebuff, mtname, strlen(mtname));
		meshCont->m_MaterialId = std::wstring(namebuff);
	}
	else
	{
		meshCont->m_MaterialId = L"";
	}

	meshCont->m_vertexCount = vertexCount;
	
	int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	if (skinCount > 0)
	{
		meshCont->InitSkinList(skinCount);
		std::vector< std::vector < std::pair<UINT, float> > > tempWeightVect;

		std::vector< std::vector<FbxSkinAnimeParams> > skinWeights;
		skinWeights.resize(skinCount);

		for (int skinloop = 0; skinloop < skinCount; skinloop++)
		{
			tempWeightVect.clear();
			tempWeightVect.resize(contCount);

			FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(skinloop, FbxDeformer::eSkin);

			skinWeights[skinloop].clear();
			skinWeights[skinloop].resize(contCount);	

			int clusterCount = pSkin->GetClusterCount();

			for (int clusterloop = 0; clusterloop < clusterCount; clusterloop++)
			{
				FbxCluster* pCluster = pSkin->GetCluster(clusterloop);

				int clIndex = GetClusterId(pCluster);

				int pointCnt = pCluster->GetControlPointIndicesCount();
				int* pPointArray = pCluster->GetControlPointIndices();
				double* pPointWeights = pCluster->GetControlPointWeights();

				for (int pointloop = 0; pointloop < pointCnt; pointloop++)
				{
					int cpIndex = pPointArray[pointloop];
					double boneWeight = pPointWeights[pointloop];

					tempWeightVect[cpIndex].push_back({ clIndex, static_cast<float>(boneWeight) });
				}
			}  

			for (int cntLoop = 0; cntLoop < contCount; cntLoop++)
			{
				int wCnt = tempWeightVect[cntLoop].size();
				FbxSkinAnimeParams* skinparam = &skinWeights[skinloop][cntLoop];

				for (; wCnt < 8; wCnt++)
				{
					tempWeightVect[cntLoop].push_back({ 0, 0.0f });
				}

				skinparam->indices0 = { tempWeightVect[cntLoop][0].first, tempWeightVect[cntLoop][1].first, tempWeightVect[cntLoop][2].first, tempWeightVect[cntLoop][3].first };
				skinparam->indices1 = { tempWeightVect[cntLoop][4].first, tempWeightVect[cntLoop][5].first, tempWeightVect[cntLoop][6].first, tempWeightVect[cntLoop][7].first };
				skinparam->weight0 = { tempWeightVect[cntLoop][0].second, tempWeightVect[cntLoop][1].second, tempWeightVect[cntLoop][2].second, tempWeightVect[cntLoop][3].second };
				skinparam->weight1 = { tempWeightVect[cntLoop][4].second, tempWeightVect[cntLoop][5].second, tempWeightVect[cntLoop][6].second, tempWeightVect[cntLoop][7].second };
			}  
		}  

		std::vector<FbxSkinAnimeVertex> skinVertex;
		skinVertex.clear();
		skinVertex.resize(vertexCount);

		for (int i = 0; i < vertexCount; i++)
		{
			int index = indices[i];		
			skinVertex[i].vertex = meshCont->m_vertexData[i];
			skinVertex[i].skinvalues = skinWeights[0][index];	
		}

		MyAccessHub::getMyGameEngine()->GetMeshManager()->
			AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
	}   
	else
	{
		if (pMesh->GetScene()->GetSrcObjectCount<FbxMesh>() > 1)
		{
			std::vector<FbxSkinAnimeVertex> skinVertex;
			skinVertex.clear();
			skinVertex.resize(vertexCount);

			UINT boneId = GetClusterId(pMesh->GetNode());
			for (int i = 0; i < vertexCount; i++)
			{
				skinVertex[i].vertex = meshCont->m_vertexData[i];
				skinVertex[i].skinvalues.indices0 = { boneId, 0, 0, 0 };	
				skinVertex[i].skinvalues.indices1 = { 0, 0, 0, 0 };
				skinVertex[i].skinvalues.weight0 = { 1.0f, 0, 0, 0 };		
				skinVertex[i].skinvalues.weight1 = { 0, 0, 0, 0 };
			}
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
		}
		else
		{
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);
		}
	}     
	m_pMeshContainer.push_back(move(meshCont));
	return hr;
}

HRESULT FBXDataContainer::LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material)
{
	HRESULT hr = S_OK;

	enum class MaterialOrder
	{
		Ambient,
		Diffuse,
		Specular,
		MaxOrder,
	};

	const fbxsdk::FbxImplementation* implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);

	wchar_t namebuff[64] = {};
	auto mtname = material->GetName();
	size_t conv = 0;
	mbstowcs_s(&conv, namebuff, mtname, strlen(mtname));
	std::wstring matName = namebuff;
	m_pMaterialContainer[matName] = make_unique<MaterialContainer>();

	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
	FbxDouble factors[(int)MaterialOrder::MaxOrder];
	FbxProperty fbxProp = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		const char* element_check_list[] =
		{
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sDiffuse,
		};

		const char* factor_check_list[] =
		{
			FbxSurfaceMaterial::sAmbientFactor,
			FbxSurfaceMaterial::sDiffuseFactor,
		};

		for (int i = 0; i < 2; i++)
		{
			fbxProp = material->FindProperty(element_check_list[i]);
			if (fbxProp.IsValid())
			{
				colors[i] = fbxProp.Get<FbxDouble3>();

			}
			else
			{
				colors[i] = FbxDouble3(1.0, 1.0, 1.0);
			}

			fbxProp = material->FindProperty(factor_check_list[i]);
			if (fbxProp.IsValid())
			{
				factors[i] = fbxProp.Get<FbxDouble>();
			}
			else
			{
				factors[i] = 1.0;
			}
		}
	}

	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
	m_pMaterialContainer[matName]->setAmbient((float)color[0], (float)color[1], (float)color[2], (float)factor);

	color = colors[(int)MaterialOrder::Diffuse];
	factor = factors[(int)MaterialOrder::Diffuse];
	m_pMaterialContainer[matName]->setDiffuse((float)color[0], (float)color[1], (float)color[2], (float)factor);

	if (implementation == nullptr)
	{
		fbxProp = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		LoadTextureFromMaterial(matName, id, FBX_TEXTURE_TYPE::FBX_DIFFUSE, &fbxProp);
	}
	else
	{
		const FbxBindingTable* rootTable = implementation->GetRootTable();
		size_t entryCount = rootTable->GetEntryCount();

		for (int ent = 0; ent < entryCount; ent++)
		{
			const FbxBindingTableEntry entry = rootTable->GetEntry(ent);

			fbxProp = material->FindPropertyHierarchical(entry.GetSource());
			if (!fbxProp.IsValid())
			{
				fbxProp = material->RootProperty.FindHierarchical(entry.GetSource());
			}

			LoadTextureFromMaterial(matName, id, GetTextureType(entry), &fbxProp);
		}
	}

	return hr;
}

HRESULT FBXDataContainer::LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const fbxsdk::FbxProperty* fbxProp)
{
	HRESULT hr = S_OK;
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();

	fbxsdk::FbxFileTexture* texture = nullptr;
	std::string keyword;
	int numOfTex = fbxProp->GetSrcObjectCount<FbxFileTexture>();
	if (numOfTex > 0)
	{
		numOfTex = 1;
	}
	else
	{
		numOfTex = fbxProp->GetSrcObjectCount<FbxLayeredTexture>();
	}

	TextureManager* texMng = MyAccessHub::getMyGameEngine()->GetTextureManager();
	for (int i = 0; i < numOfTex; i++)
	{
		texture = fbxProp->GetSrcObject<FbxFileTexture>(i);
		if (texture != nullptr)
		{
			std::string filePath = texture->GetRelativeFileName();

			std::string::size_type position(filePath.find('\\'));
			std::string fileName;

			while (position != std::string::npos)
			{
				filePath.replace(position, 1, "/");
				position = filePath.find('\\', position + 1);
			}

			std::string::size_type offset = std::string::size_type(0);
			position = filePath.find('/');

			while (position != std::string::npos)
			{
				offset = position + 1;
				position = filePath.find('/', offset);
			}

			fileName = filePath.substr(offset);

			position = fileName.find(".psd");
			if (position != std::string::npos)
			{
				while (position != std::string::npos)
				{
					offset = position;
					position = fileName.find(".psd", position + 4);
				}
				fileName.replace(offset, 4, ".tga");
			}

			wchar_t namebuff[64] = {};
			wchar_t texturePath[128];
			size_t conv = 0;
			mbstowcs_s(&conv, namebuff, fileName.c_str(), fileName.length());

			wsprintfW(texturePath, L"Resources/textures/%ls/%ls", id.c_str(), namebuff);

			wchar_t idName[128];
			wsprintfW(idName, L"%ls_%ls", id.c_str(), namebuff);
			hr = texMng->CreateTextureFromFile(engine->GetDirect3DDevice(), idName, texturePath);

			if (FAILED(hr))
			{
				break;
			}

			switch (texType)
			{
			case FBX_TEXTURE_TYPE::FBX_DIFFUSE:
				m_pMaterialContainer[matName]->m_diffuseTextures.push_back(idName);
				break;

			case FBX_TEXTURE_TYPE::FBX_NORMAL:
				m_pMaterialContainer[matName]->m_normalTextures.push_back(idName);
				break;

			case FBX_TEXTURE_TYPE::FBX_SPECUAR:
				m_pMaterialContainer[matName]->m_specularTextures.push_back(idName);
				break;

			case FBX_TEXTURE_TYPE::FBX_FALLOFF:
				m_pMaterialContainer[matName]->m_falloffTextures.push_back(idName);
				break;

			case FBX_TEXTURE_TYPE::FBX_REFLECTIONMAP:
				m_pMaterialContainer[matName]->m_reflectionMapTextures.push_back(idName);
				break;

			default:	
				break;
			}
		}
	}

	return hr;
}

HRESULT FBXDataContainer::LoadFBX(const std::wstring fileName, const std::wstring id)
{
	HRESULT hr = S_OK;

	fbxsdk::FbxManager* fbx_manager = nullptr;
	FbxScene* fbx_scene = nullptr;
	FbxImporter* fbx_importer = nullptr;
	FbxNode* fbx_node = nullptr;

	char* c_filename = nullptr;
	int wcSize = sizeof(wchar_t) * wcslen(fileName.c_str()) + 1;

	c_filename = new char[wcSize];
	size_t retVal = 0;
	wcstombs_s(&retVal, c_filename, wcSize, fileName.c_str(), wcSize);

	fbx_manager = FBXDataContainer::GetFbxManager();

	fbx_importer = FbxImporter::Create(fbx_manager, c_filename);
	if (fbx_importer == nullptr)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}
	fbx_scene = FbxScene::Create(fbx_manager, c_filename);
	if (fbx_scene == nullptr)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}
	bool res = fbx_importer->Initialize(c_filename);	
	delete[] c_filename;
	if (res == false)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}

	if (fbx_importer->Import(fbx_scene) == false)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}
	fbx_node = fbx_scene->GetRootNode();
	if (fbx_node != nullptr)
	{
		int nodes = fbx_scene->GetNodeCount();
		m_nodeNameList.clear();
		m_nodeNameList.resize(nodes);
		for (int i = 0; i < nodes; i++)
		{
			m_nodeNameList[i] = fbx_scene->GetNode(i)->GetName();
		}
		FbxGeometryConverter converter(fbx_manager);
		converter.SplitMeshesPerMaterial(fbx_scene, true);		
		converter.Triangulate(fbx_scene, true);					
		if (fbx_importer->GetAnimStackCount() > 0)
		{
			auto stack = fbx_scene->GetCurrentAnimationStack();		

			m_startTime = stack->GetLocalTimeSpan().GetStart().GetSecondDouble();
			m_endTime = stack->GetLocalTimeSpan().GetStop().GetSecondDouble();
			m_timePeriod = 1.0 / 60.0;	
			m_animeFrames = floorl((m_endTime - m_startTime) / m_timePeriod);

			m_animeStack = stack;
			fbx_scene->SetCurrentAnimationStack(m_animeStack);	
		}
		else
		{
			m_animeStack = nullptr;
			m_animeFrames = 0;
		}
		m_pMeshContainer.clear();	

		int materialCnt = fbx_scene->GetMaterialCount();

		for (int i = 0; i < materialCnt; i++)
		{
			LoadMaterial(id, fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i));
		}

		int meshCount = fbx_scene->GetSrcObjectCount<FbxMesh>();

		m_vtxTotalMax.x = FLT_MIN;
		m_vtxTotalMax.y = FLT_MIN;
		m_vtxTotalMax.z = FLT_MIN;

		m_vtxTotalMin.x = FLT_MAX;
		m_vtxTotalMin.y = FLT_MAX;
		m_vtxTotalMin.z = FLT_MAX;

		wchar_t idName[128];
		for (int i = 0; i < meshCount; i++)
		{
			wsprintfW(idName, L"%ls_%02d", id.c_str(), i);	
			if ( FAILED( ReadFbxToMeshContainer(idName, fbx_scene->GetSrcObject<FbxMesh>(i)) ) )
			{
				hr = E_FAIL;

				FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
				return hr;
			}
		}

		m_clusterCount = m_boneNameList.size();
		if (m_clusterCount > 0)
		{
			m_boneIdList.resize(m_clusterCount);
			m_F4X4Matrix.resize(m_clusterCount);
		}

		m_pFbxScene = fbx_scene;
	}

	fbx_importer->Destroy();
	return hr;
}

void FBXDataContainer::SetMeshUniqueFlag(bool meshFlag, bool materialFlag)
{
	int length = m_pMeshContainer.size();

	for (int id = 0; id < length; id++)
	{
		if (m_pMeshContainer[id] != nullptr)
		{
			m_pMeshContainer[id]->setUniqueFlag(meshFlag);
		}
	}
}

void FBXDataContainer::SetTextureUniqueFlag(bool texFlag)
{
	for (auto ite = m_pMaterialContainer.begin(); ite != m_pMaterialContainer.end(); ite++)
	{
		ite->second->setUniqueTextureFlag(texFlag);
	}
}

int FBXDataContainer::GetNodeId(const char* nodeName)
{
	int length = m_nodeNameList.size();
	for (int i = 0; i < length; i++)
	{
		if (strcmp(m_nodeNameList[i], nodeName) == 0)
		{
			return i;
		}
	}

	return -1;
}

int FBXDataContainer::GetMeshId(const char* meshName)
{
	int len = m_pMeshContainer.size();

	for (int i = 0; i < len; i++)
	{
		if (strcmp(m_pMeshContainer[i]->getMeshNodeName(), meshName) == 0)
		{
			return i;
		}
	}

	return -1;
}

FbxNode* FBXDataContainer::GetMeshNode(int id)
{
	if (id < m_pMeshContainer.size())
	{
		return m_pMeshContainer[id]->getFbxMesh()->GetNode();
	}
	return nullptr;
}

HRESULT FBXCharacterData::LoadMainFBX(const std::wstring fileName, const std::wstring id)
{
	if (m_mainFbx.get() != nullptr)
	{
		m_mainFbx.release();
	}

	m_mainFbx = make_unique<FBXDataContainer>();
	HRESULT hr = m_mainFbx->LoadFBX(fileName, id);
	if (SUCCEEDED(hr))
	{
		m_cbuffCount = 0;
		AddConstantBuffer(sizeof(XMMATRIX), nullptr);	 

		if (m_mainFbx->GetClusterCount() > 0)
		{
			int curCbuff = m_cbuffCount;
			AddConstantBuffer(sizeof(XMFLOAT4X4) * m_mainFbx->GetClusterCount(), nullptr);
			m_mainFbx->SetCBuffIndex(curCbuff);
		}
	}

	return hr;
}


HRESULT FBXCharacterData::LoadAnimationFBX(const std::wstring fileName, const std::wstring id)
{
	std::unique_ptr<FBXDataContainer> fbxCon = make_unique<FBXDataContainer>();

	HRESULT res = fbxCon->LoadFBX(fileName, id);
	if (SUCCEEDED(res))
	{
		if (m_animeFbxMap[id] != nullptr)
		{
			m_animeFbxMap[id].release();
		}
		m_animeFbxMap[id] = move(fbxCon);
	}
	return res;
}

FBXDataContainer* FBXCharacterData::GetAnimeFbx(const std::wstring fileName)
{
	if (m_animeFbxMap.find(fileName) == m_animeFbxMap.end())
		return nullptr;

	return m_animeFbxMap[fileName].get();
}

void FBXCharacterData::SetAnime(std::wstring animeLabel)
{
	if (animeLabel != m_currentAnimeLabel)
	{
		if (m_animeFbxMap[animeLabel] != nullptr)
		{
			FBXDataContainer* animeCont = m_animeFbxMap[animeLabel].get();	
			FBXDataContainer* mainCont = m_mainFbx.get();					
			MeshContainer* meshCont = nullptr;				

			for (int i = 0; (meshCont = mainCont->GetMeshContainer(i)) != nullptr; i++)
			{

				meshCont->SetBoneIdList(animeCont);	
			}

			m_currentAnimeLabel = animeLabel;	
			m_animeTime = 0;					
			animeCont->GetFbxScene()->SetCurrentAnimationStack(animeCont->GetAnimeStack());
			mainCont->SetAnimationFbx(animeCont);	
			UpdateAnimation(0);						
		}
	}
}

void FBXCharacterData::UpdateAnimation()
{
	FBXDataContainer* animeCont = m_animeFbxMap[m_currentAnimeLabel].get();	
	assert(animeCont);							
	int frames = animeCont->GetAnimeFrames();	
	if (m_animeTime < frames)					
	{
		m_animeTime++;							
	}
	else
	{
		m_animeTime -= frames;					
	}
	UpdateAnimation(m_animeTime);				
}

void FBXCharacterData::UpdateAnimation(int frameCount)
{
	FBXDataContainer* animeCont = m_animeFbxMap[m_currentAnimeLabel].get();
	FBXDataContainer* mainCont = m_mainFbx.get();
	MeshContainer* meshCont = nullptr;
	double nowTime = animeCont->GetPeriodTime() * frameCount;	

	if (nowTime > animeCont->GetEndTime())	
	{
		nowTime = nowTime - animeCont->GetEndTime();	
	}

	FbxTime currentTime;					
	currentTime.SetSecondDouble(nowTime);	

	mainCont->UpdateAnimation(currentTime);
}
MeshContainer::~MeshContainer()
{
	m_vertexData.clear();
	m_indexData.clear();
	
	m_skinCount = 0;	
	if (m_uniqueMesh)
	{
		auto meshMng = MyAccessHub::getMyGameEngine()->GetMeshManager();
		meshMng->removeVertexBuffer(m_MeshId, true);
	}
}

void MeshContainer::setFbxMesh(FbxMesh* mesh)
{
	m_mesh = mesh;
	m_meshNodeName = m_mesh->GetNode(0)->GetName();
	
	FbxTime timeZero;
	timeZero.SetFrame(0, FbxTime::EMode::eFrames60);	

	m_meshNodeName = m_mesh->GetNode(0)->GetName();		
	m_IBaseMatrix = m_mesh->GetNode(0)->EvaluateGlobalTransform(timeZero).Inverse();
}

void MeshContainer::InitSkinList(int skinCount)
{
	m_skinCount = skinCount;
}

void MeshContainer::SetBoneIdList(FBXDataContainer* animeFbxCont)
{

	m_animeFbxCont = animeFbxCont;
	m_parentNodeId = m_animeFbxCont->GetMeshId(m_meshNodeName);	
}
MaterialContainer::~MaterialContainer()
{
	if (m_uniqueTextures)
	{
		TextureManager* texMng = MyAccessHub::getMyGameEngine()->GetTextureManager();
		for (auto id : m_diffuseTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_normalTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_specularTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_falloffTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_reflectionMapTextures)
		{
			texMng->ReleaseTexture(id);
		}

	}

	m_diffuseTextures.clear();
	m_normalTextures.clear();
	m_specularTextures.clear();
	m_falloffTextures.clear();
	m_reflectionMapTextures.clear();
}

int FBXDataContainer::GetClusterId(FbxCluster* pCluster)
{
	return GetClusterId(pCluster->GetLink());
}

int FBXDataContainer::GetClusterId(FbxNode* pNode)
{
	int size = m_boneNameList.size();
	const char* nodeName = pNode->GetName();

	for (int id = 0; id < size; id++)
	{
		if (strcmp(nodeName, m_boneNameList[id]) == 0)
			return id;
	}

	m_boneNameList.push_back(nodeName);

	m_IboneMatrix.push_back(pNode->EvaluateGlobalTransform().Inverse());

	return size;	 
}

void FBXDataContainer::SetAnimationFbx(FBXDataContainer* animeCont)
{
	if (m_currentAnimeCont != animeCont)
	{
		m_currentAnimeCont = animeCont;

		int len = m_boneNameList.size();
		for (int i = 0; i < len; i++)
		{
			m_boneIdList[i] = m_currentAnimeCont->GetNodeId(m_boneNameList[i]);
		}
	}
}

void FBXDataContainer::UpdateAnimation(const FbxTime& animeTime)
{
	FbxNode* node;
	int size = m_boneNameList.size();

	FbxScene* animeScene = m_currentAnimeCont->GetFbxScene();	

	for (int i = 0; i < size; i++)
	{
		node = animeScene->GetNode(m_boneIdList[i]);	

		FbxAMatrix matrix = node->EvaluateGlobalTransform(animeTime) * m_IboneMatrix[i];
		convertFbxAMatrixToXMFLOAT4x4(matrix, m_F4X4Matrix[i]);
	}
}

const XMFLOAT4X4* FBXDataContainer::GetAnimatedMatrix()
{
	return m_F4X4Matrix.data();
}
