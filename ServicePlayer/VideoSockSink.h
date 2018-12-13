//////////////////////////////////////////////////////////////////////////////////////////
//		屏幕截屏回调类头文件 VideoSockSink.h.h
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"
#include "SkinImage.h"
#include "TimerEngine.h"
#include "LogFileManage.h"

////////////////////////////////////////////////////////////////////////////
#define DIS_READ		0					//直接读取av文件
#define DIS_REC			1					//直接录屏
//////////////////////////////////////////////////////////////////////////////////////////
class CVideoSockSink:public IClientSocketSink,public ITimerSink,public ISendMouseEvent,public IAudioDataSink
{
private:
	bool						m_bInited;						//初始化标志
	BOOL						m_bLogined;						//创建标志
	int							m_nOffsetX;						//X偏移量
	int							m_nOffsetY;						//Y偏移量
	int							m_nVideoCX;						//视频宽
	int							m_nVideoCY;						//视频高
	int							m_nWndCX;						//窗口宽
	int							m_nWndCY;						//窗口高
	DWORD						m_dwAvsConnNum;					//Avserver连接次数
	CONNECTSVRTYPE				m_cst;							//连接服务器类型
	DWORD						m_dwFailNum;					//失败次数
	UNDWORD						m_udwLastConfirm;				//最后一个确认

private:
	IVideoManage				*m_pIVideoManage;				//视频管理接口
	IAudioModuleManage			*m_pIAudioModuleManage;			//音频管理接口

private:
	CSingleSockClientHelper		m_SocketClientHelper;			//网络组件
	IClientSocket				*m_pIClientSocket;				//网络接口

private:
	BYTE						*m_pSpsData;					//Sps数据
	DWORD						m_dwSpsSize;					//Sps数据大小
	DWORD						m_dwDataSize;					//数据大小
	
#if WRITE_FILE
private:
	CLogFileManage				m_vDataFile;					//视频文件
#endif

public:
	CVideoSockSink(void);
	~CVideoSockSink(void);

public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return this!=NULL; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

public:
	//初始化视频网络调类
	bool InitVideoSockSink(IVideoManage *pIVideoManage);
	//设置音频管理接口
	bool SetAudioModuleManage(IAudioModuleManage *pIAudioModuleManage);
	//连接到服务器
	bool ConnectToServer(CONNECTSVRTYPE cst);
	//开始视频网络连接
	bool StartVideoSocket();
	//停止视频网络连接
	bool StopVideoSocket();
	//获取数据大小
	DWORD GetVideoDataSize() const {return m_dwDataSize;}

public:
	//H264SpsPps数据
	virtual bool __cdecl OnH264SpsPpsData(const void * pHeadData,DWORD dwDataSize);
	//截屏视频数据回调
	virtual bool __cdecl OnSnapVideoData(const void * pImageData,DWORD dwDataSize,VENCODETYPE sst);
	//录音数据回调
	virtual bool __cdecl OnRecordData(const void * pAudioData,DWORD dwDataSize);

public:
	//网络连接消息
	virtual bool __cdecl OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket);
	//网络读取消息
	virtual bool __cdecl OnSocketRead(CMD_Command Command, void * pBuffer, DWORD dwDataSize, IClientSocket * pIClientSocket);
	//网络关闭消息
	virtual bool __cdecl OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer);
	//定时器事件
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam);
	//发送鼠标事件
	virtual bool __cdecl SendMouseEventData(int nEventType,int x,int y);
	//发送键盘事件
	virtual bool __cdecl SendKeyboardEvent(int nKeyValue,int nKeyType);
	//发送选择结果
	virtual void __cdecl SendSelectedResult(UNDWORD udwClientCode,int nOptValue);

private:
	//请求服务器地址
	bool RequestSvrIPPort();
	//登录到配置服务器
	bool LoginToAvsSvr();
	//创建视频文件
	bool WriteVideoToFile(const void *pVideoData,DWORD dwDataSize,AVCPTYPE avType);
	//切换负载IP
	void SwitchLoadServerIpPort();

private:
	//序列化最后客户号
	void SeriralClientCode(bool bWrite);
	//读取测试数据
	void ReadVideoTestData();

private:
	//处理心跳测试
	bool OnProcActiveTest(IClientSocket * pIClientSocket);
	//处理请求服务器地址Response
	bool OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理登录服务器Response
	bool OnProcLoginToServerResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理H264SpsPps数据
	bool OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize);
	//处理H264视频数据
	bool OnProcH264VideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理请求辅助确认消息
	bool OnProcReqHelperConfirm(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理客户退出通知
	bool OnProcCustomExitNotify(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理客户音频数据
	bool OnProcUserAudioData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//处理停止辅助Response
	bool OnProcStopHelperResp(void *pDataBuffer,DWORD dwDataSize);
};

