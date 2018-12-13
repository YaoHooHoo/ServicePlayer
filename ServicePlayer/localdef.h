///////////////////////////////////////////////////////////////////////////////////////
//			全局定义头文件 localdef.h
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
#define SUPPORT_MOUSE		0				//支持鼠标键盘
#define IS_TEST_HTTP		1				//测试状态HTTP

#define NET_TIMEOUT			60000			//网络测试超时时间
#define HTTP_DOWN_NUM		1				//Http下载线程数
#define WRITE_FILE			0				//写入文件
#define READ_TEST_FILE		0				//读取测试文件
#define TEST_DEBUG			0				//调试测试

#define SAVB_APP_TIPIC		TEXT("平安视频辅助系统客服端")
////////////////////////////////////////////////////////////////////////////////////
typedef map<DWORD,void *>					CDwordPointMap;		
typedef map<DWORD,void *>::iterator			CDwordPointMapIt;
typedef map<string,string>					CTelVlaueMap;			
typedef map<string,string>::iterator		CTelVlaueMapIt;	
///////////////////////////////////////////////////////////////////////////////////////
#define MYTRACE TRACE

#define DST_V_CX		1280//1600	//1280			//目标视频宽
#define DST_V_CY		720//900		//720		//目标视频高

//////////////////////////////////////////////////////////////////////////////
#define SAMPLE_RATE		16000		//44100
#define CHANNEL_NUM		1
#define AAC_MAIN		1			//AAC-MAIN
#define AAC_LC			2			//AAC-LC

//参数类型
#define VAL_STR 0 
#define VAL_INT 1
#define VIDEO_BORDER			4				//视频边宽
#define BOTTOM_HIGHT			30				//底部高
#define MAX_WND_CX			1600					//最大窗口宽
#define MAX_WND_CY			900						//最大窗口高
#define MIN_WND_CY			800						//最小窗口高

#define LISENCE_DELAY_DAYS		-15					//Lisence延迟天数
#define MAXFAILCOUNT		3						//最大失败次数

//定时器定义
#define IDT_CONN_SERVER					0x0001					//连接到服务器id
#define IDT_SHOW_DATA_RATE				0x0002					//显示带宽定时器
#define IDT_DOWN_DEVCODE				0x0003					//下载终端编号
#define IDT_START_CONNN					0x0004					//开始连接服务器
#define IDT_LOGIN_PROCESS				0x0005					//登录进度条	
#define IDT_REQUEST_CONFIG				0x0006					//重新请求配置信息
#define IDT_CHECK_WND_ACTIVE			0x0007					//验测窗口活动状态
#define IDT_TEST_VIDEO_FILES			0x0008					//测试视频数据

////////////////////////////////////////////////////////////////////////////////
#define WM_NOTIFYICONDATA_MSG			(WM_APP+1012)			//托盘消息
#define WM_ACCEPT_NOTIFY_MSG			(WM_APP+1013)			//接受辅助消息
#define WM_CHECK_WND_ACTIVE				(WM_APP+1014)			//验测窗口活动状态

///////////////////////////////////////////////////////////////////////////
//终端持久化数据
typedef struct tagDevSNData
{
	BOOL			bAutoLogin;					//自动登录
	DWORD			dwServiceCode;				//客服号
	TCHAR			szJobNum[NAME_LEN];			//工号
	TCHAR			szTermSN[NAME_LEN];			//终端编号
	TCHAR			szLisence[MAX_NAME];		//Lisence
	TCHAR			szPassword[NAME_LEN];		//密码
	TCHAR			szSequenceID[SEQSIZE];		//唯一标识
}DEVSNDATA;

