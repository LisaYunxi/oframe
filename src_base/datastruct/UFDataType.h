/////////////////////////////////////////////////////////////////////////
///UFDataType.h
///定义了内部数据的底层支持类
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
///CUFStringType<int length>是一个数据字符串模版类，通过指定长度，
///可以得到真正可以使用的类。CUFStringType<int length>实质上就是根据长度，
///直接保存字符串的值，左对齐，最后加'\0'。
/////////////////////////////////////////////////////////////////////////
template <int length> class CUFStringType : public CUFBaseStringType
{
public:
	///构造函数
	CUFStringType(void)
	{
		//buffer[0] = '\0';
		clear();
	}

	///复制构造子
	///@param	s	构造的源对象
	CUFStringType(const CUFStringType<length>& s)
	{
		setValue(s.getValue());
	}

	///复制构造子
	///@param	p	构造的源对象
	CUFStringType(const char* p)
	{
		setValue(p);
	}

	CUFStringType(const char p)
	{
		setValue(p);
	}

	///等于操作符
	///@param	s	等于的值
	///@return	输入的值
	const CUFStringType& operator =(const CUFStringType<length>& s)
	{
		setValue(s.getValue());
		return s;
	}

	///等于操作符，从字符串中获取
	///@param	p	等于的字符串
	///@return	输入的字符串
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

	///等于操作符，从整数中获取
	///@param	v	等于的整数
	///@return	输入的整数
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

	///等于操作符，从64位长整数中获取     liumeng 20140820
	///@param	v	等于的长整数
	///@return	输入的长整数
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

	///清除字符串内容
	void clear(void)
	{
		buffer[0] = '\0';
	}

	void init(void)
	{
		buffer[0] = '\0';
	}

	///获取字符串的值
	///@return	字符串的值
	const char* getValue(void) const
	{
		return buffer;
	}

	///设置字符串的值
	///@param	s	要设置的字符串
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

	///在strBuf中得到本类型的字符串表示	
	///@param	strBuf	用于放置结果的字符串
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

	///获取内部的指针，特殊情况使用
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
	bool isNull(void) const
	{
		if (buffer[0] == '\0')
			return true;
		else
			return false;
	}

	///判断是否合法
	///@return	true表示合法，false表示不合法
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
			//引号、逗号、空格不行
			if (buffer[i] == '\'' || buffer[i] == '\"' || buffer[i] == ',' || buffer[i] == ' ' || buffer[i] == '%')
				return false;
		}
		return true;
	}

	///转化为字符串
	///@return	得到的字符串
	operator const char* (void) const
	{
		return buffer;
	}

	///将右边的空格删除
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

	///判断是否小于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator < (const char* r) const
	{
		return strcmp(buffer, r) < 0;
	}

	///判断是否大于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator > (const char* r) const
	{
		return strcmp(buffer, r) > 0;
	}

	///判断是否小于等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator <= (const char* r) const
	{
		return strcmp(buffer, r) <= 0;
	}

	///判断是否大于等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator >= (const char* r) const
	{
		return strcmp(buffer, r) >= 0;
	}

	///判断是否等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
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

	///判断是否不等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
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

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储字符串的值
	char buffer[length + 1];
};

#define DefineStringType(length,newType)				\
	typedef CUFStringType<length> C ## newType;			\
	typedef const CUFStringType<length> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFFloatType是一个内部数据的浮点数类
