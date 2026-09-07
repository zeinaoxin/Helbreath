// Skill.cpp: implementation of the CSkill class.
//
//////////////////////////////////////////////////////////////////////

#include "Skill.h"

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkill::CSkill()
{
	ZeroMemory(m_cName, sizeof(m_cName));
}

CSkill::~CSkill()
{ 

}

int CGame::_iGetWeaponSkillType(int iClientH)
{
	WORD wWeaponType;

	if (m_pClientList[iClientH] == 0) return 0;

	wWeaponType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);

	if (wWeaponType == 0) {
		return 5;
	}
	else if ((wWeaponType >= 1) && (wWeaponType <= 2)) {
		return 7;
	}
	else if ((wWeaponType > 2) && (wWeaponType < 20)) {
		if (wWeaponType == 7)
			return 9;
		else return 8;
	}
	else if ((wWeaponType >= 20) && (wWeaponType < 30)) {
		return 10;
	}
	else if ((wWeaponType >= 30) && (wWeaponType < 35)) {
		return 14;
	}
	else if ((wWeaponType >= 35) && (wWeaponType < 40)) {
		return 21;
	}
	else if (wWeaponType >= 40) {
		return 6;
	}

	return 1;
}

void CGame::PoisonEffect(int iClientH, int iV1)
{
	int iPoisonLevel, iDamage, iPrevHP, iProb;

	// 脕脽碌露脌赂路脦 脕脳脕枚麓脗 戮脢麓脗麓脵. 麓脵赂赂 脙录路脗脌脤 掳猫录脫 卤茂脌脤掳铆 脙脰录脪 1赂赂 鲁虏麓脗麓脵. 
	if (m_pClientList[iClientH] == 0)     return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	iPoisonLevel = m_pClientList[iClientH]->m_iPoisonLevel;

	iDamage = iDice(1, iPoisonLevel);

	iPrevHP = m_pClientList[iClientH]->m_iHP;
	m_pClientList[iClientH]->m_iHP -= iDamage;
	if (m_pClientList[iClientH]->m_iHP <= 0) m_pClientList[iClientH]->m_iHP = 1;

	if (iPrevHP != m_pClientList[iClientH]->m_iHP)
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);

	// 碌露录潞 脌煤脟脳 脠庐路眉路脦 脕脽碌露脌脤 脟庐赂卤 录枚 脌脰麓脵.
	iProb = m_pClientList[iClientH]->m_cSkillMastery[23] + m_pClientList[iClientH]->m_iAddPR;
	if (iProb <= 10) iProb = 10;
	if (iDice(1, 100) <= iProb) {
		m_pClientList[iClientH]->m_bIsPoisoned = false;
		// 脕脽碌露脌脤 脟庐路脠脌陆脌禄 戮脣赂掳麓脵. 
		SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false); // remove poison aura after effect complete
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
	}
}
bool CGame::bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType)
{
	int iResist, iResult;

	// 碌露录潞 脌煤脟脳脌脤 录潞掳酶脟脽麓脗脕枚赂娄 掳猫禄锚脟脩麓脵. 
	switch (cOwnerType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[sOwnerH] == 0) return false;
		iResist = m_pClientList[sOwnerH]->m_cSkillMastery[23] + m_pClientList[sOwnerH]->m_iAddPR;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[sOwnerH] == 0) return false;
		iResist = 0;
		break;
	}

	iResult = iDice(1, 1000);
	if (iResult > iResist) // 碌露录潞 脌煤脟脳 陆脟脝脨. 脕脽碌露碌脠麓脵.
		return false;

	// 碌露录潞 脌煤脟脳 录潞掳酶. 脟脙路鹿脌脤戮卯露贸赂茅 陆潞脜鲁脌禄 驴脙赂掳麓脵. 
	if (cOwnerType == DEF_OWNERTYPE_PLAYER)
		CalculateSSN_SkillIndex(sOwnerH, 23, 1);

	return true;
}

bool CGame::_bDecodeSkillConfigFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iSkillConfigListIndex = 0;
	class CStrTok* pStrTok;

	pContents = new char[dwMsgSize + 1];
	ZeroMemory(pContents, dwMsgSize + 1);
	memcpy(pContents, pData, dwMsgSize);

	pStrTok = new class CStrTok(pContents, seps);
	token = pStrTok->pGet();
	while (token != 0) {
		if (cReadModeA != 0) {
			switch (cReadModeA) {
			case 1:
				switch (cReadModeB) {
				case 1:
					// 陆潞脜鲁 鹿酶脠拢 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					if (m_pSkillConfigList[atoi(token)] != 0) {
						// 脌脤鹿脤 脟脪麓莽碌脠 鹿酶脠拢掳隆 脌脰麓脵. 驴隆路炉脌脤麓脵.
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Duplicate magic number.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[atoi(token)] = new class CSkill;
					iSkillConfigListIndex = atoi(token);

					cReadModeB = 2;
					break;

				case 2:
					// 陆潞脜鲁 脌脤赂搂 
					ZeroMemory(m_pSkillConfigList[iSkillConfigListIndex]->m_cName, sizeof(m_pSkillConfigList[iSkillConfigListIndex]->m_cName));
					memcpy(m_pSkillConfigList[iSkillConfigListIndex]->m_cName, token, strlen(token));
					cReadModeB = 3;
					break;

				case 3:
					// 陆潞脜鲁 脕戮路霉 m_sType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sType = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					// m_sValue1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue1 = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// m_sValue2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue2 = atoi(token);
					cReadModeB = 6;
					break;

				case 6:
					// m_sValue3
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue3 = atoi(token);
					cReadModeB = 7;
					break;

				case 7:
					// m_sValue4
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue4 = atoi(token);
					cReadModeB = 8;
					break;

				case 8:
					// m_sValue5
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue5 = atoi(token);
					cReadModeB = 9;
					break;

				case 9:
					// m_sValue6
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pSkillConfigList[iSkillConfigListIndex]->m_sValue6 = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					break;
				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "skill", 5) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}

		}
		token = pStrTok->pGet();

	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! SKILL configuration file contents error!");
		return false;
	}

	wsprintf(cTxt, "(!) SKILL(Total:%d) configuration - success!", iSkillConfigListIndex);
	PutLogList(cTxt);

	return true;
}

