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
		D3DXVECTOR3		vPositon;   // ���� �����̽����� ��ġ
		D3DXVECTOR3		vVelocity;	// ��ƼŬ�� �ӵ�, �ʴ� ��ġ
		D3DXVECTOR3		vAcceleration; // ��ƼŬ�� ���ӵ�, �ʴ���ġ
		float			fLifeTime;	// ��ƼŬ�� �Ҹ��ҋ����� �����ð�
		float			fAge;		// ��ƼŬ�� ���� ����
		D3DXCOLOR		dwColor;	// ��ƼŬ�� �÷�
		D3DXCOLOR		dwColorFade; // ��ƼŬ�� �÷����ð��� �帧�� ��������ϴ¹��
		bool			bisAlive;	//��ƼŬ�� ����true, �Ҹ� false
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
		unsigned long				dwNumBones;      //���� �޽ÿ� ������ ��ġ���ִ� ������ ����.
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
		//_float			fSize;		 // D3DFVF_FSIZE;  ������ �̰� ����ؾ������� ��κ��� �׷���ī�忡�� ����X ==> ���ؽ����̴����� ũ������ ����
		D3DCOLOR		vColor;		 // D3DFVF_DIFFUSE
	}PARTICLE;

	typedef struct Vertex_Texture_ViewPort
	{
		D3DXVECTOR4		vPosition; // D3DFVF_XYZRHW(��� ������������������ ��ģ ������ ���� ��ġ)
		D3DXVECTOR2		vTexUV; // D3DFVF_TEX1
	}VTXTEX_VIEWPORT;

	typedef struct Vertex_Texture
	{
		D3DXVECTOR3		vPosition; // D3DFVF_XYZ(3���� ���ý����̽� ���� ��ǥ)
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