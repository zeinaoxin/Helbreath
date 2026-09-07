// GuildsMan.cpp: implementation of the CGuildsMan class.
//
//////////////////////////////////////////////////////////////////////

#include "GuildsMan.h"

extern void PutLogList(char* cMsg);
extern char G_cTxt[512];
extern char	G_cData50000[50000];
extern void PutLogFileList(char* cStr);
extern void PutAdminLogFileList(char* cStr);
extern void PutItemLogFileList(char* cStr);
extern void PutLogEventFileList(char* cStr);
extern void PutHackLogFileList(char* cStr);
extern void PutPvPLogFileList(char* cStr);
extern FILE* pLogFile;
extern HWND	G_hWnd;

#pragma warning (disable : 4996 4018)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuildsMan::CGuildsMan()
{
	ZeroMemory(m_cName, sizeof(m_cName));
}
												 
CGuildsMan::~CGuildsMan()
{

}

// New 07/05/2004
void CGame::RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex)
{
	if (m_pClientList[iClientH] == 0) return;
	if ((iObjectID <= 0) || (iObjectID >= DEF_MAXCLIENTS)) return;

	if (m_pClientList[iObjectID] != 0) {
		// 驴盲脙禄 鹿脼脌潞 Object掳隆 戮酶麓脵.

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_REQGUILDNAMEANSWER, m_pClientList[iObjectID]->m_iGuildRank, iIndex, 0, m_pClientList[iObjectID]->m_cGuildName);

	}
}

int CGame::ObtenerX(char* pGuild)
{
	for (int i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stGuildInfo[i].cGuildName) == "NONE") continue;
		if (string(m_stGuildInfo[i].cGuildName) == pGuild)
		{
			return m_stGuildInfo[i].markX;
		}
	}
	return -1;
}
int CGame::ObtenerY(char* pGuild)
{
	for (int i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stGuildInfo[i].cGuildName) == "NONE") continue;
		if (string(m_stGuildInfo[i].cGuildName) == pGuild)
		{
			return m_stGuildInfo[i].markY;
		}
	}
	return -1;
}
int CGame::ObtenerID(char* pGuild)
{
	for (int i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stGuildInfo[i].cGuildName) == "NONE") continue;
		if (string(m_stGuildInfo[i].cGuildName) == pGuild)
		{
			return i;
		}
	}
	return -1;
}
int CGame::ObtenerNuevoID()
{
	for (int i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stGuild[i].cGuildName) == "NONE")
		{
			return i;
		}
	}
	return -1;
}

void CGame::RequestGuildMemberRank(int iClientH, char *pName, int iIndex)
{
	if (m_pClientList[iClientH] == 0) return;
	
	for (int i = 0; i < DEF_MAXCLIENTS; i++) {
		if (m_pClientList[i] != 0 && strcmp(m_pClientList[i]->m_cCharName, pName) == 0) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_REQGUILDRANKANSWER, m_pClientList[i]->m_iGuildRank, iIndex, 0, m_pClientList[i]->m_cGuildName);
			break;
		}
	}
}

void CGame::PlayerCommandAddRank(int iClientH, char* pData, DWORD dwMsgSize, int iRank)
{
    char   seps[] = "= \t\n";
    char* token, cName[11], cTargetName[11], cBuff[256], cNpcName[21], cNpcWaypoint[11], cMsg[52];
    class  CStrTok* pStrTok;
    int i, iMajesticCount;

	DWORD dwGoldCount = dwGetItemCount(iClientH, "Gold");

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iGuildRank != 0 && m_pClientList[iClientH]->m_iGuildRank != 3) {
        // ???? ??????? ?????* ?? ?????? ??????? ????.
        SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOGUILDMASTERLEVEL, 0, 0, 0, 0);
        return;
    }

	ZeroMemory(cMsg, sizeof(cMsg));
	switch (iRank)
	{
	case 1:
		wsprintf(cMsg, "You are Now a Recluiter");
		iMajesticCount = 60;
		break;
	case 2:
		wsprintf(cMsg, "You are Now a Summoner");
		iMajesticCount = 50;
		break;
	case 3:
		wsprintf(cMsg, "You are Now a Captain");
		iMajesticCount = 80;
		break;
	}

	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < iMajesticCount)
	{
		wsprintf(G_cTxt, "Not enough majestic. Required %d points.", iMajesticCount);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, G_cTxt);
		return;
	}

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new class CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0) {
        // token捞 梆 盲泼阑 阂啊瓷窍霸 父甸 荤侩磊 捞抚
        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

		if (dwGoldCount >= 1000000) {
			for (i = 0; i < DEF_MAXCLIENTS; i++) {
				if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
					// 格钎 某腐磐甫 茫疽促.     

					if (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[i]->m_cGuildName, 21) != 0) {
						// ????? ???????? ???? ?????? ????????.

						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Error: Cannot Add Rank...");
						delete pStrTok;
						return;
					}

					if (m_pClientList[i]->m_iGuildRank == 0)
					{
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Error: Cannot change a Guildmaster.");
						delete pStrTok;
						return;
					}
					SendNotifyMsg(0, i, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cMsg);
					m_pClientList[i]->m_iGuildRank = iRank;
					SendNotifyMsg(0, i, DEF_UPDATE_GUILDRANK, m_pClientList[i]->m_iGuildRank, 0, 0, 0);
				}
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= iMajesticCount;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

			SetItemCount(iClientH, "Gold", dwGoldCount - 1000000);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "New Rank Added");
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Not enough gold. Required 1kk of Gold.");
		}
    }

    delete pStrTok;
    
}

