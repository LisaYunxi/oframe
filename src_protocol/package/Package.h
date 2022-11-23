#ifndef _PACKAGE_H__
#define _PACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

//ʵ��ʹ�ð������������
#define PACKAGE_MAX_SIZE		3560
#define PACKAGE_RESERVE_SIZE	512

//CPackageBuffer����CPackage�����������Ĵ�ŵط�������¼
//��CPackage���󣬶����ݻ���ȡ�����ø���

class  CPackageBuffer
{
public:
	//���캯��
	// @param length���������ݻ������Ĵ�С 
	// @remark ʹ�øù��캯���������������������ɱ��������
	CPackageBuffer(int length);
	
	//���캯��
	// @param pData �ѷ���Ļ�������ַ
	// @param length ���������ݻ������Ĵ�С 
	// @remark ʹ�øù��캯���������󲻹�����������������
	CPackageBuffer(char *pData, int length);

	virtual ~CPackageBuffer();
	
	//���ػ��������׵�ַ
	// @return  �������ĵ�ַ
	inline char *Data();
	
	//���ػ������ĳ���
	// @return  �������ĳ���
	inline int Length();
	
	//���Ӷ����������������
	void AddRef();
	
	//�ͷ����������
	void Release();

protected:
	char *m_pData;			//ָ�򻺳������ݵ�ָ��
	int m_nLength;			//����������
	int m_nRefCount;		//�����������ø���
	bool m_bReleaseData;	//�Ƿ������������������
};

inline char *CPackageBuffer::Data()
{
	return m_pData;
}

inline int CPackageBuffer::Length()
{
	return m_nLength;
}

class  CPackage  
{
public:
	CPackage();
	virtual ~CPackage();
	
	//������ݰ�
	void Clear();

	//���ݰ��ϴ�ʱ���ϲ�Э��ı�ţ��������ʵ�ִ˺���
	// @return �ϲ�Э��ı��
	virtual unsigned int GetActiveID();

	//�����������Ƿ񹹳�һ�������İ������ж�������ݣ���ص���
	// @return >=0 ��һ�������İ� 
	// @return -1 ���ݰ�������
	// @return <-1 ���ݰ��д���
	// @remark ������һ��Ҫ���ظú�����ʵ�����Ѷ����ݵ�У��
	virtual int ValidPackage();

	//��װ���ݰ�
	// @return 0 ��װ�ɹ�
	// @return <0 ��װʧ��
	// @remark ������һ��Ҫ���ظú�������ɰ�װ��ͷ�ȹ���
	virtual int MakePackage();

	//ָ��PackageBuffer��������ָ��m_pHeadǰ��size����
	// @param size ָ���ƶ��Ĵ�С
	// @return �����ƶ�m_pHead��ĵ�ַ
	char *Push(int size);
	
	//ָ��PackageBuffer��������ָ��m_pHead����size����
	// @param size ָ���ƶ��Ĵ�С
	// @return �����ƶ�m_pHead��ĵ�ַ
	char *Pop(int size);
	
	//�������д�����ݵĳ���
	// @return ������ݵĳ���
	inline int Length();
	
	//�������д�����ݵĵ�ַ
	// @return ������ݵĵ�ַ
	inline char *Address();
	
	//����һ�����Ļ�����������������
	// @param pPackage ָ��Դ����ָ��
	void BufAddRef(CPackage *pPackage);
	
	//���뱾����Buff������
	// @return ����Buff�ĵ�ַ
	CPackageBuffer *BufRelease();
	
	//ʵ��Ϊpackage����ռ�
	// @param nCapacity package�Ŀ��ÿռ��С
	// @param nReserve package�����ռ��С
	void ConstructAllocate(int nCapacity=PACKAGE_MAX_SIZE, int nReserve=PACKAGE_RESERVE_SIZE);

	//�õ�package������ͷ�ĳ��ȣ������
	int GetHeaderLength();

	//�ӹ�����PackagBuffer�Ϸ���һ��ռ䡣
	// �ƶ����ȷ��䣬���ڴ���ˮ�����ȡ�ƶ�����package�ı���֮����Ҫ��ӱ�ͷ����Ϊ
	// @param length ����ռ�Ĵ�С
	// @return ����ռ���׵�ַ����δ����PackgeBuffer��PackageBuffer�ռ䲻�㣬�򷵻�NULL
	char* Allocate(int length);

	//��Ҫ������ݵ�package����֮ǰ�ĳ�ʼ����Head��Ҫ���пռ������ͷ
	char* AllocateToAdd();

	//����ˮ�����ȡ��Package����ĳ�ʼ����Head��Ҫ��һ����ͷ���ռ�
	char* AllocateToRead();

	//������β����ȥһ��
	// @param newLength ������β����ȥ�ĳ���
	void Truncate(int newLength);
	
	//���ٶԱ��������ã�������Ϊ0ʱ������������
	void Release();
	
	//���ӶԱ���������
	void AddRef();
	
	//��Դ�������ݿ�����������
	// @param pSrcPackage Դ��
	// @remark ���������Ѿ��������㹻�Ŀռ�
	void DupPackage(CPackage *pSrcPackage);

	void DupData(CPackage *pSrcPackage);

	//��ΪpHead�Ѿ��ŵ�������������ǰ��
	//attch֮����makepackage��ֻ��vilidpackage
	//������Ĳ��ܷ���
	void AttachBuffer(CPackageBuffer *pPackageBuffer);

	//���ð��е�����
	// @param pData ָ�����ݵ�ָ��
	// @param nDataLen ���ݵĳ���
	// @remark �ú����������PackageBuffer��ֻ��������ͷ��βָ�룬
	// @remark ��ˣ�����ֻ�����ڶ�ȡ����������д������
	void SetData(char *pData, int nDataLen);

	char* OutputHeader(char* header);

	CPackageBuffer* GetPackageBuffer();

protected:
	CPackageBuffer *m_pPackageBuffer;   /// ָ�����ݻ�������ָ��
	char *m_pHead;						/// ָ�����ݻ�������ͷָ��
	char *m_pTail;						/// ָ�����ݻ�������βָ��
	int m_nRefCount;					/// �Ա��������ø���
	int m_nReserve;						/// �����ռ��С��������ͷʹ��
	int m_nHeadLength;					/// ����ͷ�ĳ���
	int m_nCapacity;					/// ��������
};

inline int CPackage::Length()
{
	return (int)(m_pTail-m_pHead);
}

inline char *CPackage::Address()
{
	return m_pHead;
}

#endif
