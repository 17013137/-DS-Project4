#include "stdafx.h"
#include "..\Public\UI_MainBar_BG.h"
#include "Client_Level_Manager.h"
#include "Player_Manager.h"
#include "Scene_Manager.h"

CUI_MainBar_BG::CUI_MainBar_BG(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CUI(pDeviceOut, pDeviceContextOut)
{
}

CUI_MainBar_BG::CUI_MainBar_BG(const CUI_MainBar_BG & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_MainBar_BG::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype())) {
		MSG_BOX(L"Failed To CUI_MainBar_BG : NativeConstruct_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_MainBar_BG::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg))) {
		MSG_BOX(L"Failed To CUI_MainBar_BG : NativeConstruct, __super::NativeConstruct");
		return E_FAIL;
	}
	if (FAILED(SetUp_Components())) {
		MSG_BOX(L"Failed To CUI_MainBar_BG : NativeConstruct, SetUp_Components");
		return E_FAIL;
	}

	m_tUIInfo.fX = 268.f;
	m_tUIInfo.fY = 55.f;
	m_tUIInfo.fSizeX = 330.f;
	m_tUIInfo.fSizeY = 71.2f;

	m_iShaderIndex = 5;
	m_iSprite = 0;
	RenderGroup = CRenderer::GROUP_UI3;

	m_fPercent = 1.f;

	m_iHealth = CPlayer_Manager::Get_Instance()->Get_MaxPlayerHP();

	return S_OK;
}

void CUI_MainBar_BG::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_REALITY || CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_LOADING
		|| CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_COFFEE || CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_BAR
		|| CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_PARK || CScene_Manager::Get_Instance()->Get_OnAir() == true
		|| CClient_Level_Manager::Get_Instance()->Get_Level() == LEVEL_ENDING)
		m_bRenderTrue = false;
	else
		m_bRenderTrue = true;
}

void CUI_MainBar_BG::LateTick(_double TimeDelta)
{

	m_iPreHealth = CPlayer_Manager::Get_Instance()->Get_MaxPlayerHP();

	if (m_iPreHealth != m_iHealth)
	{
		m_PreSize = m_tUIInfo.fSizeX;
		m_tUIInfo.fSizeX = 3.3f * _float(CPlayer_Manager::Get_Instance()->Get_MaxPlayerHP());
		m_tUIInfo.fX += ((m_tUIInfo.fSizeX - m_PreSize) * 0.5f);
		m_iHealth = m_iPreHealth;
	}

	if (nullptr != m_pRendererCom && m_bRenderTrue == true)
		m_pRendererCom->Add_RenderGroup(RenderGroup, this);
	//m_tUIInfo.fX = m_tUIInfo.fX - (_float(CPlayer_Manager::Get_Instance()->Get_MaxPlayerHP() - 100) * 0.5f);
}

HRESULT CUI_MainBar_BG::Render()
{
	if (m_bRenderTrue)
	{
		if (nullptr == m_pShaderCom ||
			nullptr == m_pVIBufferCom)
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		_float4x4	ViewMatrix;
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;

		_float4x4	ProjMatrixTP;
		XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fPercent", &m_fPercent, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", m_iSprite)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_MainBar_BG::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainBar_BG"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CUI_MainBar_BG * CUI_MainBar_BG::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CUI_MainBar_BG*	pInstance = new CUI_MainBar_BG(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CUI_MainBar_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_MainBar_BG::Clone(void * pArg)
{
	CUI_MainBar_BG*	pInstance = new CUI_MainBar_BG(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CUI_MainBar_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainBar_BG::Free()
{
	__super::Free();
}