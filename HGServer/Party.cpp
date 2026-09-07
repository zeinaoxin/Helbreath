#include "Party.h"

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

#pragma warning (disable : 4996)

Party::Party()
{}

Party::~Party()
{}

void CGame::CreateNewPartyHandler(int iClientH)
{
	int bFlag;

	if (m_pClientList[iClientH] == 0) return;

	bFlag = m_pClientList[iClientH]->bCreateNewParty();
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_RESPONSE_CREATENEWPARTY, bFlag, 0, 0, 0);
}

void CGame::JoinPartyHandler(int iClientH, int iV1, char* pMemberName)
{
	char* cp, cData[120];
	short sAppr2;
	DWORD* dwp;
	WORD* wp;
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty == false) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Party mode is disabled in this map.");
		return;
	}

	switch (iV1) {
	case 0: // 脝脛脝录 脜禄脜冒 陆脜脙禄
		RequestDeletePartyHandler(iClientH);
		break;

	case 1: // 脝脛脝录 掳隆脌脭 陆脜脙禄


		if ((m_pClientList[iClientH]->m_iPartyID != 0) || (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL)) {
			// 脌脤鹿脤 脝脛脝录驴隆 掳隆脌脭脟脴 脌脰麓脵赂茅 脝脛脝录 掳隆脌脭 陆脜脙禄脌禄 脟脪 录枚 戮酶麓脵.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);
			m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
			ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
			m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;

			break;
		}

		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, pMemberName) == 0)) {
				// 掳掳脌潞 脌脤赂搂脌禄 掳隆脕酶 脟脙路鹿脌脤戮卯赂娄 脙拢戮脪麓脵.
				sAppr2 = (short)((m_pClientList[i]->m_sAppr2 & 0xF000) >> 12);
				if (sAppr2 != 0) {
					// 脌眉脜玫 赂冒碌氓 禄贸脜脗脌脟 禄贸麓毛驴隆掳脭麓脗 脝脛脝录 掳隆脌脭 陆脜脙禄脌禄 脟脪 录枚 戮酶麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);

				}
				else if (m_pClientList[i]->m_cSide != m_pClientList[iClientH]->m_cSide) {
					// 脝铆脌脤 麓脵赂拢赂茅 脝脛脝录驴隆 碌茅 录枚 戮酶麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);

				}
				else if (m_pClientList[i]->m_iPartyStatus == DEF_PARTYSTATUS_PROCESSING) {
					// 脝脛脝录 掳隆脌脭脌禄 陆脜脙禄脟脩 麓毛禄贸脌脤 脌脤鹿脤 麓脵赂楼 脝脛脝录 掳隆脌脭 掳眉路脙 脙鲁赂庐赂娄 脟脧掳铆 脌脰麓脵. 陆脜脙禄 潞脪掳隆.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);


					m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
					ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
					m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
				}
				else {
					// 掳隆脌脭 陆脗脌脦 驴漏潞脦赂娄 鹿炉麓脗麓脵.
					m_pClientList[i]->m_iReqJoinPartyClientH = iClientH;
					ZeroMemory(m_pClientList[i]->m_cReqJoinPartyName, sizeof(m_pClientList[i]->m_cReqJoinPartyName));
					strcpy(m_pClientList[i]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName);
					SendNotifyMsg(0, i, DEF_NOTIFY_QUERY_JOINPARTY, 0, 0, 0, m_pClientList[iClientH]->m_cCharName);

					// 陆脜脙禄脟脩 脙酶驴隆麓脗 脝脛脝录 赂露陆潞脜脥掳隆 碌脡 脛鲁赂炉脜脥脌脟 脌脦碌娄陆潞赂娄 鲁脰戮卯脕脺. 脙毛录脪脟脪露搂 脙鲁赂庐脟脧卤芒 脌搂脟脭脌脫.
					m_pClientList[iClientH]->m_iReqJoinPartyClientH = i;
					ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
					strcpy(m_pClientList[iClientH]->m_cReqJoinPartyName, m_pClientList[i]->m_cCharName);
					// 脝脛脝录 禄贸脜脗 录录脝庐
					m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
				}
				break;
			}
		break;

	case 2: // 脝脛脝录 赂芒鹿枚 脠庐脌脦 赂铆路脡 
		if (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM)
		{
			ZeroMemory(cData, sizeof(cData));
			cp = (char*)cData;
			dwp = (DWORD*)cp;
			*dwp = MSGID_PARTYOPERATION;
			cp += 4;
			wp = (WORD*)cp;
			*wp = 6; // 脝脛脝录 赂芒鹿枚 赂庐陆潞脝庐 驴盲脙禄
			cp += 2;
			wp = (WORD*)cp;
			*wp = iClientH;
			cp += 2;
			memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
			cp += 10;
			wp = (WORD*)cp;
			*wp = m_pClientList[iClientH]->m_iPartyID;
			cp += 2;
			SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
		}

		RefreshPartyStatus(iClientH);
		break;
	}
}

