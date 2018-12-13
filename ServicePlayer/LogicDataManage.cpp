////////////////////////////////////////////////////////////////////////////////////
//		Http逻辑数据管理类实现文件 LogicDataManage.cpp
//		2016-08-04 Bowen.hu
////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LogicDataManage.h"

////////////////////////////////////////////////////////////////////////////////////
CLogicDataManage::CLogicDataManage(void)
{
}

CLogicDataManage::~CLogicDataManage(void)
{
}

//获取Api参数
bool CLogicDataManage::GetHttpApiParam(OITLDT ldt,string &strUrl,string &strJson,WPARAM wParam)
{
	switch(ldt)
	{
	case LDT_REQ_CONFIG:
		return RequestConfigData(wParam,strUrl,strJson);
	case LDT_REQ_DEV_CODE:
		return RequestDevCodeParam(wParam,strUrl,strJson);
	}
	return false;
}

//解析Api数据
bool CLogicDataManage::ParseHttpApiJson(OITLDT ldt,int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpSink)
{
	switch(ldt)
	{
	case LDT_REQ_DEV_CODE:
		return OnProcReqDevCodeResult(nStatus,lpszJsonData,wParam,pIHttpSink,ldt);		
	case LDT_REQ_CONFIG:
		return OnProcReqConfigResult(nStatus,lpszJsonData,wParam,pIHttpSink,ldt);		
	}
	return false;
}

//获取Http全路径
inline bool CLogicDataManage::GetHttpFullUrl(LPCSTR lpszPath,string &strUrl)
{
	CHAR szUrl[URL_LEN]="";
	_snprintf_c(szUrl,CountArray(szUrl),"%s/%s",g_GlobalData.szApiUrl,lpszPath);

	strUrl=szUrl;

	return true;
}

//获取添加用户活动参数
bool CLogicDataManage::RequestDevCodeParam(WPARAM wParam,string &strUrl,string &strJson)
{
	TCHAR szServCode[32]=TEXT("");
	mysprintf(szServCode,CountArray(szServCode),TEXT("%u"),g_GlobalData.mDevSnData.dwServiceCode);

 	GetHttpFullUrl("devCode",strUrl);
	strUrl+="?scode=";
	strUrl+=szServCode;

	return true;
}

//请求配置文件
bool CLogicDataManage::RequestConfigData(WPARAM wParam,string &strUrl,string &strJson)
{
	int nTermLen=lstrlen(g_GlobalData.mDevSnData.szTermSN);
	if(nTermLen>0)
		GetHttpFullUrl("config",strUrl);
	else
		GetHttpFullUrl("create",strUrl);
	strUrl+="?termno=";
	strUrl+=g_GlobalData.mDevSnData.szTermSN;
	strUrl+="&jobnum=";
	strUrl+=g_GlobalData.mDevSnData.szJobNum;
	strUrl+="&pass=";
	TCHAR szEncPass[WNAME_LEN]=TEXT("");
	EncryptData32(g_GlobalData.mDevSnData.szPassword,szEncPass);
	szEncPass[32]=0;
	strUrl+=szEncPass;
	strUrl+="&uname=";
	strUrl+=g_GlobalData.szUserName;
	strUrl+="&host=";
	strUrl+=g_GlobalData.szHostName;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //获取jsonBody部份
inline bool CLogicDataManage::GetJsonBodyObject(LPCSTR lpszJsonData,Json::Value & jsonBody)
{
	//解析数据
	Json::Features features;
	features.strictMode();
	Json::Value root;
	Json::Reader reader(features);
	std::string input=lpszJsonData;
	bool parsingSuccessful=reader.parse(input,root);
	if (!parsingSuccessful)
	{
		return false;
	}

	Json::ValueType vt=root.type();
	if(vt!=Json::objectValue) return false;

	jsonBody=root;

	return true;
}

/////////////
//处理请求终端号返回结果
bool CLogicDataManage::OnProcReqDevCodeResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt)
{
	IHttpApiEventSink *pIHttpApiEventSink=(IHttpApiEventSink *)pIHttpApiSink;
	
	if(nStatus<0)
	{
		if(pIHttpApiEventSink!=NULL)
			pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,lpszJsonData,0,ldt);
		return true;
	}
	try
	{
		do
		{
			//解析数据	result
			Json::Value jsonBody;			
			Json::Value cList;	 
			if(!GetJsonBodyObject(lpszJsonData,jsonBody)) break;
			if(jsonBody.isNull()) break;

			double dbResult=jsonBody["result"].asDouble();
			DWORD dwCount=(DWORD)dbResult;
			if(pIHttpApiEventSink!=NULL)
				pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,&dwCount,sizeof(dwCount),ldt);
			 
		}while(false);
	}
	catch(...){}
	//处理完成
	if(pIHttpApiEventSink!=NULL)
		pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,NULL,0,ldt);

	return true;
}

