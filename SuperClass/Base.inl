
CBase::CBase(void)
	: m_dwRefCnt(0)
{

}

CBase::~CBase(void)
{

}

unsigned long CBase::AddRef(void) 
{
	return ++(this->m_dwRefCnt);
}

unsigned long CBase::Release(void)
{
	if (0 == this->m_dwRefCnt)
	{
		unsigned long dwFreeCnt = 0;

		if (dwFreeCnt = Free())
			return dwFreeCnt;

		delete this;

		return 0;
	}
	else
		return this->m_dwRefCnt--;
}