// New 06/05/2004
// Party Code
void CGame::RequestCreatePartyHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty == false) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Party mode is disabled in this map.");
		return;
	}

	if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL) {
		// 脝脛脝录 禄贸脜脗掳隆 脌脤鹿脤 脕赂脌莽脟脧赂茅 脝脛脝录赂娄 赂赂碌茅 录枚 戮酶麓脵.
		return;
	}

	m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;

	// Gate Server驴隆 脝脛脝录 禄媒录潞脠脛 PartyID赂娄 戮脣路脕脕脵 掳脥脌禄 驴盲脙禄脟脩麓脵. 
	ZeroMemory(cData, sizeof(cData));
	cp = (char*)cData;

	dwp = (DWORD*)cp;
	*dwp = MSGID_PARTYOPERATION;
	cp += 4;
	wp = (WORD*)cp;
	*wp = 1; // 1, request
	cp += 2;

	wp = (WORD*)cp;
	*wp = iClientH;
	cp += 2;

	memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
	cp += 10;

	SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);


}

// Last Updated October 28, 2004 - 3.51 translation
void CGame::PartyOperationResultHandler(char* pData)
{
	char* cp, cResult, cName[12];
	WORD* wp;
	int i, iClientH, iPartyID, iTotal;

	cp = (char*)(pData + 4);
	wp = (WORD*)cp;
	cp += 2;

	switch (*wp) {
	case 1:
		cResult = *cp;
		cp++;

		wp = (WORD*)cp;
		iClientH = (int)*wp;
		cp += 2;

		ZeroMemory(cName, sizeof(cName));
		memcpy(cName, cp, 10);
		cp += 10;

		wp = (WORD*)cp;
		iPartyID = (int)*wp;
		cp += 2;

		PartyOperationResult_Create(iClientH, cName, cResult, iPartyID);


		break;

	case 2:
		wp = (WORD*)cp;
		iPartyID = *wp;
		cp += 2;

		PartyOperationResult_Delete(iPartyID);


		break;

	case 3:
		wp = (WORD*)cp;
		iClientH = *wp;
		cp += 2;

		ZeroMemory(cName, sizeof(cName));
		memcpy(cName, cp, 10);
		cp += 10;

		// Strange bug here, <0 and above positive integer!!!!
		if ((iClientH < 1) || (iClientH > DEF_MAXCLIENTS)) return;

		if (m_pClientList[iClientH] == 0) return;
		if (strcmp(m_pClientList[iClientH]->m_cCharName, cName) != 0) return;

		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = 0;
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers--;


				break;
			}

		for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
			if ((m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) && (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] != 0)) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1];
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i + 1] = 0;
			}

		m_pClientList[iClientH]->m_iPartyID = 0;
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;



		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 8, 0, 0, 0);
		break;

	case 4:
		cResult = *cp;
		cp++;

		wp = (WORD*)cp;
		iClientH = (int)*wp;
		cp += 2;

		ZeroMemory(cName, sizeof(cName));
		memcpy(cName, cp, 10);
		cp += 10;

		wp = (WORD*)cp;
		iPartyID = (int)*wp;
		cp += 2;

		PartyOperationResult_Join(iClientH, cName, cResult, iPartyID);


		break;

	case 5:
		wp = (WORD*)cp;
		iClientH = (int)*wp;
		cp += 2;

		ZeroMemory(cName, sizeof(cName));
		memcpy(cName, cp, 10);
		cp += 10;

		wp = (WORD*)cp;
		iTotal = (int)*wp;
		cp += 2;

		PartyOperationResult_Info(iClientH, cName, iTotal, cp);


		break;

	case 6:
		cResult = *cp;
		cp++;

		wp = (WORD*)cp;
		iClientH = (int)*wp;
		cp += 2;

		ZeroMemory(cName, sizeof(cName));
		memcpy(cName, cp, 10);
		cp += 10;

		wp = (WORD*)cp;
		iPartyID = (int)*wp;
		cp += 2;

		PartyOperationResult_Dismiss(iClientH, cName, cResult, iPartyID);


		break;
	}
}