///@author	xuzh
///@version	2.0,20060218
/////////////////////////////////////////////////////////////////////////
///xuzh，20070328
///增加了对浮点数范围的判定，防止出现超越范围的数据
template <int length, int precision> class CUFFloatType : public CUFBaseFloatType
{
public:
	///构造函数
	CUFFloatType(void)
	{
		clear();
	}

	///复制构造子
	///@param	f	构造的源对象
	CUFFloatType(const CUFFloatType& f)
	{
		setValue(f.getValue());
	}

	///复制构造子
	///@param	value	构造的源对象
	CUFFloatType(const double value)
	{
		setValue(value);
	}

	///等于操作符
	///@param	f	等于的值
	///@return	输入的值
	const CUFFloatType& operator =(const CUFFloatType& f)
	{
		setValue(f.getValue());
		return f;
	}

	///等于操作符，从浮点数中获取
	///@param	v	等于的浮点数
	///@return	输入的字浮点数
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

	///等于操作符，从字符串中获取
	///@param	v	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atof(v));
		return v;
	}

	///清除内容，就是将值设为0
	void clear(void)
	{
		value = DOUBLE_NaN;
	}

	void init(void)
	{
		value = 0.0;
	}

	///获取值
	///@return	获取的值
	const double getValue(void) const
	{
		return value;
	}

	///设置值
	///@param	v	要设置的值
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

	//财务对账用于和其他大数做计算的时候，为了能计算准确这里做舍弃的牺牲
	//由于保留10位小数，从第九位开始，就需要舍弃一位，
	//void cutByBigFloat(const double tick1, const double tick2)
	//{
	//	toRoundTick(min(tick1, tick2));
	//}

	const bool isTick(const double v) const
	{
		if (value == DOUBLE_NaN || v == DOUBLE_NaN || v == 0.0 || value == 0.0)
			return true;

		//如果值过大也会出问题
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

		//价格按照tick值化为整数，不能大于int的最大值，如果后面需要使用getTick的值，需要做这个判断
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
	bool isNull(void) const
	{
		if (value == DOUBLE_NaN)
			return true;
		else
			return false;
	}

	///判断是否合法
	///@return	true表示合法，false表示不合法
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

	///转化为实数
	///@return	得到的实数
	operator const double(void) const
	{
		return value;
	}

	///加上某个值
	///@param	v	要加的值
	///@return	运算以后的结果
	const double operator += (const double v)
	{
		//这样是为了防止出现-0.00000的情况。对于某些浮点计算器，在计算例如-31.0+31.0时，得到的结果是-0.0，而非0.0
		//精度之外的需要清零
		double r = value + v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			value = 0.0;
		else
			value = r;
		return v;
	}

	///减去某个值
	///@param	v	要减的值
	///@return	运算以后的结果
	const double operator -= (double v)
	{
		//这样是为了防止出现-0.00000的情况。对于某些浮点计算器，在计算例如-31.0+31.0时，得到的结果是-0.0，而非0.0
		//精度之外的需要清零
		double r = value - v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			value = 0.0;
		else
			value = r;
		return v;
	}

	CUFFloatType operator - (double v)
	{
		//这样是为了防止出现-0.00000的情况。对于某些浮点计算器，在计算例如-31.0+31.0时，得到的结果是-0.0，而非0.0
		//精度之外的需要清零
		double r = value - v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			r = 0.0;
		return CUFFloatType(r);
	}

	CUFFloatType operator + (double v)
	{
		//这样是为了防止出现-0.00000的情况。对于某些浮点计算器，在计算例如-31.0+31.0时，得到的结果是-0.0，而非0.0
		//精度之外的需要清零
		double r = value + v;
		if (fabs(r) <= doubleCompareSmall(value, precision))
			r = 0.0;
		return CUFFloatType(r);
	}

	///乘上某个值
	///@param	v	要乘的值
	///@return	运算以后的结果
	const double operator *= (const double v)
	{
		setValue(value * v);
		return v;
	}

	///除以某个值
	///@param	v	要除的值
	///@return	运算以后的结果
	const double operator /= (const double v)
	{
		setValue(value / v);
		return v;
	}

	///整除某个值
	///@param	v	要除的值
	///@return	运算以后的结果
	//const int operator &(const double v) const
	//{
	//	//修正了当value/v的结果为负值时的错误
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

	///判断是否小于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator < (double r) const
	{
		return value < r - doubleCompareSmall(value, precision);
	}

	///判断是否大于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator > (double r) const
	{
		return value > r + doubleCompareSmall(value, precision);
	}

	///判断是否小于等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator <= (double r) const
	{
		return value <= r + doubleCompareSmall(value, precision);
	}

	///判断是否大于等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator >= (double r) const
	{
		return value >= r - doubleCompareSmall(value, precision);
	}

	///判断是否等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
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


	///判断是否不等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator != (double r) const
	{
		return fabs(value - r) > doubleCompareSmall(value, precision);
	}

	///判断是否小于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator < (int r) const
	{
		return value < r - doubleCompareSmall(value, precision);
	}

	///判断是否大于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator > (int r) const
	{
		return value > r + doubleCompareSmall(value, precision);
	}

	///判断是否小于等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator <= (int r) const
	{
		return value <= r + doubleCompareSmall(value, precision);
	}

	///判断是否大于等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator >= (int r) const
	{
		return value >= r - doubleCompareSmall(value, precision);
	}

	///判断是否等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator == (int r) const
	{
		return fabs(value - r) <= doubleCompareSmall(value, precision);
	}

	///判断是否不等于某个值
	///@param	r	右边项
	///@return	true表示是，false表示否
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

	//	//只有累加的字段需要对累加值进行按照tick清理
	//	double ticks = floor(value / compareTick + 0.1);
	//	value = compareTick * ticks;
	//}

	//bool IsEqual(double r) const
	//{
	//	return doubleEqual(value, r);
	//}

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储浮点数的值
	double value;
};

