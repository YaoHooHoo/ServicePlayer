///////////////////////////////////////////////////////////////////////////////////////
//			ȫ�ֶ���ͷ�ļ� localdef.h
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __DEIR_BROWSER_LOCALDEF_HEAD_FILE__
#define __DEIR_BROWSER_LOCALDEF_HEAD_FILE__

#include <iostream>
#include "../../../globaldef/globaldef.h"
#include "../../../globaldef/ProtocolFunction.h"
#include "../../../globaldef/WinShareMem.h"
#include "../../../globaldef/MemManage.h"
#include "../../../globaldef/ServiceThread.h"
#include "../../../globaldef/pcclientdef.h"
#include "../../../globaldef/LogicDataType.h"
#include "../../../globaldef/myFile.h"
#include "../../../globaldef/ReadIniFile.h"
#include "../../../SnapVideoModule/SnapVideoModule/SnapVideoModule/SnapVideo.h"
#include "../../../Server/Websocket/JsonParse/JsonParse/include/json.h"
#include "../../../Server/SingleSockClient/SingleSockClient/SocketClient.h"
#include "../../../FileTransport/FileTransport/FileTransport.h"
#include "../../../AudioModule/AudioModule/AudioModule/AudioModule.h"
#include "../../../DataEncrypt/DataEncrypt/EncryptData.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////
#define SUPPORT_MOUSE		0				//֧��������
#define IS_TEST_HTTP		1				//����״̬HTTP

#define NET_TIMEOUT			60000			//������Գ�ʱʱ��
#define HTTP_DOWN_NUM		1				//Http�����߳���
#define WRITE_FILE			0				//д���ļ�
#define READ_TEST_FILE		0				//��ȡ�����ļ�
#define TEST_DEBUG			0				//���Բ���

#define SAVB_APP_TIPIC		TEXT("ƽ����Ƶ����ϵͳ�ͷ���")
////////////////////////////////////////////////////////////////////////////////////
typedef map<DWORD,void *>					CDwordPointMap;		
typedef map<DWORD,void *>::iterator			CDwordPointMapIt;
typedef map<string,string>					CTelVlaueMap;			
typedef map<string,string>::iterator		CTelVlaueMapIt;	
///////////////////////////////////////////////////////////////////////////////////////
#define MYTRACE TRACE

#define DST_V_CX		1280//1600	//1280			//Ŀ����Ƶ��
#define DST_V_CY		720//900		//720		//Ŀ����Ƶ��

//////////////////////////////////////////////////////////////////////////////
#define SAMPLE_RATE		16000		//44100
#define CHANNEL_NUM		1
#define AAC_MAIN		1			//AAC-MAIN
#define AAC_LC			2			//AAC-LC

//��������
#define VAL_STR 0 
#define VAL_INT 1
#define VIDEO_BORDER			4				//��Ƶ�߿�
#define BOTTOM_HIGHT			30				//�ײ���
#define MAX_WND_CX			1600					//��󴰿ڿ�
#define MAX_WND_CY			900						//��󴰿ڸ�
#define MIN_WND_CY			800						//��С���ڸ�

#define LISENCE_DELAY_DAYS		-15					//Lisence�ӳ�����
#define MAXFAILCOUNT		3						//���ʧ�ܴ���

//��ʱ������
#define IDT_CONN_SERVER					0x0001					//���ӵ�������id
#define IDT_SHOW_DATA_RATE				0x0002					//��ʾ����ʱ��
#define IDT_DOWN_DEVCODE				0x0003					//�����ն˱��
#define IDT_START_CONNN					0x0004					//��ʼ���ӷ�����
#define IDT_LOGIN_PROCESS				0x0005					//��¼������	
#define IDT_REQUEST_CONFIG				0x0006					//��������������Ϣ
#define IDT_CHECK_WND_ACTIVE			0x0007					//��ⴰ�ڻ״̬
#define IDT_TEST_VIDEO_FILES			0x0008					//������Ƶ����

