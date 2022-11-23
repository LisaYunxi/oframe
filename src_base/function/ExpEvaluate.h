////////////////////////////////////////////////////////////////////////////////////////////////////
//ʵ��һ�����ʽ��ֵ
//�ļ���:ExpEvaluate.h
//�ļ����ܣ�ʵ�ֱ��ʽ����׺ת���ɺ�׺,���Ҽ�������

#ifndef EXPEVALUATE_H_INCLUDED
#define EXPEVALUATE_H_INCLUDED
#include <iostream>
#include "stack.h"
#pragma   warning(disable:4996)		//ȥ�����뾯�棺��warning C4996: 'strcpy' was declared deprecated��
#define     EXPRESION_SIZE    1000
class expr
{
private:
	char   Exp[EXPRESION_SIZE];
	char   PostExp[EXPRESION_SIZE];
	double _value;

public:
	expr()
	{
		_value = 0;
		for (int i=0; i<EXPRESION_SIZE; i++)
		{
			Exp[i] = '\0';
			PostExp[i]= '\0';
		}
	}

	void init(void)
	{
		for (int i=0; i<EXPRESION_SIZE; i++)
		{
			Exp[i] = '\0';
			PostExp[i]= '\0';
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///�õ��ñ��ʽ�����Ľ��
	//
	double GetValue(void)
	{
		return _value;
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///�ж��û�����ı��ʽ�Ƿ�Ϊ�Ϸ���
	//
	bool IsLegal(char *a)
	{
		char temp[EXPRESION_SIZE];
		char test;
		int   length = strlen(a);
		int   left=0,right=0;
		int   isnumber=0, ischar=0;

		if(length == 0)
		{
			//cout<<"����Ϊ��,��������ʽ��"<<endl;
			return false;
		}
		int i = 0,k = 0,j=0;
		for(i=0; i<EXPRESION_SIZE; i++)
			temp[i] = '\0';
		///////////////////////////////////////////////////////////////////
		///���˵��ո�
		for(i=0,k=0; i<length; i++,k++)
		{
			test = a[i];
			if(test == ' ')
			{
				k--;
				continue;
			}
			temp[k] = test;
		}
		strcpy(a,temp);
		length = strlen(a);
		///////////////////////////////////////////////////////////////////
		///���Ȳ��Ϸ�
		for(i=0; i<EXPRESION_SIZE; i++)
			temp[i] = '\0';
		if(length > EXPRESION_SIZE)
		{
			//cout<<"�洢�ռ䲻����"<<endl;
			exit(0);
		}
		///////////////////////////////////////////////////////////////////
		///�������������ż�ʡ�����ŵ����
		for(j=0,k=0; j<length; j++,k++)
		{
			test = a[j];
			if(test == '(')
			{
				if(j!=0)
				{
					if((a[j-1]>='0' && a[j-1]<='9') || a[j-1] == ')')
					{
						temp[k] = '*';    ///����ʡ�Ե�����
						temp[k+1] = test;
						k++;
						continue;
					}
				}
			}
			temp[k] = test;
		}
		strcpy(a,temp);
		length = strlen(a);
		for(i=0; i<EXPRESION_SIZE; i++)
			temp[i] = '\0';
		///////////////////////////////////////////////////////////////////
		///����������
		for(j=0,k=0; j<length; j++,k++)
		{
			test = a[j];
			if(j==0 && test == '-')
			{
				temp[k] = '0';        ///��һ�����Ǹ�����ʱ��,����ǰ���һ��0
				temp[k++]='-';
				continue;
			}
			if(test == '(' && a[j+1] == '-')
			{
				temp[k]   = '(';     ///���������ŵĸ���ʱ,�����ź͸���֮������һ��0
				temp[k+1] = '0';
				temp[k+2] = '-';
				k+=2;
				j++;
				continue;
			}
			temp[k] = test;
		}
		strcpy(a,temp);
		length = strlen(a);
		///////////////////////////////////////////////////////////////////
		///�ж���û�зǷ����ַ�
		int m = 0;
		for( m=0; m<length; m++)
		{
			test = a[m];
			if(!(test=='+'||test=='-'||test=='*'||test=='/'||test=='^'||test=='('||test==')'||(test<='Z'&&test>='A')||(test<='9'&&test>='0')||test=='.'))
			{
				//cout<<"���зǷ����ַ���"<<endl;
				return false;
			}
		}
		///////////////////////////////////////////////////////////////////
		///��������Ƿ�ƥ��
		for(m=0; m<length; m++)
		{
			test = a[m];
			if(test == '(')
				left++;
			if(test == ')')
				right++;
		}
		if(left != right)
		{
			//cout<<"���Ų�ƥ�䣡"<<endl;
			return false;
		}
		///////////////////////////////////////////////////////////////////
		///�жϱ��ʽ��һЩ��������ŵĴ���
		for(int p=0; p<length; p++)
		{
			test = a[p];
			///////////////////////////////////////////////////////////////////
			///�ж�С���ķǷ�
			if(test=='.')
			{
				//��һλ�����һλ������С����
				if(p==0 || p==length-1)
				{
					//cout<<"С�����д���"<<endl;
					return false;
				}
				//С����ǰ������Ƿ�����
				if((a[p+1]>'9' && a[p+1]<'0') || (a[p-1]>'9' && a[p-1]<'0'))
				{
					//cout<<"С�����д���"<<endl;
					return false;
				}
			}
			///////////////////////////////////////////////////////////////////
			///�ж�����������ֵ������Ƿ�Ϸ�
			if((test<='9'&&test>='0') || test=='.'|| (test<='Z'&&test>='A'))
			{
				while(test<='9'&&test>='0' || test== '.' || (test<='Z'&&test>='A'))
				{
					test = a[++p];
				}
				isnumber++;
				p--;
				continue;
			}
			if(test=='+'||test=='-'||test=='*'||test=='/'||test=='^')
			{
				ischar++;
				continue;
			}
		}
		///����������
		if(isnumber > ischar+1)
		{
			////cout<<"ȱ�ٷ��ţ�"<<endl;
			return false;
		}
		///����������
		if(isnumber < ischar+1)
		{
			////cout<<"ȱ�����֣�"<<endl;
			return false;
		}
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///ʹ���ṩ���ַ��������ñ��ʽǰ׺��ֵ
	//
	bool SetExp(char* a)
	{
		if (IsLegal(a))
		{
			strcpy(Exp, a);
			return true;
		}
		else
			//cout<<"����ı��ʽ���Ϸ������������룡"<<endl;
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///�����ʽǰ׺ת��Ϊ��׺,�������ȼ��Ѿ������ڳ�����,û�����⽨���жϺ���
	//
	void translate(void)
	{
		stackchar oprate;
		char temp = *Exp;
		char Top;
		char elem;
		int   j=0;

		for (int i=0; temp != '\0'; i++)
		{
			temp = *(Exp+i);
			switch (temp)
			{
			case '(':        ///�������Ž�ջ
				oprate.push(temp);
				break;
			case ')':
				if (!oprate.IsEmpty())    ///��������һֱ��ջֱ��ջ��Ϊ������,���ҽ�������ɾ��
				{
					oprate.GetTop(Top);
					while (Top != '(')
					{
						oprate.pop(elem);
						PostExp[j++] = elem;
						if (!oprate.IsEmpty())
							oprate.GetTop(Top);
						else
							break;
					}

					oprate.pop(elem);
				}
				break;
			case '+':
			case '-':        ///���Ӽ������ȼ���ͬ,����һͬ����
				if (!oprate.IsEmpty())
				{
					oprate.GetTop(Top);
					while (Top != '(')
					{
						oprate.pop(elem);
						PostExp[j++] = elem;
						if (!oprate.IsEmpty())
							oprate.GetTop(Top);
						else
							break;
					}
				}
				oprate.push(temp);
				break;
			case '^':
				if (!oprate.IsEmpty())
				{
					oprate.GetTop(Top);
					while (Top=='^')      ///���ȼ����
					{
						oprate.pop(elem);
						PostExp[j++] = elem;
						if (!oprate.IsEmpty())
							oprate.GetTop(Top);
						else
							break;
					}
				}
				oprate.push(temp);
				break;
			case '*':
			case '/':        ///�˳������ȼ�Ҳ����ͬ��
				if (!oprate.IsEmpty())
				{
					oprate.GetTop(Top);
					while (Top=='*' || Top=='/' || Top=='^')
					{
						oprate.pop(elem);
						PostExp[j++] = elem;
						if (!oprate.IsEmpty())
							oprate.GetTop(Top);
						else
							break;
					}
				}
				oprate.push(temp);
				break;
			case ' ':          ///���˿ո�
				break;
			default:          ///��������,����#����Ϊ���ֽ�������
				while ((temp>='0' && temp<='9') || temp=='.')
				{
					PostExp[j++] = temp;
					i++;
					temp = *(Exp+i);
				}
				i--;
				PostExp[j++] = '#';
				break;
			}
		}

		while (!oprate.IsEmpty())
		{
			oprate.pop(elem);
			PostExp[j++] = elem;
		}
		PostExp[j] = '\0';
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///�����׺���ʽ��ֵ
	//
	bool ComputeValue(void)
	{
		stackdouble number;
		char   temp = *PostExp;         ///�����ݴ��ַ�
		double one=0, two=0, value=0;     ///��������ͽ��
		double date = 0, ispoint = 0;
		int    count=0;
		int    length = strlen(PostExp);    ///��׺���ʽ�ĳ���

		for (int i=0; i<length; i++)
		{
			temp = *(PostExp+i);

			switch (temp)
			{
			case '+':          ///�����ӺŽ����ֳ�ջ,��������
				one = 0;
				two = 0;
				value = 0;
				number.pop(one);
				number.pop(two);
				value = one + two;
				number.push(value);
				break;
			case '-':          ///�����Ž����ֳ�ջ,��������
				one = 0;
				two = 0;
				value = 0;
				number.pop(one);
				number.pop(two);
				value = two - one;
				number.push(value);
				break;
			case '*':                               ///����˺�
				one = 0;
				two = 0;
				value = 0;
				number.pop(one);
				number.pop(two);
				value = two * one;
				number.push(value);
				break;
			case '/':                               ///�������
				one = 0;
				two = 0;
				value = 0;
				number.pop(one);
				number.pop(two);
				if (one != 0)
					value = two / one;
				else
				{
					//cout<<"��������˳�"<<endl; /// ����Ϊ�����ʧ��
					return false;
				}
				number.push(value);
				break;
			case '^':           ///����ָ��
				one = 0;
				two = 1;
				value = 0;
				number.pop(one);
				number.pop(two);
				if (two > 0)
					value = pow(two, one);
				else
				{
					//cout<<"�����˳�"<<endl;
					exit(0);
				}
				number.push(value);
				break;
			default:
				if (temp != '#')
				{
					date    = 0;
					ispoint = 0;
					count   = 0;
					///////////////////////////////////////////////////////////////////////////////
					///���ַ���������ת��Ϊ�������ÿ⺯��,�����Լ���д��
					while ((temp>='0' && temp <='9') || temp=='.')
					{
						if(temp == '.')    ///����С����Ȼ���������ּ��㷽ʽ��ı�
						{
							ispoint = 1;
							i++;
							temp = *(PostExp+i);
							continue;
						}
						if (ispoint==0)
						{
							date=date*10+temp-'0';
						}
						else
						{
							count++;
							date += (temp-'0')*pow(0.1,count);
						}
						i++;
						temp = *(PostExp+i);
					}
					i--;
					number.push(date);
				}
			}
		}
		number.pop(_value);
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	///�����ʽ����׺ʽ�ͺ�׺ʽ�Լ����յĽ����ӡ���
	//
	void Display(void)
	{
		//cout<<"��׺���ʽΪ��"<<Exp<<endl;
		//cout<<"��׺���ʽΪ��"<<PostExp<<endl;
	}
};

#endif
