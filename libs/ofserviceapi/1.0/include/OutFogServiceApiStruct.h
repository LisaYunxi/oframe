/////////////////////////////////////////////////////////////////////////
///OutFogServiceApiStruct.h
///�������� OutFogServiceApiStruct
///@history 
///���һ� �������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef _OUTFOG_SERVICE_API_STRUCT_H_
#define _OUTFOG_SERVICE_API_STRUCT_H_

//Stream������С
#define OUTFOG_MAX_PACKAGE_SIZE 4096 

//Type �ֵ䶨��
#define OUTFOG_TYPE_NULL		'A'		//�޲���
#define OUTFOG_TYPE_INIT		'B'		//ҵ���ʼ����־
#define OUTFOG_TYPE_START		'C'		//��ʼ����,�����߳�
#define OUTFOG_TYPE_ADDSESSION	'D'		//����
#define OUTFOG_TYPE_DELSESSION	'E'		//�ǳ�
#define OUTFOG_TYPE_INITLINK	'F'		//��ʼ��Link
#define OUTFOG_TYPE_TIMER		'G'		//��ʱ��
#define OUTFOG_TYPE_REDOOVER	'R'		//Redo��ɱ�־

	//	SubIndexName	�������ַ�����
	//	UserID			�������ַ�ʱ��
	//	RequestNo		���������
	//	SessionNo		�����׼ʱ������

#define OUTFOG_TYPE_REGTOPIC	'H'		//ע������
#define OUTFOG_TYPE_SUBTOPIC	'I'		//��������
#define OUTFOG_TYPE_UNSUBTOPIC	'J'		//�˶�����

#define OUTFOG_TYPE_MANAGER		'K'		//ϵͳ��������
#define OUTFOG_TYPE_STOP		'L'		//OMQ���Ա���,��������ʧ��ϵͳֹͣ����
#define OUTFOG_TYPE_ADDAPIKEY	'M'		//����ApiKey
#define OUTFOG_TYPE_LOOP		'N'		//��Ϊδ�л���ϵͳ����ǰ��ѭ���ص�
#define OUTFOG_TYPE_DELAPIKEY	'O'		//ɾ��ApiKey
#define OUTFOG_TYPE_ADDAPIKEYALLOWIP	'P'		//����ApiKey IP
#define OUTFOG_TYPE_DELAPIKEYALLOWIP    'Q'		//ɾ��ApiKey IP

#define OUTFOG_TYPE_REQUEST		'1'		//����
#define OUTFOG_TYPE_RESPONSE	'2'		//Ӧ��
#define OUTFOG_TYPE_PUBLISH		'3'		//����
#define OUTFOG_TYPE_LOGIN		'4';	//��¼

#define OUTFOG_TYPE_INSERT		'a'		//����
#define OUTFOG_TYPE_DELETE		'b'		//ɾ��
#define OUTFOG_TYPE_UPDATE		'c'		//����
#define OUTFOG_TYPE_INSUPD		'd'		//������߸���

	//	FrontNo			��redis������������������棬0��ʾ������������ʾ��ౣ�������
	//					����DB������0��ʾ��ౣ��n������ζ���������豣�����ݿ⣬
	//  SessionNo		����ʵ��ʹ�õ�FieldNo

//Direction �ֵ䶨��
#define OUTFOG_DIRECTION_TO_NULL	'N'		//û�й涨
#define OUTFOG_DIRECTION_TO_FRONT	'F'		//��ǰ�÷���
#define OUTFOG_DIRECTION_TO_OFFER	'O'		//���̻�����
#define OUTFOG_DIRECTION_TO_KERNEL	'K'		//����ķ���

//Chain�ֵ䶨��
//--ʹ�ö�Field���һ�����ĳ���
#define OUTFOG_CHAIN_FIRST			'F'		//��һ��
#define OUTFOG_CHAIN_CONTINUE		'C'		//���к���Field
#define OUTFOG_CHAIN_LAST			'L'		//���һ��
//--ʹ��Fieldһ�����ĳ���
#define OUTFOG_CHAIN_SINGLE			'S'		//ֻ��һ������

//DataFormat�ֵ䶨��
#define OUTFOG_DATAFORMAT_STRUCT	'C'		//����߽ṹ��
#define OUTFOG_DATAFORMAT_STREAM	'S'		//�ֽ���()
#define OUTFOG_DATAFORMAT_JSON		'J'		//Json��ʽ
#define OUTFOG_DATAFORMAT_OUTPUT	'O'		//��ӡ��ʽ
#define OUTFOG_DATAFORMAT_FIX		'F'		//FIX��ʽ
#define OUTFOG_DATAFORMAT_CSV		'V'		//CSV��ʽ
#define OUTFOG_DATAFORMAT_FORMDATA	'D'		//GetFormData��ʽ

//SubjectID �ֵ䶨��
#define OUTFOG_SUBJECTID_NULL		INT_MAX	//����Ҫ�����ⷢ��
#define OUTFOG_SUBJECTID_MULTI		0		//���˻Ự(1)֮��Ķ�����⣬ʹ��USERID(2,3,4)��ʶ

//Tid�Ķ���
#define OUTFOG_TNO_NULL		 0xFFFFFFFF //��ʾû��TID

//Remark�Ķ���
#define OUTFOG_REMARK_UNSUBTOPIC_ALL	"ALL"

#define OUTFOG_RESERVE_SYNC_YES		'1'
#define OUTFOG_RESERVE_SYNC_NO		'\0'

//����Token�Ƿ�Ϊ����Token
#define OUTFOG_REMARK_TOKEN_MANAGER		'm'

// #ifdef  WINDOWS
// typedef __int64		OF_INT8;
// #else
// typedef long long	OF_INT8;
// #endif

//typedef double	OF_INT8;
//#pragma pack()

//ϵͳĬ����8�ֽڶ��룬����ڲ������Ϊ4�ֽڶ������������
//����Ѱ����ֽڴӴ�С���У�������4�ֽڻ���8�ֽڣ����ܴ���
//double:8,int:4,bool:4,char:1 ����Ĵ�СΪ2��n�η�

struct TOutFogMetaRef
{
	//�����ַ�����˳����ζ�ź�����ַ������Ա�ʹ�õģ�����Remark����ʹ�õľ���128���ַ�
	char Remark[24];		//�Զ����ֶ�,���޿���ʹ��128���ַ�ΪToken��ErrorMsg
	char SubIndexName[48];	//��������,����ʹ��96���ַ�,Reserve��ʹ��SubIndexName��ʱ���к��岻��ʹ��
	char UserID[48];		//�û���,����ʹ��53���ַ�
	char Reserve[5];		//Ԥ���ֶ�(5���ֽ�double����);ͬ��Push����([0]:�Ƿ�ͬ�����ݿ�;[1]:�Ƿ�ͬ��Redis)
	char Direction;			//���ݷ��ͷ���(�����ֵ䶨��)
	char Type;				//��������(�����ֵ䶨��)
	char Chain;				//������־(�����ֵ䶨��);����ת�����ܶ���Ϊ���ݸ�ʽ���ࡢ�ֽ�����json��CSV��
	double SequenceNo;		//�������ڲ������
	double GlobalNo;		//ȫ��ͳһ�����
	double BusinessNo;		//ҵ���;Req������Link��ʶ��(����)
	int FrontNo;			//ǰ�ú�(����);ͬ��Push����(��ʾ��ౣ��Redis������)
	int RequestNo;			//�ͻ����������(������ʾ����Ҫ�ظ�);ͬ��Push���Ĵ������߳����
	int SessionNo;			//�Ự��;ͬ��Push���Ĵ���ʵ��ʹ�õ�FieldNo
	int MessageNo;			//���Ͱ�ͷ������ʶ��id
	int Length;				//���ݳ���
	int SubjectNo;			//���ڵ������,������ϢErrorNo
	int FieldNo;			//�������ͺ�	
	bool IsReplay;			//�Ƿ��Ƿ��ݵ�����
};
#endif