////////////////////////////////////////////////////////////////////////////////
#define WM_NOTIFYICONDATA_MSG			(WM_APP+1012)			//������Ϣ
#define WM_ACCEPT_NOTIFY_MSG			(WM_APP+1013)			//���ܸ�����Ϣ
#define WM_CHECK_WND_ACTIVE				(WM_APP+1014)			//��ⴰ�ڻ״̬

///////////////////////////////////////////////////////////////////////////
//�ն˳־û�����
typedef struct tagDevSNData
{
	BOOL			bAutoLogin;					//�Զ���¼
	DWORD			dwServiceCode;				//�ͷ���
	TCHAR			szJobNum[NAME_LEN];			//����
	TCHAR			szTermSN[NAME_LEN];			//�ն˱��
	TCHAR			szLisence[MAX_NAME];		//Lisence
	TCHAR			szPassword[NAME_LEN];		//����
	TCHAR			szSequenceID[SEQSIZE];		//Ψһ��ʶ
}DEVSNDATA;

//ȫ�����ݶ���
typedef struct tagGlobalData
{
	int				nConnectIpType;				//���ӷ�����IP(0=lvs ip,1=avs ip)
	BOOL			bOpenglDraw;				//ʹ��OpenGL����
	DWORD			dwCurrTheadID;				//��ǰ�߳�ID
	int				nBusinessType;				//ҵ��������
	int				nLisenceDays;				//Lisence��������
	BOOL			bInvalidLisence;			//��Чlisence
	BOOL			bHaveVoice;					//������־
	BOOL			bIsHaveMouseCtrl;			//�Ƿ���������
	BOOL			bIsMinWnd;					//��С������
	BOOL			bWndNotActive;				//�Ƿ�
	BOOL			bIsNotifyIcon;				//����״̬��־
	HWND			hMainWnd;					//�����ھ��
	UNDWORD			udwClientCode;				//�ͻ���
	DWORD			dwUserID;					//�û�ID
	DWORD			dwAvSvrIP;					//��Ƶ������IP
	DWORD			dwLBSvrIP;					//���ط�����IP
	WORD			wAvSvrPort;					//��Ƶ�������˿�
	WORD			wLBSvrPort;					//���ط������˿�
	WORD			wVideoCX;					//��Ƶ��
	WORD			wVideoCY;					//��Ƶ��
	LBSVRDATA		mLbsList[MAX_LBS];			//���ؾ��������
	WORD			wLbsCount;					//���ؾ��������
	BOOL			bExitApp;					//�˳�APP��־
	BOOL			bServicing;					//�����б�־
	BOOL			bConfirming;				//ȷ����
	TCHAR			szApiUrl[MAX_PATH];			//Api��ַ
	TCHAR			szSavePath[MAX_PATH];		//�洢�ļ�
	TCHAR			szWorkPath[MAX_PATH];		//����·��
	TCHAR			szLogPath[MAX_PATH];		//��־Ŀ¼
	TCHAR			szHostName[NAME_LEN];		//������
	TCHAR			szUserName[NAME_LEN];		//�û���

	DEVSNDATA		mDevSnData;					//�ն˳־û�����
	
}GLOBALDATA;

//�����ļ�����
typedef struct tagReqConfData
{
	int				nConnectIpType;				//���ӷ�����IP(0=lvs ip,1=avs ip)
	int				nBusinessType;				//ҵ��������
	WORD			wVideoCX;					//��Ƶ��
	WORD			wVideoCY;					//��Ƶ��
	DWORD			dwSerivceCode;				//�ͷ���
	TCHAR			szLbSvrIpPort[MAX_PATH];	//���ط�����ip�˿�
	TCHAR			szLisence[MAX_NAME];		//Lisence
	TCHAR			szTermSN[WNAME_LEN];		//�ն˺�
}REQCONFDATA;

//����ip����
enum emConnIpType
{
	CIPT_LVS,					//Lvs ������IP
	CIPT_AVS,					//avs ������IP
};