void CGame::TrainSkillResponse(bool bSuccess, int iClientH, int iSkillNum, int iSkillLevel)
{
	char* cp, cData[100];
	DWORD* dwp;
	WORD* wp;
	int   iRet;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((iSkillNum < 0) || (iSkillNum > 100)) return;
	if ((iSkillLevel < 0) || (iSkillLevel > 100)) return;

	if (bSuccess == true) 
	{
		if (m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] != 0) return;

		m_pClientList[iClientH]->m_cSkillMastery[iSkillNum] = iSkillLevel;
		bCheckTotalSkillMasteryPoints(iClientH, iSkillNum);

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_SKILLTRAINSUCCESS;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		*cp = iSkillNum;
		cp++;

		*cp = iSkillLevel;
		cp++;

		if (m_pSkillConfigList[iSkillNum]->m_cName != 0)
			_bItemLog(DEF_ITEMLOG_SKILLLEARN, iClientH, m_pSkillConfigList[iSkillNum]->m_cName, 0);

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 8);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 皋矫瘤甫 焊尘锭 俊矾啊 惯积沁促搁 力芭茄促.
			DeleteClient(iClientH, true, true);
			break;
		}
	}
}


int CGame::_iGetSkillNumber(char* pSkillName)
{
	int i;
	char cTmpName[21];

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pSkillName);

	for (i = 1; i < DEF_MAXSKILLTYPE; i++)
		if (m_pSkillConfigList[i] != 0) 
		{
			if (memcmp(cTmpName, m_pSkillConfigList[i]->m_cName, 20) == 0) 
			{
				return i;
			}
		}

	return 0;
}

int CGame::_iCalcSkillSSNpoint(int iLevel)
{
	int iRet;

	if (iLevel < 1) return 1;

	if (iLevel <= 50)
		iRet = iLevel;
	else if (iLevel > 50) {
		iRet = (iLevel * iLevel) / 10;
	}

	return iRet;
}

void CGame::CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue)
{
	short sSkillIndex;
	int   iOldSSN, iSSNpoint, iWeaponIndex;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_pItemList[sWeaponIndex] == 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	sSkillIndex = m_pClientList[iClientH]->m_pItemList[sWeaponIndex]->m_sRelatedSkill;
	if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;
	if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] == 100) return;

	iOldSSN = m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex];
	
	m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] += iValue;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLPOINT, sSkillIndex, m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex], 0, 0);

	iSSNpoint = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] + 1];

	if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] >= iSSNpoint) {

		m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]++;

		switch (sSkillIndex) {

		case 0:  // Mining
		case 5:  // Hand-Attack
		case 13: // Manufacturing
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iStr) * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 3: // Magic-Resistance
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iLevel) * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 4:  // Magic
		case 18: // Crafting
		case 21: // Staff-Attack
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iMag) * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 6:  // Archery
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iCharisma * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 1:  // Fishing
		case 7:  // Short-Sword
		case 8:  // Long-Sword
		case 9:  // Fencing 
		case 10: // Axe-Attack
		case 11: // Shield        	
		case 14: // Hammer 
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iDex) * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 2:	 // Farming
		case 12: // Alchemy
		case 15: // 览鞭贸摹
		case 19: // Pretend-Corpse
		case 22: // Taming
		case 20: // Enchanting
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iInt) * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 23: // Poison-Resistance
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iVit * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		default:
			m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;
		}

		if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0) 
		{
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1) 
			{
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex) 
				{
					m_pClientList[iClientH]->m_iHitRatio++;
				}
			}

			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) 
			{
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex) 
				{
					m_pClientList[iClientH]->m_iHitRatio++;
				}
			}
		
			bCheckTotalSkillMasteryPoints(iClientH, sSkillIndex);
			//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex], 0);
		}
	}
}



