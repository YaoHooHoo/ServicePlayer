////////////////////////////////////////////////////////////////////////////////////
//		Http逻辑数据管理类头文件 LogicDataManage.h
//		2016-08-04 Bowen.hu
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////
class CLogicDataManage
{
public:
	CLogicDataManage(void);
	~CLogicDataManage(void);

public:
	//获取Api参数
	bool GetHttpApiParam(OITLDT ldt,string &strUrl,string &strJson,WPARAM wParam=NULL);
	//解析Api数据
	bool ParseHttpApiJson(OITLDT ldt,int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpCompleteSink);

private:
	//获取jsonBody部份
	inline bool GetJsonBodyObject(LPCSTR lpszJsonData,Json::Value &jsonBody);
	//获取Http全路径
	inline bool GetHttpFullUrl(LPCSTR lpszPath,string &strUrl);

private:
	//处理请求终端号返回结果
	bool OnProcReqDevCodeResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt);
	//处理请求配置数据返回结果
	bool OnProcReqConfigResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt);

private:
	//请求终端号
	bool RequestDevCodeParam(WPARAM wParam,string &strUrl,string &strJson);
	//请求配置文件
	bool RequestConfigData(WPARAM wParam,string &strUrl,string &strJson);
	
private:
	//获取Http错误信息
	LPCTSTR GetHttpErrInfo(int nStatus);
};
