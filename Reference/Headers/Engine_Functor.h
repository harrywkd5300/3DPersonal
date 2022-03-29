#pragma once

namespace Engine
{
	class CStringCmp
	{
	private:
		const TCHAR* m_pKey;

	public:
		CStringCmp() {}
		CStringCmp(const TCHAR* _pKey) :m_pKey(_pKey) {}
		~CStringCmp() {}

	public:
		template<typename T>
		bool operator ()(T& pKey)
		{
			return !lstrcmp(pKey.first, m_pKey);
		}
	};

	class CFind_Tag
	{
	public:
		explicit CFind_Tag(const TCHAR* pFindTag) : m_pTag(pFindTag) {}	
		~CFind_Tag() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{		
			return !lstrcmp(Pair.first, m_pTag);
		}	
	private:
		const TCHAR*			m_pTag = nullptr;
	};

	class CRelease_Pair
	{
	public:
		explicit CRelease_Pair() {}
		~CRelease_Pair() = default;		
	public:
		template <typename T>
		void operator () (T& Pair)
		{
			if (nullptr != Pair.second)
			{
				Pair.second->Release();
				Pair.second = nullptr;
			}
		}
	};

	class CRelease_Single
	{
	public:
		explicit CRelease_Single() {}
		~CRelease_Single() = default;
	public:
		template <typename T>
		void operator () (T& pInstance)
		{
			if (nullptr != pInstance)
			{
				pInstance->Release();
				pInstance = nullptr;
			}
		}
	};
}