void CGame::PartyOperationResult_Create(int iClientH, char* pName, int iResult, int iPartyID)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty == false) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Party mode is disabled in this map.");
		return;
	}

	// Prevent creation of a n?0 party
	if ((iPartyID == 0) && (iResult != 0)) return;

	switch (iResult) {
	case 0: // 脝脛脝录 禄媒录潞 陆脟脝脨 
		if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
		if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

		m_pClientList[iClientH]->m_iPartyID = 0;
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
		m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;

		// SNOOPY added this for safety
		ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 1, 0, 0, 0);
		break;

	case 1: // 脝脛脝录 禄媒录潞 录潞掳酶 
		if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
		if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

		m_pClientList[iClientH]->m_iPartyID = iPartyID;
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_CONFIRM;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 1, 1, 0, 0);

		// 掳脭脌脫 录颅鹿枚脌脟 脝脛脝录 赂庐陆潞脝庐驴隆 碌卯路脧.
		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = iClientH;
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers++;

				break;
			}

		if ((m_pClientList[iClientH]->m_iReqJoinPartyClientH != 0) && (strlen(m_pClientList[iClientH]->m_cReqJoinPartyName) != 0)) {
			ZeroMemory(cData, sizeof(cData));

			cp = (char*)cData;

			dwp = (DWORD*)cp;
			*dwp = MSGID_PARTYOPERATION;
			cp += 4;

			wp = (WORD*)cp;
			*wp = 3;
			cp += 2;

			wp = (WORD*)cp;
			*wp = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
			cp += 2;

			memcpy(cp, m_pClientList[iClientH]->m_cReqJoinPartyName, 10);
			cp += 10;

			wp = (WORD*)cp;
			*wp = m_pClientList[iClientH]->m_iPartyID;
			cp += 2;

			SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
			m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
			ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
		}
		break;
	}
}

// Last Updated October 28, 2004 - 3.51 translation
void CGame::PartyOperationResult_Join(int iClientH, char* pName, int iResult, int iPartyID)
{
	int i;

	if (m_pClientList[iClientH] == 0) return;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty == false) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Party mode is disabled in this map.");
		return;
	}

	// Prevent creation of a n?0 party
	if ((iPartyID == 0) && (iResult != 0)) return;

	switch (iResult) {
	case 0:
		if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
		if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

		m_pClientList[iClientH]->m_iPartyID = 0;
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 4, 0, 0, pName);

		m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
		ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
		break;

	case 1:
		if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) return;
		if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;

		m_pClientList[iClientH]->m_iPartyID = iPartyID;
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_CONFIRM;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 4, 1, 0, pName);

		m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
		ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));

		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == 0) {
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] = iClientH;
				m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers++;


				break;
			}

		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((i != iClientH) && (m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID)) {
				SendNotifyMsg(0, i, DEF_NOTIFY_PARTY, 4, 1, 0, pName);
			}
		break;
	}
}

