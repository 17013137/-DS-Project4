#include "stdafx.h"
#include "..\Public\Regret_HandTrail1.h"
#include "GameInstance.h"

#include "Layer.h"


CRegret_HandTrail1::CRegret_HandTrail1(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CRegret_HandTrail1::CRegret_HandTrail1(const CRegret_HandTrail1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRegret_HandTrail1::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRegret_HandTrail1::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(-90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	m_bLeft = *((_bool*)pArg);
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	m_pModelCom = (CModel*)pGameInstance->Get_Component(LEVEL_BOSS_REGRET, TEXT("Layer_Boss_Regret"), TEXT("Com_Model"));
	m_pBoneNameLeft = "Regret_l_Arm_WristSHJnt";
	m_pBoneNameRight = "Regret_r_Arm_WristSHJnt";
	

	m_pLeftSocketMatrix = m_pModelCom->Get_CombinedTransformationMatrix(m_pBoneNameLeft);
	m_pRightSocketMatrix = m_pModelCom->Get_CombinedTransformationMatrix(m_pBoneNameRight);
	

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	m_pRegretTrans = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSS_REGRET, TEXT("Layer_Boss_Regret"), TEXT("Com_Transform"));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CRegret_HandTrail1::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_bLeft)
	{
		_matrix	LBoneMatrix;

		LBoneMatrix = XMLoadFloat4x4(m_pLeftSocketMatrix);
		LBoneMatrix.r[0] = XMVector3Normalize(LBoneMatrix.r[0]);
		LBoneMatrix.r[1] = XMVector3Normalize(LBoneMatrix.r[1]);
		LBoneMatrix.r[2] = XMVector3Normalize(LBoneMatrix.r[2]);

		LBoneMatrix = XMMatrixTranspose(LBoneMatrix * XMLoadFloat4x4(&m_PivotMatrix) * m_pRegretTrans->Get_WorldMatrix());

		XMStoreFloat4x4(&m_pLeftWorldMat,XMMatrixRotationZ(XMConvertToRadians(-89.f))*XMMatrixTranspose(LBoneMatrix*XMMatrixScaling(150.f, 150.f, 150.f)));

	}
	else
	{
		_matrix RBoneMatrix;
		RBoneMatrix = XMLoadFloat4x4(m_pRightSocketMatrix);
		RBoneMatrix.r[0] = XMVector3Normalize(RBoneMatrix.r[0]);
		RBoneMatrix.r[1] = XMVector3Normalize(RBoneMatrix.r[1]);
		RBoneMatrix.r[2] = XMVector3Normalize(RBoneMatrix.r[2]);

		RBoneMatrix = XMMatrixTranspose(RBoneMatrix * XMLoadFloat4x4(&m_PivotMatrix) * m_pRegretTrans->Get_WorldMatrix());

		XMStoreFloat4x4(&m_pRightWorldMat, XMMatrixRotationZ(XMConvertToRadians(80.f))*XMMatrixTranspose(RBoneMatrix*XMMatrixScaling(230.f, 230.f, 230.f)));
	}
	
	if (m_bLeft)
		m_pVIBufferCom->Update(&m_pLeftWorldMat);
	else
		m_pVIBufferCom->Update(&m_pRightWorldMat);
	

}

void CRegret_HandTrail1::LateTick(_double TimeDelta)
{
	
	__super::LateTick(TimeDelta);

		Compute_CamDistance();

			if (nullptr != m_pRendererCom)
				m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
		
	
}

HRESULT CRegret_HandTrail1::Render()
{
		if (nullptr == m_pShaderCom || 
			nullptr == m_pVIBufferCom)
			return E_FAIL;

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		m_pShaderCom->Begin(11);

		m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CRegret_HandTrail1::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexBlur"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_BOSS_REGRET, TEXT("Prototype_Component_Texture_Regret_Trail"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_BOSS_REGRET, TEXT("Prototype_Component_VIBuffer_Regret_Trail0"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CRegret_HandTrail1::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRegret_HandTrail1 * CRegret_HandTrail1::Create(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
{
	CRegret_HandTrail1*	pInstance = new CRegret_HandTrail1(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRegret_HandTrail1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRegret_HandTrail1::Clone(void * pArg)
{
	CRegret_HandTrail1*	pInstance = new CRegret_HandTrail1(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRegret_HandTrail1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRegret_HandTrail1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pRendererCom);
}