void CGame::PlayerOrder_DeleteRank(int iClientH, char* pData, DWORD dwMsgSize)
{
    char   seps[] = "= \t\n";
    char* token, cName[11], cTargetName[11], cBuff[256], cNpcName[21], cNpcWaypoint[11];
    class  CStrTok* pStrTok;
    int i;

    if (m_pClientList[iClientH] == 0) return;
    if ((dwMsgSize) <= 0) return;

    if (m_pClientList[iClientH]->m_iGuildRank != 0 && m_pClientList[iClientH]->m_iGuildRank != 3) {
        // ???? ??????? ?????* ?? ?????? ??????? ????.
        SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOGUILDMASTERLEVEL, 0, 0, 0, 0);
        return;
    }

    ZeroMemory(cName, sizeof(cName));
    ZeroMemory(cNpcWaypoint, sizeof(cNpcWaypoint));
    ZeroMemory(cTargetName, sizeof(cTargetName));
    ZeroMemory(cNpcName, sizeof(cNpcName));
    ZeroMemory(cBuff, sizeof(cBuff));
    memcpy(cBuff, pData, dwMsgSize);

    pStrTok = new class CStrTok(cBuff, seps);
    token = pStrTok->pGet();
    token = pStrTok->pGet();

    if (token != 0) {
        // token?€??梆 ?儰???€??挵〈??囅??父甸 荤侩?€???€?尭?        if (strlen(token) > 10)
            memcpy(cTargetName, token, 10);
        else memcpy(cTargetName, token, strlen(token));

		for (i = 0; i < DEF_MAXCLIENTS; i++) {
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
				// 格?嚗 ?劤腐?呁甫 ?儯?挻??     

				if (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[i]->m_cGuildName, 21) != 0) {
					// ????? ???????? ???? ?????? ????????.

					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Error: Cannot Delete rank...");
					delete pStrTok;
					return;
				}
				if (m_pClientList[i]->m_iGuildRank == 0)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Error: Cannot delete a Guildmaster.");
					delete pStrTok;
					return;
				}
				m_pClientList[i]->m_iGuildRank = 12;
				SendNotifyMsg(0, i, DEF_UPDATE_GUILDRANK, m_pClientList[i]->m_iGuildRank, 0, 0, 0);
				SendNotifyMsg(0, i, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "You are now a Guildsman");

			}
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Rank Deleted");
    }

    delete pStrTok;
    
}

void CGame::ResponseCreateNewGuildHandler(char* pData, DWORD dwMsgSize)
{
	int i;
	WORD* wp, wResult;
	DWORD* dwp;
	char* cp, cCharName[11], cData[100], cTxt[120];
	int iRet;

	// 路脦卤脳 录颅鹿枚路脦潞脦脜脥 卤忙碌氓 禄媒录潞 驴盲脙禄驴隆 麓毛脟脩 脌脌麓盲碌楼脌脤脜脥掳隆 碌碌脗酶脟脽麓脵. 
	ZeroMemory(cCharName, sizeof(cCharName));
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cCharName, cp, 10);
	cp += 10;

	// 脌脤赂搂脌脤 脌脧脛隆脟脧麓脗 脜卢露贸脌脤戮冒脝庐赂娄 脙拢麓脗麓脵.
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0) //&&
			//(m_pClientList[i]->m_iLevel >= 100) && (m_pClientList[i]->m_iCharisma >= 20) &&
			// centu - guild cost gold
			/*(dwGetItemCount(i, "Gold") >= m_iGuildCost)*/) {

			wp = (WORD*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wp) {
			case DEF_LOGRESMSGTYPE_CONFIRM:
				// 脜卢露贸脌脤戮冒脝庐脌脟 卤忙碌氓 禄媒录潞 驴盲卤赂掳隆 录潞掳酶脟脧驴麓麓脵. 
				wResult = DEF_MSGTYPE_CONFIRM;
				// 卤忙碌氓 脌脤赂搂脌潞 脌脤鹿脤 脌煤脌氓碌脟戮卯 脌脰脌赂鹿脟路脦 路漏脜漏赂娄 0脌赂路脦 鹿脵虏茫 脌炉脠驴脠颅 脟脩麓脵.
				m_pClientList[i]->m_iGuildRank = 0;	// 卤忙碌氓 路漏脜漏麓脗 0. 卤忙碌氓赂露陆潞脜脥脌脫 
				wsprintf(cTxt, "(!) New guild(%s) creation success! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
				PutLogList(cTxt);
				break;

			case DEF_LOGRESMSGTYPE_REJECT:
				// 脜卢露贸脌脤戮冒脝庐脌脟 卤忙碌氓 禄媒录潞 驴盲卤赂掳隆 陆脟脝脨脟脧驴麓麓脵.
				// 脟脴麓莽 脜卢露贸脌脤戮冒脝庐脌脟 卤忙碌氓脌脤赂搂脌禄 脙脢卤芒脠颅脟脩麓脵 "NONE".
				wResult = DEF_MSGTYPE_REJECT;
				ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
				memcpy(m_pClientList[i]->m_cGuildName, "NONE", 4);
				m_pClientList[i]->m_iGuildRank = -1;
				m_pClientList[i]->m_iGuildGUID = -1;
				wsprintf(cTxt, "(!) New guild(%s) creation Fail! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
				PutLogList(cTxt);
				break;
			}

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = wResult;

			// 卤忙碌氓 禄媒录潞 驴盲卤赂 脌脌麓盲 赂脼陆脙脕枚赂娄 脜卢露贸脌脤戮冒脝庐驴隆掳脭 脌眉录脹
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
				DeleteClient(i, true, true);
				break;
			}

			return;
		}

	// 脌脤赂搂脌脤 脌脧脛隆脟脧麓脗 脜卢露贸脌脤戮冒脝庐赂娄 脙拢脌禄 录枚 戮酶麓脵.
	wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
	PutLogList(cTxt);
}