void CGame::CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue)
{
	int   iOldSSN, iSSNpoint, iWeaponIndex;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((sSkillIndex < 0) || (sSkillIndex >= DEF_MAXSKILLTYPE)) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;

	if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] == 100) return;
	
	iOldSSN = m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex];
	
	m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] += iValue;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLPOINT, sSkillIndex, m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex], 0, 0);

	iSSNpoint = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] + 1];

	if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] >= iSSNpoint) {

		// 陆潞脜鲁脌脤 驴脙露煤麓脵.
		m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]++;
		// 脝炉录潞脛隆 脕娄脟脩脌脤 脌脰麓脗 Skill脌禄 脙鲁赂庐脟脩麓脵.
		switch (sSkillIndex) {
		case 0:
		case 5:
		case 13:
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iStr) * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 3:
			// 赂露鹿媒 脌煤脟脳脌潞 脙脰麓毛 Level*2赂赂脜颅 驴脌赂楼麓脵.
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iLevel) * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 4:
		case 18: // Crafting
		case 21:
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iMag) * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 1:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iDex) * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 2:
		case 12:
		case 14:
		case 15:
		case 19:
		case 22:
		case 20: // Enchanting
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > ((m_pClientList[iClientH]->m_iInt) * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 23: // 碌露录潞 脌煤脟脳 
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iVit * 2)) {
				// 脕娄脟脩脛隆潞赂麓脵 脛驴脕鲁麓脵. 鹿芦脠驴脌脤鹿脟路脦 脌脤脌眉禄贸脜脗路脦 碌脟碌鹿赂掳麓脵.
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		case 6:
			if (m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex] > (m_pClientList[iClientH]->m_iCharisma * 2)) {
				m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex]--;
				m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = iOldSSN;
			}
			else m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;

		default:
			m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] = 0;
			break;
		}

		// 赂赂戮脿 鹿芦卤芒驴脥 掳眉路脙脌脰麓脗 陆潞脜鲁脌脤 驴脙露煤掳铆 脟枚脌莽 卤脳 鹿芦卤芒赂娄 禄莽驴毛 脕脽脌脤露贸赂茅 脟枚脌莽 赂铆脕脽路眉驴隆 1脌禄 麓玫脟脩麓脵. 
		if (m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex] == 0) {
			// 脛芦驴卯脝庐掳隆 0脌脤赂茅 陆潞脜鲁脌脤 脕陇禄贸脌没脌赂路脦 驴脙露煤麓脵麓脗 脌脟鹿脤. 
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1) {
				// 戮莽录脮鹿芦卤芒掳隆 脌氓脗酶碌脟戮卯 脌脰戮煤麓脵. 
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex) {
					// 脠掳路霉脌脟 禄莽驴毛驴隆 脌脟脟脩 陆潞脜鲁脌脟 禄贸陆脗脌脤戮煤麓脵. 赂铆脕脽路眉脌禄 1 驴脙赂掳麓脵. 
					m_pClientList[iClientH]->m_iHitRatio++;
				}
			}

			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) {
				// 鹿芦卤芒掳隆 脌氓脗酶碌脟戮卯 脌脰戮煤麓脵. 
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sSkillIndex) {
					// 掳脣脌脤鲁陋 Mace路霉脌脟 禄莽驴毛驴隆 脌脟脟脩 陆潞脜鲁脌脟 禄贸陆脗脌脤戮煤麓脵. 赂铆脕脽路眉脌禄 1 驴脙赂掳麓脵. 
					m_pClientList[iClientH]->m_iHitRatio++;
				}
			}

			// SKill脌脟 脙脩脟脮脌脤 700脌禄 鲁脩脌赂赂茅 麓脵赂楼 陆潞脜鲁脕脽 脟脧鲁陋赂娄 1 鲁禄赂掳麓脵. 
			bCheckTotalSkillMasteryPoints(iClientH, sSkillIndex);

			// Skill脌脤 驴脙露煤麓脵麓脗 掳脥脌禄 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脣路脕脕脴麓脵.
			//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sSkillIndex], m_pClientList[iClientH]->m_iSkillSSN[sSkillIndex], 0);
		}
	}
}

char _tmp_cCorpseX[] = { 0,  1, 1, 1, 0, -1, -1, -1, 0, 0, 0, 0 };
char _tmp_cCorpseY[] = { -1, -1, 0, 1, 1,  1,  0, -1, 0, 0, 0 };

void CGame::ClearSkillUsingStatus(int iClientH)
{
	int i;
	short tX, fX, tY, fY;

	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == true) {
		tX = m_pClientList[iClientH]->m_sX;
		tY = m_pClientList[iClientH]->m_sY;
		if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(tX, tY, 0) == false) {
			fX = m_pClientList[iClientH]->m_sX + _tmp_cCorpseX[m_pClientList[iClientH]->m_cDir];
			fY = m_pClientList[iClientH]->m_sY + _tmp_cCorpseY[m_pClientList[iClientH]->m_cDir];
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(fX, fY, 0) == false) {
				m_pClientList[iClientH]->m_cDir = iDice(1, 8);
				fX = m_pClientList[iClientH]->m_sX + _tmp_cCorpseX[m_pClientList[iClientH]->m_cDir];
				fY = m_pClientList[iClientH]->m_sY + _tmp_cCorpseY[m_pClientList[iClientH]->m_cDir];
				if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetMoveable(fX, fY, 0) == false) {
					return;
				}
			}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_DAMAGEMOVE, m_pClientList[iClientH]->m_cDir, 0, 0, 0);
		}
	}
	// Taming
	else if (m_pClientList[iClientH]->m_bSkillUsingStatus[22] == true)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
		bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_DELAYEVENTTYPE_TAMING_SKILL);
	}
	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		m_pClientList[iClientH]->m_bSkillUsingStatus[i] = false;
		m_pClientList[iClientH]->m_iSkillUsingTimeID[i] = 0;
	}

	if (m_pClientList[iClientH]->m_iAllocatedFish != 0) {
		if (m_pFish[m_pClientList[iClientH]->m_iAllocatedFish] != 0)
			m_pFish[m_pClientList[iClientH]->m_iAllocatedFish]->m_sEngagingCount--;

		m_pClientList[iClientH]->m_iAllocatedFish = 0;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FISHCANCELED, 0, 0, 0, 0);
	}

}