////////////// FLV ���� ///////////////////////////////////////////////////
//	FLV tag ����
#define FT_AUDIO	0x08			//��Ƶ
#define FT_VIDEO	0x09			//��Ƶ
#define FT_SCRIPT	0x12			//�ű�

typedef enum emAMFType
{
	AMFT_NUMBER,					//��ֵ(8Byte)
	AMFT_BOOLLEAN,					//����ֵ(1Byte)
	AMFT_STRING,					//�ַ���(2Byte����+�ַ�������)
	AMFT_OBJECT,					//����(�������AMF��������)
	AMFT_MOVCLIP,					//
	AMFT_NULL,						//��ֵ
	AMFT_UNDEFINED,					//δ��������
	AMFT_REFERENCE,					//��������
	AMFT_ECMA_ARRAY,				//ECMA��������
	AMFT_OBJECT_END,				//����
	AMFT_STRICT_ARRAY=10,			//Strict��������
	AMFT_DATE,						//��������
	AMFT_LONGSTRING,				//���ַ���(4Byte����+�ַ�������)
}AMFTYPE;

//��Ƶ��������
typedef enum emVCodecType
{
	VCT_JPEG=1,
	VCT_H263,
	VCT_SCREENVIDEO,
	VCT_VP6,
	VCT_VP6ALPHA,
	VCT_SCREENVIDEO2,
	VCT_AVC,					//h264
}VCODECTYPE;

//��Ƶ֡����
typedef enum emVFrameType
{
	VFT_KEYFRAME=1,				//�ؼ�֡
	VFT_NOKEYFRAME,				//�ǹؼ�֡
	VFT_263NOKEYFRAME,			//h263�ǹؼ�֡
	VFT_SERVERKEYFRAME,			//���������ɹؼ�֡
	VFT_VIDEOINFO,				//��Ƶ��Ϣ
}VFRAMETYPE;

//AVC������
typedef enum emAVCPType
{
	AVCPT_SEQHEADER,			//AVCSequence Header
	AVCPT_NALU,					//AVC NALU ����
	AVCPT_SEQEND,				//AVC end ofsequence
}AVCPTYPE;

//AAC������
typedef enum emAACPType
{
	AACPT_SEQUENCE,				//AAC Sequence Header
	AACPT_RAW,					//AAC ����
}AACPTYPE;

typedef struct tagAMFValue
{
	WORD			wOffset;
	union
	{
		double		dbValue;
		BOOL		bValue;
		WORD		wValue;
		DWORD		dwValue;
	};
	string strValue;
}AMFVALUTE;

typedef struct AVal
{
	char *av_val;
	int av_len;
}AVal;

//Flv�ļ�ͷ
#pragma pack(push,1)
typedef struct tagFlvHeader
{
	CHAR		szType[3];				//�ļ�����
	BYTE		cbVersion;				//�汾��Ϣ
	union
	{
		BYTE		cbStreamInfo;
		struct
		{
			BYTE		cbHaveVideo:1;			//�Ƿ�����Ƶ������Ϣ(ǰ5λ����,����Ϊ0;��6λ��ʾ�Ƿ���Ƶ����7λ����������Ϊ0;��8λ��ʾ�Ƿ�����Ƶ)
			BYTE		cbReseve7:1;			//����(1bit)
			BYTE		cbHaveAudio:1;			//�Ƿ�����Ƶ
			BYTE		cbReseve5:5;			//����(5bit)		
		};
	};
	DWORD		dwHeadLength;			//ͷ����
}FLVHEADER;

//Tagͷ
typedef struct tagTagHeader
{
	DWORD		dwPrevTagSize;			//ǰһ��tag��С
	BYTE		cbTagType;				//Tag����(8:��Ƶ,9:��Ƶ;18:�ű�����(AMF��),����������)
	DWORD		dwDataSize;				//���ݳ���
	DWORD		dwTimeStamp;			//ʱ���
	BYTE		cbHighTime;				//ʱ���λֵ(��3���ֽ�ʱ�䲻����ʱʹ�ô��ֶ�)
	DWORD		dwStreamID;				//��ID����Ϊ0
}TAGHEADER;