void CGame::RequestCreateNewGuildHandler(int iClientH, char* pData, DWORD dwMsgSize)
{
	char* cp, cGuildName[21], cTxt[120], cData[100];
	DWORD* dwp;
	WORD* wp;
	int     iRet;
	SYSTEMTIME SysTime;
	
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_bIsCrusadeMode == true) return;
	if (m_bIsHeldenianMode == true) return;
	if (m_bIsApocalypseMode == true) return;

	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cp += 30;

	ZeroMemory(cGuildName, sizeof(cGuildName));
	memcpy(cGuildName, cp, 20);
	cp += 20;

	DWORD dwGoldCount = dwGetItemCount(iClientH, "Gold");

	if (m_pClientList[iClientH]->m_iGuildRank != -1) 
	{
		wsprintf(cTxt, "(!)Cannot create guild! Already guild member.: CharName(%s)", m_pClientList[iClientH]->m_cCharName);
		PutLogList(cTxt);
	}
	else 
	{
		if ((m_pClientList[iClientH]->m_iLevel < 100) || //(m_pClientList[iClientH]->m_iCharisma < 20) ||
			(memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) ||
			(memcmp(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName, 10) != 0))
		{ // v1.4
		   ZeroMemory(cData, sizeof(cData));

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_RESPONSE_CREATENEWGUILD;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) 
			{
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, true, true);
				break;
			}
		}
		else 
		{
			if (dwGoldCount >= m_iGuildCost) 
			{
				ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
				strcpy(m_pClientList[iClientH]->m_cGuildName, cGuildName);
				ZeroMemory(m_pClientList[iClientH]->m_cLocation, sizeof(m_pClientList[iClientH]->m_cLocation));
				strcpy(m_pClientList[iClientH]->m_cLocation, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cLocationName);
				
				SetItemCount(iClientH, "Gold", dwGoldCount - m_iGuildCost); // reduce gold by guildcost

				GetLocalTime(&SysTime);
				m_pClientList[iClientH]->m_iGuildGUID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

				bSendMsgToLS(MSGID_REQUEST_CREATENEWGUILD, iClientH);

				//_CreateNewGuildFile(cGuildName);
			}
			else 
			{
				wsprintf(cTxt, " Not enough gold. You need %d gold.", m_iGuildCost);
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTxt);

				ZeroMemory(cData, sizeof(cData));

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_RESPONSE_CREATENEWGUILD;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, true, true);
					break;
				}
			}
		}
	}
}