//全局数据定义
typedef struct tagGlobalData
{
	int				nConnectIpType;				//连接服务器IP(0=lvs ip,1=avs ip)
	BOOL			bOpenglDraw;				//使用OpenGL绘制
	DWORD			dwCurrTheadID;				//当前线程ID
	int				nBusinessType;				//业务线类型
	int				nLisenceDays;				//Lisence乘余天数
	BOOL			bInvalidLisence;			//无效lisence
	BOOL			bHaveVoice;					//语音标志
	BOOL			bIsHaveMouseCtrl;			//是否有鼠标控制
	BOOL			bIsMinWnd;					//最小化窗口
	BOOL			bWndNotActive;				//是否活动
	BOOL			bIsNotifyIcon;				//托盘状态标志
	HWND			hMainWnd;					//主窗口句柄
	UNDWORD			udwClientCode;				//客户号
	DWORD			dwUserID;					//用户ID
	DWORD			dwAvSvrIP;					//视频服务器IP
	DWORD			dwLBSvrIP;					//负载服务器IP
	WORD			wAvSvrPort;					//视频服务器端口
	WORD			wLBSvrPort;					//负载服务器端口
	WORD			wVideoCX;					//视频宽
	WORD			wVideoCY;					//视频高
	LBSVRDATA		mLbsList[MAX_LBS];			//负载均衡服务器
	WORD			wLbsCount;					//负载均衡服务数
	BOOL			bExitApp;					//退出APP标志
	BOOL			bServicing;					//服务中标志
	BOOL			bConfirming;				//确认中
	TCHAR			szApiUrl[MAX_PATH];			//Api地址
	TCHAR			szSavePath[MAX_PATH];		//存储文件
	TCHAR			szWorkPath[MAX_PATH];		//工作路径
	TCHAR			szLogPath[MAX_PATH];		//日志目录
	TCHAR			szHostName[NAME_LEN];		//主机名
	TCHAR			szUserName[NAME_LEN];		//用户名

	DEVSNDATA		mDevSnData;					//终端持久化数据
	
}GLOBALDATA;

//配置文件数据
typedef struct tagReqConfData
{
	int				nConnectIpType;				//连接服务器IP(0=lvs ip,1=avs ip)
	int				nBusinessType;				//业务线类型
	WORD			wVideoCX;					//视频宽
	WORD			wVideoCY;					//视频高
	DWORD			dwSerivceCode;				//客服号
	TCHAR			szLbSvrIpPort[MAX_PATH];	//负载服务器ip端口
	TCHAR			szLisence[MAX_NAME];		//Lisence
	TCHAR			szTermSN[WNAME_LEN];		//终端号
}REQCONFDATA;

//连接ip类型
enum emConnIpType
{
	CIPT_LVS,					//Lvs 服务器IP
	CIPT_AVS,					//avs 服务器IP
};

////////////// FLV 定义 ///////////////////////////////////////////////////
//	FLV tag 类型
#define FT_AUDIO	0x08			//音频
#define FT_VIDEO	0x09			//视频
#define FT_SCRIPT	0x12			//脚本

typedef enum emAMFType
{
	AMFT_NUMBER,					//数值(8Byte)
	AMFT_BOOLLEAN,					//布尔值(1Byte)
	AMFT_STRING,					//字符串(2Byte长度+字符串数据)
	AMFT_OBJECT,					//对象(包含多个AMF类型数据)
	AMFT_MOVCLIP,					//
	AMFT_NULL,						//空值
	AMFT_UNDEFINED,					//未定义类型
	AMFT_REFERENCE,					//引用类型
	AMFT_ECMA_ARRAY,				//ECMA数组类型
	AMFT_OBJECT_END,				//结束
	AMFT_STRICT_ARRAY=10,			//Strict数组类型
	AMFT_DATE,						//日期类型
	AMFT_LONGSTRING,				//长字符串(4Byte长度+字符串数据)
}AMFTYPE;

//视频编码类型
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

//视频帧类型
typedef enum emVFrameType
{
	VFT_KEYFRAME=1,				//关键帧
	VFT_NOKEYFRAME,				//非关键帧
	VFT_263NOKEYFRAME,			//h263非关键帧
	VFT_SERVERKEYFRAME,			//服务器生成关键帧
	VFT_VIDEOINFO,				//视频信息
}VFRAMETYPE;

//AVC包类型
typedef enum emAVCPType
{
	AVCPT_SEQHEADER,			//AVCSequence Header
	AVCPT_NALU,					//AVC NALU 数据
	AVCPT_SEQEND,				//AVC end ofsequence
}AVCPTYPE;