int CGame::iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY)
{
	class CItem* pItem;
	char  cItemName[21];
	short lX, lY;
	int   iResult, iFish;

	switch (cOwnerType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[iOwnerH] == 0) return 0;
		if (m_pClientList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
		lX = m_pClientList[iOwnerH]->m_sX;
		lY = m_pClientList[iOwnerH]->m_sY;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[iOwnerH] == 0) return 0;
		if (m_pNpcList[iOwnerH]->m_cMapIndex != cMapIndex) return 0;
		lX = m_pNpcList[iOwnerH]->m_sX;
		lY = m_pNpcList[iOwnerH]->m_sY;
		break;
	}

	// 陆潞脜鲁 禄莽驴毛 驴漏潞脦 脕脰禄莽脌搂赂娄 卤录赂掳麓脵. 
	if (cOwnerSkill == 0) return 0;

	// 陆潞脜鲁脌脤 100脌脤露贸掳铆 脟脴碌碌 掳隆虏没 鲁卢陆脙赂娄 陆脟脝脨脟脧掳脭 脟脧卤芒 脌搂脟脴录颅 1D105 
	iResult = iDice(1, 100);
	if (cOwnerSkill <= iResult)	return 0;  // 陆脟脝脨麓脵.

	// 露楼驴隆录颅麓脗 鲁卢陆脙掳隆 潞脪掳隆麓脡 
	if (m_pMapList[cMapIndex]->bGetIsWater(dX, dY) == false) return 0;

	// 录潞掳酶脟脽脌赂鹿脟路脦 陆潞脜鲁 脛芦驴卯脝庐赂娄 驴脙赂掳麓脵.
	if (cOwnerType == DEF_OWNERTYPE_PLAYER)
		CalculateSSN_SkillIndex(iOwnerH, iSkillNum, 1);

	switch (m_pSkillConfigList[iSkillNum]->m_sType) {
	case DEF_SKILLEFFECTTYPE_TAMING:
		// 卤忙碌茅脌脤卤芒 卤芒录煤: dX, dY 潞脦卤脵脌脟 赂贸陆潞脜脥赂娄 卤忙碌茅脌脦麓脵.
		_TamingHandler(iOwnerH, iSkillNum, cMapIndex, dX, dY);
		break;

	case DEF_SKILLEFFECTTYPE_GET:
		// 戮脝脌脤脜脹脌禄 戮貌麓脗 卤芒录煤脌脤戮煤麓脵. 
		ZeroMemory(cItemName, sizeof(cItemName));
		switch (m_pSkillConfigList[iSkillNum]->m_sValue1) {
		case 1:
			// 卤陇鹿掳 
			wsprintf(cItemName, "Meat");
			break;

		case 2:
			// 卤脵脙鲁驴隆 麓脵脌脤鲁陋鹿脥 驴脌潞锚脕搂脝庐 鹿掳掳铆卤芒掳隆 脕赂脌莽脟脩麓脵赂茅 潞禄掳脻鲁卢陆脙 赂冒碌氓路脦 碌茅戮卯掳拢麓脵.
			if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
				iFish = iCheckFish(iOwnerH, cMapIndex, dX, dY);
				if (iFish == 0) wsprintf(cItemName, "Fish");
			}
			else wsprintf(cItemName, "Fish");
			break;
		}

		if (strlen(cItemName) != 0) {

			// 鲁卢陆脙驴隆 录潞掳酶脟脽麓脵赂茅 赂脼陆脙脕枚赂娄 脌眉录脹.
			if (memcmp(cItemName, "Fish", 4) == 0) {
				SendNotifyMsg(0, iOwnerH, DEF_NOTIFY_FISHSUCCESS, 0, 0, 0, 0);
				// v1.41 戮脿掳拢脌脟 掳忙脟猫脛隆 禄贸陆脗 
				GetExp(iOwnerH, iDice(1, 2));
			}

			pItem = new class CItem;
			if (pItem == 0) return 0;
			if (_bInitItemAttr(pItem, cItemName) == true) {
				// 戮脝脌脤脜脹脌禄 鲁玫麓脗麓脵. 
				m_pMapList[cMapIndex]->bSetItem(lX, lY, pItem);

				// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
				//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, cMapIndex,
				//	lX, lY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4

				SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, cMapIndex,
					lX, lY, pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
			}
		}
		break;
	}

	return 1;
}