void CGame::_CreateNewGuildFile(char *cGuildName)
{
	char cTxt[256], cFileName[256], cDir[11], * pData, * cp;
	FILE* pFile;
	int    iSize, id = ObtenerNuevoID();

	m_stGuild[id].iGuildLevel = 0;
	strcpy(m_stGuild[id].cGuildName, cGuildName);

	pData = new char[30000];
	if (pData == 0) return;
	ZeroMemory(pData, 30000);

	cp = (char*)(pData);
	iSize = _iComposeGuildDataFileContents(id, cp);

	// Centuu - guild upgrade
	strcat(cFileName, "Guild");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	wsprintf(cTxt, "AscII%d", (unsigned char)cGuildName[0]);
	strcat(cFileName, cTxt);
	strcpy(cDir, cFileName);
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, cGuildName);
	strcat(cFileName, ".txt");

	_mkdir("Guild");
	_mkdir(cDir);

	if (iSize == 0) {
		PutLogList("(!) Guild data body empty: Cannot create & save temporal guild data file.");
		delete[] pData;
		return;
	}

	pFile = fopen(cFileName, "wt");
	if (pFile == 0) {
		wsprintf(cTxt, "(!) Cannot create new Guild file : Name(%s)", cFileName);
		PutLogList(cTxt);
		return;
	}
	
	wsprintf(cTxt, "(!) Guild data file saved : Name(%s)", cFileName);
	PutLogList(cTxt);
	fwrite(cp, iSize, 1, pFile);
	fclose(pFile);
	
}

int CGame::_iComposeGuildDataFileContents(int iGuildH, char* pData)
{
	SYSTEMTIME SysTime;
	char  cTxt[120], cTmp[21];
	int   i;

	GetLocalTime(&SysTime);
	strcat(pData, "[FILE-DATE]\n\n");

	wsprintf(cTxt, "file-saved-date: %d %d %d %d %d\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(pData, cTxt);
	strcat(pData, "\n\n");

	strcat(pData, "[LEVEL]\n\n");
	wsprintf(cTxt, "guild-level       = %d", m_stGuild[iGuildH].iGuildLevel);
	strcat(pData, cTxt);
	strcat(pData, "\n\n");
	
	strcat(pData, "[ITEMLIST]\n\n");

	for (i = 0; i < DEF_MAXBANKITEMS; i++) {
		if (m_stGuild[iGuildH].m_pItemInBankList[i] != 0) {
			strcat(pData, "guild-bank-item = ");
			memset(cTmp, ' ', 21);
			strcpy(cTmp, m_stGuild[iGuildH].m_pItemInBankList[i]->m_cName);
			cTmp[strlen(m_stGuild[iGuildH].m_pItemInBankList[i]->m_cName)] = (char)' ';
			cTmp[20] = 0;
			strcat(pData, cTmp);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_dwCount, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sTouchEffectType, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sTouchEffectValue1, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sTouchEffectValue2, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sTouchEffectValue3, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_cItemColor, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sItemSpecEffectValue1, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sItemSpecEffectValue2, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sItemSpecEffectValue3, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_wCurLifeSpan, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_dwAttribute, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			//----------------------------------------------------------------------------
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sNewEffect1, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			//--------------------
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sNewEffect2, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			//--------------------
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sNewEffect3, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, " ");
			//--------------------
			itoa(m_stGuild[iGuildH].m_pItemInBankList[i]->m_sNewEffect4, cTxt, 10);
			strcat(pData, cTxt);
			strcat(pData, "\n");
		}
	}
	strcat(pData, "\n\n");

	strcat(pData, "[EOF]");
	strcat(pData, "\n\n\n\n");

	return strlen(pData);
}

void CGame::RequestDisbandGuildHandler(int iClientH, char* pData, DWORD dwMsgSize)
{
	char* cp, cGuildName[21], cTxt[120];

	if (m_bIsCrusadeMode == true) return;
	if (m_bIsHeldenianMode == true) return;
	if (m_bIsApocalypseMode == true) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp += 30;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	if ((m_pClientList[iClientH]->m_iGuildRank != 0) || (memcmp(m_pClientList[iClientH]->m_cGuildName, cGuildName, 20) != 0)) {
		// 卤忙碌氓赂露陆潞脜脥掳隆 戮脝麓脧掳脜鲁陋 卤忙碌氓脌脟 脌脤赂搂脌脤 麓脵赂拢鹿脟路脦 卤忙碌氓脟脴禄锚脌脟 卤脟脟脩脌脤 戮酶麓脵.
		wsprintf(cTxt, "(!)Cannot Disband guild! Not guildmaster.: CharName(%s)", m_pClientList[iClientH]->m_cCharName);
		PutLogList(cTxt);
	}
	else {
		// 卤忙碌氓 脟脴禄锚 赂脼陆脙脕枚赂娄 路脦卤脳录颅鹿枚路脦 脌眉录脹脟脩麓脵.
		bSendMsgToLS(MSGID_REQUEST_DISBANDGUILD, iClientH);
	}
}

