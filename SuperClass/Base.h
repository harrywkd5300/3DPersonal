#pragma once

// ��� ������Ʈ�� �����ϴ� �������� Ŭ������ �θ𰡵ȴ�.
// RefCnt(��������)�� �����ϸ�, �����Ѵ�.
// �������� : ��ü���� �ּҸ� �����ϴ� �������� ������ ����
// (������ü�� �ּҸ� �����ϴ� �����ͺ����� ��� �������� �����Ѵ�.)

// abstract : Class�� �߻�Ŭ����ȭ ��Ų��.
class _declspec(dllexport) CBase abstract
{
protected:
	inline explicit CBase(void);
	inline virtual ~CBase(void);
public:
	// RefCnt�� ������Ų��.
	// ���ϰ� : ���������� ������Ų ������ ���� ����.
	inline unsigned long AddRef(void);
	// RefCnt�� ���ҽ�Ű��, RefCnt == 0�̶�� ��ü�� �����Ѵ�.
	// ���ϰ� : ���ҽ�Ű�� ������ ���� �����Ѵ�.
	inline unsigned long Release(void);
private:
	unsigned long	m_dwRefCnt = 0;	
public:
	virtual unsigned long Free(void) = 0;
};

#include "Base.inl"