#pragma pack(pop)

typedef struct tagInfoItem
{
	AMFTYPE		amfType;
	union
	{
		double		dbValue;
		BOOL		bValue;
		WORD		wValue;
		DWORD		dwValue;
	};
	CHAR		szName[24];
	CHAR		szValue[64];
}INFOITEM;

//Flv�ļ���Ƶ��Ϣ
typedef struct tagFlvFileAudio
{
	INFOITEM	audiocodecid;			//��Ƶ������ID
	INFOITEM	audiodatarate;			//��Ƶ����
	INFOITEM	audiosamplerate;		//��Ƶ������
	INFOITEM	audiosamplesize;		//��Ƶ������С
	INFOITEM	stereo;					//��Ƶ�Ƿ�������
}FLVFILEAUDIO;

//Flv�ļ���Ƶ��Ϣ
typedef struct tagFlvFileVideo
{
	INFOITEM	width;					//��Ƶ��
	INFOITEM	height;					//��Ƶ��
	INFOITEM	videodatarate;			//��Ƶ����
	INFOITEM	framerate;				//֡��
	INFOITEM	videocodecid;			//��Ƶ������ID
}FLVFILEVIDEO;

//Flv�ļ���Ϣ�ṹ
typedef struct tagFlvFileInfo
{
	FLVFILEAUDIO	ffAudio;				//��Ƶ��Ϣ
	FLVFILEVIDEO	ffVideo;				//��Ƶ��Ϣ
	INFOITEM		duration;				//ʱ�� 
	INFOITEM		filesize;				//�ļ���С

}FLVFILEINFO;

//����ID��
enum emLayoutWndID
{
	LWID_TOP=4100,			//��������		
	 
	LWID_VIDEO_WND,			//��ʦ��Ƶ����ID
	LWID_VIDEO_WND1,		//ѧ����Ƶ����ID
	LWID_VIDEO_WND2,		//ѧ����Ƶ����ID
	LWID_RES_LIST,			//��Դ�б�ؼ�ID
	LWID_TOOL_BTN,			//���߰�ťID
	LWID_TOOL_BTN_MAX=LWID_TOOL_BTN+60,			//���߰�ťID
	LWID_PAGE_BTN1,			//��һҳ��ť
	LWID_PAGE_BTN12,		//��һҳ��ť
	LWID_TOOL_WND,			//���߰�ť����
	LWID_BBOARD_WND,		//�ڰ崰��ID
	LWID_PLAY_CONTORL,		//���ſ��ƴ���id

	LWID_MAX_BB_VIEW=LWID_BBOARD_WND+200,		//���ڰ崰��ID
};

enum emHelperOpt
{
	HOPT_ACCEPT,			//����
	HOPT_REJECT,			//�ܾ�
	HOPT_STOP,				//ֹͣ����
};

//////////// �ӿڶ��� ////////////////////////////////////////////////////////////////
#define API_VERSION			"/soft/version?ct=1"		//��ȡ�汾��
#define API_PUBLISHMSG		"/msg/publishMsg"			//��������
#define API_MSGLIST			"/msg/leaveWords"			//���������б�
#define API_UPBBPAGE		"/udfile/Upbbpage"			//�ϴ��ڰ�ҳ�ļ�URL
#define API_UPFILE			"/file/upload"				//�ϴ��ļ�
#define API_CLASSTABLE		"/oits/kcTable"				//���ؿγ̱�

//Json��
#define __JSON_BODY "{\"head\":{\"aid\":\"oitor\",\"ver\":\"1.0\",\"ln\":\"cn\",\"mod\":\"win\",\"sync\":1,\"cmd\":%d},\"con\":%s}"
//////////////////////////////////////////////////////////////////////////////////////
#define SETTIMER(id,elapse,repeat,param,sink) {if(g_pITimerEngine!=NULL){g_pITimerEngine->SetTimer(id,elapse,repeat,param,sink);}}
#define KILLTIMER(id){if(g_pITimerEngine!=NULL){g_pITimerEngine->KillTimer(id);}}