void CGame::ResponseDisbandGuildHandler(char* pData, DWORD dwMsgSize)
{
	int i;
	WORD* wp, wResult;
	DWORD* dwp;
	char* cp, cCharName[11], cData[100], cTxt[120];
	int iRet;

	// 肺弊 辑滚肺何磐 辨靛 秦魂 夸没俊 措茄 览翠单捞磐啊 档馒沁促. 
	ZeroMemory(cCharName, sizeof(cCharName));
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	memcpy(cCharName, cp, 10);
	cp += 10;

	// 捞抚捞 老摹窍绰 努扼捞攫飘甫 茫绰促.
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cCharName, 10) == 0)) {

			wp = (WORD*)(pData + DEF_INDEX2_MSGTYPE);
			switch (*wp) {
			case DEF_LOGRESMSGTYPE_CONFIRM:
				// 努扼捞攫飘狼 辨靛 秦魂 夸备啊 己傍窍看促. 
				wResult = DEF_MSGTYPE_CONFIRM;
				wsprintf(cTxt, "(!) Disband guild(%s) success! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
				PutLogList(cTxt);

				// 立加吝牢 辨靛盔甸俊霸 辨靛啊 秦魂登菌澜阑 舅府绰 皋矫瘤甫 傈价茄促. 
				SendGuildMsg(i, DEF_NOTIFY_GUILDDISBANDED, 0, 0, 0);

				// 辨靛捞抚 努府绢
				ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
				memcpy(m_pClientList[i]->m_cGuildName, "NONE", 4);
				m_pClientList[i]->m_iGuildRank = -1;		// 辨靛 珐农绰 -1. 辨靛盔捞 酒聪促. 
				m_pClientList[i]->m_iGuildGUID = -1;
				break;

			case DEF_LOGRESMSGTYPE_REJECT:
				// 努扼捞攫飘狼 辨靛 秦魂 夸备啊 角菩窍看促.
				wResult = DEF_MSGTYPE_REJECT;
				wsprintf(cTxt, "(!) Disband guild(%s) Fail! : character(%s)", m_pClientList[i]->m_cGuildName, m_pClientList[i]->m_cCharName);
				PutLogList(cTxt);
				break;
			}

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_RESPONSE_DISBANDGUILD;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = wResult;

			// 辨靛 秦魂 夸备 览翠 皋矫瘤甫 努扼捞攫飘俊霸 傈价
			iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 皋矫瘤甫 焊尘锭 俊矾啊 惯积沁促搁 力芭茄促.
				DeleteClient(i, true, true);
				break;
			}
			return;
		}

	// 捞抚捞 老摹窍绰 努扼捞攫飘甫 茫阑 荐 绝促.
	wsprintf(cTxt, "(!)Non-existing player data received from Log server(2): CharName(%s)", cCharName);
	PutLogList(cTxt);
}

void CGame::JoinGuildApproveHandler(int iClientH, char* pName)
{
	int i;
	bool bIsExist = false;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	// pName脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐脌脟 iClientH 卤忙碌氓驴隆 麓毛脟脩 掳隆脌脭驴盲卤赂掳隆 录潞掳酶脟脧驴麓麓脵.

	// pName脌脟 脌脤赂搂脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐 卤赂脕露脙录赂娄 掳脣禄枚脟脩麓脵.
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0)) {
			// v1.4 录脪录脫 赂露脌禄脌脤 麓脼露贸碌碌 鹿芦陆脙碌脠麓脵.
			if (memcmp(m_pClientList[i]->m_cLocation, m_pClientList[iClientH]->m_cLocation, 10) != 0) return;

			// 卤忙碌氓脌脟 脌脤赂搂脌禄 潞鹿禄莽脟脧掳铆 录枚脛隆赂娄 脙脢卤芒脠颅脟脴 脕脴麓脵.
			ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
			strcpy(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName);

			// 卤忙碌氓 GUID潞鹿禄莽脟脩麓脵.
			m_pClientList[i]->m_iGuildGUID = m_pClientList[iClientH]->m_iGuildGUID;

			// 卤忙碌氓脌脟 禄媒录潞脌搂脛隆赂娄 脙脢卤芒脠颅脟脩麓脵.
			ZeroMemory(m_pClientList[i]->m_cLocation, sizeof(m_pClientList[i]->m_cLocation));
			strcpy(m_pClientList[i]->m_cLocation, m_pClientList[iClientH]->m_cLocation);

			m_pClientList[i]->m_iGuildRank = DEF_GUILDSTARTRANK; //@@@  GuildRank脌脟 陆脙脌脹脌潞 DEF_GUILDSTARTRANK

			// 掳隆脌脭 陆脜脙禄脌脷驴隆掳脭 掳隆脌脭脌脤 录潞掳酶脟脽脌陆脌禄 戮脣赂庐麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕脴麓脵.
			SendNotifyMsg(0, i, DEF_COMMONTYPE_JOINGUILDAPPROVE, 0, 0, 0, 0);

			// 脝炉录潞脌脤 鹿脵虏卯鹿脟路脦 驴脺戮莽脌禄 禄玫路脦 潞赂鲁陆麓脵. 
			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0); // Centuu - iClientH -> i

			// 麓脵赂楼 卤忙碌氓驴酶碌茅驴隆掳脭 禄玫 卤忙碌氓驴酶脌脤 脌脰脌陆脌禄 戮脣赂掳麓脵.
			SendGuildMsg(i, DEF_NOTIFY_NEWGUILDSMAN, 0, 0, 0);

			// 卤忙碌氓脕陇潞赂脠颅脌脧驴隆 禄玫 卤忙碌氓驴酶脌脟 脌脤赂搂脌禄 卤芒路脧脟脩麓脵.
			bSendMsgToLS(MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN, i);
			break;
		}

	// 掳隆脌脭脌禄 陆脜脙禄脟脩 脜卢露贸脌脤戮冒脝庐赂娄 脙拢脌禄录枚 戮酶麓脵.(脕垄录脫脌脤 卤脳禄莽脌脤 虏梅掳氓麓脵麓酶脕枚) 鹿芦脠驴脌脫 
}

