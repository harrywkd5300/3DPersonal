#pragma once

// 모든 프로젝트에 존재하는 내가만든 클래스의 부모가된다.
// RefCnt(참조갯수)를 보관하며, 관리한다.
// 참조갯수 : 객체들의 주소를 보관하는 포인터형 변수의 갯수
// (원본객체의 주소를 보관하는 포인터변수의 경우 갯수에서 제외한다.)

// abstract : Class를 추상클래스화 시킨다.
class _declspec(dllexport) CBase abstract
{
protected:
	inline explicit CBase(void);
	inline virtual ~CBase(void);
public:
	// RefCnt를 증가시킨다.
	// 리턴값 : 참조갯수를 증가시킨 이후의 값을 리턴.
	inline unsigned long AddRef(void);
	// RefCnt를 감소시키되, RefCnt == 0이라면 객체를 삭제한다.
	// 리턴값 : 감소시키기 이전의 값을 리턴한다.
	inline unsigned long Release(void);
private:
	unsigned long	m_dwRefCnt = 0;	
public:
	virtual unsigned long Free(void) = 0;
};

#include "Base.inl"