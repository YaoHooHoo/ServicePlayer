////////////////////////////////////////////////////////////////////////////////////
//		���ߺ���ͷ�ļ� ToolsFunction.h
/////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "localdef.h"
////////////////////////////////////////////////////////////////////////////////
//ת��Ϊʱ��
void OnConvertToTime(DWORD dwTime,LPTSTR lpszTime,int nMaxSize);
//���ز���
bool LoadParameter();
//��ȡ�����ļ���
bool GetConfigFile(LPCTSTR lszFile,LPTSTR lpszFullPath);
//���ͷ�������ַ
DWORD GetLocalHostAddr();
//�������
void SerailParameter(bool bRead);