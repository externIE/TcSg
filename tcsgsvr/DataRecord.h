#pragma once
#include <map>
#include <set>

/*
	ʹ��˵����
		1.��ʼ����
			a.��Sever.cpp��ӱ�ͷ�ļ�
			b.��CGameServer::Initialize������������´��룺
				DataRecord::Instance()->init(m_szIniFile, this);
				DataRecord::Instance()->ReadInfos();

		2.����TransmitGameResultEx��������´��룺
			BOOL ret = __super::TransmitGameResultEx(pTable, lpContext, lpRefreshResult, lpGameResult, nGameResultSize);
			if (!ret)
				return FALSE;
		
			DataRecord::Instance()->UpdateInfos(pTable, lpRefreshResult, lpGameResult);
			return ret;

		3.����OnGameWin��������´��룺
			BOOL ret = __super::OnGameWin(lpContext, pRoom, pTable, chairno, bout_invalid, roomid);
			if (ret && !bout_invalid)
			{
				DataRecord::Instance()->UpdateRoomBout(roomid, pTable->m_nTableNO);
			}
			return ret;

		4.��Server.h��CGameServer�������friend class DataRecord;

		5.���˲�Ҳ�ɺ��ԣ���CGameServer����������������´��룺
			DataRecord::Instance()->Purge();

	ini������Ϣ��
		�����[DataRecordParams]����
		1.timeInterval=180����ʾ������ʱ��180�룬Ĭ��180��
		2.formatLen=20����ʾÿ�����ݵ��ַ������ȣ�������룬Ĭ��20��������64λ������������19���ַ������ȣ�����20����С�ĳ��ȣ�����20���µĶ�ȡ20��
		3.roomSvrName=roomsvr ��ʾroomsvr���ƣ���Щ��Ϸ�����õ��Զ����roomsvr�������Ĵ��齫roomsvrxzmk��Ĭ��roomsvr
		
	ע��㣺
		1.��������Ļ�Ծ�����������ᱻ���ã����������Զ���ȡ
*/

typedef struct _tagPlayerInfo
{
	INT64 nWinDeposit;		//����Ӯȡ��
	int nBout;				//����
	INT64 nFee;				//��ˮ��
	int nDeposit;			//ʣ������
	int nUserType;			//�������(�ֻ���pc)
	_tagPlayerInfo()
	{
		nWinDeposit = 0;
		nBout = 0;
		nFee = 0;
		nDeposit = 0;
		nUserType = 0;
	}
}PlayerInfo, *PPlayerInfo;

typedef struct _tagRoomInfo
{
	INT64 nFee;
	DWORD nBout;
	std::set<int> userIds;
	_tagRoomInfo()
	{
		nFee = 0;
		nBout = 0; 
		userIds.clear();
	}
	const _tagRoomInfo& operator+=(const _tagRoomInfo& roominfo) 
	{
		this->nFee += roominfo.nFee;
		this->nBout += roominfo.nBout;
		
		
		for (std::set<int>::const_iterator iter = roominfo.userIds.begin(); iter != roominfo.userIds.end(); ++iter)
		{
			this->userIds.insert(*iter);
		}
		//this->userIds.insert(roominfo.userIds.begin(), roominfo.userIds.end());
		return *this;
	}
	
	friend _tagRoomInfo operator+(const _tagRoomInfo& roominfo1, const _tagRoomInfo& roominfo2)
	{
		_tagRoomInfo roomInfo;
		roomInfo.nFee = roominfo1.nFee + roominfo2.nFee;
		roomInfo.nBout = roominfo1.nBout + roominfo2.nBout;
		for (std::set<int>::const_iterator iter = roominfo1.userIds.begin(); iter != roominfo1.userIds.end(); ++iter)
		{
			roomInfo.userIds.insert(*iter);
		}
		for (iter = roominfo2.userIds.begin(); iter != roominfo2.userIds.end(); ++iter)
		{
			roomInfo.userIds.insert(*iter);
		}
//		roomInfo.userIds.insert(roominfo1.userIds.begin(), roominfo1.userIds.end());
// 		roomInfo.userIds.insert(roominfo2.userIds.begin(), roominfo2.userIds.end());
		return roomInfo;
	}
}RoomInfo, *PRoomInfo;

typedef std::map<int, PlayerInfo> PlayerInfoMap;
typedef std::map<int, RoomInfo> RoomInfoMap;

class DataRecord
{
public:
	static DataRecord* Instance();
	static void Purge();
	DataRecord();
	virtual ~DataRecord();

	virtual void init(TCHAR szIniFile[], CGameServer* gameSvr);
	virtual void UpdateInfos(CTable* pTable, LPREFRESH_RESULT_EX lpRefreshResult, LPGAME_RESULT_EX lpGameResult);
	virtual void UpdateRoomBout(int roomid, int tableno);
	virtual void ReadInfos();
	virtual void SaveInfos(BOOL force = FALSE);
	virtual void ResetInfos();
protected:
	CString GetRecordFile(); 
	void FormatStr(CString& str, int nFomatLen);				//��str�ַ����ÿո���䵽nFomatLen����
	CString FormatOneRowStr(CString strs[], int len, int nFomatLen);	//����strs������һ��д������
	int SplitStr(CString outStrs[], int len, const CString& str, TCHAR splitChar = ' ', bool removeNewLine = true);	//��splitChar�ָ��ַ���	
	
	int GetRoomKeyByNo(int roomid, int tableno);
	int GetRoomKeyByIndex(int roomid, int tableIndex);
	void ParseRoomKey(int roomKey, int& roomid, int& tableIndex);
	int GetTableIndex(int roomid, int tableno);		//��������ֲ���index��������������ֲ��Ļ��򷵻�0����index��1��ʼ

	//��������
	virtual BOOL IsTableDepositRoom(int roomid);
	BOOL GetRoomSvrIniFile(int roomid, CString& sRoomSvrIniFile);
protected:
	PlayerInfoMap m_playerInfos;
	RoomInfoMap m_roomInfos;
	CString m_szIniFile;

	CCritSec m_csInfo;
	CString m_strRecordFile;
	DWORD m_dwTimeLastSave;

	CGameServer* m_gameSvr;
private:
	static DataRecord* s_Instance;
};