void CGame::JoinGuildRejectHandler(int iClientH, char* pName)
{
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	// pName脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐脌脟 iClientH 卤忙碌氓驴隆 麓毛脟脩 掳隆脌脭 驴盲卤赂掳隆 陆脟脝脨 脟脧驴麓麓脵.

	// pName脌脟 脌脤赂搂脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐 卤赂脕露脙录赂娄 掳脣禄枚脟脩麓脵.
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0)) {

			// 掳隆脌脭 陆脜脙禄脌脷驴隆掳脭 掳隆脌脭脌脤 陆脟脝脨脟脽脌陆脌禄 戮脣赂庐麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕脴麓脵.
			SendNotifyMsg(0, i, DEF_COMMONTYPE_JOINGUILDREJECT, 0, 0, 0, 0);
			break;
		}

	// 掳隆脌脭脌禄 陆脜脙禄脟脩 脜卢露贸脌脤戮冒脝庐赂娄 脙拢脌禄录枚 戮酶麓脵.(脕垄录脫脌脤 卤脳禄莽脌脤 虏梅掳氓麓脵麓酶脕枚) 鹿芦脠驴脌脫 
}

void CGame::DismissGuildApproveHandler(int iClientH, char* pName)
{
	int i;


	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0)) {

			bSendMsgToLS(MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN, i);
			SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, 0, 0, 0);

			ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
			strcpy(m_pClientList[i]->m_cGuildName, "NONE");
			m_pClientList[i]->m_iGuildRank = -1;
			m_pClientList[i]->m_iGuildGUID = -1;

			SendNotifyMsg(0, i, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, 0, 0, 0, 0);

			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
			break;
		}

}

void CGame::DismissGuildRejectHandler(int iClientH, char* pName)
{
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	// pName脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐脌脟 iClientH 卤忙碌氓驴隆 麓毛脟脩 脜禄脜冒 驴盲卤赂掳隆 陆脟脝脨 脟脧驴麓麓脵.

	// pName脌脟 脌脤赂搂脌禄 掳庐麓脗 脜卢露贸脌脤戮冒脝庐 卤赂脕露脙录赂娄 掳脣禄枚脟脩麓脵.
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pName, 10) == 0)) {

			// 掳隆脌脭 陆脜脙禄脌脷驴隆掳脭 脜禄脜冒掳隆 陆脟脝脨脟脽脌陆脌禄 戮脣赂庐麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕脴麓脵.
			SendNotifyMsg(iClientH, i, DEF_COMMONTYPE_DISMISSGUILDREJECT, 0, 0, 0, 0);
			break;
		}

	// 脜禄脜冒赂娄 陆脜脙禄脟脩 脜卢露贸脌脤戮冒脝庐赂娄 脙拢脌禄录枚 戮酶麓脵.(脕垄录脫脌脤 卤脳禄莽脌脤 虏梅掳氓麓脵麓酶脕枚) 鹿芦脠驴脌脫 
}