void CGame::PartyOperationResult_Dismiss(int iClientH, char* pName, int iResult, int iPartyID)
{
	int i;
	// iClientH麓脗 脌脤鹿脤 禄莽露贸脕枚掳铆 戮酶麓脗 脛鲁赂炉脜脥脌脧录枚碌碌 脌脰麓脵.

	// Prevent dismissing th 0 party
	if ((iPartyID == 0) && (iResult != 0)) return;

	switch (iResult) {
	case 0: // 脝脛脝录 脜禄脜冒 陆脟脝脨 ? 脌脤路卤 脌脧脌脤?
		break;

	case 1: // 脝脛脝录 脜禄脜冒 录潞掳酶 
		if (iClientH == 0) {
			// iClientH 掳隆 NULL脌脤赂茅 录颅鹿枚 脌脤碌驴脕脽 脕垄录脫 脕戮路谩碌脟戮卯 掳颅脕娄 脕娄掳脜 驴盲脙禄碌脠 掳脥脌脫.
			for (i = 0; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, pName) == 0)) {
					iClientH = i;
					break;
				}

			// 掳脭脌脫 录颅鹿枚脌脟 脝脛脝录 赂庐陆潞脝庐驴隆录颅 脟脴脕娄.
			for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
				if (m_stPartyInfo[iPartyID].iIndex[i] == iClientH) {
					m_stPartyInfo[iPartyID].iIndex[i] = 0;
					m_stPartyInfo[iPartyID].iTotalMembers--;

					break;
				}

			// 赂庐陆潞脝庐 脌脦碌娄陆潞脌脟 潞贸掳酶掳拢脌禄 脕娄掳脜脟脩麓脵.
			for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
				if ((m_stPartyInfo[iPartyID].iIndex[i] == 0) && (m_stPartyInfo[iPartyID].iIndex[i + 1] != 0)) {
					m_stPartyInfo[iPartyID].iIndex[i] = m_stPartyInfo[iPartyID].iIndex[i + 1];
					m_stPartyInfo[iPartyID].iIndex[i + 1] = 0;
				}

			if (m_pClientList[iClientH] != 0) {
				m_pClientList[iClientH]->m_iPartyID = 0;
				m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
				m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;

				// SNOOPY: Added safety here
				ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
			}

			// 赂冒碌莽 脝脛脝录驴酶碌茅驴隆掳脭 戮脣路脕脕脴麓脵.
			for (i = 0; i < DEF_MAXCLIENTS; i++)
				if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID)) {
					SendNotifyMsg(0, i, DEF_NOTIFY_PARTY, 6, 1, 0, pName);
				}
			break;
		}

		if ((m_pClientList[iClientH] != 0) && (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING)) return;
		if ((m_pClientList[iClientH] != 0) && (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0)) return;

		// 赂冒碌莽 脝脛脝录驴酶碌茅驴隆掳脭 戮脣路脕脕脴麓脵.
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID != 0) && (m_pClientList[i]->m_iPartyID == iPartyID)) {
				SendNotifyMsg(0, i, DEF_NOTIFY_PARTY, 6, 1, 0, pName);
			}

		// 掳脭脌脫 录颅鹿枚脌脟 脝脛脝录 赂庐陆潞脝庐驴隆录颅 脟脴脕娄.
		for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
			if (m_stPartyInfo[iPartyID].iIndex[i] == iClientH) {
				m_stPartyInfo[iPartyID].iIndex[i] = 0;
				m_stPartyInfo[iPartyID].iTotalMembers--;

				break;
			}
		// 赂庐陆潞脝庐 脌脦碌娄陆潞脌脟 潞贸掳酶掳拢脌禄 脕娄掳脜脟脩麓脵.
		for (i = 0; i < DEF_MAXPARTYMEMBERS - 1; i++)
			if ((m_stPartyInfo[iPartyID].iIndex[i] == 0) && (m_stPartyInfo[iPartyID].iIndex[i + 1] != 0)) {
				m_stPartyInfo[iPartyID].iIndex[i] = m_stPartyInfo[iPartyID].iIndex[i + 1];
				m_stPartyInfo[iPartyID].iIndex[i + 1] = 0;
			}

		if (m_pClientList[iClientH] != 0) {
			m_pClientList[iClientH]->m_iPartyID = 0;
			m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
			m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
			// SNOOPY: Added safety here
			ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
		}
		break;
	}
}

