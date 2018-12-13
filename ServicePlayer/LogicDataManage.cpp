////////////////////////////////////////////////////////////////////////////////////
//		Http�߼����ݹ�����ʵ���ļ� LogicDataManage.cpp
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

//��ȡApi����
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

//����Api����
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

//��ȡHttpȫ·��
inline bool CLogicDataManage::GetHttpFullUrl(LPCSTR lpszPath,string &strUrl)
{
	CHAR szUrl[URL_LEN]="";
	_snprintf_c(szUrl,CountArray(szUrl),"%s/%s",g_GlobalData.szApiUrl,lpszPath);

	strUrl=szUrl;

	return true;
}

//��ȡ����û������
bool CLogicDataManage::RequestDevCodeParam(WPARAM wParam,string &strUrl,string &strJson)
{
	TCHAR szServCode[32]=TEXT("");
	mysprintf(szServCode,CountArray(szServCode),TEXT("%u"),g_GlobalData.mDevSnData.dwServiceCode);

 	GetHttpFullUrl("devCode",strUrl);
	strUrl+="?scode=";
	strUrl+=szServCode;

	return true;
}

//���������ļ�
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
 //��ȡjsonBody����
inline bool CLogicDataManage::GetJsonBodyObject(LPCSTR lpszJsonData,Json::Value & jsonBody)
{
	//��������
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
//���������ն˺ŷ��ؽ��
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
			//��������	result
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
	//�������
	if(pIHttpApiEventSink!=NULL)
		pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,NULL,0,ldt);

	return true;
}

//���������������ݷ��ؽ��
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
			//��������	{"termsn":"2017-01-04","service":1,"lisence":"4","conf":{"lbsip":"192.168.9.228:52288","vcx":1280,"vcy":720,"BusType":1}}
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
			reqData.nConnectIpType=-1;		//��Ч����
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
				Json::Value busType=objConf["BusType"];	//ҵ��������
				if(!busType.isNull())
				{
					reqData.nBusinessType=busType.asInt();
				}
				Json::Value ipType=objConf["iptype"];	//ip����
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
	//�������
	LPCTSTR lpszErrMsg=NULL;
	if(nStatus!=0) lpszErrMsg=GetHttpErrInfo(nStatus);
	if(pIHttpApiEventSink!=NULL)
		pIHttpApiEventSink->HttpEventNotify(nStatus,wParam,lpszErrMsg,0,ldt);

	return true;
}

//��ȡHttp������Ϣ
LPCTSTR CLogicDataManage::GetHttpErrInfo(int nStatus)
{
	static TCHAR szErrTxt[MAX_PATH]=TEXT("δ֪����");

	switch(nStatus)
	{
	case JSON_INVLAID:
		lstrcpy(szErrTxt,TEXT("Json��ʽ��Ч"));
		break;
	case CONF_NOT_EXIST:
		lstrcpy(szErrTxt,TEXT("�������ݲ�����"));
		break;
	case NAME_PASS_ERR:
		lstrcpy(szErrTxt,TEXT("�û������������"));
		break;
	case DB_OPT_FAILD:
		lstrcpy(szErrTxt,TEXT("���ݿ����ʧ��"));
		break;
	case DATA_NOT_EXISTS:
		lstrcpy(szErrTxt,TEXT("û�п���ʹ�õ�Lisence������������"));
		break;
	case DB_OPT_EXCEPTION:
		lstrcpy(szErrTxt,TEXT("���ݿ�����쳣"));
		break;
	case DB_UPDATE_FAILD:
		lstrcpy(szErrTxt,TEXT("���ݿ���°�"));
		break;
	case NO_ENOUGH_MEM:
		lstrcpy(szErrTxt,TEXT("û���㹻���ڴ�"));
		break;
	case JSON_VAL_INVALID:
		lstrcpy(szErrTxt,TEXT("��ЧJson����"));
		break;
	}
	return szErrTxt;
}
