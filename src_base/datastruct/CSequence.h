/////////////////////////////////////////////////////////////////////////
///CSequenceInterface.h
///�����˽ӿ�CSequenceInterface
/////////////////////////////////////////////////////////////////////////

#ifndef _CSEQUENCEINTERFACE_H
#define _CSEQUENCEINTERFACE_H

#include "platform.h"

//�µ�CSequence���ܴ����������
class CSequence
{
public:
	//���췽��������������
	//ÿ��Ԥ�ƴ������Ŀ�����CurrentTime�������˲���Ҫ���¼�����
	//������һ���ƻ����ڵ��ۼ�ֵ�����ܴ���һ��һֱÿ��50000�ܵ�ϵͳ
	CSequence(UF_INT8 nGapValue = 50000);

	///�����������������ͷ���ռ�õĿռ�
	virtual ~CSequence();

	///��ȡ�����е���һ��ֵ
	///@return	��һ��ֵ
	virtual const char* getNextValueByString();
	virtual const UF_INT8 getNextValueByDouble();
	virtual const char* getNextValueByHex();

	//��ȡ��һ�������ǲ�����
	virtual const char* touchNextValueByString(UF_INT8 nNext = 1.0);
	virtual const UF_INT8 touchNextValueByDouble(UF_INT8 nNext = 1.0);
	virtual const char* touchNextValueByHex(UF_INT8 nNext = 1.0);

	///��ȡ����Ҫ�ĵ�ǰֵ
	///@return	��ǰֵ
	virtual const char* getCurrentValueByString();
	virtual const double getCurrentValueByDouble();
	virtual const char* getCurrentValueByHex();
	///��ʼ��������
	///@param nCurrValue ���г�ʼֵ
	virtual void initValueByTime(UF_INT8 nTime);
	virtual void initValueByValue(UF_INT8 nValue);
	virtual void initValueByValue(const char* sValue);
	virtual void initValueByHex(const char* sValue);
	virtual void initValueByValue(int nValue);

private:
	UF_INT8 m_digits;			//β����λ��
	UF_INT8 m_curValue;			//������
	char m_seqString[17];		//���ڷ���ֵ,���ܴ���IndexType��ֵ
	char m_seqTouchString[17];	//����Touch����ֵ,���ܴ���IndexType��ֵ
	char m_seqHex[14];			//���ڷ���ֵ����ʮ�����Ʒ���
	char m_seqTouchHex[14];		//���ڷ���ֵ����ʮ������Touch����
};


#endif
