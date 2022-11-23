/////////////////////////////////////////////////////////////////////////
///UFDataType.h
///�������ڲ����ݵĵײ�֧����
/////////////////////////////////////////////////////////////////////////

#ifndef _UFDATATYPE_H__
#define _UFDATATYPE_H__

#include "platform.h"
#include "BigDigit.h"

class CUFBaseStringType
{
};

class CUFBaseIntType
{
};

class CUFBaseLongType
{
};

class CUFBaseFloatType
{
};

class CUFBaseCharType
{
};

/////////////////////////////////////////////////////////////////////////
///CUFStringType<int length>��һ�������ַ���ģ���࣬ͨ��ָ�����ȣ�
///���Եõ���������ʹ�õ��ࡣCUFStringType<int length>ʵ���Ͼ��Ǹ��ݳ��ȣ�
///ֱ�ӱ����ַ�����ֵ������룬����'\0'��
/////////////////////////////////////////////////////////////////////////
template <int length> class CUFStringType : public CUFBaseStringType
{
public:
	///���캯��
	CUFStringType(void)
	{
		//buffer[0] = '\0';
		clear();
	}

	///���ƹ�����
	///@param	s	�����Դ����
	CUFStringType(const CUFStringType<length>& s)
	{
		setValue(s.getValue());
	}

	///���ƹ�����
	///@param	p	�����Դ����
	CUFStringType(const char* p)
	{
		setValue(p);
	}

	CUFStringType(const char p)
	{
		setValue(p);
	}

	///���ڲ�����
	///@param	s	���ڵ�ֵ
	///@return	�����ֵ
	const CUFStringType& operator =(const CUFStringType<length>& s)
	{
		setValue(s.getValue());
		return s;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	p	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* p)
	{
		setValue(p);
		return p;
	}

	const char* operator +=(const char* p)
	{
		if (strlen(p) + strlen(buffer) > length)
			return NULL;

		strcat(buffer, p);
		buffer[length] = '\0';
		return buffer;
	}

	const char* operator +=(const char p)
	{
		int mylength = strlen(buffer);
		if (1 + mylength > length)
			return NULL;

		buffer[mylength] = p;
		buffer[mylength + 1] = '\0';
		buffer[length] = '\0';
		return buffer + mylength;
	}

	const char* operator +=(const double v)
	{
		string tmp = doubleToString(v, 12);
		return (*this) += tmp.c_str();
	}

	///���ڲ��������������л�ȡ
	///@param	v	���ڵ�����
	///@return	���������
	const int operator =(const int v)
	{
		char tmp[100];
		sprintf(tmp, "%d", v);
		setValue(tmp);
		return v;
	}

	const double operator =(const double v)
	{
		string tmp = doubleToString(v, 12);
		setValue(tmp.c_str());
		return v;
	}

	///���ڲ���������64λ�������л�ȡ     liumeng 20140820
	///@param	v	���ڵĳ�����
	///@return	����ĳ�����
	const long operator =(const long v)
	{
		char tmp[100];
		sprintf(tmp, "%*ld", length, v);
		setValue(tmp);
		return v;
	}

	const char operator =(const char v)
	{
		setValue(v);
		return v;
	}

	///����ַ�������
	void clear(void)
	{
		buffer[0] = '\0';
	}

	void init(void)
	{
		buffer[0] = '\0';
	}

	///��ȡ�ַ�����ֵ
	///@return	�ַ�����ֵ
	const char* getValue(void) const
	{
		return buffer;
	}

	///�����ַ�����ֵ
	///@param	s	Ҫ���õ��ַ���
	void setValue(const char* s)
	{
		if (s == NULL)
			buffer[0] = '\0';
		else
		{
			strncpy(buffer, s, length);
			buffer[length] = '\0';
		}
	}

	bool contains(const char* s)
	{
		if (s == NULL)
			return false;
		int slength = strlen(s);
		return !strncmp(buffer, s, slength);
	}

	void setValue(const char s)
	{
		buffer[0] = s;
		buffer[1] = '\0';
	}

	///��strBuf�еõ������͵��ַ�����ʾ	
	///@param	strBuf	���ڷ��ý�����ַ���
	const string getString() const
	{
		return buffer;
	}

	const string getAllString() const
	{
		return buffer;
	}

	const int getInt() const
	{
		return atoi(buffer);
	}

	const double getDouble() const
	{
		return atof(buffer);
	}

	///��ȡ�ڲ���ָ�룬�������ʹ��
	char* getBuffer()
	{
		return buffer;
	}

	int getLength()
	{
		return length + 1;
	}

	int getSize()
	{
		return strlen(buffer);
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (buffer[0] == '\0')
			return true;
		else
			return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;
		for (int i = 0; i < length; i++)
		{
			if (buffer[i] == '\0')
				break;
			if (!(buffer[i] >= '!' && buffer[i] <= '~'))
				return false;
			//���š����š��ո���
			if (buffer[i] == '\'' || buffer[i] == '\"' || buffer[i] == ',' || buffer[i] == ' ' || buffer[i] == '%')
				return false;
		}
		return true;
	}

	///ת��Ϊ�ַ���
	///@return	�õ����ַ���
	operator const char* (void) const
	{
		return buffer;
	}

	///���ұߵĿո�ɾ��
	void trimRight(void)
	{
		char* p = buffer + strlen(buffer) - 1;
		while (p >= buffer)
		{
			if (*p != ' ')
				break;
			*p = '\0';
			p--;
		}
	}

	///�ж��Ƿ�С��ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator < (const char* r) const
	{
		return strcmp(buffer, r) < 0;
	}

	///�ж��Ƿ����ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator > (const char* r) const
	{
		return strcmp(buffer, r) > 0;
	}

	///�ж��Ƿ�С�ڵ���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator <= (const char* r) const
	{
		return strcmp(buffer, r) <= 0;
	}

	///�ж��Ƿ���ڵ���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator >= (const char* r) const
	{
		return strcmp(buffer, r) >= 0;
	}

	///�ж��Ƿ����ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator == (const char* r) const
	{
		return strcmp(buffer, r) == 0;
	}

	bool operator == (const char r) const
	{
		if (buffer[1] == '\0')
		{
			if (buffer[0] == r)
				return true;
		}
		return false;
	}

	bool operator == (const double r) const
	{
		char tmp[100];
		snprintf(tmp, 100, "%0.f", r);
		return strcmp(buffer, tmp) == 0;
	}

	bool operator == (const int r) const
	{
		char tmp[100];
		sprintf(tmp, "%*d", length, r);
		return strcmp(buffer, tmp) == 0;
	}

	///�ж��Ƿ񲻵���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator != (const char* r) const
	{
		return strcmp(buffer, r) != 0;
	}

	bool operator != (const char r) const
	{
		if (buffer[1] == '\0')
		{
			if (buffer[0] == r)
				return false;
		}
		return true;
	}

	bool operator != (const double r) const
	{
		char tmp[100];
		snprintf(tmp, 100, "%0.f", r);
		return strcmp(buffer, tmp) != 0;
	}

	bool operator != (const int r) const
	{
		char tmp[100];
		sprintf(tmp, "%*d", length, r);
		return strcmp(buffer, tmp) != 0;
	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		const char* str = buffer;

		if (*str == '\0')
		{
			return(ret);
		}
		long n = 0x100;
		while (*str)
		{
			unsigned long v = n | (*str);
			n += 0x100;
			int r = (int)((v >> 2) ^ v) & 0x0f;
			ret = (ret << r) | (ret >> (32 - r));
			ret &= 0xFFFFFFFFL;
			ret ^= v * v;
			str++;
		}
		return ((ret >> 16) ^ ret);
	}


protected:
	///�洢�ַ�����ֵ
	char buffer[length + 1];
};

