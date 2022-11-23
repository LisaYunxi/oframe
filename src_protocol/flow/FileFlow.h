#ifndef AFX_FILEFLOW_H__
#define AFX_FILEFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"
#include "Flow.h"
#include "Mutex.h"

struct SFlowFileInfo
{
	FILE *pContentFile;		//�ļ�ָ��
	string FileName;		//�ļ���
	UF_INT8 BeginID;
};

typedef map<int,SFlowFileInfo*> VFileInfoMap;

//CLongFileFlow���ļ����࣬��ʹ�������ļ�����������һ���ļ��������ƫ������
// ��һ���ļ�����������ݡ�Ϊ������ٶȣ�ÿBLOCK_SIZE��������һ��ƫ������
// �ļ������ļ���������Ŀ¼�ڹ���ʱָ����

class CFileFlow : public CFlow  
{
public:
	//���캯��
	// @param pszFlowName ���ļ������ƣ��������ļ������������Զ�����"flow"����չ��
	// @param pszPath ���ļ�������Ŀ¼
	// @�쳣  CRuntimeError	����ļ���Ϊ�ջ��ļ���д�������׳����쳣
	// @remark ��������ļ�������ʱ������ļ��ĺϷ��ԡ�
	
	CFileFlow(const char *pszFlowName, const char *pszPath, bool bReUse = false,int nFlowGap=LONGFILEFLOWGAP);
	
	virtual ~CFileFlow();

	//����β���İ��ص�
	// @param nCount �����İ��ĸ���
	virtual bool Truncate(UF_INT8 nCount); 
	
	//����������һ������
	// @param	pObject Ҫ���ӵĶ���
	// @param length �������ݵĳ���
	// @return ���������е���ţ�С��0��ʾ����ʧ��
	virtual UF_INT8 Append(void *pObject, int length);

	//��ȡָ����ŵĶ���
	// @param	id	�������
	// @param pObject ��ȡ����Ļ�����
	// @param length ��������С
	// @return ȡ������ĳ���
 	virtual int Get(UF_INT8 id, void *pObject, int length);

protected:	
	//��ʼ�����������ļ�
	// @return true ��ʼ���ɹ�  false ��ʼ��ʧ��
	bool InitFile(UF_INT8 nCount);
	
	//���´����ļ�
	// @param	szIdFilename	�洢��ַ���ļ���
	// @param	szContentFilename	�洢���ݵ��ļ���
	// @param	bReuse	�Ƿ����ô��ļ�
	// @�쳣	CRuntimeError	����ļ���Ϊ�ջ��ļ���д�������׳����쳣
	FILE* OpenFile(int nFileIndex);

	//��ȡдλ��
	fpos_t GetPos(int nFileIndex);

	fpos_t GetPos(int nFileIndex, UF_INT8 id);

	//��ȡһ��index�����������к�
	UF_INT8 GetIndexMaxCount(int nFileIndex);

	//�رմ��������ļ�
	void CloseFile(SFlowFileInfo* pFlowFileInfo);
	virtual int GetFlowType();

private:
	VFileInfoMap m_flowIndex;		// �Ѿ��������ļ�������
	int m_nFirstIndex;				// ��һ��λ��
	fpos_t m_nCurrOffset;			// ���´洢�ļ������ݳ���
	int m_nCurrIndex;				// д���λ��

	//���ļ�ʹ��
	fpos_t m_nReadOffset;			// ���һ�ζ�ȡ�İ����ļ��е�ƫ����
	UF_INT8 m_nReadId;				// ���һ�ζ�ȡ�İ������
	int m_nReadIndex;				// ���һ�ζ�ȡ���ļ���λ��

	//CRITICAL_VAR m_criticalVar;		// �ٽ�������
	CRWLock m_RWLock;
	char m_sPreFileName[128];		// �ļ���ǰ׺
	int m_nLongFileFlowGap;			//����gapһ���ļ�
	char m_sError[512];				//������Ϣ��ʱ����
};

#endif