void CGame::PartyOperationResult_Delete(int iPartyID)
{
	int i;

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++)
	{
		m_stPartyInfo[iPartyID].iIndex[i] = 0;
		m_stPartyInfo[iPartyID].iTotalMembers = 0;
	}

	// 脝脛脝录 脟脴禄锚碌脢 
	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iPartyID == iPartyID)) {
			SendNotifyMsg(0, i, DEF_NOTIFY_PARTY, 2, 0, 0, 0);
			m_pClientList[i]->m_iPartyID = 0;
			m_pClientList[i]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
			m_pClientList[i]->m_iReqJoinPartyClientH = 0;

			// SNOOPY: Added safety here
			ZeroMemory(m_pClientList[i]->m_cReqJoinPartyName, sizeof(m_pClientList[i]->m_cReqJoinPartyName));

		}
}



void CGame::RequestJoinPartyHandler(int iClientH, char* pData, DWORD dwMsgSize)
{
	char   seps[] = "= \t\n";
	char* cp, * token, cBuff[256], cData[120], cName[12];
	class  CStrTok* pStrTok;
	DWORD* dwp;
	WORD* wp;
	int i;

	// 脝脛脝录 掳隆脌脭 驴漏潞脦赂娄 鹿炉掳铆 脌脰麓脵.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_NULL) return;
	if ((dwMsgSize) <= 0) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapParty == false) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Party mode is disabled in this map.");
		return;
	}

	if (m_pClientList[iClientH]->IsInMap("team")) return;
	ZeroMemory(cBuff, sizeof(cBuff));
	memcpy(cBuff, pData, dwMsgSize);

	pStrTok = new class CStrTok(cBuff, seps);
	token = pStrTok->pGet();

	token = pStrTok->pGet();
	if (token != 0) {
		ZeroMemory(cName, sizeof(cName));
		strcpy(cName, token);
	}
	else return;

	for (i = 0; i < DEF_MAXCLIENTS; i++)
		if ((m_pClientList[i] != 0) && (strcmp(m_pClientList[i]->m_cCharName, cName) == 0)) {
			// 掳脭脌脤脝庐 录颅鹿枚路脦 脝脛脝录 赂芒鹿枚 掳隆脌脭脌禄 陆脜脙禄脟脩麓脵.
			if ((m_pClientList[i]->m_iPartyID == 0) || (m_pClientList[i]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM)) {
				// 脝脛脝录驴隆 掳隆脌脭脟脧掳铆脌脷 脟脩 脛鲁赂炉脜脥掳隆 脝脛脝录 赂露陆潞脜脥掳隆 戮脝麓脧麓脵.
				return;
			}

			ZeroMemory(cData, sizeof(cData));

			cp = (char*)cData;
			dwp = (DWORD*)cp;
			*dwp = MSGID_PARTYOPERATION;
			cp += 4;
			wp = (WORD*)cp;
			*wp = 3; // 脝脛脝录 赂芒鹿枚 脙脽掳隆 驴盲脙禄
			cp += 2;
			wp = (WORD*)cp;
			*wp = iClientH;
			cp += 2;
			memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
			cp += 10;
			wp = (WORD*)cp;
			*wp = m_pClientList[i]->m_iPartyID;
			cp += 2;
			SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
			return;
		}

	// 掳隆脌脭脟脧掳铆脌脷 脟脧麓脗 脝脛脝录 赂露陆潞脜脥掳隆 脟枚脌莽 录颅鹿枚驴隆 脕赂脌莽脟脧脕枚 戮脢麓脗麓脵.
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_PLAYERNOTONGAME, 0, 0, 0, cName);
}