void CGame::SendGuildMsg(int iClientH, WORD wNotifyMsgType, short sV1, short sV2, char* pString)
{
	char cData[500];
	DWORD* dwp;
	WORD* wp;
	char* cp;
	int i, iRet;

	// 掳掳脌潞 卤忙碌氓驴酶碌茅驴隆掳脭赂赂 潞赂鲁禄麓脗 赂脼陆脙脕枚碌茅
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) &&
			(memcmp(m_pClientList[i]->m_cGuildName, m_pClientList[iClientH]->m_cGuildName, 20) == 0)) {

			// ### BUG POINT 脌搂脛隆掳隆 脌脽赂酶碌脟戮卯 脝梅脌脦脜脥 驴卢禄锚脌脤 脌脽赂酶碌脟戮煤麓脵. 
			ZeroMemory(cData, sizeof(cData));

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = wNotifyMsgType;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			// 掳掳脌潞 卤忙碌氓 脌脤赂搂脌禄 掳庐掳铆 脌脰麓脗 脜卢露贸脌脤戮冒脝庐赂娄 脙拢戮脪麓脵.
			switch (wNotifyMsgType) {
			case DEF_NOTIFY_GUILDDISBANDED:
				if (i == iClientH) break; // <-- 卤忙碌氓 赂露陆潞脜脥 脌脷陆脜驴隆掳脭麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕枚 戮脢麓脗麓脵.
				// 卤忙碌氓 脟脴禄锚 脜毛潞赂驴隆 脟脴麓莽 脜卢露贸脌脤戮冒脝庐脌脟 卤忙碌氓 路漏脜漏赂娄 脜卢赂庐戮卯脟脩麓脵. 
				memcpy(cp, m_pClientList[iClientH]->m_cGuildName, 20);
				cp += 20;

				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 26);
				// 脟脴麓莽 脜卢露贸脌脤戮冒脝庐脌脟 卤忙碌氓鲁禄驴毛脌禄 脜卢赂庐戮卯脟脩麓脵. @@@@@@@
				ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
				strcpy(m_pClientList[i]->m_cGuildName, "NONE");
				m_pClientList[i]->m_iGuildRank = -1;
				m_pClientList[i]->m_iGuildGUID = -1;
				break;

			case DEF_NOTIFY_EVENTMSGSTRING:
				// 卤忙碌氓驴酶碌茅驴隆掳脭 脌眉麓脼碌脟麓脗 脌脤潞楼脝庐 赂脼陆脙脕枚 陆潞脝庐赂碌 
				strcpy(cp, pString);
				cp += strlen(pString);

				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6 + strlen(pString) + 1);
				break;

			case DEF_NOTIFY_NEWGUILDSMAN:
			case DEF_NOTIFY_DISMISSGUILDSMAN:
				memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
				cp += 10;

				iRet = m_pClientList[i]->m_pXSock->iSendMsg(cData, 6 + 10 + 1);
				break;
			}

			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
				DeleteClient(i, true, true);
				break;
			}
		}

}

void CGame::GuildNotifyHandler(char* pData, DWORD dwMsgSize)
{
	// 麓脵赂楼 掳脭脌脫录颅鹿枚路脦潞脦脜脥 卤忙碌氓 脌脤潞楼脝庐掳隆 碌碌脗酶脟脽麓脵. 
	char* cp, cCharName[11], cGuildName[21];

	ZeroMemory(cCharName, sizeof(cCharName));
	ZeroMemory(cGuildName, sizeof(cGuildName));

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);

	memcpy(cCharName, cp, 10);
	cp += 10;

	memcpy(cGuildName, cp, 20);
	cp += 20;

	// 戮脝脕梅 卤赂脟枚碌脟脕枚 戮脢戮脪麓脵. 
}

// v1.4311-3 脙脽掳隆鹿脳 潞炉掳忙 脟脭录枚  卤忙碌氓驴酶 掳颅脜冒 赂铆路脡 void CGame::UserCommand_BanGuildsman
void CGame::UserCommand_BanGuildsman(int iClientH, char* pData, DWORD dwMsgSize)
{
	char   seps[] = "= \t\n";
	char* token, cTargetName[11], cBuff[256];
	class  CStrTok* pStrTok;
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((dwMsgSize) <= 0) return;

	if (m_pClientList[iClientH]->m_iGuildRank != 0 && m_pClientList[iClientH]->m_iGuildRank != 3) {
		// 卤忙碌氓 赂露陆潞脜脥掳隆 戮脝麓脧戮卯录颅 脌脤 卤芒麓脡脌禄 禄莽驴毛脟脪录枚 戮酶麓脵.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOGUILDMASTERLEVEL, 0, 0, 0, 0);
		return;
	}

	ZeroMemory(cTargetName, sizeof(cTargetName));
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();
	token = pStrTok->pGet();

	if (token != 0) {
		// token脌脤 卤忙碌氓赂娄 脜禄脜冒陆脙脜鲁 禄莽露梅 
		if (strlen(token) > 10)
			memcpy(cTargetName, token, 10);
		else memcpy(cTargetName, token, strlen(token));

		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0)) {
				// 赂帽脟楼 脛鲁赂炉脜脥赂娄 脙拢戮脪麓脵. 掳颅脕娄路脦 卤忙碌氓赂娄 掳颅脜冒 陆脙脜虏麓脵. 

				if (memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[i]->m_cGuildName, 21) != 0) {
					// 脌脷陆脜脌脟 卤忙碌氓驴酶脌脤 戮脝麓脧露贸 脟茫露么脌脤 潞脪掳隆麓脡脟脧麓脵.

					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTBANGUILDMAN, 0, 0, 0, 0);
					delete pStrTok;
					return;
				}
				if (m_pClientList[i]->m_iGuildRank == 0)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Error: Cannot ban a Guildmaster.");
					delete pStrTok;
					return;
				}
				// 卤忙碌氓 脕陇潞赂 脠颅脌脧驴隆录颅 脜禄脜冒脟脩 卤忙碌氓驴酶脌脟 脌脤赂搂脌禄 禄猫脕娄脟脩麓脵.
				bSendMsgToLS(MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN, i);

				// 麓脵赂楼 卤忙碌氓驴酶碌茅驴隆掳脭 卤忙碌氓驴酶脌脟 脜禄脜冒赂娄 戮脣赂掳麓脵.
				SendGuildMsg(i, DEF_NOTIFY_DISMISSGUILDSMAN, 0, 0, 0);

				// 卤忙碌氓脌脟 脌脤赂搂脌禄 脙脢卤芒脠颅脟脴 脕脴麓脵.
				ZeroMemory(m_pClientList[i]->m_cGuildName, sizeof(m_pClientList[i]->m_cGuildName));
				strcpy(m_pClientList[i]->m_cGuildName, "NONE");
				m_pClientList[i]->m_iGuildRank = -1;
				m_pClientList[i]->m_iGuildGUID = -1;

				// 卤忙碌氓 赂露陆潞脜脥驴隆掳脭 掳颅脕娄 脜禄脜冒驴隆 录潞掳酶脟脽脌陆脌禄 戮脣赂庐麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕脴麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SUCCESSBANGUILDMAN, 0, 0, 0, 0);

				// 掳颅脕娄 脜禄脜冒碌脠 卤忙碌氓驴酶驴隆掳脭 掳颅脕娄 脜禄脜冒 碌脟戮煤脌陆脌禄 戮脣赂掳麓脵.
				SendNotifyMsg(0, i, DEF_COMMONTYPE_BANGUILD, 0, 0, 0, 0);

				// 脝炉录潞脌脤 鹿脵虏卯鹿脟路脦 驴脺戮莽脌禄 禄玫路脦 潞赂鲁陆麓脵. 
				SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

				delete pStrTok;
				return;
			}
		// 脟枚脌莽 脕垄录脫脕脽脌脤 戮脝麓脧麓脵.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cTargetName);
	}

	delete pStrTok;
}