void CGame::UseSkillHandler(int iClientH, int iV1, int iV2, int iV3)
{
	char  cOwnerType;
	short sAttackerWeapon, sOwnerH;
	int   iResult, iPlayerSkillLevel;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	if ((iV1 < 0) || (iV1 >= DEF_MAXSKILLTYPE)) return;
	if (m_pSkillConfigList[iV1] == 0) return;
	// 脌脤鹿脤 卤芒录煤脌禄 禄莽驴毛脕脽脌脤露贸碌碌 赂庐脜脧.
	if (m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] == true) return;

	// 脟脙路鹿脌脤戮卯脌脟 卤芒录煤录枚脕脴驴隆 碌没露贸 录潞掳酶驴漏潞脦赂娄 掳猫禄锚脟脩麓脵. 
	iPlayerSkillLevel = m_pClientList[iClientH]->m_cSkillMastery[iV1];
	iResult = iDice(1, 100);

	if (iResult > iPlayerSkillLevel) {
		// 陆脟脝脨麓脵. 
		// 卤芒录煤 禄莽驴毛脌脤 脕脽脕枚 碌脟戮煤脌陆脌禄 戮脣赂掳麓脵.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
		return;
	}

	// iV1脌脤 卤芒录煤 鹿酶脠拢 
	switch (m_pSkillConfigList[iV1]->m_sType) {

	case DEF_SKILLEFFECTTYPE_TAMING: // Taming
		if (((m_pClientList[iClientH]->m_iStatus & 0x00000010) == 0)      // Invi character can not tame !
			&& (m_pClientList[iClientH]->m_bSkillUsingStatus[19] == false)) // PretendCorpse character canot Tame
		{
			m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] = true;
			m_pClientList[iClientH]->m_iSkillUsingTimeID[iV1] = (int)timeGetTime();
			// Register delay to perform Taming
			bRegisterDelayEvent(DEF_DELAYEVENTTYPE_TAMING_SKILL // int iDelayType
				, 22 // int iEffectType
				, timeGetTime() + m_pSkillConfigList[22]->m_sValue2 * 100 // nbe of 1/10th seconds in Skill.cfg
				, iClientH // int iTargetH
				, DEF_OWNERTYPE_PLAYER // char cTargetType
				, m_pClientList[iClientH]->m_cMapIndex // char cMapIndex
				, m_pClientList[iClientH]->m_sX // int X
				, m_pClientList[iClientH]->m_sY // int Y
				, (int)m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] // int iV1
				, m_pClientList[iClientH]->m_iSkillUsingTimeID[iV1] // int iV2
				, 0); // int iV3		
		}
		else SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
		break;

	case DEF_SKILLEFFECTTYPE_PRETEND:
		switch (m_pSkillConfigList[iV1]->m_sValue1) {
		case 1:
			// 脕脳脌潞脙麓脟脧卤芒 卤芒录煤脌脤麓脵.	

			// v1.44 禄莽脜玫脌氓脌脤赂茅 脕脳脌潞脙麓脟脧卤芒 赂酶脟脩麓脵.
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == true) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
				return;
			}

			//赂赂戮脿 鹿脵麓脷驴隆 陆脙脙录掳隆 脌脰麓脵赂茅 脕脳脌潞脙麓 脟脧卤芒赂娄 脟脪 录枚 戮酶麓脵. 
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
			if (sOwnerH != 0) {
				// 脕脳脌潞脙麓 脟脧卤芒赂娄 脟脪 脌脷赂庐驴隆 陆脙脙录掳隆 脌脰戮卯 卤芒录煤 禄莽驴毛脌脤 脕脽脕枚 碌脟戮煤脌陆脌禄 戮脣赂掳麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
				return;
			}

			// 脕脳脌潞脙麓 脟脧掳铆脌脷 脟脧麓脗 脕脰潞炉驴隆 鹿掳脙录掳隆 脌脰戮卯碌碌 脟脪 录枚 戮酶麓脵. 
			iResult = 0;
			if (m_pClientList[iClientH]->m_iAdminUserLevel <= 0) {
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY - 1);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY + 1);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY);
				iResult += sOwnerH;

				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY - 1);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY - 1);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX - 1, m_pClientList[iClientH]->m_sY + 1);
				iResult += sOwnerH;
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, m_pClientList[iClientH]->m_sX + 1, m_pClientList[iClientH]->m_sY + 1);
				iResult += sOwnerH;
			}

			if (iResult != 0) {
				// 脕脳脌潞脙麓 脟脧卤芒赂娄 脟脪 脌脷赂庐驴隆 陆脙脙录掳隆 脌脰戮卯 卤芒录煤 禄莽驴毛脌脤 脕脽脕枚 碌脟戮煤脌陆脌禄 戮脣赂掳麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
				return;
			}

			// 陆潞脜鲁 脛芦驴卯脝庐 驴脙赂掳麓脵. <-- 鲁禄潞脦驴隆录颅 脛芦驴卯脝脙脌禄 脟脴戮脽 戮没露脳脟脩 陆潞脜鲁脌脤 驴脌赂拢麓脗 掳忙驴矛掳隆 戮酶麓脵. 
			CalculateSSN_SkillIndex(iClientH, iV1, 1);

			// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 脕脳麓脗 碌驴脌脹 脌眉录脹.
			sAttackerWeapon = 1;
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTDYING, 0, sAttackerWeapon, 0);
			// 脕陇禄贸 脌搂脛隆驴隆录颅 脕枚驴卯麓脵.
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(14, iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
			// 脕脳脌潞 脌搂脛隆 脟楼陆脙赂娄 脟脩麓脵.
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetDeadOwner(iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY);
			break;
		}
		break;

	}

	m_pClientList[iClientH]->m_bSkillUsingStatus[iV1] = true;
}

void CGame::SetDownSkillIndexHandler(int iClientH, int iSkillIndex)
{
	if (m_pClientList[iClientH] == 0) return;
	if ((iSkillIndex < 0) || (iSkillIndex >= DEF_MAXSKILLTYPE)) return;

	if (m_pClientList[iClientH]->m_cSkillMastery[iSkillIndex] > 0)
		m_pClientList[iClientH]->m_iDownSkillIndex = iSkillIndex;

	SendNotifyMsg(0, iClientH, DEF_NOTIFY_DOWNSKILLINDEXSET, m_pClientList[iClientH]->m_iDownSkillIndex, 0, 0, 0);
}