void CGame::RequestDismissPartyHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

	ZeroMemory(cData, sizeof(cData));
	cp = (char*)cData;
	dwp = (DWORD*)cp;
	*dwp = MSGID_PARTYOPERATION;
	cp += 4;
	wp = (WORD*)cp;
	*wp = 4; // 赂芒鹿枚 脕娄掳脜 驴盲脙禄
	cp += 2;
	wp = (WORD*)cp;
	*wp = iClientH;
	cp += 2;
	memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
	cp += 10;
	wp = (WORD*)cp;
	*wp = m_pClientList[iClientH]->m_iPartyID;
	cp += 2;
	SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);

	m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
}


void CGame::GetPartyInfoHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

	ZeroMemory(cData, sizeof(cData));
	cp = (char*)cData;
	dwp = (DWORD*)cp;
	*dwp = MSGID_PARTYOPERATION;
	cp += 4;
	wp = (WORD*)cp;
	*wp = 5; // 脝脛脝录 脕陇潞赂 驴盲脙禄
	cp += 2;
	wp = (WORD*)cp;
	*wp = iClientH;
	cp += 2;
	memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
	cp += 10;
	wp = (WORD*)cp;
	*wp = m_pClientList[iClientH]->m_iPartyID;
	cp += 2;
	SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
}


void CGame::PartyOperationResult_Info(int iClientH, char* pName, int iTotal, char* pNameList)
{
	if (m_pClientList[iClientH] == 0) return;
	if (strcmp(m_pClientList[iClientH]->m_cCharName, pName) != 0) return;
	if (m_pClientList[iClientH]->m_iPartyStatus != DEF_PARTYSTATUS_CONFIRM) return;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_PARTY, 5, 1, iTotal, pNameList);
}

void CGame::RequestDeletePartyHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iPartyID != 0) {
		ZeroMemory(cData, sizeof(cData));
		cp = (char*)cData;
		dwp = (DWORD*)cp;
		*dwp = MSGID_PARTYOPERATION;
		cp += 4;
		wp = (WORD*)cp;
		*wp = 4; // 赂芒鹿枚 脕娄掳脜 驴盲脙禄
		cp += 2;
		wp = (WORD*)cp;
		*wp = iClientH;
		cp += 2;
		memcpy(cp, m_pClientList[iClientH]->m_cCharName, 10);
		cp += 10;
		wp = (WORD*)cp;
		*wp = m_pClientList[iClientH]->m_iPartyID;
		cp += 2;
		SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
		// 禄贸脜脗 潞炉脠炉
		m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_PROCESSING;
	}
}