//AAC包类型
typedef enum emAACPType
{
	AACPT_SEQUENCE,				//AAC Sequence Header
	AACPT_RAW,					//AAC 数据
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

//Flv文件头
#pragma pack(push,1)
typedef struct tagFlvHeader
{
	CHAR		szType[3];				//文件类型
	BYTE		cbVersion;				//版本信息
	union
	{
		BYTE		cbStreamInfo;
		struct
		{
			BYTE		cbHaveVideo:1;			//是否有视频；流信息(前5位保留,必须为0;第6位表示是否不音频；第7位保留，必须为0;第8位表示是否有视频)
			BYTE		cbReseve7:1;			//保留(1bit)
			BYTE		cbHaveAudio:1;			//是否有音频
			BYTE		cbReseve5:5;			//保留(5bit)		
		};
	};
	DWORD		dwHeadLength;			//头长度
}FLVHEADER;

//Tag头
typedef struct tagTagHeader
{
	DWORD		dwPrevTagSize;			//前一个tag大小
	BYTE		cbTagType;				//Tag类型(8:音频,9:视频;18:脚本数据(AMF包),其他：保留)
	DWORD		dwDataSize;				//数据长度
	DWORD		dwTimeStamp;			//时间戳
	BYTE		cbHighTime;				//时间高位值(当3个字节时间不够用时使用此字段)
	DWORD		dwStreamID;				//流ID，总为0
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

//Flv文件音频信息
typedef struct tagFlvFileAudio
{
	INFOITEM	audiocodecid;			//音频解码器ID
	INFOITEM	audiodatarate;			//音频码率
	INFOITEM	audiosamplerate;		//音频采样率
	INFOITEM	audiosamplesize;		//音频采样大小
	INFOITEM	stereo;					//音频是否立体声
}FLVFILEAUDIO;

//Flv文件视频信息
typedef struct tagFlvFileVideo
{
	INFOITEM	width;					//视频宽
	INFOITEM	height;					//视频高
	INFOITEM	videodatarate;			//视频码率
	INFOITEM	framerate;				//帧率
	INFOITEM	videocodecid;			//视频解码器ID
}FLVFILEVIDEO;

//Flv文件信息结构
typedef struct tagFlvFileInfo
{
	FLVFILEAUDIO	ffAudio;				//音频信息
	FLVFILEVIDEO	ffVideo;				//视频信息
	INFOITEM		duration;				//时长 
	INFOITEM		filesize;				//文件大小

}FLVFILEINFO;

//窗口ID号
enum emLayoutWndID
{
	LWID_TOP=4100,			//顶部窗口		
	 
	LWID_VIDEO_WND,			//教师视频窗口ID
	LWID_VIDEO_WND1,		//学生视频窗口ID
	LWID_VIDEO_WND2,		//学生视频窗口ID
	LWID_RES_LIST,			//资源列表控件ID
	LWID_TOOL_BTN,			//工具按钮ID
	LWID_TOOL_BTN_MAX=LWID_TOOL_BTN+60,			//工具按钮ID
	LWID_PAGE_BTN1,			//上一页按钮
	LWID_PAGE_BTN12,		//下一页按钮
	LWID_TOOL_WND,			//工具按钮窗口
	LWID_BBOARD_WND,		//黑板窗口ID
	LWID_PLAY_CONTORL,		//播放控制窗口id

	LWID_MAX_BB_VIEW=LWID_BBOARD_WND+200,		//最大黑板窗口ID
};

enum emHelperOpt
{
	HOPT_ACCEPT,			//接受
	HOPT_REJECT,			//拒绝
	HOPT_STOP,				//停止服务
};

//////////// 接口定义 ////////////////////////////////////////////////////////////////
#define API_VERSION			"/soft/version?ct=1"		//获取版本号
#define API_PUBLISHMSG		"/msg/publishMsg"			//发布留言
#define API_MSGLIST			"/msg/leaveWords"			//下载留言列表
#define API_UPBBPAGE		"/udfile/Upbbpage"			//上传黑板页文件URL
#define API_UPFILE			"/file/upload"				//上传文件
#define API_CLASSTABLE		"/oits/kcTable"				//下载课程表

//Json包
#define __JSON_BODY "{\"head\":{\"aid\":\"oitor\",\"ver\":\"1.0\",\"ln\":\"cn\",\"mod\":\"win\",\"sync\":1,\"cmd\":%d},\"con\":%s}"
//////////////////////////////////////////////////////////////////////////////////////
#define SETTIMER(id,elapse,repeat,param,sink) {if(g_pITimerEngine!=NULL){g_pITimerEngine->SetTimer(id,elapse,repeat,param,sink);}}
#define KILLTIMER(id){if(g_pITimerEngine!=NULL){g_pITimerEngine->KillTimer(id);}}

//////////////////////////////////////////////////////////////////////////////////////////
interface ILogOutput
{
	//输出消息到窗口
	virtual void __cdecl OutputLogToFile(LPCTSTR lpszMsg)=0;
	//输出消息到文件
	virtual void __cdecl OutputFormateLog(LPCTSTR lpszMsg,...)=0;
};

typedef emLogLevel LOGLEVEL;
//显示状态
interface IShowLogToWnd
{
	//输出消息到窗口
	virtual void __cdecl OutputLogToWnd(LPCTSTR lpszMsg,LOGLEVEL ll=LL_NORMAL,BOOL bFlashWnd=FALSE)=0;
	//输出消息到窗口
	virtual void __cdecl OutputFormateToWnd(LOGLEVEL ll,BOOL bFlashWnd,LPCTSTR lpszMsg,...)=0;
	//输出消息到窗口
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd)=0;
};
 
//Http数据接口
struct IHttpDataSink
{
	//数据传输完成接口
	virtual bool __cdecl HttpCompleteNotify(int nStatus,LPCSTR lpszResult,WPARAM wParam,const void * pDownObject)=0;
	//获取要传输的数据
	virtual DOWNPARAM * __cdecl GetHttpTransData()=0;
};

//下载完成数据回调
struct IHttpApiEventSink
{
	//数据传输完成接口
	virtual bool __cdecl HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt)=0;
};

