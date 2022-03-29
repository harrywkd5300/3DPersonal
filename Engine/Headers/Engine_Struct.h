#pragma once


namespace Engine
{

	typedef struct BoundingBox
	{
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMax;

		//BoundingBox()
		//{
		//	vMin.x = INFINITY;
		//	vMin.y = INFINITY;
		//	vMin.z = INFINITY;

		//	vMax.x = INFINITY;
		//	vMax.y = INFINITY;
		//	vMax.z = INFINITY;
		//};

		bool	isPointInside(D3DXVECTOR3& p)
		{
			if (p.x > vMin.x && p.y > vMin.y && p.z > vMin.z &&
				p.x < vMax.x && p.y < vMax.y && p.z < vMax.z)
			{
				return true;
			}
			else
			{
				return false;
			}
		};

	}BOUNDINGBOX;

	typedef struct Paticle_attribute
	{
		D3DXVECTOR3		vPositon;   // 월드 스페이스상의 위치
		D3DXVECTOR3		vVelocity;	// 파티클의 속도, 초당 위치
		D3DXVECTOR3		vAcceleration; // 파티클의 가속도, 초당위치
		float			fLifeTime;	// 파티클이 소멸할떄까지 유지시간
		float			fAge;		// 파티클의 현재 나이
		D3DXCOLOR		dwColor;	// 파티클의 컬러
		D3DXCOLOR		dwColorFade; // 파티클의 컬러가시간의 흐름에 따라퇴색하는방법
		bool			bisAlive;	//파티클의 생존true, 소멸 false
	}PATICLEATTRIBUTE;

	// For. Object
	typedef struct Object_Data
	{
		TCHAR FullPath[256];
		TCHAR FileName[64];
		TCHAR ComName[64];

		D3DXVECTOR3	vPostion;
		D3DXVECTOR3	vScale;
		D3DXVECTOR3	vRotation;

	}OBJECT;

	//For. Image
	typedef struct IMAGE_INFO
	{
		TCHAR FullPath[256];
		TCHAR FileName[64];
		wstring FileRootName;
		wstring RootName;
		TCHAR ComName[64];

	}IMAGE_INFO;

	// For. Mesh


	// For.Frame
	typedef struct Frame_Derived : public D3DXFRAME
	{
		D3DXMATRIX		matCombineTransformationMatrix;
	}D3DXFRAME_DERIVED;


	// For.Mesh_Texture
	typedef struct MeshTexture
	{
		enum TYPETEXTURE { TEX_DIFFUSED, TEX_NORMAL, TEX_SPECULAR, TEX_END };

		LPDIRECT3DTEXTURE9	pTexture[TEX_END];
	}MESHTEXTURE;

	//For.Mesh_Container
	typedef struct MeshContainer_Derived : public D3DXMESHCONTAINER
	{
		MESHTEXTURE*				pMeshTexture;
		LPD3DXMESH					pOriginal_Mesh;
		D3DXVECTOR3					vMin, vMax;
		unsigned long				dwNumBones;      //현재 메시에 영향을 미치고있는 뼈들의 갯수.
		D3DXMATRIX**				ppCombinedTransformationMatrices;
		D3DXMATRIX*					pOffsetMatrices;
		D3DXMATRIX*					pRenderingMatrices;
	}D3DXMESHCONTAINER_DERIVED;

	// For. Material alpha
	typedef struct Material_Derived : public D3DXMATERIAL
	{
		bool	isAlpha;
	}D3DXMATERIAL_DERIVED;

	// For.Vertex Struct
	typedef struct Vertex_Color
	{
		
		D3DXVECTOR3		vPosition;	
		unsigned long	dwColor;

	}VTXCOL;

	typedef struct Particle
	{
		//enum STATE_PARTICLE { D3DFVF_XYZ | D3DFVF_DIFFUSE };

		D3DXVECTOR3		vPosition;   // D3DFVF_XYZ
		//_float			fSize;		 // D3DFVF_FSIZE;  원래는 이걸 사용해야하지만 대부분의 그래픽카드에서 지원X ==> 버텍스세이더에서 크기조절 가능
		D3DCOLOR		vColor;		 // D3DFVF_DIFFUSE
	}PARTICLE;

	typedef struct Vertex_Texture_ViewPort
	{
		D3DXVECTOR4		vPosition; // D3DFVF_XYZRHW(모든 렌더링파이프라인을 거친 이후의 정점 위치)
		D3DXVECTOR2		vTexUV; // D3DFVF_TEX1
	}VTXTEX_VIEWPORT;

	typedef struct Vertex_Texture
	{
		D3DXVECTOR3		vPosition; // D3DFVF_XYZ(3차원 로컬스페이스 상의 좌표)
		D3DXVECTOR2		vTexUV; // D3DFVF_TEX1
	}VTXTEX;

	typedef struct Vertex_Light
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		DWORD			vDiffuse;
		D3DXVECTOR2		vTexUV;
	}VTXTEXLIGHT;

	typedef struct Vertex_Texture_NormalTexture
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		//D3DXVECTOR3		vTangent;
		//D3DXVECTOR3		vBiNormal;
		D3DXVECTOR2		vTexUV;
	}VTXTEXNORMAL;

	typedef struct Vertex_Texture_Normal
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vTexUV;
	}VTXCUBETEX;

	// For.Index Struct
	typedef struct Index_16
	{
		unsigned short _0, _1, _2;
	}INDEX16;

	typedef struct Index_32
	{
		unsigned long _0, _1, _2;
	}INDEX32;

}