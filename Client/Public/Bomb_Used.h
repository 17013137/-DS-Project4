#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "HpPotion.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBomb_Used final : public CGameObject
{
private:
	explicit CBomb_Used(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBomb_Used(const CBomb_Used& rhs);
	virtual ~CBomb_Used() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

private:
	_double				m_Frame = 0.0;
	_float				m_fAlpha = 1.f;
	_double				m_Time = 0;

	CGameObject*		m_UI = nullptr;

	_bool				m_bisRemoved = false;
	_uint				m_iIndex = 0;

	CHpPotion::POTIONINFO			m_tBombInfo;
public:
	HRESULT SetUp_Components();

	CHpPotion::POTIONINFO Get_BombInfo() { return m_tBombInfo; };
public:
	static CBomb_Used* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END