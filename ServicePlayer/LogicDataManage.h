////////////////////////////////////////////////////////////////////////////////////
//		Http�߼����ݹ�����ͷ�ļ� LogicDataManage.h
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
	//��ȡApi����
	bool GetHttpApiParam(OITLDT ldt,string &strUrl,string &strJson,WPARAM wParam=NULL);
	//����Api����
	bool ParseHttpApiJson(OITLDT ldt,int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpCompleteSink);

private:
	//��ȡjsonBody����
	inline bool GetJsonBodyObject(LPCSTR lpszJsonData,Json::Value &jsonBody);
	//��ȡHttpȫ·��
	inline bool GetHttpFullUrl(LPCSTR lpszPath,string &strUrl);

private:
	//���������ն˺ŷ��ؽ��
	bool OnProcReqDevCodeResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt);
	//���������������ݷ��ؽ��
	bool OnProcReqConfigResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt);

private:
	//�����ն˺�
	bool RequestDevCodeParam(WPARAM wParam,string &strUrl,string &strJson);
	//���������ļ�
	bool RequestConfigData(WPARAM wParam,string &strUrl,string &strJson);
	
private:
	//��ȡHttp������Ϣ
	LPCTSTR GetHttpErrInfo(int nStatus);
};