void CGame::RequestAcceptJoinPartyHandler(int iClientH, int iResult)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;
	int iH;

	if (m_pClientList[iClientH] == 0) return;

	switch (iResult) {
	case 0: // 脝脛脝录 掳隆脌脭 陆脜脙禄 掳脜潞脦碌脟戮煤脌陆脌禄 戮脣路脕脕脴麓脵.
		iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
		if (m_pClientList[iH] == 0) {
			// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
			return;
		}
		if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0) {
			// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
			return;
		}
		if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) {
			// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯脌脟 禄贸脜脗掳隆 潞炉掳忙碌脟戮煤麓脵.
			return;
		}
		if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0)) {
			// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 卤脳露搂 卤脳 脟脙路鹿脌脤戮卯掳隆 戮脝麓脧麓脵.
			return;
		}

		SendNotifyMsg(0, iH, DEF_NOTIFY_PARTY, 7, 0, 0, 0);


		m_pClientList[iH]->m_iPartyID = 0;
		m_pClientList[iH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
		m_pClientList[iH]->m_iReqJoinPartyClientH = 0;
		ZeroMemory(m_pClientList[iH]->m_cReqJoinPartyName, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));

		m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
		ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
		break;

	case 1: // 脝脛脝录 掳隆脌脭 陆脜脙禄 陆脗脌脦
		if ((m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM) && (m_pClientList[iClientH]->m_iPartyID != 0)) {
			iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
			if (m_pClientList[iH] == 0) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
				return;
			}
			if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
				return;
			}
			if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯脌脟 禄贸脜脗掳隆 潞炉掳忙碌脟戮煤麓脵.
				return;
			}
			if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0)) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 卤脳露搂 卤脳 脟脙路鹿脌脤戮卯掳隆 戮脝麓脧麓脵.
				return;
			}

			// 掳隆脌脭 脙鲁赂庐 脟脩麓脵.
			ZeroMemory(cData, sizeof(cData));
			cp = (char*)cData;
			dwp = (DWORD*)cp;
			*dwp = MSGID_PARTYOPERATION;
			cp += 4;
			wp = (WORD*)cp;
			*wp = 3; // 脝脛脝录 赂芒鹿枚 脙脽掳隆 驴盲脙禄
			cp += 2;
			wp = (WORD*)cp;
			*wp = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
			cp += 2;
			memcpy(cp, m_pClientList[iClientH]->m_cReqJoinPartyName, 10);
			cp += 10;
			wp = (WORD*)cp;
			*wp = m_pClientList[iClientH]->m_iPartyID;
			cp += 2;
			SendMsgToGateServer(MSGID_PARTYOPERATION, iClientH, cData);
		}
		else {
			iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;
			if (m_pClientList[iH] == 0) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
				return;
			}
			if (strcmp(m_pClientList[iH]->m_cCharName, m_pClientList[iClientH]->m_cReqJoinPartyName) != 0) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯掳隆 录颅鹿枚禄贸驴隆 戮酶麓脵.
				return;
			}
			if (m_pClientList[iH]->m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 脟脙路鹿脌脤戮卯脌脟 禄贸脜脗掳隆 潞炉掳忙碌脟戮煤麓脵.
				return;
			}
			if ((m_pClientList[iH]->m_iReqJoinPartyClientH != iClientH) || (strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) != 0)) {
				// 掳隆脌脭脌禄 陆脜脙禄脟脽麓酶 卤脳露搂 卤脳 脟脙路鹿脌脤戮卯掳隆 戮脝麓脧麓脵.
				return;
			}

			if (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_NULL) {
				// 陆脜脙禄 鹿脼脌潞 脟脙路鹿脌脤戮卯掳隆 戮脝鹿芦路卤 脝脛脝录驴隆碌碌 掳隆脌脭脟脧掳铆 脌脰脕枚 戮脢麓脵. 脌脤 脛鲁赂炉脜脥赂娄 脝脛脝录 赂露陆潞脜脥路脦 陆脜脙禄脟脩麓脵.
				RequestCreatePartyHandler(iClientH);
			}
		}
		break;

	case 2: // 脝脛脝录 掳隆脌脭 陆脜脙禄 脙毛录脪
		// 脌脤 赂脼陆脙脕枚赂娄 鹿脼卤芒 脌眉驴隆 脌脤鹿脤 脝脛脝录驴隆 掳隆脌脭碌脟戮煤麓脵赂茅 脜禄脜冒 脙鲁赂庐赂娄 脟脩麓脵.
		if ((m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM)) {
			// 赂芒鹿枚 脕娄掳脜赂娄 驴盲脙禄脟脩麓脵.
			RequestDismissPartyHandler(iClientH);
		}
		else {
			// 掳隆脌脭 陆脜脙禄 禄贸脜脗驴麓麓酶 掳陋碌茅脌禄 脙脢卤芒脠颅 脟脩麓脵.
			iH = m_pClientList[iClientH]->m_iReqJoinPartyClientH;

			// 陆脜脙禄脟脽麓酶 脛鲁赂炉脜脥掳隆 脕垄录脫脌禄 虏梅脌潞 禄贸脜脗露贸赂茅 NULL脌脧 录枚 脌脰麓脵.
			if ((m_pClientList[iH] != 0) && (m_pClientList[iH]->m_iReqJoinPartyClientH == iClientH) &&
				(strcmp(m_pClientList[iH]->m_cReqJoinPartyName, m_pClientList[iClientH]->m_cCharName) == 0)) {
				// 陆脜脙禄脟脽麓酶 脛鲁赂炉脜脥脌脟 脝脛脝录 戮脝脌脤碌冒驴脥 禄贸脜脗麓脗 掳脟碌氓赂庐脕枚 戮脢麓脗麓脵.
				m_pClientList[iH]->m_iReqJoinPartyClientH = 0;
				ZeroMemory(m_pClientList[iH]->m_cReqJoinPartyName, sizeof(m_pClientList[iH]->m_cReqJoinPartyName));
			}

			m_pClientList[iClientH]->m_iPartyID = 0;
			m_pClientList[iClientH]->m_iPartyStatus = DEF_PARTYSTATUS_NULL;
			m_pClientList[iClientH]->m_iReqJoinPartyClientH = 0;
			ZeroMemory(m_pClientList[iClientH]->m_cReqJoinPartyName, sizeof(m_pClientList[iClientH]->m_cReqJoinPartyName));
		}
		break;
	}
}