void CGame::_TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY)
{
	int iSkillLevel, iRange, iTamingLevel, iResult, iX, iY, iTamingType = 0, iNbeTamed = 0;
	short sOwnerH;
	char cOwnerType, cTamedString[500];
	if (m_pClientList[iClientH] == 0) return;
	if (m_pMapList[cMapIndex] == 0) return;
	if (m_pClientList[iClientH]->m_iAdminUserLevel == 0)
	{
		/*if ((m_pClientList[iClientH]->m_iStr > 100)
			&& (m_pClientList[iClientH]->m_iInt > 49))
		{
			iTamingType = 1; // War
		}
		else if ((m_pClientList[iClientH]->m_iMag > 100)
			&& (m_pClientList[iClientH]->m_iInt > 49))
		{
			iTamingType = 2; // Mage
		}
		else if ((m_pClientList[iClientH]->m_iCharisma > 100)
			&& (m_pClientList[iClientH]->m_iInt > 49))
		{
			iTamingType = 3; // Archer
		}*/

		iTamingType = m_pClientList[iClientH]->m_iClass;
	}
	else iTamingType = 4; // GameMaster
	iSkillLevel = (int)m_pClientList[iClientH]->m_cSkillMastery[iSkillNum];
	iRange = 2 + iSkillLevel / 20;
	for (iX = dX - iRange; iX <= dX + iRange; iX++)
		for (iY = dY - iRange; iY <= dY + iRange; iY++)
		{
			sOwnerH = 0;
			if ((iX > 0) && (iY > 0) && (iX < m_pMapList[cMapIndex]->m_sSizeX) && (iY < m_pMapList[cMapIndex]->m_sSizeY))
			{
				m_pMapList[cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, iX, iY);
			}
			if (sOwnerH != 0) {
				switch (cOwnerType) {
				case DEF_OWNERTYPE_PLAYER:
					break;
				case DEF_OWNERTYPE_NPC:
					if (m_pNpcList[sOwnerH] == 0) break;
					// 1st list for everybody			
					iTamingLevel = 201; // 201 means impossible
					switch (m_pNpcList[sOwnerH]->m_sType) {
					case 14: iTamingLevel = 35; break;	// Orc
					case 28: iTamingLevel = 65; break;	// Troll
					case 29: iTamingLevel = 100; break;	// Ogre
					case 58: iTamingLevel = 100; break;	// MG
					case 59: iTamingLevel = 120; break;	// Ettin
					case 77: iTamingLevel = 160; break;	// MasterMage-Orc 
					case 78: iTamingLevel = 170; break;	// Minos 
					}
					switch (iTamingType) {
					case 2: // Mage				
						if (iTamingLevel > 199) iTamingLevel = 199;
						switch (m_pNpcList[sOwnerH]->m_sType) {
						case 12:							// Stone
						case 23: iTamingLevel = 40; break;	// Clay
						case 65: iTamingLevel = 70; break;	// IceGolem
						case 80: iTamingLevel = 80; break;	// Tentocle
						case 53: iTamingLevel = 100; break;	// BB
						case 31: iTamingLevel = 170; break;	// DD
						}
						break;
					case 1: // War	
						if (iTamingLevel > 197) iTamingLevel = 197;
						switch (m_pNpcList[sOwnerH]->m_sType) {
						case 18:							// Zombie 
						case 11: iTamingLevel = 35; break;	// Skel
						case 27: iTamingLevel = 65; break;	// hh
						case 30: iTamingLevel = 120; break;	// Liche
						case 31: iTamingLevel = 170; break;	// DD
						case 32: // Uni possible for some purest Wars
							if ((m_pClientList[iClientH]->m_iPKCount == 0) // Uni
								&& (m_pClientList[iClientH]->m_iEnemyKillCount == 0)
								&& (m_pClientList[iClientH]->m_iRating > 0))
							{
								iTamingLevel = 100;
							}
							break;
						}
						break;
					case 3: // Archer
						break;
					case 4: // GameMaster
						iTamingLevel = 30;
						if (m_pNpcList[sOwnerH]->m_cSide == 0) iTamingLevel = 201;
						break;
					}
					iResult = iDice(2, iSkillLevel);
					if ((iResult >= iTamingLevel)
						&& (m_pNpcList[sOwnerH]->m_dwTamingTime == 0)
						&& (m_pNpcList[sOwnerH]->m_cSide != m_pClientList[iClientH]->m_cSide))
					{
						iNbeTamed++;
						// Now change npc side...
						m_pNpcList[sOwnerH]->m_cOriginalSide = m_pNpcList[sOwnerH]->m_cSide;
						switch (iTamingType) {
						case 4: // GameMaster
							m_pNpcList[sOwnerH]->m_cSide = 0;
							break;
						case 2: // Mage -> npc becomes friendly 					
							switch (m_pNpcList[sOwnerH]->m_sType) {
							case 12: // Stone
							case 23: // Clay
							case 65: // IceGolem
							case 80: // Tentocle
							case 53: // BB
							case 31: // DD						
								m_pNpcList[sOwnerH]->m_cSide = m_pClientList[iClientH]->m_cSide;
								break;
							default:
								m_pNpcList[sOwnerH]->m_cSide = 0;
								break;
							}
							break;
						case 1: // War	-> npc becomes friendly
							switch (m_pNpcList[sOwnerH]->m_sType) {
							case 18: // Zombie 
							case 11: // Skel
							case 27: // hh
							case 30: // Liche
							case 31: // DD
								if (iDice(1, 95) <= iSkillLevel)
									m_pNpcList[sOwnerH]->m_cSide = m_pClientList[iClientH]->m_cSide;
								else m_pNpcList[sOwnerH]->m_cSide = 0;
								break;
							case 32: // Uni
								m_pNpcList[sOwnerH]->m_cSide = m_pClientList[iClientH]->m_cSide;
								break;
							default:
								m_pNpcList[sOwnerH]->m_cSide = 0;
								break;
							}
							break;
						case 3: // Archer
							break;
						}
						if (iGetFollowerNumber(sOwnerH, cOwnerType) > 5) return;
						m_pNpcList[sOwnerH]->m_dwTamingTime = timeGetTime() + 3000 * iSkillLevel + 120000;
						m_pNpcList[sOwnerH]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						m_pNpcList[sOwnerH]->m_sBehaviorTurnCount = 0;
						m_pNpcList[sOwnerH]->m_iTargetIndex = 0;
						if (m_pNpcList[sOwnerH]->m_cSide == m_pClientList[iClientH]->m_cSide)
						{
							m_pNpcList[sOwnerH]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
							m_pNpcList[sOwnerH]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
							m_pNpcList[sOwnerH]->m_iFollowOwnerIndex = iClientH;
						}
						SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
						// Increase skill if not easy Taming
						if (iTamingLevel >= iSkillLevel)
						{
							CalculateSSN_SkillIndex(iClientH, 22, iNbeTamed);
						}
					} // End of succes in taming 1 creature
					break;
				}
			}
		}
	if (iNbeTamed == 0)  // No creature tamed
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 0, 0, 0, 0);
	}
	else
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILLUSINGEND, 1, 0, 0, 0);
		ZeroMemory(cTamedString, sizeof(cTamedString));
		switch (iTamingType) {
		case 1:	// War
			wsprintf(cTamedString, "Undead turned: %d", iNbeTamed);
			break;
		case 2:	// Mage
			wsprintf(cTamedString, "Magic things controled: %d", iNbeTamed);
			break;
		case 3: // Archer
			break;
		default: // GameMaster
			wsprintf(cTamedString, "Creatures neutralised: %d", iNbeTamed);
			break;
		}
		SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTamedString);
		// Debug	
		wsprintf(cTamedString, "(!) Taming skill: %s is using Taming skill (%d%%) (Type %d), he tamed %d creatures."
			, m_pClientList[iClientH]->m_cCharName
			, iSkillLevel
			, iTamingType
			, iNbeTamed);
		PutLogList(cTamedString);
	}
}

void CGame::GetMagicAbilityHandler(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_cSkillMastery[4] != 0) return;

	m_pClientList[iClientH]->m_cSkillMastery[4] = 20;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, 4, m_pClientList[iClientH]->m_cSkillMastery[4], 0, 0);
	bCheckTotalSkillMasteryPoints(iClientH, 4);
}