//////////////////////////////////////////////////////////////////////////////////////////
interface ILogOutput
{
	//�����Ϣ������
	virtual void __cdecl OutputLogToFile(LPCTSTR lpszMsg)=0;
	//�����Ϣ���ļ�
	virtual void __cdecl OutputFormateLog(LPCTSTR lpszMsg,...)=0;
};

typedef emLogLevel LOGLEVEL;
//��ʾ״̬
interface IShowLogToWnd
{
	//�����Ϣ������
	virtual void __cdecl OutputLogToWnd(LPCTSTR lpszMsg,LOGLEVEL ll=LL_NORMAL,BOOL bFlashWnd=FALSE)=0;
	//�����Ϣ������
	virtual void __cdecl OutputFormateToWnd(LOGLEVEL ll,BOOL bFlashWnd,LPCTSTR lpszMsg,...)=0;
	//�����Ϣ������
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd)=0;
};
 
//Http���ݽӿ�
struct IHttpDataSink
{
	//���ݴ�����ɽӿ�
	virtual bool __cdecl HttpCompleteNotify(int nStatus,LPCSTR lpszResult,WPARAM wParam,const void * pDownObject)=0;
	//��ȡҪ���������
	virtual DOWNPARAM * __cdecl GetHttpTransData()=0;
};

//����������ݻص�
struct IHttpApiEventSink
{
	//���ݴ�����ɽӿ�
	virtual bool __cdecl HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt)=0;
};

//Http���ع���ӿ�
struct IHttpDownManage
{
	//������ؿ�ʱ�б��¼�
	virtual bool __cdecl AddHttpApiEvent(const int ldt,WPARAM wParam,void *pHttpSink)=0;
	//���δ��������
	virtual bool __cdecl ClearHttpApiEvent()=0;
};

//Flv���ݻص��ӿ�
struct IFlvDataSink
{
	//��Ƶ�ص��ӿ�
	virtual bool __cdecl OnX264VideoData(AVCPTYPE avpt,BYTE *pVideoData,DWORD dwDataSize,DWORD dwTagTime,DWORD dwPackTime)=0;
};
 
//��Ƶ����ӿ�
interface IVideoManage
{
	//����H264SpsPps����
	virtual bool __cdecl SetH264SpsPpsData(const BYTE * pSpsPps,WORD wDataSize)=0;
	//������Ƶ��
	virtual bool __cdecl PlayVideoStream(const BYTE *pVideoData,DWORD dwDataSize)=0;
	//��ȡ�¼�����С
	virtual void __cdecl GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY)=0;
	//�û��˳�֪ͨ
	virtual void __cdecl OnCustomExitNotify()=0;
};

//��������¼�
interface ISendMouseEvent
{
	//��������¼�
	virtual bool __cdecl SendMouseEventData(int nEventType,int x,int y)=0;
	//���ͼ����¼�
	virtual bool __cdecl SendKeyboardEvent(int nKeyValue,int nKeyType)=0;
	//����ѡ����
	virtual void __cdecl SendSelectedResult(UNDWORD udwClientCode,int nResult)=0;
};

////////////////////////////////////////////////////////////////////////////////////////
//��ȡ����+��Ƶ�ӿ�
ISnapVideoManage *AfxGetSnapVideoManage();
//���ÿμ���Ƶ״̬
BOOL AfxCourseAudioStatus();
//��ȡHttp���ؽӿ�
IHttpDownManage *AfxGetHttpDownManage();

//ʱ��ת�ַ���
void Duration2String(DWORD dwTime,TCHAR *pszTimer,int nBufferSize);
//////////////////////////////////////////////////////////////////////////////////////
extern HINSTANCE g_hInstance;
extern GLOBALDATA g_GlobalData;
extern ILogOutput *g_pILogOutput;
extern IShowLogToWnd *g_pIShowLogToWnd;
extern TCHAR	g_szWorkPath[MAX_PATH];	

#endif //__DEIR_BROWSER_LOCALDEF_HEAD_FILE__