void CGame::AdminOrder_SummonGuild(int iClientH)
{
	char cTemp[51], cMapName[11], cGuildName[20];
	int    pX, pY, i;
	
	if (m_pClientList[iClientH] == 0) return;
	
	if (m_pClientList[iClientH]->m_iGuildRank != 0 && m_pClientList[iClientH]->m_iGuildRank != 2) return;
	DWORD dwGoldCount = dwGetItemCount(iClientH, "Gold");  // dwGoldCount = player gold
	ZeroMemory(cTemp, sizeof(cTemp));
	if (m_iSummonGuildCost > dwGoldCount)
	{
		wsprintf(cTemp, "Not enough gold. Required %d Gold", m_iSummonGuildCost);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTemp);
		return;
	}
	else // if summonguildcost is less than player gold
	{
		SetItemCount(iClientH, "Gold", dwGoldCount - (DWORD)m_iSummonGuildCost); // reduce gold by summonguildcost   
		wsprintf(cTemp, "You've used %d Gold!", m_iSummonGuildCost);
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTemp);
	}
	
	pX = m_pClientList[iClientH]->m_sX;
	pY = m_pClientList[iClientH]->m_sY;
	
	ZeroMemory(cMapName, sizeof(cMapName));
	memcpy(cMapName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 11);
	
	ZeroMemory(cGuildName, sizeof(cGuildName));
	memcpy(cGuildName, m_pClientList[iClientH]->m_cGuildName, 21);
	
	bool bExiste = false;
	for (i = 0; i < DEF_MAXGUILDS; i++)
	{
		if (string(m_stSummonGuild[i].cGuildName) == "NONE") continue;
		if (string(m_stSummonGuild[i].cGuildName) == cGuildName)
		{
			m_stSummonGuild[i].sX = pX;
			m_stSummonGuild[i].sY = pY;
			strcpy(m_stSummonGuild[i].cMap, cMapName);
			bExiste = true;
			break;
		}
	}
	if (!bExiste)
	{
		for (i = 0; i < DEF_MAXGUILDS; i++)
		{
			if (string(m_stSummonGuild[i].cGuildName) == "NONE")
			{
				strcpy(m_stSummonGuild[i].cGuildName, cGuildName);
				m_stSummonGuild[i].sX = pX;
				m_stSummonGuild[i].sY = pY;
				strcpy(m_stSummonGuild[i].cMap, cMapName);
				break;
			}
		}
	}

	for (i = 0; i < DEF_MAXCLIENTS; i++) 
	{
		if (i == iClientH) continue;
		if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cGuildName, cGuildName) == 0))
		{
			SendNotifyMsg(0, i, DEF_NOTIFY_SUMMONGUILD, 0, 0, 0, 0);
		}
	}
	wsprintf(G_cTxt, "Guild(%s) summoned by PC(%s) to %s(%d,%d).", cGuildName, m_pClientList[iClientH]->m_cCharName, cMapName, pX, pY);
	PutLogList(G_cTxt);
}