bool CGame::bPlantSeedBag(int iMapIndex, int dX, int dY, int iItemEffectValue1, int iItemEffectValue2, int iClientH)
{
	int iNamingValue, tX, tY;
	short sOwnerH;
	char cOwnerType, cNpcName[21], cName[21], cNpcWaypointIndex[11];
	bool bRet;

	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture >= 200) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOMOREAGRICULTURE, 0, 0, 0, 0);
		return false;
	}

	if (iItemEffectValue2 > m_pClientList[iClientH]->m_cSkillMastery[2]) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_AGRICULTURESKILLLIMIT, 0, 0, 0, 0);
		return false;
	}

	iNamingValue = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->iGetEmptyNamingValue();

	if (iNamingValue != -1) {

		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
		if (sOwnerH != 0 && sOwnerH == DEF_OWNERTYPE_NPC && m_pNpcList[sOwnerH]->m_cActionLimit == 5) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, 0, 0, 0, 0);
			return false;
		}
		else {
			if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bGetIsFarm(dX, dY) == false) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_AGRICULTURENOAREA, 0, 0, 0, 0);
				return false;
			}

			ZeroMemory(cNpcName, sizeof(cNpcName));
			strcpy(cNpcName, "Crops");
			ZeroMemory(cName, sizeof(cName));
			wsprintf(cName, "XX%d", iNamingValue);
			cName[0] = '_';
			cName[1] = iMapIndex + 65;

			ZeroMemory(cNpcWaypointIndex, sizeof(cNpcWaypointIndex));
			tX = dX;
			tY = dY;

			bRet = bCreateNewNpc(cNpcName, cName, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_cName, 0, 0, DEF_MOVETYPE_RANDOM, &tX, &tY, cNpcWaypointIndex, 0, 0, 0, false, true);
			if (bRet == false) {
				m_pMapList[iMapIndex]->SetNamingValueEmpty(iNamingValue);
			}
			else {
				m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX, tY);
				if (m_pNpcList[sOwnerH] == 0) return 0;
				m_pNpcList[sOwnerH]->m_cCropType = iItemEffectValue1;
				switch (iItemEffectValue1) {
				case 1: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 2: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 3: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 4: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 5: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 6: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 7: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 8: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 9: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 10: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 11: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 12: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				case 13: m_pNpcList[sOwnerH]->m_cCropSkill = iItemEffectValue2; break;
				default: m_pNpcList[sOwnerH]->m_cCropSkill = 100; break;
				}
				m_pNpcList[sOwnerH]->m_sAppr2 = 1;
				SendEventToNearClient_TypeA(sOwnerH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);
				wsprintf(G_cTxt, "(skill:%d type:%d)plant(%s) Agriculture begin(%d,%d) sum(%d)!", m_pNpcList[sOwnerH]->m_cCropSkill, m_pNpcList[sOwnerH]->m_cCropType, cNpcName, tX, tY, m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_iTotalAgriculture);
				PutLogList(G_cTxt);
				return true;
			}
		}
	}
	return false;
}

void CGame::_CheckFarmingAction(short sAttackerH, short sTargetH, bool bType)
{
	char cCropType;
	int iItemID;
	class CItem* pItem;

	iItemID = 0;
	cCropType = 0;

	cCropType = m_pNpcList[sTargetH]->m_cCropType;
	switch (cCropType) {
	case 1: GetExp(sAttackerH, iDice(3, 100) * 100); iItemID = 820; break; // WaterMelon
	case 2: GetExp(sAttackerH, iDice(3, 100) * 100); iItemID = 821; break; // Pumpkin
	case 3: GetExp(sAttackerH, iDice(4, 100) * 100); iItemID = 822; break; // Garlic
	case 4: GetExp(sAttackerH, iDice(4, 100) * 100); iItemID = 823; break; // Barley
	case 5: GetExp(sAttackerH, iDice(5, 100) * 100); iItemID = 824; break; // Carrot
	case 6: GetExp(sAttackerH, iDice(5, 100) * 100); iItemID = 825; break; // Radish
	case 7: GetExp(sAttackerH, iDice(6, 100) * 100); iItemID = 826; break; // Corn
	case 8: GetExp(sAttackerH, iDice(6, 100) * 100); iItemID = 827; break; // ChineseBellflower
	case 9: GetExp(sAttackerH, iDice(7, 100) * 100); iItemID = 828; break; // Melone
	case 10: GetExp(sAttackerH, iDice(7, 100) * 100); iItemID = 829; break; // Tommato
	case 11: GetExp(sAttackerH, iDice(8, 100) * 100); iItemID = 830; break; // Grapes
	case 12: GetExp(sAttackerH, iDice(8, 100) * 100); iItemID = 831; break; // BlueGrapes
	case 13: GetExp(sAttackerH, iDice(9, 100) * 100); iItemID = 832; break; // Mushroom
	default: GetExp(sAttackerH, iDice(1, 100) * 100); iItemID = 721; break; // Ginseng

	}

	pItem = new class CItem;
	if (_bInitItemAttr(pItem, iItemID) == false) {
		delete pItem;
	}
	if (bType == 0) {
		m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->bSetItem(m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY, pItem);
		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[sAttackerH]->m_cMapIndex,
			m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY, pItem->m_sSprite,
			pItem->m_sSpriteFrame, pItem->m_cItemColor, false);*/

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[sAttackerH]->m_cMapIndex,
			m_pClientList[sAttackerH]->m_sX, m_pClientList[sAttackerH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
	}
	else if (bType == 1) {
		m_pMapList[m_pNpcList[sTargetH]->m_cMapIndex]->bSetItem(m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY, pItem);
		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[sTargetH]->m_cMapIndex,
			m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY, pItem->m_sSprite,
			pItem->m_sSpriteFrame, pItem->m_cItemColor, false);*/
		
		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[sTargetH]->m_cMapIndex,
			m_pNpcList[sTargetH]->m_sX, m_pNpcList[sTargetH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
	}

}