//Http下载管理接口
struct IHttpDownManage
{
	//添加下载课时列表事件
	virtual bool __cdecl AddHttpApiEvent(const int ldt,WPARAM wParam,void *pHttpSink)=0;
	//清除未下载数据
	virtual bool __cdecl ClearHttpApiEvent()=0;
};

//Flv数据回调接口
struct IFlvDataSink
{
	//视频回调接口
	virtual bool __cdecl OnX264VideoData(AVCPTYPE avpt,BYTE *pVideoData,DWORD dwDataSize,DWORD dwTagTime,DWORD dwPackTime)=0;
};
 
//视频管理接口
interface IVideoManage
{
	//设置H264SpsPps数据
	virtual bool __cdecl SetH264SpsPpsData(const BYTE * pSpsPps,WORD wDataSize)=0;
	//播放视频流
	virtual bool __cdecl PlayVideoStream(const BYTE *pVideoData,DWORD dwDataSize)=0;
	//获取事件区大小
	virtual void __cdecl GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY)=0;
	//用户退出通知
	virtual void __cdecl OnCustomExitNotify()=0;
};

//发送鼠标事件
interface ISendMouseEvent
{
	//发送鼠标事件
	virtual bool __cdecl SendMouseEventData(int nEventType,int x,int y)=0;
	//发送键盘事件
	virtual bool __cdecl SendKeyboardEvent(int nKeyValue,int nKeyType)=0;
	//发送选择结果
	virtual void __cdecl SendSelectedResult(UNDWORD udwClientCode,int nResult)=0;
};

////////////////////////////////////////////////////////////////////////////////////////
//获取截屏+视频接口
ISnapVideoManage *AfxGetSnapVideoManage();
//设置课件音频状态
BOOL AfxCourseAudioStatus();
//获取Http下载接口
IHttpDownManage *AfxGetHttpDownManage();

//时长转字符串
void Duration2String(DWORD dwTime,TCHAR *pszTimer,int nBufferSize);
//////////////////////////////////////////////////////////////////////////////////////
extern HINSTANCE g_hInstance;
extern GLOBALDATA g_GlobalData;
extern ILogOutput *g_pILogOutput;
extern IShowLogToWnd *g_pIShowLogToWnd;
extern TCHAR	g_szWorkPath[MAX_PATH];	

#endif //__DEIR_BROWSER_LOCALDEF_HEAD_FILE__