#define DefineFloatType(maxValue,minValue,newType)			\
	typedef CUFFloatType<maxValue,minValue> C ## newType;	\
	typedef const CUFFloatType<maxValue,minValue> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFIntType是一个内部数据的整数类
///@author	xuzh
///@version	2.0,20060218
/////////////////////////////////////////////////////////////////////////
class CUFIntType : public CUFBaseIntType
{
public:
	///构造函数
	CUFIntType(void)
	{
		clear();
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFIntType(const CUFIntType& i)
	{
		setValue(i.getValue());
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFIntType(const int i)
	{
		setValue(i);
	}

	///等于操作符
	///@param	i	等于的值
	///@return	输入的值
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

	///等于操作符，从整数中获取
	///@param	i	等于的整数
	///@return	输入的整数
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

	///等于操作符，从字符串中获取
	///@param	v	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atoi(v));
		return v;
	}

	///清除内容，就是将值设为0
	void clear(void)
	{
		value = INT_32_MAX;
	}

	void init(void)
	{
		value = 0;
	}

	///获取值
	///@return	获取的值
	const int getValue(void) const
	{
		return value;
	}

	///设置值
	///@param	v	要设置的值
	void setValue(const int v)
	{
		value = v;
	}

	///在strBuf中得到本类型的字符串表示	
	///@param	strBuf	用于放置结果的字符串
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
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

	///判断是否合法
	///@return	true表示合法，false表示不合法
	bool isValid(void) const
	{
		return true;
	}

	///转化为整数
	///@return	得到的整数
	operator const int(void) const
	{
		return value;
	}

	///本对象加上某个值
	///@param	v	要加的值
	///@return	得到的值
	const int operator += (const int v)
	{
		setValue(value + v);
		return value;
	}

	///本对象减去某个值
	///@param	v	要减的值
	///@return	得到的值
	const int operator -= (const int v)
	{
		setValue(value - v);
		return value;
	}

	///本对象乘以某个值
	///@param	v	要乘的值
	///@return	得到的值
	const int operator *= (const int v)
	{
		setValue(value * v);
		return value;
	}

	///本对象除以某个值
	///@param	v	要除的值
	///@return	得到的值
	const int operator /= (const int v)
	{
		setValue(value / v);
		return value;
	}

	///本对象模某个值
	///@param	v	要模的值
	///@return	得到的值
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

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储整数的值
	int value;
};

typedef const CUFIntType CReadOnlyUFIntType;

/////////////////////////////////////////////////////////////////////////
///CUFLongType是一个内部数据的长整数类
/////////////////////////////////////////////////////////////////////////
class CUFLongType : public CUFBaseLongType
{
public:
	///构造函数
	CUFLongType(void)
	{
		clear();
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFLongType(const CUFLongType& i)
	{
		value = i.getValue();
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFLongType(const UF_INT8 i)
	{
		value = i;
	}

	///等于操作符
	///@param	i	等于的值
	///@return	输入的值
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

	///等于操作符，从字符串中获取
	///@param	v	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atof(v));
		return v;
	}

	///清除内容，就是将值设为0
	void clear(void)
	{
		value = DOUBLE_NaN;
	}

	void init(void)
	{
		value = 0.0;
	}

	///获取值
	///@return	获取的值
	const UF_INT8 getValue(void) const
	{
		return value;
	}

	///设置值
	///@param	v	要设置的值
	void setValue(const UF_INT8 v)
	{
		value = v;
	}

	///在strBuf中得到本类型的字符串表示	
	///@param	strBuf	用于放置结果的字符串
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
	bool isNull(void) const
	{
		if (value == DOUBLE_NaN)
			return true;
		else
			return false;
	}

	///判断是否合法
	///@return	true表示合法，false表示不合法
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

	///转化为整数
	///@return	得到的整数
	operator const UF_INT8(void) const
	{
		return value;
	}

	///本对象加上某个值
	///@param	v	要加的值
	///@return	得到的值
	const UF_INT8 operator += (const UF_INT8 v)
	{
		setValue(value + v);
		return value;
	}

	///本对象减去某个值
	///@param	v	要减的值
	///@return	得到的值
	const UF_INT8 operator -= (const UF_INT8 v)
	{
		setValue(value - v);
		return value;
	}

	///本对象乘以某个值
	///@param	v	要乘的值
	///@return	得到的值
	const UF_INT8 operator *= (const UF_INT8 v)
	{
		setValue(value * v);
		return value;
	}

	///本对象除以某个值
	///@param	v	要除的值
	///@return	得到的值
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

	///本对象模某个值
	///@param	v	要模的值
	///@return	得到的值
// 	const UF_INT8 operator %= (const UF_INT8 v)
// 	{
// 		setValue(value%v);
// 		return value;
// 	}

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储整数的值
	UF_INT8 value;
};

typedef const CUFLongType CReadOnlyUFLongType;


/////////////////////////////////////////////////////////////////////////
///CUFRangeIntType<int from,int to>是一个内部数据有范围限定的整数
///模版类，通过指定范围，可以得到真正可以使用的类。CUFRangeIntType<int 
///from, int to>实质上就是存储一个整数，但是在设置值时将做检测
/////////////////////////////////////////////////////////////////////////
template <int from, int to> class CUFRangeIntType : public CUFBaseIntType
{
public:
	///构造函数
	CUFRangeIntType(void)
	{
		clear();
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFRangeIntType(const CUFRangeIntType<from, to>& i)
	{
		setValue(i.getValue());
	}

	///复制构造子
	///@param	i	构造的源对象
	CUFRangeIntType(const int i)
	{
		setValue(i);
	}

	///等于操作符
	///@param	i	等于的值
	///@return	输入的值
	const CUFRangeIntType<from, to>& operator =(const CUFRangeIntType<from, to>& i)
	{
		setValue(i.getValue());
		return i;
	}

	///等于操作符，从整数中获取
	///@param	i	等于的整数
	///@return	输入的整数
	const int operator =(const int i)
	{
		setValue(i);
		return i;
	}

	///等于操作符，从字符串中获取
	///@param	v	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* v)
	{
		if (v[0] == '\0')
			clear();
		else
			setValue(atoi(v));
		return v;
	}

	///清除内容，就是将值设为from
	void clear(void)
	{
		value = INT_32_MAX;
	}

	void init(void)
	{
		value = from;
	}

	///获取值
	///@return	获取的值
	const int getValue(void) const
	{
		return value;
	}

	///设置值
	///@param	v	要设置的值
	///@exception	CRuntimeError	发现数值越界，则抛出此异常
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

	///在strBuf中得到本类型的字符串表示	
	///@param	strBuf	用于放置结果的字符串
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
	bool isNull(void) const
	{
		if (value == INT_32_MAX)
			return true;
		else
			return false;
	}

	///判断是否合法
	///@return	true表示合法，false表示不合法
	bool isValid(void) const
	{
		if (isNull())
			return true;
		if ((value > to) || (value < from))
			return false;
		else
			return true;
	}

	///转化为整数
	///@return	得到的整数
	operator const int(void) const
	{
		return value;
	}

	///本对象加上某个值
	///@param	v	要加的值
	///@return	得到的值
	const int operator += (const int v)
	{
		setValue(value + v);
		return value;
	}

	///本对象减去某个值
	///@param	v	要减的值
	///@return	得到的值
	const int operator -= (const int v)
	{
		setValue(value - v);
		return value;
	}

	///本对象乘以某个值
	///@param	v	要乘的值
	///@return	得到的值
	const int operator *= (const int v)
	{
		setValue(value * v);
		return value;
	}

	///本对象除以某个值
	///@param	v	要除的值
	///@return	得到的值
	const int operator /= (const int v)
	{
		setValue(value / v);
		return value;
	}

	///本对象模某个值
	///@param	v	要模的值
	///@return	得到的值
	const int operator %= (const int v)
	{
		setValue(value % v);
		return value;
	}

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储整数的值
	int value;
};

#define DefineRangeIntType(from,to,newType)					\
	typedef CUFRangeIntType<from,to> C ## newType;			\
	typedef const CUFRangeIntType<from,to> CReadOnly ## newType;

/////////////////////////////////////////////////////////////////////////
///CUFCharType是一个内部数据的字符类
/////////////////////////////////////////////////////////////////////////
class CUFCharType : public CUFBaseCharType
{
public:
	///构造函数
	CUFCharType(void)
	{
		clear();
	}

	///复制构造子
	///@param	c	构造的源对象
	CUFCharType(const CUFCharType& c)
	{
		setValue(c.getValue());
	}

	///复制构造子
	///@param	c	构造的源对象
	CUFCharType(const char c)
	{
		setValue(c);
	}

	///等于操作符
	///@param	c	等于的值
	///@return	输入的值
	const CUFCharType& operator =(const CUFCharType& c)
	{
		setValue(c.getValue());
		return c;
	}

	///等于操作符，从字符中获取
	///@param	c	等于的字符
	///@return	输入的字符
	const char operator =(const char c)
	{
		setValue(c);
		return c;
	}

	///等于操作符，从字符串中获取
	///@param	v	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* v)
	{
		setValue(*v);
		return v;
	}

	///清除内容，就是将值设为'\0'
	void clear(void)
	{
		value = '\0';
	}

	void init(void)
	{
		value = '\0';
	}

	///获取值
	///@return	获取的值
	char getValue(void) const
	{
		return value;
	}

	///设置值
	///@param	v	要设置的值
	void setValue(const char v)
	{
		value = v;
	}

	///在strBuf中得到本类型的字符串表示	
	///@param	strBuf	用于放置结果的字符串
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
	bool isNull(void) const
	{
		if (value == '\0')
			return true;
		return false;
	}

	///判断是否合法
	///@return	true表示合法，false表示不合法
	bool isValid(void) const
	{
		if (isNull())
			return true;
		if (value >= '!' && value <= '~')
		{
			//引号、逗号不行
			if (value == '\'' || value == '\"' || value == ',')
				return false;
		}
		return true;
	}

	///转化为字符
	///@return	得到的字符
	operator const char(void) const
	{
		return value;
	}

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储字符的值
	char value;
};

typedef const CUFCharType CReadOnlyUFCharType;

/////////////////////////////////////////////////////////////////////////
///CUFBigDecimalType<int length, int precision>是一个数据字符串模版类，通过指定长度，
///可以得到真正可以使用的类。CUFBigDecimalType<int length, int precision>实质上就是根据长度，
///直接保存字符串的值，左对齐，最后加'\0'
/////////////////////////////////////////////////////////////////////////
template <int length, int precision> class CUFBigDecimalType : public CUFBaseStringType
{
public:
	///构造函数
	CUFBigDecimalType(void)
	{
		buffer[0] = '\0';
	}

	///复制构造子
	///@param	s	构造的源对象
	CUFBigDecimalType(const CUFBigDecimalType<length, precision>& s)
	{
		setValue(s.getValue());
	}

	///复制构造子
	///@param	p	构造的源对象
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

	///等于操作符
	///@param	s	等于的值
	///@return	输入的值
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

	///等于操作符，从字符串中获取
	///@param	p	等于的字符串
	///@return	输入的字符串
	const char* operator =(const char* p)
	{
		setValue(p);
		return p;
	}

	///等于操作符，从整数中获取
	///@param	v	等于的整数
	///@return	输入的整数
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

	///清除字符串内容
	void clear(void)
	{
		buffer[0] = '\0';
	}

	void init(void)
	{
		buffer[0] = '\0';
	}

	///获取字符串的值
	///@return	字符串的值
	const char* getValue(void) const
	{
		return buffer;
	}

	///设置字符串的值
	///@param	s	要设置的字符串
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

	///判断是否为空
	///@return	true表示为空，false表示不为空
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

	///判断是否合法
	///@return	true表示合法，false表示不合法
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

	///转化为字符串
	///@return	得到的字符串
	operator const char* (void) const
	{
		return buffer;
	}

	///将右边的空格删除
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

	///判断是否小于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator < (const char* r) const
	{
		return strcmp(buffer, r) < 0;
	}

	///判断是否大于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator > (const char* r) const
	{
		return strcmp(buffer, r) > 0;
	}

	///判断是否小于等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator <= (const char* r) const
	{
		return strcmp(buffer, r) <= 0;
	}

	///判断是否大于等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator >= (const char* r) const
	{
		return strcmp(buffer, r) >= 0;
	}

	///判断是否等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator == (const char* r) const
	{
		return strcmp(buffer, r) == 0;
	}

	///判断是否不等于某个字符串
	///@param	r	右边项
	///@return	true表示是，false表示否
	bool operator != (const char* r) const
	{
		return strcmp(buffer, r) != 0;
	}

	///产生hash值
	///@param	seed	种子
	///@return	得到的hash值
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
	///存储字符串的值
	char buffer[length + 1];
};

#define DefineBigDecimalType(length,precision,newType)			\
	typedef CUFBigDecimalType<length,precision> C ## newType;	\
	typedef const CUFBigDecimalType<length,precision> CReadOnly ## newType;

#endif