//LifeX Auto Skills
void CGame::AutoSkill(int iClientH)
{
	int i;

	if (m_pClientList[iClientH] == 0) return;

	for (i = 0; i < 24; i++)
	{
		m_pClientList[iClientH]->m_cSkillMastery[i] = 100;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, i, m_pClientList[iClientH]->m_cSkillMastery[i], 0, 0);
	}
	//m_pClientList[iClientH]->m_cMagicMastery[12] = true;
}

void CGame::bCheckTotalSkillMasteryPoints(int iClientH, int iSkill)
{
	int i;
	int iRemainPoint, iTotalPoints, iWeaponIndex, iDownSkillSSN, iDownPoint;
	short sDownSkillIndex;

	if (m_pClientList[iClientH] == 0) return;

	iTotalPoints = 0;
	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		iTotalPoints += m_pClientList[iClientH]->m_cSkillMastery[i];

	iRemainPoint = iTotalPoints - m_sCharSkillLimit;

	while (iRemainPoint > 0) {

		sDownSkillIndex = -1;
		if ((m_pClientList[iClientH]->m_iDownSkillIndex != -1)
			&& (m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_iDownSkillIndex] > 0))
		{
			sDownSkillIndex = m_pClientList[iClientH]->m_iDownSkillIndex;
		}
		else
		{
			iDownSkillSSN = 99999999;
			for (i = 0; i < DEF_MAXSKILLTYPE; i++)
			{
				if ((m_pClientList[iClientH]->m_cSkillMastery[i] >= 21) && (i != iSkill)
					&& (m_pClientList[iClientH]->m_iSkillSSN[i] <= iDownSkillSSN))
				{
					iDownSkillSSN = m_pClientList[iClientH]->m_iSkillSSN[i];
					m_pClientList[iClientH]->m_iDownSkillIndex = i; // Snoopy: impose a downskill index !
					sDownSkillIndex = i; // returns the skill further from next up, over 20...
				}
			}
		}

		if (sDownSkillIndex != -1)
		{
			if (m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] <= 20) // v1.4
				iDownPoint = m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex];
			else iDownPoint = 1;
			
			m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] -= iDownPoint; // v1.4
			m_pClientList[iClientH]->m_iSkillSSN[sDownSkillIndex] = m_iSkillSSNpoint[m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex] + 1] - 1;
			
			iRemainPoint -= iDownPoint; // v1.4				
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
			{
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
				{
					m_pClientList[iClientH]->m_iHitRatio -= iDownPoint; // v1.4
					if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
				}
			}
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) {
				iWeaponIndex = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND];
				if (m_pClientList[iClientH]->m_pItemList[iWeaponIndex]->m_sRelatedSkill == sDownSkillIndex)
				{
					m_pClientList[iClientH]->m_iHitRatio -= iDownPoint; // v1.4
					if (m_pClientList[iClientH]->m_iHitRatio < 0) m_pClientList[iClientH]->m_iHitRatio = 0;
				}
			}
			//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sDownSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex], 0, 0);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_SKILL, sDownSkillIndex, m_pClientList[iClientH]->m_cSkillMastery[sDownSkillIndex], m_pClientList[iClientH]->m_iSkillSSN[sDownSkillIndex], 0);
		}
		else 
		{
			return;
		}
	}
}

// 05/24/2004 - Hypnotoad - Hammer and Wand train to 100% fixed
void CGame::_CheckAttackType(int iClientH, short* spType)
{
	WORD wType;

	if (m_pClientList[iClientH] == 0) return;
	wType = ((m_pClientList[iClientH]->m_sAppr2 & 0x0FF0) >> 4);

	switch (*spType) 
	{
	case 2:
		if (m_pClientList[iClientH]->m_cArrowIndex == -1) *spType = 0;
		if (wType < 40) *spType = 1;
		break;

	case 20:
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[5] < 100) *spType = 1;
		break;

	case 21:
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[7] < 100) *spType = 1;
		break;

	case 22:
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[9] < 100) *spType = 1;
		break;

	case 23:
		// 脙鈧兟ッ偮懊冣€?脙鈥∶兣犆偮幻兟偮泵兟?
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)   *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[8] < 100) *spType = 1;
		break;

	case 24:
		// 脗碌脗碌脗鲁脗垄 脙鈥∶兣犆偮幻兟偮泵兟?
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[10] < 100) *spType = 1;
		break;

	case 25:
		// 脙藛脗掳 脙鈥∶兣犆偮幻兟偮泵兟?
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 2;
		if (m_pClientList[iClientH]->m_cSkillMastery[6] < 100) *spType = 2;
		if (m_pClientList[iClientH]->m_cArrowIndex == -1)      *spType = 0;
		// 脙藛脗掳 脗掳脙赂脗掳脙聺脙鈧兣矫偮得偮?脙鈧兟ッ冣€毭兟该冣€∶冣€?脗鹿脗芦脗卤脙垄脗掳脗隆 脙藛脗掳脙鈧兣?脗戮脙鈥犆偮疵兟徝偮疵冣劉. 脙鈧兟徝偮姑兟?脗掳脙赂脗掳脙聺脙鈧偮该偮访兣?脙鈧兟济兯喢偮?
		if (wType < 40) *spType = 1;
		break;

	case 26:
		// v2.16 2002-5-27 脟脴赂脫 脟脢禄矛卤芒 
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[14] < 100) *spType = 1;
		break;

	case 27:
		// v2.16 2002-5-27 脕枚脝脦脌脤 脟脢禄矛卤芒 
		if (m_pClientList[iClientH]->m_iSuperAttackLeft <= 0)  *spType = 1;
		if (m_pClientList[iClientH]->m_cSkillMastery[21] < 100) *spType = 1;
		break;
	}
}