//New Party Status - ZeroEoyPnk - 06/09/2010
void CGame::RefreshPartyStatus(int iClientH)
{
	int i, HPParty, NotifyClient, PartyId, x, MaxHpParty, TotalMembers, PartySex, MPParty, MaxMpParty;

	NotifyClient = 0;
	HPParty = 0;
	PartySex = 0;
	MaxHpParty = 0;
	TotalMembers = 0;
	MPParty = 0;
	MaxMpParty = 0;
	x = 0;

	if (m_pClientList[iClientH] == 0) return;

	if ((m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
	{
		HPParty = m_pClientList[iClientH]->m_iHP;
		MaxHpParty = iGetMaxHP(iClientH, false);
		MPParty = m_pClientList[iClientH]->m_iMP;
		MaxMpParty = iGetMaxMP(iClientH);
		PartySex = m_pClientList[iClientH]->m_cSex;
		TotalMembers = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers;
		for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
		{
			x++;
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH)
			{
				PartyId = x;
			}
		}
		for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
		{
			NotifyClient = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];
			if (m_pClientList[NotifyClient] != 0)
			{
				SendNotifyMsg(0, NotifyClient, DEF_SEND_PARTYHP, PartyId, HPParty, MaxHpParty, 0, PartySex, MPParty, MaxMpParty);
			}
		}
	}
}

//LifeX Party Cords
void CGame::RefreshPartyCoords(int iClientH)
{
	int i, x, iH, PartyId;
	char MapName[12];
	int Party_sX, Party_sY;

	iH = 0;
	PartyId = 0;
	Party_sX = 0;
	Party_sY = 0;
	x = 0;

	if (m_pClientList[iClientH] == 0) return;

	if ((!bShinning) && (m_pClientList[iClientH]->m_iPartyID != 0) && (m_pClientList[iClientH]->m_iPartyStatus == DEF_PARTYSTATUS_CONFIRM))
	{
		Party_sX = m_pClientList[iClientH]->m_sX;
		Party_sY = m_pClientList[iClientH]->m_sY;
		ZeroMemory(MapName, sizeof(MapName));
		strcpy(MapName, m_pClientList[iClientH]->m_cMapName);

		for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
		{
			x++;
			if (m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i] == iClientH)
			{
				PartyId = x;
			}
		}

		for (i = 0; i < m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iTotalMembers; i++)
		{
			iH = m_stPartyInfo[m_pClientList[iClientH]->m_iPartyID].iIndex[i];

			if (m_pClientList[iH] != 0)
			{
				SendNotifyMsg(0, iH, DEF_SEND_PARTYCOORDS, PartyId, Party_sX, Party_sY, MapName, 0, 0);
			}
		}
	}
}