//处理请求配置数据返回结果
bool CLogicDataManage::OnProcReqConfigResult(int nStatus,LPCSTR lpszJsonData,WPARAM wParam,void *pIHttpApiSink,OITLDT ldt)
{
	IHttpApiEventSink *pIHttpApiEventSink=(IHttpApiEventSink *)pIHttpApiSink;
	#if IS_TEST_HTTP
		if(g_pILogOutput!=NULL)
			g_pILogOutput->OutputFormateLog(TEXT("--JsonData:%s"),lpszJsonData);
	#endif

	if(nStatus<0)
	{
		if(pIHttpApiEventSink!=NULL)
			pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,lpszJsonData,0,ldt);
		return true;
	}
	try
	{
		do
		{
			//解析数据	{"termsn":"2017-01-04","service":1,"lisence":"4","conf":{"lbsip":"192.168.9.228:52288","vcx":1280,"vcy":720,"BusType":1}}
			Json::Value jsonBody;
			if(!GetJsonBodyObject(lpszJsonData,jsonBody))
			{
				nStatus=JSON_INVLAID;
				break;
			}
			if(jsonBody.isNull())
			{
				nStatus=JSON_INVLAID;
				break;
			}

			REQCONFDATA reqData={0};
			reqData.nConnectIpType=-1;		//无效类型
			const TCHAR *pLinsence=jsonBody["lisence"].asCString();
			int nValLength=lstrlen(pLinsence);
			if(nValLength>=MAX_NAME)
			{
				nStatus=JSON_VAL_INVALID;
				break;
			}
			lstrcpy(reqData.szLisence,pLinsence);
			const TCHAR *pTermSN=jsonBody["termsn"].asCString();
			nValLength=lstrlen(pTermSN);
			if(nValLength>=WNAME_LEN)
			{
				nStatus=JSON_VAL_INVALID;
				break;
			}
			lstrcpy(reqData.szTermSN,pTermSN);
			DWORD dwServiceCode=jsonBody["service"].asUInt();
			nStatus=jsonBody["ret"].asInt();
			if(nStatus!=0) break;

			Json::Value objConf=jsonBody["conf"];
			if(objConf.isNull())
			{
				nStatus=CONF_NOT_EXIST;
				break;
			}
			if(!objConf.empty())
			{
				const TCHAR *pLbsIP=objConf["lbsip"].asCString();
				nValLength=lstrlen(pTermSN);
				if(nValLength>=MAX_PATH)
				{
					nStatus=JSON_VAL_INVALID;
					break;
				}
				lstrcpy(reqData.szLbSvrIpPort,pLbsIP);

				Json::Value vcxValue=objConf["vcx"];
				if(!vcxValue.isNull())
				{
					reqData.wVideoCX=vcxValue.asInt();
				}

				Json::Value vcyValue=objConf["vcy"];
				if(!vcyValue.isNull())
				{
					reqData.wVideoCY=vcyValue.asInt();
				}
				Json::Value busType=objConf["BusType"];	//业务线类型
				if(!busType.isNull())
				{
					reqData.nBusinessType=busType.asInt();
				}
				Json::Value ipType=objConf["iptype"];	//ip类型
				if(!ipType.isNull())
				{
					reqData.nConnectIpType=ipType.asInt();
				}
			}
						
			reqData.dwSerivceCode=dwServiceCode;

			if(pIHttpApiEventSink!=NULL)
				pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,&reqData,sizeof(reqData),ldt);
			 
		}while(false);
	}
	catch(...){}
	//处理完成
	LPCTSTR lpszErrMsg=NULL;
	if(nStatus!=0) lpszErrMsg=GetHttpErrInfo(nStatus);
	if(pIHttpApiEventSink!=NULL)
		pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,lpszErrMsg,0,ldt);

	return true;
}

//获取Http错误信息
LPCTSTR CLogicDataManage::GetHttpErrInfo(int nStatus)
{
	static TCHAR szErrTxt[MAX_PATH]=TEXT("未知错误");

	switch(nStatus)
	{
	case JSON_INVLAID:
		lstrcpy(szErrTxt,TEXT("Json格式无效"));
		break;
	case CONF_NOT_EXIST:
		lstrcpy(szErrTxt,TEXT("配置内容不存在"));
		break;
	case NAME_PASS_ERR:
		lstrcpy(szErrTxt,TEXT("用户名或密码错误"));
		break;
	case DB_OPT_FAILD:
		lstrcpy(szErrTxt,TEXT("数据库操作失败"));
		break;
	case DATA_NOT_EXISTS:
		lstrcpy(szErrTxt,TEXT("没有可以使用的Lisence，请重新申请"));
		break;
	case DB_OPT_EXCEPTION:
		lstrcpy(szErrTxt,TEXT("数据库操作异常"));
		break;
	case DB_UPDATE_FAILD:
		lstrcpy(szErrTxt,TEXT("数据库更新败"));
		break;
	case NO_ENOUGH_MEM:
		lstrcpy(szErrTxt,TEXT("没有足够的内存"));
		break;
	case JSON_VAL_INVALID:
		lstrcpy(szErrTxt,TEXT("无效Json数据"));
		break;
	}
	return szErrTxt;
}