#define DefineStringType(length,newType)				\
	typedef CUFStringType<length> C ## newType;			\
	typedef const CUFStringType<length> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFFloatType��һ���ڲ����ݵĸ�������
///@author	xuzh
///@version	2.0,20060218
/////////////////////////////////////////////////////////////////////////
///xuzh��20070328
///�����˶Ը�������Χ���ж�����ֹ���ֳ�Խ��Χ������
template <int length, int precision> class CUFFloatType : public CUFBaseFloatType
{
public:
	///���캯��
	CUFFloatType(void)
	{
		clear();
	}

	///���ƹ�����
	///@param	f	�����Դ����
	CUFFloatType(const CUFFloatType& f)
	{
		setValue(f.getValue());
	}

	///���ƹ�����
	///@param	value	�����Դ����
	CUFFloatType(const double value)
	{
		setValue(value);
	}

	///���ڲ�����
	///@param	f	���ڵ�ֵ
	///@return	�����ֵ
	const CUFFloatType& operator =(const CUFFloatType& f)
	{
		setValue(f.getValue());
		return f;
	}

	///���ڲ��������Ӹ������л�ȡ
	///@param	v	���ڵĸ�����
	///@return	������ָ�����
	const int operator =(const int v)
	{
		setValue(v);
		return v;
	}

	const double operator =(const double v)
	{
		setValue(v);
		return v;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	v	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atof(v));
		return v;
	}

	///������ݣ����ǽ�ֵ��Ϊ0
	void clear(void)
	{
		value = DOUBLE_NaN;
	}

	void init(void)
	{
		value = 0.0;
	}

	///��ȡֵ
	///@return	��ȡ��ֵ
	const double getValue(void) const
	{
		return value;
	}

	///����ֵ
	///@param	v	Ҫ���õ�ֵ
	void setValue(const double v)
	{
		value = v;
	}

	void toRoundTick(const double v)
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return;

		//if (fabs(value - v) <= getCompareTick())
		//	return;

		double ticks = floor(value / v + 0.51);
		setValue(v * ticks);
	}

	void toLowerTick(const double v)
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return;

		//if (fabs(value - v) <= getCompareTick())
		//	return;

		double ticks = floor(value / v + 0.1);
		setValue(v * ticks);
	}

	void toTick(const double v)
	{
		toLowerTick(v);
		//if (value == DOUBLE_NaN || v == DOUBLE_NaN)
		//	return;

		////if (fabs(value - v) <= getCompareTick())
		////	return;

		////double ticks = floor((value + v / 10) / v + SMALL_DOUBLE);
		////double ticks = floor((value + v / 10) / v);
		//double ticks = floor((value / v + 0.1);
		//setValue(v * ticks);
	}

	void toUpperTick(const double v)
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return;

		//if (fabs(value - v) <= getCompareTick())
		//	return;

		double ticks = ceil(value / v - 0.1);
		setValue(v * ticks);
	}

	//����������ں����������������ʱ��Ϊ���ܼ���׼ȷ����������������
	//���ڱ���10λС�����ӵھ�λ��ʼ������Ҫ����һλ��
	//void cutByBigFloat(const double tick1, const double tick2)
	//{
	//	toRoundTick(min(tick1, tick2));
	//}

	const bool isTick(const double v) const
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return true;

		//���ֵ����Ҳ�������
		double maxValue = 2621440000000000 * v;
		if (value > maxValue || value < -maxValue)
			return false;

		//if (fabs(value - v) <= getCompareTick())
		//	return true;

		//double ticks = floor((value + v / 10) / v + SMALL_DOUBLE);
		//double ticks = floor((value + v / 10) / v);
		double ticks = floor(value / v + 0.1);

		//return IsEqual(tickValue);
		//if (fabs(value - tickValue) <= getCompareTick(value))
		//	return true;
		//else
		//	return false;

		return operator==(v * ticks);

		//if (fabs(tickValue - value) <= SMALL_DOUBLE)
		//	return true;
		//else
		//{
		//	bool x = IsEqual(tickValue);
		//	return false;
		//}
	}

	const bool isIntTick(const double v) const
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return true;

		//if (fabs(value - v) <= getCompareTick())
		//	return true;

		//double ticks = floor((value + v / 10) / v + SMALL_DOUBLE);

		//double ticks = floor((value + v / 10) / v);
		double ticks = floor(value / v + 0.1);

		//�۸���tickֵ��Ϊ���������ܴ���int�����ֵ�����������Ҫʹ��getTick��ֵ����Ҫ������ж�
		double tickValue = v * ticks;

		if (sizeof(void*) == 4)
		{
			if (tickValue > 0x7FFFFFFF || tickValue < -0x7FFFFFFF)
				return false;
		}
		else
		{
			if (tickValue > 0x7FFFFFFFFFFFFFF || tickValue < -0x7FFFFFFFFFFFFFF)
				return false;
		}

		//if (fabs(tickValue - value) <= SMALL_DOUBLE)
		//	return true;
		//else
		//	return false;

		//return IsEqual(tickValue);
		return operator==(tickValue);
	}

	const size_t getTicks(const double v) const
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0)
		{
			if (sizeof(void*) == 4)
				return INT_32_MAX;
			else
				return INT_64_MAX;
		}

		//int ticks = (int)(floor((value + v / 10) / v + SMALL_DOUBLE));
		//int ticks = (int)(floor((value + v / 10) / v ));
		size_t ticks = (size_t)(floor(value / v + 0.1));
		return ticks;
	}

	const string getString() const
	{
		return doubleToString(value, precision);
	}

	const string getDiffString(double v) const
	{
		return doubleToStringFormat(value - v);
	}

	const string getAllString() const
	{
		return doubleToStringFormat(value);
	}

	//const string getString(int nPrecision)
	//{
	//	char strBuf[32] = { 0 };
	//	if (value != DOUBLE_NaN)
	//		snprintf(strBuf, 32, "%.*lf", nPrecision, value);
	//	return strBuf;
	//}

	const int getInt() const
	{
		return value;
	}

	const double getDouble() const
	{
		return value;
	}

	const double getInverse() const
	{
		if (operator==(0.0))
			return 1.0;
		else
			return 1.0 / value;
	}

	const double getDiff(double v) const
	{
		return value - v;
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (value == DOUBLE_NaN)
			return true;
		else
			return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;

		if (::isnan(value))
			return false;

		if (::isinf(value))
			return false;

		if (value >= pow(10, length + 1 - precision))
			return false;

		if (value <= -pow(10, length + 1 - precision))
			return false;

		return true;
	}

	bool isValid(int nPrecision)
	{
		if (value > getMaxValue(nPrecision))
			return false;
		if (value < -getMaxValue(nPrecision))
			return false;
		return true;
	}

	bool isValid(double tick)
	{
		int nPrecision = (int)(1 / tick);
		if (value > getMaxValue(nPrecision))
			return false;
		if (value < -getMaxValue(nPrecision))
			return false;
		return true;
	}

	///ת��Ϊʵ��
	///@return	�õ���ʵ��
	operator const double(void) const
	{
		return value;
	}

	///����ĳ��ֵ
	///@param	v	Ҫ�ӵ�ֵ
	///@return	�����Ժ�Ľ��
	const double operator += (const double v)
	{
		//������Ϊ�˷�ֹ����-0.00000�����������ĳЩ������������ڼ�������-31.0+31.0ʱ���õ��Ľ����-0.0������0.0
		//����֮�����Ҫ����
		double r = value + v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			value = 0.0;
		else
			value = r;
		return v;
	}

	///��ȥĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�����Ժ�Ľ��
	const double operator -= (double v)
	{
		//������Ϊ�˷�ֹ����-0.00000�����������ĳЩ������������ڼ�������-31.0+31.0ʱ���õ��Ľ����-0.0������0.0
		//����֮�����Ҫ����
		double r = value - v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			value = 0.0;
		else
			value = r;
		return v;
	}

	CUFFloatType operator - (double v)
	{
		//������Ϊ�˷�ֹ����-0.00000�����������ĳЩ������������ڼ�������-31.0+31.0ʱ���õ��Ľ����-0.0������0.0
		//����֮�����Ҫ����
		double r = value - v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			r = 0.0;
		return CUFFloatType(r);
	}

	CUFFloatType operator + (double v)
	{
		//������Ϊ�˷�ֹ����-0.00000�����������ĳЩ������������ڼ�������-31.0+31.0ʱ���õ��Ľ����-0.0������0.0
		//����֮�����Ҫ����
		double r = value + v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			r = 0.0;
		return CUFFloatType(r);
	}

	///����ĳ��ֵ
	///@param	v	Ҫ�˵�ֵ
	///@return	�����Ժ�Ľ��
	const double operator *= (const double v)
	{
		setValue(value * v);
		return v;
	}

	///����ĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�����Ժ�Ľ��
	const double operator /= (const double v)
	{
		setValue(value / v);
		return v;
	}

	///����ĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�����Ժ�Ľ��
	//const int operator &(const double v) const
	//{
	//	//�����˵�value/v�Ľ��Ϊ��ֵʱ�Ĵ���
	//	//	double x = (value + SMALL_DOUBLE )/ v;
	//	//	double x1 = (value) / v;
	//	//	int b = (int)x;
	//	//	double y = x + SMALL_DOUBLE;
	//	//	double z = floor(x);
	//	//	double z1 = floor(x1);
	//	//	int a1 = (int)z1;
	//	//	int a = (int)z;		
	//	//	return a;
	//	return (int)(floor((value + SMALL_DOUBLE) / v + SMALL_DOUBLE));
	//}

	///�ж��Ƿ�С��ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator < (double r) const
	{
		return value < r - doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ����ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator > (double r) const
	{
		return value > r + doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ�С�ڵ���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator <= (double r) const
	{
		return value <= r + doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ���ڵ���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator >= (double r) const
	{
		return value >= r - doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ����ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator == (double r) const
	{
		return fabs(value - r) <= doubleCompareSmall(value, precision);
	}

	bool equal (double r, int margin) const
	{
		if (margin <= 0)
			margin = 1;
		return fabs(value - r) <= doubleCompareSmall(value, precision) * margin;
	}

	bool equal(double r, double tick) const
	{
		return fabs(value - r) < tick;
	}


	///�ж��Ƿ񲻵���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator != (double r) const
	{
		return fabs(value - r) > doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ�С��ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator < (int r) const
	{
		return value < r - doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ����ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator > (int r) const
	{
		return value > r + doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ�С�ڵ���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator <= (int r) const
	{
		return value <= r + doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ���ڵ���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator >= (int r) const
	{
		return value >= r - doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ����ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator == (int r) const
	{
		return fabs(value - r) <= doubleCompareSmall(value, precision);
	}

	///�ж��Ƿ񲻵���ĳ��ֵ
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator != (int r) const
	{
		return fabs(value - r) > doubleCompareSmall(value, precision);
	}

	double getCompareTick() const
	{
		return doubleCompareTick(value, precision);
	}

	int getPrecision()
	{
		return precision;
	}

	//void upPrecision(int nUpNumber = 1)
	//{
	//	return;
	//	if (value == DOUBLE_NaN || value == 0.0)
	//		return;

	//	double compareTick = doubleCompareTick(value, precision);

	//	if (fabs(value) < compareTick)
	//	{
	//		value = 0.0;
	//		return;
	//	}

	//	//if (nUpNumber >= 1)
	//	//	compareTick *= pow(10, nUpNumber);

	//	int i = 0;
	//	while (i < nUpNumber)
	//	{
	//		compareTick *= 10;
	//		i++;
	//	}

	//	//ֻ���ۼӵ��ֶ���Ҫ���ۼ�ֵ���а���tick����
	//	double ticks = floor(value / compareTick + 0.1);
	//	value = compareTick * ticks;
	//}

	//bool IsEqual(double r) const
	//{
	//	return doubleEqual(value, r);
	//}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		const char* str = (char*)&value;
		int i;

		long n = 0x100;
		for (i = 0; i < sizeof(double); i++)
		{
			unsigned long v = n | (*str);
			n += 0x100;
			int r = (int)((v >> 2) ^ v) & 0x0f;
			ret = (ret << r) | (ret >> (32 - r));
			ret &= 0xFFFFFFFFL;
			ret ^= v * v;
			str++;
		}
		return ((ret >> 16) ^ ret);
	}

protected:
	///�洢��������ֵ
	double value;
};

#define DefineFloatType(maxValue,minValue,newType)			\
	typedef CUFFloatType<maxValue,minValue> C ## newType;	\
	typedef const CUFFloatType<maxValue,minValue> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFIntType��һ���ڲ����ݵ�������
///@author	xuzh
///@version	2.0,20060218
/////////////////////////////////////////////////////////////////////////
class CUFIntType : public CUFBaseIntType
{
public:
	///���캯��
	CUFIntType(void)
	{
		clear();
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFIntType(const CUFIntType& i)
	{
		setValue(i.getValue());
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFIntType(const int i)
	{
		setValue(i);
	}

	///���ڲ�����
	///@param	i	���ڵ�ֵ
	///@return	�����ֵ
	const CUFIntType& operator =(const CUFIntType& i)
	{
		setValue(i.getValue());
		return i;
	}

	const long& operator =(const long i)
	{
		value = i;
		return i;
	}

	const unsigned long& operator =(const unsigned long i)
	{
		value = i;
		return i;
	}

	///���ڲ��������������л�ȡ
	///@param	i	���ڵ�����
	///@return	���������
	const int operator =(const int i)
	{
		setValue(i);
		return i;
	}

	const unsigned int operator =(const unsigned int i)
	{
		value = i;
		return i;
	}

	const double operator =(const double i)
	{
		value = (int)i;
		return i;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	v	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atoi(v));
		return v;
	}

	///������ݣ����ǽ�ֵ��Ϊ0
	void clear(void)
	{
		value = INT_32_MAX;
	}

	void init(void)
	{
		value = 0;
	}

	///��ȡֵ
	///@return	��ȡ��ֵ
	const int getValue(void) const
	{
		return value;
	}

	///����ֵ
	///@param	v	Ҫ���õ�ֵ
	void setValue(const int v)
	{
		value = v;
	}

	///��strBuf�еõ������͵��ַ�����ʾ	
	///@param	strBuf	���ڷ��ý�����ַ���
	const string getString() const
	{
		if (value == INT_32_MAX)
			return "";
		char buf[19];
		sprintf(buf, "%d", value);
		return buf;
	}

	const string getAllString() const
	{
		return getString();
	}

	const int getInt() const
	{
		return value;
	}

	const double getDouble() const
	{
		return value;
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (value == INT_32_MAX)
			return true;
		else
			return false;
	}

	bool isTick(const int v) const
	{
		if (value % v == 0)
			return true;
		else
			return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		return true;
	}

	///ת��Ϊ����
	///@return	�õ�������
	operator const int(void) const
	{
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�ӵ�ֵ
	///@return	�õ���ֵ
	const int operator += (const int v)
	{
		setValue(value + v);
		return value;
	}

	///�������ȥĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const int operator -= (const int v)
	{
		setValue(value - v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�˵�ֵ
	///@return	�õ���ֵ
	const int operator *= (const int v)
	{
		setValue(value * v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const int operator /= (const int v)
	{
		setValue(value / v);
		return value;
	}

	///������ģĳ��ֵ
	///@param	v	Ҫģ��ֵ
	///@return	�õ���ֵ
	const int operator %= (const int v)
	{
		setValue(value % v);
		return value;
	}

	bool operator <= (int r) const
	{
		return value <= r;
	}

	bool operator >= (int r) const
	{
		return value >= r;
	}

	bool operator == (int r) const
	{
		return value == r;
	}

	bool operator > (int r) const
	{
		return value > r;
	}

	bool operator < (int r) const
	{
		return value < r;
	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		unsigned  long v = 0x100 | value;
		int r = (int)((v >> 2) ^ v) & 0x0f;
		ret = (ret << r) | (ret >> (32 - r));
		ret &= 0xFFFFFFFFL;
		ret ^= v * v;
		return ((ret >> 16) ^ ret);
	}

protected:
	///�洢������ֵ
	int value;
};

typedef const CUFIntType CReadOnlyUFIntType;

/////////////////////////////////////////////////////////////////////////
///CUFLongType��һ���ڲ����ݵĳ�������
/////////////////////////////////////////////////////////////////////////
class CUFLongType : public CUFBaseLongType
{
public:
	///���캯��
	CUFLongType(void)
	{
		clear();
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFLongType(const CUFLongType& i)
	{
		value = i.getValue();
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFLongType(const UF_INT8 i)
	{
		value = i;
	}

	///���ڲ�����
	///@param	i	���ڵ�ֵ
	///@return	�����ֵ
	const CUFLongType& operator =(const CUFLongType& i)
	{
		value = i.getValue();
		return i;
	}

	const int operator =(const unsigned int i)
	{
		value = i;
		return i;
	}

	const int operator =(const int i)
	{
		value = i;
		return i;
	}

	const double operator =(const double i)
	{
		value = i;
		return i;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	v	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atof(v));
		return v;
	}

	///������ݣ����ǽ�ֵ��Ϊ0
	void clear(void)
	{
		value = DOUBLE_NaN;
	}

	void init(void)
	{
		value = 0.0;
	}

	///��ȡֵ
	///@return	��ȡ��ֵ
	const UF_INT8 getValue(void) const
	{
		return value;
	}

	///����ֵ
	///@param	v	Ҫ���õ�ֵ
	void setValue(const UF_INT8 v)
	{
		value = v;
	}

	///��strBuf�еõ������͵��ַ�����ʾ	
	///@param	strBuf	���ڷ��ý�����ַ���
	const string getString() const
	{
		return LargeNumberToString(value);
	}

	const string getAllString() const
	{
		return getString();
	}

	const int getInt() const
	{
		return (int)value;
	}

	const double getDouble() const
	{
		return value;
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (value == DOUBLE_NaN)
			return true;
		else
			return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;

		if (value > 4503599627370496 || value < -4503599627370496)
			return false;

		double ticks = floor(value + 0.1);
		if (fabs(value - ticks) <= 0.1)
			return true;
		else
			return false;
	}

	///ת��Ϊ����
	///@return	�õ�������
	operator const UF_INT8(void) const
	{
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�ӵ�ֵ
	///@return	�õ���ֵ
	const UF_INT8 operator += (const UF_INT8 v)
	{
		setValue(value + v);
		return value;
	}

	///�������ȥĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const UF_INT8 operator -= (const UF_INT8 v)
	{
		setValue(value - v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�˵�ֵ
	///@return	�õ���ֵ
	const UF_INT8 operator *= (const UF_INT8 v)
	{
		setValue(value * v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const UF_INT8 operator /= (const UF_INT8 v)
	{
		setValue(value / v);
		return value;
	}

	const double operator % (const int v) const
	{
		return fmod(value, v);
	}

	const double operator % (const double v) const
	{
		return fmod(value, v);
	}

	///������ģĳ��ֵ
	///@param	v	Ҫģ��ֵ
	///@return	�õ���ֵ
// 	const UF_INT8 operator %= (const UF_INT8 v)
// 	{
// 		setValue(value%v);
// 		return value;
// 	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		const char* str = (char*)&value;
		int i;

		long n = 0x100;
		for (i = 0; i < sizeof(UF_INT8); i++)
		{
			unsigned long v = n | (*str);
			n += 0x100;
			int r = (int)((v >> 2) ^ v) & 0x0f;
			ret = (ret << r) | (ret >> (32 - r));
			ret &= 0xFFFFFFFFL;
			ret ^= v * v;
			str++;
		}
		return ((ret >> 16) ^ ret);
	}

protected:
	///�洢������ֵ
	UF_INT8 value;
};

typedef const CUFLongType CReadOnlyUFLongType;


/////////////////////////////////////////////////////////////////////////
///CUFRangeIntType<int from,int to>��һ���ڲ������з�Χ�޶�������
///ģ���࣬ͨ��ָ����Χ�����Եõ���������ʹ�õ��ࡣCUFRangeIntType<int 
///from, int to>ʵ���Ͼ��Ǵ洢һ������������������ֵʱ�������
/////////////////////////////////////////////////////////////////////////
template <int from, int to> class CUFRangeIntType : public CUFBaseIntType
{
public:
	///���캯��
	CUFRangeIntType(void)
	{
		clear();
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFRangeIntType(const CUFRangeIntType<from, to>& i)
	{
		setValue(i.getValue());
	}

	///���ƹ�����
	///@param	i	�����Դ����
	CUFRangeIntType(const int i)
	{
		setValue(i);
	}

	///���ڲ�����
	///@param	i	���ڵ�ֵ
	///@return	�����ֵ
	const CUFRangeIntType<from, to>& operator =(const CUFRangeIntType<from, to>& i)
	{
		setValue(i.getValue());
		return i;
	}

	///���ڲ��������������л�ȡ
	///@param	i	���ڵ�����
	///@return	���������
	const int operator =(const int i)
	{
		setValue(i);
		return i;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	v	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atoi(v));
		return v;
	}

	///������ݣ����ǽ�ֵ��Ϊfrom
	void clear(void)
	{
		value = INT_32_MAX;
	}

	void init(void)
	{
		value = from;
	}

	///��ȡֵ
	///@return	��ȡ��ֵ
	const int getValue(void) const
	{
		return value;
	}

	///����ֵ
	///@param	v	Ҫ���õ�ֵ
	///@exception	CRuntimeError	������ֵԽ�磬���׳����쳣
	void setValue(const int v)
	{
		/*
		if ((v>to)||(v<from))
		{
			RAISE_RUNTIME_WARNING("out of range");
		}
		*/
		value = v;
	}

	///��strBuf�еõ������͵��ַ�����ʾ	
	///@param	strBuf	���ڷ��ý�����ַ���
	const string getString() const
	{
		if (value == INT_32_MAX)
			return "";
		char buf[32];
		sprintf(buf, "%d", value);
		return buf;
	}

	const string getAllString() const
	{
		return getString();
	}

	const int getInt() const
	{
		return value;
	}

	const double getDouble() const
	{
		return value;
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (value == INT_32_MAX)
			return true;
		else
			return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;
		if ((value > to) || (value < from))
			return false;
		else
			return true;
	}

	///ת��Ϊ����
	///@return	�õ�������
	operator const int(void) const
	{
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�ӵ�ֵ
	///@return	�õ���ֵ
	const int operator += (const int v)
	{
		setValue(value + v);
		return value;
	}

	///�������ȥĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const int operator -= (const int v)
	{
		setValue(value - v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ�˵�ֵ
	///@return	�õ���ֵ
	const int operator *= (const int v)
	{
		setValue(value * v);
		return value;
	}

	///���������ĳ��ֵ
	///@param	v	Ҫ����ֵ
	///@return	�õ���ֵ
	const int operator /= (const int v)
	{
		setValue(value / v);
		return value;
	}

	///������ģĳ��ֵ
	///@param	v	Ҫģ��ֵ
	///@return	�õ���ֵ
	const int operator %= (const int v)
	{
		setValue(value % v);
		return value;
	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		unsigned  long v = 0x100 | value;
		int r = (int)((v >> 2) ^ v) & 0x0f;
		ret = (ret << r) | (ret >> (32 - r));
		ret &= 0xFFFFFFFFL;
		ret ^= v * v;
		return ((ret >> 16) ^ ret);
	}
protected:
	///�洢������ֵ
	int value;
};

#define DefineRangeIntType(from,to,newType)					\
	typedef CUFRangeIntType<from,to> C ## newType;			\
	typedef const CUFRangeIntType<from,to> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFCharType��һ���ڲ����ݵ��ַ���
/////////////////////////////////////////////////////////////////////////
class CUFCharType : public CUFBaseCharType
{
public:
	///���캯��
	CUFCharType(void)
	{
		clear();
	}

	///���ƹ�����
	///@param	c	�����Դ����
	CUFCharType(const CUFCharType& c)
	{
		setValue(c.getValue());
	}

	///���ƹ�����
	///@param	c	�����Դ����
	CUFCharType(const char c)
	{
		setValue(c);
	}

	///���ڲ�����
	///@param	c	���ڵ�ֵ
	///@return	�����ֵ
	const CUFCharType& operator =(const CUFCharType& c)
	{
		setValue(c.getValue());
		return c;
	}

	///���ڲ����������ַ��л�ȡ
	///@param	c	���ڵ��ַ�
	///@return	������ַ�
	const char operator =(const char c)
	{
		setValue(c);
		return c;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	v	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* v)
	{
		setValue(*v);
		return v;
	}

	///������ݣ����ǽ�ֵ��Ϊ'\0'
	void clear(void)
	{
		value = '\0';
	}

	void init(void)
	{
		value = '\0';
	}

	///��ȡֵ
	///@return	��ȡ��ֵ
	char getValue(void) const
	{
		return value;
	}

	///����ֵ
	///@param	v	Ҫ���õ�ֵ
	void setValue(const char v)
	{
		value = v;
	}

	///��strBuf�еõ������͵��ַ�����ʾ	
	///@param	strBuf	���ڷ��ý�����ַ���
	const string getString() const
	{
		char buf[2];
		if (isprint(value))
			sprintf(buf, "%c", value);
		else
			sprintf(buf, "");
		return buf;
	}

	const string getAllString() const
	{
		return getString();
	}

	const int getInt() const
	{
		return value - '0';
	}

	const double getDouble() const
	{
		return value - '0';
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		if (value == '\0')
			return true;
		return false;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;
		if (value >= '!' && value <= '~')
		{
			//���š����Ų���
			if (value == '\'' || value == '\"' || value == ',')
				return false;
		}
		return true;
	}

	///ת��Ϊ�ַ�
	///@return	�õ����ַ�
	operator const char(void) const
	{
		return value;
	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		unsigned  long v = 0x100 | value;
		int r = (int)((v >> 2) ^ v) & 0x0f;
		ret = (ret << r) | (ret >> (32 - r));
		ret &= 0xFFFFFFFFL;
		ret ^= v * v;
		return ((ret >> 16) ^ ret);
	}
protected:
	///�洢�ַ���ֵ
	char value;
};

typedef const CUFCharType CReadOnlyUFCharType;

/////////////////////////////////////////////////////////////////////////
///CUFBigDecimalType<int length, int precision>��һ�������ַ���ģ���࣬ͨ��ָ�����ȣ�
///���Եõ���������ʹ�õ��ࡣCUFBigDecimalType<int length, int precision>ʵ���Ͼ��Ǹ��ݳ��ȣ�
///ֱ�ӱ����ַ�����ֵ������룬����'\0'
/////////////////////////////////////////////////////////////////////////
template <int length, int precision> class CUFBigDecimalType : public CUFBaseStringType
{
public:
	///���캯��
	CUFBigDecimalType(void)
	{
		buffer[0] = '\0';
	}

	///���ƹ�����
	///@param	s	�����Դ����
	CUFBigDecimalType(const CUFBigDecimalType<length, precision>& s)
	{
		setValue(s.getValue());
	}

	///���ƹ�����
	///@param	p	�����Դ����
	CUFBigDecimalType(const char* p)
	{
		setValue(p);
	}

	CUFBigDecimalType(const int& v)
	{
		char tmp[100];
		sprintf(tmp, "%d", v);
		setValue(tmp);
	}
	CUFBigDecimalType(const double& f)
	{
		char tmp[512];
		snprintf(tmp, 512, "%f", f);
		setValue(tmp);
	}
	CUFBigDecimalType(const CFloat& f)
	{
		char tmp[100];
		setValue(f.toString(tmp));
	}

	///���ڲ�����
	///@param	s	���ڵ�ֵ
	///@return	�����ֵ
	const CUFBigDecimalType& operator =(const CUFBigDecimalType<length, precision>& s)
	{
		setValue(s.getValue());
		return s;
	}
	const CFloat& operator =(const CFloat& f)
	{
		char tmp[100];
		setValue(f.toString(tmp));
		return f;
	}
	const double& operator =(const double& v)
	{
		char tmp[512];
		snprintf(tmp, 512, "%f", v);
		setValue(tmp);
		return v;
	}

	///���ڲ����������ַ����л�ȡ
	///@param	p	���ڵ��ַ���
	///@return	������ַ���
	const char* operator =(const char* p)
	{
		setValue(p);
		return p;
	}

	///���ڲ��������������л�ȡ
	///@param	v	���ڵ�����
	///@return	���������
	const int operator =(const int v)
	{
		char tmp[100];
		sprintf(tmp, "%d", v);
		setValue(tmp);
		return v;
	}

	const char* operator +=(const char* p)
	{
		if (strlen(p) + strlen(buffer) >= length)
		{
			return NULL;
		}
		strcat(buffer, p);
		buffer[length] = '\0';
		return p;
	}

	///����ַ�������
	void clear(void)
	{
		buffer[0] = '\0';
	}

	void init(void)
	{
		buffer[0] = '\0';
	}

	///��ȡ�ַ�����ֵ
	///@return	�ַ�����ֵ
	const char* getValue(void) const
	{
		return buffer;
	}

	///�����ַ�����ֵ
	///@param	s	Ҫ���õ��ַ���
	void setValue(const char* s)
	{
		if (s == NULL)
			buffer[0] = '\0';
		else
		{
			strncpy(buffer, s, length);
			buffer[length] = '\0';
		}
	}

	const string getString() const
	{
		return buffer;
	}

	const string getAllString() const
	{
		return buffer;
	}

	///�ж��Ƿ�Ϊ��
	///@return	true��ʾΪ�գ�false��ʾ��Ϊ��
	bool isNull(void) const
	{
		const char* p = buffer;
		while (*p)
		{
			if (*p != ' ')
			{
				return false;
			}
			p++;
		}
		return true;
	}

	///�ж��Ƿ�Ϸ�
	///@return	true��ʾ�Ϸ���false��ʾ���Ϸ�
	bool isValid(void) const
	{
		if (isNull())
			return true;
		if (CFloat(buffer) > 0)
		{
			if (CFloat(buffer) < CFloat(0.000000000000000001))
			{
				return false;
			}
			if (CFloat(buffer) > CFloat(999999999999999999))
			{
				return false;
			}
		}

		if (CFloat(buffer) < 0)
		{
			if (CFloat(buffer) > CFloat(-0.000000000000000001))
			{
				return false;
			}
			if (CFloat(buffer) < CFloat(-999999999999999999))
			{
				return false;
			}
		}

		return true;
	}

	///ת��Ϊ�ַ���
	///@return	�õ����ַ���
	operator const char* (void) const
	{
		return buffer;
	}

	///���ұߵĿո�ɾ��
	void trimRight(void)
	{
		char* p = buffer + strlen(buffer) - 1;
		while (p >= buffer)
		{
			if (*p != ' ')
			{
				break;
			}
			*p = '\0';
			p--;
		}
	}

	///�ж��Ƿ�С��ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator < (const char* r) const
	{
		return strcmp(buffer, r) < 0;
	}

	///�ж��Ƿ����ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator > (const char* r) const
	{
		return strcmp(buffer, r) > 0;
	}

	///�ж��Ƿ�С�ڵ���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator <= (const char* r) const
	{
		return strcmp(buffer, r) <= 0;
	}

	///�ж��Ƿ���ڵ���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator >= (const char* r) const
	{
		return strcmp(buffer, r) >= 0;
	}

	///�ж��Ƿ����ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator == (const char* r) const
	{
		return strcmp(buffer, r) == 0;
	}

	///�ж��Ƿ񲻵���ĳ���ַ���
	///@param	r	�ұ���
	///@return	true��ʾ�ǣ�false��ʾ��
	bool operator != (const char* r) const
	{
		return strcmp(buffer, r) != 0;
	}

	///����hashֵ
	///@param	seed	����
	///@return	�õ���hashֵ
	unsigned int hash(unsigned int seed) const
	{
		unsigned long ret = seed;
		const char* str = buffer;

		if (*str == '\0')
		{
			return(ret);
		}
		long n = 0x100;
		while (*str)
		{
			unsigned long v = n | (*str);
			n += 0x100;
			int r = (int)((v >> 2) ^ v) & 0x0f;
			ret = (ret << r) | (ret >> (32 - r));
			ret &= 0xFFFFFFFFL;
			ret ^= v * v;
			str++;
		}
		return ((ret >> 16) ^ ret);
	}


protected:
	///�洢�ַ�����ֵ
	char buffer[length + 1];
};

#define DefineBigDecimalType(length,precision,newType)			\
	typedef CUFBigDecimalType<length,precision> C ## newType;	\
	typedef const CUFBigDecimalType<length,precision> CReadOnly ## newType;

#endif
