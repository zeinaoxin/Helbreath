// Npc.cpp: implementation of the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "Npc.h"
#include "Game.h"

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

CNpc::CNpc(char * pName5)
{
 int i;
	ZeroMemory(m_cName, sizeof(m_cName));
	memcpy(m_cName, pName5, 5);
	
	for (i = 0; i < DEF_MAXWAYPOINTS; i++)			  
		m_iWayPointIndex[i] = -1;
	
	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++) 
		m_cMagicEffectStatus[i]	= 0;

	m_bIsSummoned       = false;
	m_bIsPermAttackMode = false;

	m_dwRegenTime = 0;
	m_bIsKilled   = false;

	m_sOriginalType      = 0;
	m_iSummonControlMode = 0;
	
	m_cAttribute = 0;
	m_iAbsDamage = 0;
	m_iStatus    = 0;
	m_sAppr2     = 0;

	m_iAttackRange    = 1;
	m_cSpecialAbility = 0;
	m_sAreaSize = 0;

	m_iExp = 0;

	m_iSummonSlave = 0;

	m_cPoisonResistance = 110;
	m_iPoisonLevel = 0;

	m_iBuildCount = 0;
	m_iManaStock  = 0;
	m_bIsUnsummoned = false;
	m_cCropType = 0;
	m_cCropSkill = 0;

	m_bIsMaster  = false;
	m_iGuildGUID = 0;
	m_iV1 = 0;

	m_iNpcItemType = 0;
	m_iNpcItemMax = 0;
	m_bIsBossMob = false;
}

CNpc::~CNpc()
{

}

// 2002-09-06 #1
CNpcItem::CNpcItem()
{
    ZeroMemory(m_cName, sizeof(m_cName));
    m_sItemID = 0;          // 2002-09-09 #1
    m_sFirstProbability = 0;
    m_cFirstTargetValue = 0;
    m_sSecondProbability = 0;
    m_cSecondTargetValue = 0;
}

void CGame::TargetSearch(int iNpcH, short* pTarget, char* pTargetType)
{
	int ix, iy, iPKCount;
	short sX, sY, rX, rY, dX, dY;
	short sOwner, sTargetOwner, sDistance, sTempDistance;
	char  cOwnerType, cTargetType, cTargetSide;
	int   iInv;

	sTargetOwner = 0;
	cTargetType = 0;
	sDistance = 100;

	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;

	// 脌脦碌娄陆潞 iNpcH驴隆 脟脴麓莽脟脧麓脗 NPC掳隆 掳酶掳脻麓毛禄贸脌禄 脙拢麓脗麓脵. 
	rX = m_pNpcList[iNpcH]->m_sX - m_pNpcList[iNpcH]->m_cTargetSearchRange;
	rY = m_pNpcList[iNpcH]->m_sY - m_pNpcList[iNpcH]->m_cTargetSearchRange;

	for (ix = rX; ix < rX + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; ix++)
		for (iy = rY; iy < rY + m_pNpcList[iNpcH]->m_cTargetSearchRange * 2 + 1; iy++) {

			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
			if (sOwner != 0) {
				// 鹿芦戮冒掳隆 鹿脽掳脽碌脟戮煤麓脵. 
				if ((sOwner == iNpcH) && (cOwnerType == DEF_OWNERTYPE_NPC)) break; // 脌脷卤芒 脌脷陆脜脌脤赂茅 鹿芦陆脙 

				// 赂帽脟楼 录鲁脕陇
				// 脌搂脛隆赂娄 卤赂脟脩麓脵.
				iPKCount = 0;
				switch (cOwnerType) {
				case DEF_OWNERTYPE_PLAYER:
					if (m_pClientList[sOwner] == 0) {
						// 脌脽赂酶碌脠 脌脦碌娄陆潞赂娄 掳隆脕酶 掳陋脌脤麓脵. 脜卢赂庐戮卯脟脩麓脵.	
						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(5, sOwner, DEF_OWNERTYPE_PLAYER, ix, iy);
					}
					else {
						if (m_pClientList[sOwner]->m_iAdminUserLevel > 0) goto SKIP_SEARCH;

						dX = m_pClientList[sOwner]->m_sX;
						dY = m_pClientList[sOwner]->m_sY;
						cTargetSide = m_pClientList[sOwner]->m_cSide;
						iPKCount = m_pClientList[sOwner]->m_iPKCount;
						iInv = m_pClientList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];
					}
					break;

				case DEF_OWNERTYPE_NPC:
					if (m_pNpcList[sOwner] == 0) {
						// 脌脽赂酶碌脠 脌脦碌娄陆潞赂娄 掳隆脕酶 掳陋脌脤麓脵. 脜卢赂庐戮卯脟脩麓脵.	
						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(6, sOwner, DEF_OWNERTYPE_NPC, ix, iy);
					}
					else {
						dX = m_pNpcList[sOwner]->m_sX;
						dY = m_pNpcList[sOwner]->m_sY;
						cTargetSide = m_pNpcList[sOwner]->m_cSide;
						iPKCount = 0;
						iInv = m_pNpcList[sOwner]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY];

						if (m_pNpcList[iNpcH]->m_sType == 21) {
							// 赂赂戮脿 掳忙潞帽掳隆 NPC赂娄 赂帽脟楼鹿掳脌禄 脙拢麓脗 掳脥脌脤露贸赂茅 NPC脕脰脌搂驴隆 脟脙路鹿脌脤戮卯掳隆 赂鹿脌脤 脌脰麓脵赂茅 掳酶掳脻脟脧脕枚 戮脢麓脗麓脵. 
							if (_iCalcPlayerNum(m_pNpcList[sOwner]->m_cMapIndex, dX, dY, 2) != 0) {
								sOwner = 0;
								cOwnerType = 0;
							}
						}
					}
					break;
				}

				if (m_pNpcList[iNpcH]->m_cSide < 10) {
					// NPC脌脟 Side掳隆 10潞赂麓脵 脌脹麓脵麓脗 掳脥脌潞 NPC掳隆 脟脩 赂露脌禄驴隆 录脪录脫碌脟戮煤麓脵麓脗 露忙.  Ex: Guard 脠陇脌潞 录脪脠炉赂梅  
					if (cTargetSide == 0) {
						// 赂帽脟楼鹿掳脌脤 脕脽赂鲁脌脤麓脵. PK掳隆 戮脝麓脧露贸赂茅 掳酶掳脻脟脧脕枚 戮脢麓脗麓脵. 
						if (iPKCount == 0) goto SKIP_SEARCH;
					}
					else {
						// 赂帽脟楼鹿掳脌脤 赂露脌禄 录脪录脫脌脤麓脵. Side掳隆 麓脵赂拢麓脵赂茅 掳酶掳脻脟脩麓脵. v1.12
						if ((iPKCount == 0) && (cTargetSide == m_pNpcList[iNpcH]->m_cSide)) goto SKIP_SEARCH;
						// 脕脽赂鲁赂梅脌潞 赂露脌禄 录脪录脫 赂帽脟楼鹿掳脌禄 掳酶掳脻脟脧脕枚 戮脢麓脗麓脵. v1.12
						if (m_pNpcList[iNpcH]->m_cSide == 0) goto SKIP_SEARCH;
					}
				}
				else {
					// 脌脧鹿脻 赂贸陆潞脜脥碌茅脌脤麓脵. 脌脷卤芒驴脥 Side掳隆 掳掳脌赂赂茅 鹿芦陆脙.
					if ((cOwnerType == DEF_OWNERTYPE_NPC) && (cTargetSide == 0)) goto SKIP_SEARCH;
					if (cTargetSide == m_pNpcList[iNpcH]->m_cSide) goto SKIP_SEARCH;
				}

				if (m_pNpcList[iNpcH]->m_sType == 99)
				{
					if (abs(sX - dX) >= abs(sY - dY))
						sTempDistance = abs(sX - dX);
					else sTempDistance = abs(sY - dY);

					if (sTempDistance < sDistance) {
						sDistance = sTempDistance;
						sTargetOwner = sOwner;
						cTargetType = cOwnerType;
					}
				}

				// 赂赂戮脿 INVISIBILITY掳隆 碌脟戮卯 脌脰掳铆 脜玫赂铆 脜陆脕枚 麓脡路脗脌脤 戮酶麓脵赂茅 
				if ((iInv != 0) && (m_pNpcList[iNpcH]->m_cSpecialAbility != 1)) goto SKIP_SEARCH;

				if (abs(sX - dX) >= abs(sY - dY))
					sTempDistance = abs(sX - dX);
				else sTempDistance = abs(sY - dY);

				if (sTempDistance < sDistance) {
					sDistance = sTempDistance;
					sTargetOwner = sOwner;
					cTargetType = cOwnerType;
				}
			SKIP_SEARCH:;
			}
		}

	*pTarget = sTargetOwner;
	*pTargetType = cTargetType;
}

void CGame::RemoveFromTarget(short sTargetH, char cTargetType, int iCode, int iAttackerH, char cAttackerType)
// Used into deleteclient(), ClientKilled(), deleteNpc(), npcKilled(), invisibility, Teleport
{
	int i;
	//renew crusade summons duration	
	// Somebody from guild invied => renew summon duration !
	//  "   "     "     "  killed/teleported/deco 
	if ((m_bIsCrusadeMode == true) && (cTargetType == DEF_OWNERTYPE_PLAYER))
	{
		DWORD dwTime = timeGetTime();
		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != 0)
			{
				if ((m_pNpcList[i]->m_iGuildGUID != 0)
					&& (m_pClientList[sTargetH]->m_iGuildGUID == m_pNpcList[i]->m_iGuildGUID))
				{
					if (m_pNpcList[i]->m_cActionLimit < 2)
					{
						m_pNpcList[i]->m_bIsSummoned = true;
						m_pNpcList[i]->m_dwSummonedTime = dwTime;
					}
				}
			}
	}

	// If master was killed or deco set follower mode
	if ((cTargetType == DEF_OWNERTYPE_PLAYER) && (iCode == 0))// not DEF_MAGICTYPE_INVISIBILITY neither DEF_MAGICTYPE_TELEPORT
	{
		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != 0)
			{
				if ((m_pNpcList[i]->m_bIsSummoned == true)
					&& (m_pNpcList[i]->m_bIsKilled == false)
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
					&& (m_pNpcList[i]->m_iFollowOwnerIndex == sTargetH))
				{	// Client suposed to be disconnected and not dead.
					if ((m_pClientList[sTargetH] != 0)
						&& (m_pClientList[sTargetH]->m_bIsKilled == false))
					{	// All Summons will live only 1 minute in ths case
						// other summons will live for standard time				
						m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
						m_pNpcList[i]->m_iFollowOwnerIndex = 0;
						m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
					}
					// Client is dead
					else if ((m_bIsHeldenianMode == true)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != 0)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						switch (m_cHeldenianSummonMode) {
						case 0: // Mode 0, default, Kill the summon
							NpcKilledHandler(0, 0, i, 0);
							break;
						case 1: // Mode 1: Summon will wait for his own master	
							m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
							m_pNpcList[i]->m_iFollowOwnerIndex = 0;
							m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
							m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
							// Summon tries to avenge his master
							if ((iAttackerH != 0) && (cAttackerType != 0))
							{
								m_pNpcList[i]->m_iSummonControlMode = 0;
								m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
								m_pNpcList[i]->m_sBehaviorTurnCount = 0;
								m_pNpcList[i]->m_iTargetIndex = iAttackerH;
								m_pNpcList[i]->m_cTargetType = cAttackerType;
								m_pNpcList[i]->m_bIsPermAttackMode = true;
							}
							break;
						case 2: // Mode 3: Summon can be taken over by anybody else
							m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_FREE;
							m_pNpcList[i]->m_iFollowOwnerIndex = 0;
							ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
							// Summon tries to avenge his master
							if ((iAttackerH != 0) && (cAttackerType != 0))
							{
								m_pNpcList[i]->m_iSummonControlMode = 0;
								m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
								m_pNpcList[i]->m_sBehaviorTurnCount = 0;
								m_pNpcList[i]->m_iTargetIndex = iAttackerH;
								m_pNpcList[i]->m_cTargetType = cAttackerType;
								m_pNpcList[i]->m_bIsPermAttackMode = true;
							}
							break;
						}
					}
					else //Dead client, not Heldenian
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;	// No regain control possible
						m_pNpcList[i]->m_iFollowOwnerIndex = 0;
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
						// Summon tries to avenge his master
						if ((iAttackerH != 0) && (cAttackerType != 0))
						{
							m_pNpcList[i]->m_iSummonControlMode = 0;
							m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
							m_pNpcList[i]->m_sBehaviorTurnCount = 0;
							m_pNpcList[i]->m_iTargetIndex = iAttackerH;
							m_pNpcList[i]->m_cTargetType = cAttackerType;
							m_pNpcList[i]->m_bIsPermAttackMode = true;
						}
					}
				}
				// Tamed mobs
				else if ((m_pNpcList[i]->m_dwTamingTime != 0) // For tamed mobs
					&& (m_pNpcList[i]->m_bIsSummoned == false)
					&& (m_pNpcList[i]->m_bIsKilled == false)
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
					&& (m_pNpcList[i]->m_iFollowOwnerIndex == sTargetH))
				{	// Client suposed to be disconnected and not dead.
					if ((m_pClientList[sTargetH] != 0)
						&& (m_pClientList[sTargetH]->m_bIsKilled == false))
					{	// All Summons will live only 1 minute in ths case
						// other summons will live for standard time				
						m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_WAITING;
						m_pNpcList[i]->m_iFollowOwnerIndex = 0;
						m_pNpcList[i]->m_dwSummonWaitTime = timeGetTime();
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
					}
					else// Client is dead
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;	// No regain control possible
						m_pNpcList[i]->m_iFollowOwnerIndex = 0;
						m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOM;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
						ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
						// Summon tries to avenge his master
						if ((iAttackerH != 0) && (cAttackerType != 0))
						{
							m_pNpcList[i]->m_iSummonControlMode = 0;
							m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
							m_pNpcList[i]->m_sBehaviorTurnCount = 0;
							m_pNpcList[i]->m_iTargetIndex = iAttackerH;
							m_pNpcList[i]->m_cTargetType = cAttackerType;
							m_pNpcList[i]->m_bIsPermAttackMode = true;
						}
					}
				}
			}
	}

	// Stop attack the disapeared/dead/teleported/deco target
	for (i = 0; i < DEF_MAXNPCS; i++)
		if (m_pNpcList[i] != 0)
		{
			if ((m_pNpcList[i]->m_iTargetIndex == sTargetH)
				&& (m_pNpcList[i]->m_cTargetType == cTargetType))
			{
				switch (iCode) {
				case DEF_MAGICTYPE_INVISIBILITY:
					if (m_pNpcList[i]->m_sType != 99)
					{
						if (m_pNpcList[i]->m_cSpecialAbility != 1)
						{
							m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
							m_pNpcList[i]->m_iTargetIndex = 0;
							m_pNpcList[i]->m_cTargetType = 0;
						}
					}
					break;
				case DEF_MAGICTYPE_TELEPORT:
				default:
					m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
					m_pNpcList[i]->m_iTargetIndex = 0;
					m_pNpcList[i]->m_cTargetType = 0;
					break;
				}
			}
		}
}

void CGame::NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)
{
	short  sAttackerWeapon;
	int* ip, i, iQuestIndex, iConstructionPoint, iWarContribution, iMapIndex, j;
	double dTmp1, dTmp2, dTmp3;
	char* cp, cData[120], cTargetName[21];
	DWORD iExp;

	if (m_pNpcList[iNpcH] == 0) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

	m_pNpcList[iNpcH]->m_bIsKilled = true;
	m_pNpcList[iNpcH]->m_iHP = 0;
	m_pNpcList[iNpcH]->m_iLastDamage = sDamage;

	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

	RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);
	ReleaseFollowMode(iNpcH);

	minimap_clear_apoc(iNpcH);

	m_pNpcList[iNpcH]->m_cFollowOwnerType = 0; // Snoopy's add
	m_pNpcList[iNpcH]->m_iFollowOwnerIndex = 0; // Snoopy's add

	m_pNpcList[iNpcH]->m_iTargetIndex = 0;
	m_pNpcList[iNpcH]->m_cTargetType = 0;
	if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		sAttackerWeapon = ((m_pClientList[sAttackerH]->m_sAppr2 & 0x0FF0) >> 4);
	}
	else sAttackerWeapon = 1;
	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, sDamage, sAttackerWeapon, 0);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pNpcList[iNpcH]->m_cBehavior = DEF_BEHAVIOR_DEAD;

	m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_cType == DEF_MAPTYPE_NOPENALTY_NOREWARD) return;

	NpcDeadItemGenerator(iNpcH, sAttackerH, cAttackerType);
	if ((m_pNpcList[iNpcH]->m_bIsSummoned != true) && (cAttackerType == DEF_OWNERTYPE_PLAYER) &&
		(m_pClientList[sAttackerH] != 0)) {
		iExp = (m_pNpcList[iNpcH]->m_iExp / 3);
		if (m_pNpcList[iNpcH]->m_iNoDieRemainExp > 0)
			iExp += m_pNpcList[iNpcH]->m_iNoDieRemainExp;

		if (m_pClientList[sAttackerH]->m_iAddExp != 0) {
			dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddExp;
			dTmp2 = (double)iExp;
			dTmp3 = (dTmp1 / 100.0f) * dTmp2;
			iExp += (int)dTmp3;
		}

		if (m_pNpcList[iNpcH]->m_sType == 99) {
			
			// Centu - end apocalypse
			DWORD dwTime = timeGetTime();
			dwTime += 1000 * 60 * 5; 
			bRegisterDelayEvent(DEF_DELAYEVENTTYPE_END_APOCALYPSE, 0, dwTime, 0
				, 0, m_pNpcList[iNpcH]->m_cMapIndex, 0, 0, 0, 0, 0);

			/*for (i = 1; i < DEF_MAXCLIENTS; i++) {
				if (m_pClientList[i] != 0) {
					SendNotifyMsg(sAttackerH, i, DEF_NOTIFY_ABADDONKILLED, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
				}
			}*/
		}

		if (m_bIsCrusadeMode == true || m_bIsHeldenianMode == true) {
			if (iExp > 10) iExp = iExp / 3;
		}

		// New 09/05/2004
		GetExp(sAttackerH, iExp, true);

		for (j = 0; j < DEF_MAXQUEST; j++) { // Magn0S:: Multi Quest
			if (m_pClientList[sAttackerH]->m_iQuest[j] != 0) {
				iQuestIndex = m_pClientList[sAttackerH]->m_iQuest[j];
				if (iQuestIndex != 0) {
					if (m_pQuestConfigList[iQuestIndex] != 0) {
						switch (m_pQuestConfigList[iQuestIndex]->m_iType) {
						case DEF_QUESTTYPE_MONSTERHUNT:
							//if ((m_pClientList[sAttackerH]->m_bQuestMatchFlag_Loc == true) &&
							//	(m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType))
							//{
							ZeroMemory(cTargetName, sizeof(cTargetName));
							memcpy(cTargetName, m_pQuestConfigList[iQuestIndex]->m_cTargetName, 20);
							if ((memcmp(m_pMapList[m_pClientList[sAttackerH]->m_cMapIndex]->m_cName, cTargetName, 10) == 0) && 
								(m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_pNpcList[iNpcH]->m_sType)) {
								m_pClientList[sAttackerH]->m_iCurQuestCount[j]++;
								//cQuestRemain = (m_pQuestConfigList[m_pClientList[sAttackerH]->m_iQuest[j]]->m_iMaxCount - m_pClientList[sAttackerH]->m_iCurQuestCount[j]);
								//SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_QUESTCOUNTER, j, cQuestRemain, 0, 0);
								SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_QUESTCOUNTER, j, m_pClientList[sAttackerH]->m_iCurQuestCount[j], 0, 0);
								_bCheckIsQuestCompleted(sAttackerH, j);
							}
							break;
						}
					}
				}
			}
		}
	}

	//Magn0S:: Removed.
	/*if (cAttackerType == DEF_OWNERTYPE_PLAYER) {
		switch (m_pNpcList[iNpcH]->m_sType) {
		case 32:
			m_pClientList[sAttackerH]->m_iRating -= 5;
			// MORLA 2.4 - Actualizo la rep
			SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_REPDGDEATHS, m_pClientList[sAttackerH]->m_iDGPoints, m_pClientList[sAttackerH]->m_iTotalDGKills, m_pClientList[sAttackerH]->m_iRating, 0);
			break;

		case 33:
			break;
		}
	}*/

	if (m_pNpcList[iNpcH]->m_sType == 91) // Remove bloqued area
	{
		for (short sx1 = (m_pNpcList[iNpcH]->m_sX - 1); sx1 <= m_pNpcList[iNpcH]->m_sX + 1; sx1++)
			for (short sy1 = (m_pNpcList[iNpcH]->m_sY - 1); sy1 <= m_pNpcList[iNpcH]->m_sY + 1; sy1++)
			{
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetTempMoveAllowedFlag(sx1, sy1, true);
			}
	}

	if (m_bIsCrusadeMode == true) {
		// Crusade
		iConstructionPoint = 0;
		switch (m_pNpcList[iNpcH]->m_sType) {
		case 1:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 2:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 3:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 4:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 5:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 6:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 36: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 37: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 38: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 39: iConstructionPoint = 500; iWarContribution = 2000; break;
		case 40: iConstructionPoint = 1500; iWarContribution = 5000; break;
		case 41: iConstructionPoint = 5000; iWarContribution = 10000; break;
		case 43: iConstructionPoint = 500; iWarContribution = 1000; break;
		case 44: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 45: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 46: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 47: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 64: m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum(); break;

		}

		// Crusade
		if (iConstructionPoint != 0) {
			switch (cAttackerType) {
			case DEF_OWNERTYPE_PLAYER:
				if (m_pClientList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide) {
					m_pClientList[sAttackerH]->m_iConstructionPoint += iConstructionPoint;
					m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
					//wsprintf(G_cTxt, "Enemy Npc Killed by player! Construction: +%d WarContribution: +%d", iConstructionPoint, iWarContribution);
					//PutLogList(G_cTxt);

					SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
				}
				else {
					m_pClientList[sAttackerH]->m_iWarContribution -= (iWarContribution * 2);
					if (m_pClientList[sAttackerH]->m_iWarContribution < 0)
						m_pClientList[sAttackerH]->m_iWarContribution = 0;
					//wsprintf(G_cTxt, "Friendly Npc Killed by player! WarContribution: -%d", iWarContribution);
					//PutLogList(G_cTxt);

					SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[sAttackerH]->m_iConstructionPoint, m_pClientList[sAttackerH]->m_iWarContribution, 0, 0);
				}
				break;

			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sAttackerH]->m_iGuildGUID != 0) {
					if (m_pNpcList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide) {
						for (i = 0; i < DEF_MAXCLIENTS; i++)
							if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_iGuildGUID == m_pNpcList[sAttackerH]->m_iGuildGUID) &&
								(m_pClientList[i]->m_iCrusadeDuty == 3)) {

								m_pClientList[i]->m_iConstructionPoint += iConstructionPoint;
								//wsprintf(G_cTxt, "Enemy Npc Killed by Npc! Construct point +%d", iConstructionPoint);
								//PutLogList(G_cTxt);
								SendNotifyMsg(0, i, DEF_NOTIFY_CONSTRUCTIONPOINT, m_pClientList[i]->m_iConstructionPoint, m_pClientList[i]->m_iWarContribution, 0, 0);
								goto NKH_GOTOPOINT1;
							}

						ZeroMemory(cData, sizeof(cData));
						cp = (char*)cData;
						*cp = GSM_CONSTRUCTIONPOINT;
						cp++;
						ip = (int*)cp;
						*ip = m_pNpcList[sAttackerH]->m_iGuildGUID;
						cp += 4;
						ip = (int*)cp;
						*ip = iConstructionPoint;
						cp += 4;
						bStockMsgToGateServer(cData, 9);
					}
				}
				break;
			}
		}
	}

	// new - heldenian
	else if (m_bIsHeldenianMode == true) {
		if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_cSide != m_pNpcList[iNpcH]->m_cSide)) {
			switch (m_pNpcList[iNpcH]->m_sType) {
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
				iWarContribution = 1000;
				break;
			}
			if (iWarContribution > 0) m_pClientList[sAttackerH]->m_iWarContribution += iWarContribution;
			SendNotifyMsg(0, sAttackerH, DEF_NOTIFY_CONSTRUCTIONPOINT, 0, m_pClientList[sAttackerH]->m_iWarContribution, 1, 0, 0, 0);
		}
	}

NKH_GOTOPOINT1:;

	// v1.411 赂赂戮脿 Explosive脟脩 赂贸陆潞脜脥掳隆 脕脳脌潞 掳脜露贸赂茅 Explosive

	if (m_pNpcList[iNpcH]->m_sType == 99)
	{
		m_pNpcList[iNpcH]->m_iMana = 100;
		m_pNpcList[iNpcH]->m_iMagicHitRatio = 1000;
		NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 61);
		for (i = 1; i <= 5; i++) 
		{
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX - i, m_pNpcList[iNpcH]->m_sY, 61); // 1
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX - i, m_pNpcList[iNpcH]->m_sY - i, 61); // 2
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY - i, 61); // 3
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX + i, m_pNpcList[iNpcH]->m_sY - i, 61); // 4
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX + i, m_pNpcList[iNpcH]->m_sY, 61); // 5
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX + i, m_pNpcList[iNpcH]->m_sY + i, 61); // 6
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY + i, 61); // 7
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX - i, m_pNpcList[iNpcH]->m_sY + i, 61); // 8
		}
	}
	else {
		if (m_pNpcList[iNpcH]->m_cSpecialAbility == 7) {
			m_pNpcList[iNpcH]->m_iMana = 100;
			m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 30);
		}
		else if (m_pNpcList[iNpcH]->m_cSpecialAbility == 8) {
			m_pNpcList[iNpcH]->m_iMana = 100;
			m_pNpcList[iNpcH]->m_iMagicHitRatio = 100;
			NpcMagicHandler(iNpcH, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, 61);
		}
	}
	// new spawns boss when no monsters exist and apocalypse zone == 2
	if ((m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject == 0) &&
		(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 2)) {
		MobBossGenerator(m_pNpcList[iNpcH]->m_cMapIndex);
	}
	// if boss mob is killed and zone is RecallImpossible, recall all people inside place
	if (m_pNpcList[iNpcH]->m_bIsBossMob == true) {
		iMapIndex = 0;
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			iMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;
		if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsInitComplete == true) && (iMapIndex >= 0) && (iMapIndex < DEF_MAXMAPS) && (m_pMapList[iMapIndex] != 0) && (m_pMapList[iMapIndex]->m_bIsRecallImpossible == true)) {
			m_pClientList[i]->m_iTimeLeft_ForceRecall = 200;
			m_pClientList[i]->m_bIsImpossibleZone = true;
			SendNotifyMsg(0, i, DEF_NOTIFY_FORCERECALLTIME, m_pClientList[i]->m_iTimeLeft_ForceRecall, 0, 0, 0);
		}
	}
	if ((m_bIsHeldenianMode == true) && (m_cHeldenianType == 1)
		&& (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_bIsHeldenianMap == true))
	{
		if ((m_pNpcList[iNpcH]->m_sType == 87) || (m_pNpcList[iNpcH]->m_sType == 89))
		{
			if (m_pNpcList[iNpcH]->m_cSide == 1)
			{
				m_iHeldenianAresdenLeftTower--;
				if (m_cHeldenianFirstDestroyedTowerWinner == 0)  m_cHeldenianFirstDestroyedTowerWinner = 2;

				UpdateHeldenianStatus(-1);
				if (m_iHeldenianAresdenLeftTower == 0)	HeldenianVictoryNow(2);
			}
			else if (m_pNpcList[iNpcH]->m_cSide == 2)
			{
				m_iHeldenianElvineLeftTower--;
				if (m_cHeldenianFirstDestroyedTowerWinner == 0)  m_cHeldenianFirstDestroyedTowerWinner = 1;

				UpdateHeldenianStatus(-1);
				if (m_iHeldenianElvineLeftTower == 0)	HeldenianVictoryNow(1);
			}
		}
	}

	if (m_bIsApocalypseMode) {
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (m_pClientList[i]->m_bIsOnApocMap == true) && (m_pClientList[i]->m_bIsInitComplete == true)) {
				int iMonsterCount;
				iMonsterCount = m_pMapList[m_pClientList[i]->m_cMapIndex]->m_iTotalAliveObject;
				SendNotifyMsg(0, i, DEF_NOTIFY_MONSTERCOUNTAPOC, iMonsterCount, 0, 0, 0);
			}
	}
}

bool CGame::_bInitNpcAttr(class CNpc* pNpc, char* pNpcName, short sClass, char cSA)
{
	int i, iTemp;
	char cTmpName[21];
	int sTemp;
	double dV1, dV2, dV3;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pNpcName);

	for (i = 0; i < DEF_MAXNPCTYPES; i++)
		if (m_pNpcConfigList[i] != 0) {
			if (memcmp(cTmpName, m_pNpcConfigList[i]->m_cNpcName, 20) == 0) {
				// 掳掳脌潞 脌脤赂搂脌禄 掳隆脕酶 NPC 录鲁脕陇脌禄 脙拢戮脪麓脵. 录鲁脕陇掳陋脌赂路脦 脙脢卤芒脠颅脟脩麓脵.
				ZeroMemory(pNpc->m_cNpcName, sizeof(pNpc->m_cNpcName));
				memcpy(pNpc->m_cNpcName, m_pNpcConfigList[i]->m_cNpcName, 20);

				pNpc->m_sType = m_pNpcConfigList[i]->m_sType;

				// HitDice驴隆 碌没露贸 掳酶陆脛脌脤 麓脵赂拢麓脵. 赂梅掳拢脌脟 脗梅潞掳脌禄 碌脦卤芒 脌搂脟脭.
				if (m_pNpcConfigList[i]->m_iHitDice <= 5)
					pNpc->m_iHP = abs(iDice(m_pNpcConfigList[i]->m_iHitDice, 4) + m_pNpcConfigList[i]->m_iHitDice);
				else pNpc->m_iHP = abs((m_pNpcConfigList[i]->m_iHitDice * 4) + m_pNpcConfigList[i]->m_iHitDice + iDice(1, m_pNpcConfigList[i]->m_iHitDice));
				// v1.4 脠庐脌脦脛脷碌氓
				if (pNpc->m_iHP == 0) pNpc->m_iHP = 1;

				//50Cent - HP Bar
				pNpc->m_iMaxHP = pNpc->m_iHP;

				pNpc->m_iExpDiceMin = m_pNpcConfigList[i]->m_iExpDiceMin;
				pNpc->m_iExpDiceMax = m_pNpcConfigList[i]->m_iExpDiceMax;
				pNpc->m_iGoldDiceMin = m_pNpcConfigList[i]->m_iGoldDiceMin;
				pNpc->m_iGoldDiceMax = m_pNpcConfigList[i]->m_iGoldDiceMax;
				pNpc->m_iExp = (iDice(1, (m_pNpcConfigList[i]->m_iExpDiceMax - m_pNpcConfigList[i]->m_iExpDiceMin)) + m_pNpcConfigList[i]->m_iExpDiceMin);

				pNpc->m_iHitDice = m_pNpcConfigList[i]->m_iHitDice;
				pNpc->m_iDefenseRatio = m_pNpcConfigList[i]->m_iDefenseRatio;
				pNpc->m_iHitRatio = m_pNpcConfigList[i]->m_iHitRatio;
				pNpc->m_iMinBravery = m_pNpcConfigList[i]->m_iMinBravery;
				pNpc->m_cAttackDiceThrow = m_pNpcConfigList[i]->m_cAttackDiceThrow;
				pNpc->m_cAttackDiceRange = m_pNpcConfigList[i]->m_cAttackDiceRange;
				pNpc->m_cSize = m_pNpcConfigList[i]->m_cSize;
				pNpc->m_cSide = m_pNpcConfigList[i]->m_cSide;

				pNpc->m_dwTamingTime = 0; // Taming
				pNpc->m_cOriginalSide = m_pNpcConfigList[i]->m_cSide;

				// Npc Poison Resistance
				pNpc->m_cPoisonResistance = m_pNpcConfigList[i]->m_cResistMagic;
				pNpc->m_iPoisonLevel = 0;

				pNpc->m_cActionLimit = m_pNpcConfigList[i]->m_cActionLimit;
				pNpc->m_dwActionTime = m_pNpcConfigList[i]->m_dwActionTime;
				pNpc->m_dwRegenTime = m_pNpcConfigList[i]->m_dwRegenTime;
				pNpc->m_cResistMagic = m_pNpcConfigList[i]->m_cResistMagic;
				pNpc->m_cMagicLevel = m_pNpcConfigList[i]->m_cMagicLevel;
				pNpc->m_iMaxMana = m_pNpcConfigList[i]->m_iMaxMana; // v1.4
				pNpc->m_iMana = m_pNpcConfigList[i]->m_iMaxMana;
				pNpc->m_cChatMsgPresence = m_pNpcConfigList[i]->m_cChatMsgPresence;
				pNpc->m_cDayOfWeekLimit = m_pNpcConfigList[i]->m_cDayOfWeekLimit;
				pNpc->m_cTargetSearchRange = m_pNpcConfigList[i]->m_cTargetSearchRange;
				pNpc->m_sAreaSize = m_pNpcConfigList[i]->m_sAreaSize;
				switch (sClass) {
				case 43:
				case 44:
				case 45:
				case 46:
				case 47: // 脌眉脌茂驴毛 脌炉麓脰脌脟 掳忙驴矛 鹿芦脕露掳脟 掳酶掳脻赂赂 脟脧麓脗 脌眉路芦脌禄 卤赂禄莽.
					pNpc->m_iAttackStrategy = DEF_ATTACKAI_NORMAL;
					break;

				default:
					pNpc->m_iAttackStrategy = iDice(1, 10);
					break;
				}

				pNpc->m_iAILevel = iDice(1, 3);
				pNpc->m_iAbsDamage = m_pNpcConfigList[i]->m_iAbsDamage;
				pNpc->m_iMagicHitRatio = m_pNpcConfigList[i]->m_iMagicHitRatio;
				pNpc->m_iAttackRange = m_pNpcConfigList[i]->m_iAttackRange;
				pNpc->m_cSpecialAbility = cSA;
				pNpc->m_iBuildCount = m_pNpcConfigList[i]->m_iMinBravery;

				// v1.411 NPC脌脟 脝炉录枚 脠驴掳煤 掳猫禄锚. 掳忙脟猫脛隆 掳隆脕脽 
				switch (pNpc->m_cSpecialAbility) {
				case 1:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 25.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 2:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 30.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 3: // Absorbing Physical Damage
					if (pNpc->m_iAbsDamage > 0) {
						// 卤芒潞禄脌没脌赂路脦 赂露鹿媒 麓毛鹿脤脕枚 脌媒掳篓 麓脡路脗脌脤 脌脰麓脗 掳忙驴矛 鹿掳赂庐 麓毛鹿脤脕枚 脌媒掳篓脌潞 脌没驴毛碌脟脕枚 戮脢麓脗麓脵. 
						pNpc->m_cSpecialAbility = 0;
						cSA = 0;
					}
					else {
						iTemp = 20 + iDice(1, 60);
						pNpc->m_iAbsDamage -= iTemp;
						if (pNpc->m_iAbsDamage < -90) pNpc->m_iAbsDamage = -90;
					}

					dV2 = (double)pNpc->m_iExp;
					dV3 = (double)abs(pNpc->m_iAbsDamage) / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 4: // Absorbing Magical Damage
					if (pNpc->m_iAbsDamage < 0) {
						// 卤芒潞禄脌没脌赂路脦 鹿掳赂庐 麓毛鹿脤脕枚 脌媒掳篓 麓脡路脗脌脤 脌脰麓脗 掳忙驴矛 赂露鹿媒 麓毛鹿脤脕枚 脌媒掳篓脌潞 脌没驴毛碌脟脕枚 戮脢麓脗麓脵. 
						pNpc->m_cSpecialAbility = 0;
						cSA = 0;
					}
					else {
						iTemp = 20 + iDice(1, 60);
						pNpc->m_iAbsDamage += iTemp;
						if (pNpc->m_iAbsDamage > 90) pNpc->m_iAbsDamage = 90;
					}

					dV2 = (double)pNpc->m_iExp;
					dV3 = (double)(pNpc->m_iAbsDamage) / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 5:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 15.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 6:
				case 7:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 20.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;

				case 8:
					dV2 = (double)pNpc->m_iExp;
					dV3 = 25.0f / 100.0f;
					dV1 = dV2 * dV3;
					pNpc->m_iExp += (int)dV1;
					break;
				}

				// v1.411 脌搂脛隆 驴脜卤猫 
				pNpc->m_iNoDieRemainExp = (pNpc->m_iExp) - (pNpc->m_iExp / 3);

				// v1.411 NPC脌脟 脝炉录枚 麓脡路脗 禄冒脌脭 
				pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFF0FF;
				sTemp = cSA;
				sTemp = sTemp << 8;
				pNpc->m_iStatus = pNpc->m_iStatus | sTemp;

				// v1.41 NPC脌脟 Class禄冒脌脭.
				pNpc->m_iStatus = pNpc->m_iStatus & 0xFFFFFFF0;
				pNpc->m_iStatus = pNpc->m_iStatus | (sClass);

				return true;
			}
		}

	// NPC赂庐陆潞脝庐赂娄 赂冒碌脦 掳脣禄枚脟脽脌陆驴隆碌碌 鹿脽掳脽碌脟脕枚 戮脢戮脪麓脵. 
	return false;
}

void CGame::ReleaseFollowMode(short sOwnerH)
{
	int i;
	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((i != sOwnerH) && (m_pNpcList[i] != 0))
		{
			if ((m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW)
				&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_NPC)
				&& (m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH))
			{
				m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
				m_pNpcList[i]->m_iFollowOwnerIndex = 0;
				m_pNpcList[i]->m_cFollowOwnerType = 0;
			}
		}
}

void CGame::CalcNextWayPointDestination(int iNpcH)
{
	short sRange, sX, sY;
	int i, j, iMapIndex;
	bool bFlag;

	switch (m_pNpcList[iNpcH]->m_cMoveType) {
	case DEF_MOVETYPE_GUARD:
		// 脕枚脕陇碌脠 脌搂脛隆赂娄 鹿镁戮卯鲁陋脕枚 戮脢脌赂赂莽 脙脽掳脻脌脤 鲁隆鲁颅 脠脛驴隆 碌鹿戮脝驴脗麓脵.
		break;

	case DEF_MOVETYPE_SEQWAYPOINT:

		m_pNpcList[iNpcH]->m_cCurWaypoint++;
		if (m_pNpcList[iNpcH]->m_cCurWaypoint >= m_pNpcList[iNpcH]->m_cTotalWaypoint)
			m_pNpcList[iNpcH]->m_cCurWaypoint = 1;
		m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
		m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
		break;

	case DEF_MOVETYPE_RANDOMWAYPOINT:

		m_pNpcList[iNpcH]->m_cCurWaypoint = (short)((rand() % (m_pNpcList[iNpcH]->m_cTotalWaypoint - 1)) + 1);
		m_pNpcList[iNpcH]->m_dX = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].x);
		m_pNpcList[iNpcH]->m_dY = (short)(m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_WaypointList[m_pNpcList[iNpcH]->m_iWayPointIndex[m_pNpcList[iNpcH]->m_cCurWaypoint]].y);
		break;

	case DEF_MOVETYPE_RANDOMAREA:

		sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.right - m_pNpcList[iNpcH]->m_rcRandomArea.left);
		m_pNpcList[iNpcH]->m_dX = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.left);
		sRange = (short)(m_pNpcList[iNpcH]->m_rcRandomArea.bottom - m_pNpcList[iNpcH]->m_rcRandomArea.top);
		m_pNpcList[iNpcH]->m_dY = (short)((rand() % sRange) + m_pNpcList[iNpcH]->m_rcRandomArea.top);
		break;

	case DEF_MOVETYPE_RANDOM:
		// 驴脧脌眉 路拢麓媒 
		iMapIndex = m_pNpcList[iNpcH]->m_cMapIndex;

		for (i = 0; i <= 30; i++) {
			// 掳楼 录枚 脌脰麓脗 脕脗脟楼掳隆 鲁陋驴脙露搂 卤卯脕枚 30鹿酶 鹿脻潞鹿 
			sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15;
			sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15;

			bFlag = true;
			for (j = 0; j < DEF_MAXMGAR; j++)
				if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left != -1) {
					// 脟脟脟脴戮脽 脟脪 脕脗脟楼掳隆 脌脰麓脵. 
					if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].left) &&
						(sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].right) &&
						(sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].top) &&
						(sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[j].bottom)) {
						// Avoid Rect戮脠脌脤鹿脟路脦 脌脤 脌搂脛隆驴隆麓脗 禄媒录潞陆脙脜鲁 录枚 戮酶麓脵.	
						bFlag = false;
					}
				}
			if (bFlag == true) goto CNW_GET_VALIDLOC_SUCCESS;
		}
		// Fail! 
		m_pNpcList[iNpcH]->m_tmp_iError = 0;
		return;

	CNW_GET_VALIDLOC_SUCCESS:;
		m_pNpcList[iNpcH]->m_dX = sX;
		m_pNpcList[iNpcH]->m_dY = sY;
		break;
	}

	m_pNpcList[iNpcH]->m_tmp_iError = 0; // @@@ !!! @@@
}

void CGame::DeleteNpc(int iNpcH)
{
	int  i, iNamingValue, iNumItem, iItemID, iItemIDs[MAX_NPCITEMDROP], iSlateID;
	char cTmp[21], cItemName[21];
	class CItem* pItem, * pItem2;
	DWORD dwCount, dwTime;
	POINT ItemPositions[MAX_NPCITEMDROP];
	char cTemp[256];
	SYSTEMTIME SysTime;
	if (m_pNpcList[iNpcH] == 0) return;

	dwTime = timeGetTime();

	//Init number of items to 1 unless its a multidrop;
	iNumItem = 0;
	iItemID = 0; // No current item

	//矛拢陆矛鈥撀疵舅喢嵥?矛艙鈥灻顾溍€斅惷€炁?矛鈥毬犈?
	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_REJECT, 0, 0, 0);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

	ZeroMemory(cTmp, sizeof(cTmp));
	strcpy(cTmp, (char*)(m_pNpcList[iNpcH]->m_cName + 2));
	// NPC矛聺藴 NamigValue毛楼录 矛鈥撀幻€撀疵劉鈧?矛鈥毬÷┟も€樏澛?铆鈥樑撁€古撁ヂ?铆鈥⒙疵р偓铆鈥⑴撁€孤?
	iNamingValue = atoi(cTmp);

	// NamingValue毛楼录 毛鹿鈥灻÷懊陈?毛聫鈩⒚锯€樏も€樏澛?锚掳艙矛虏麓 矛藛藴毛楼录 锚掳聬矛鈥犈捗€古撁€毬€孤?
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject--;
	
	if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject == 0)
	{
		if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 1)
		{	// See if we need to open a dynamic Gate, then tell clients
			Open_EmptyMap_Gate(m_pNpcList[iNpcH]->m_cMapIndex);
		}
		if (m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType == 2)
		{	// generate Abaddon and open Dynamic Gate
			GenerateApocalypseBoss(m_pNpcList[iNpcH]->m_cMapIndex);
			// Then change m_iApocalypseMobGenType to 1 for Gate openning when this boss is dead
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iApocalypseMobGenType = 1;
		}
	}

	// Spot-mob-generator矛聽鈥⒚陈?
	if (m_pNpcList[iNpcH]->m_iSpotMobIndex != 0)
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stSpotMobGenerator[m_pNpcList[iNpcH]->m_iSpotMobIndex].iCurMobs--;

	RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);

	// 铆聛卢毛拢篓矛鈥灺该澛疵€溑撁÷?锚卤麓矛露鈥⒚济澛疵澛济┞?矛搂鈧撍溍粹偓 铆鈥犅得陈疵÷?锚卤麓矛露鈥⒚?毛娄卢矛艩陇铆艩赂毛楼录 铆鈥⒙疵犈?	
	switch (m_pNpcList[iNpcH]->m_sType) {
	case 36:
	case 37:
	case 38:
	case 39:
	case 42:
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCrusadeStructureInfo(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
		// 锚赂赂毛鈥溑?锚卤麓矛露鈥⒚?锚掳艙矛藛藴毛聫鈥?锚掳聬矛鈥犈捗€古撁€毬?
		for (i = 0; i < DEF_MAXGUILDS; i++) {
			if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID) {
				m_pGuildTeleportLoc[i].m_dwTime = dwTime;
				m_pGuildTeleportLoc[i].m_iV2--;
				if (m_pGuildTeleportLoc[i].m_iV2 < 0) m_pGuildTeleportLoc[i].m_iV2 = 0;
				break;
			}
		}
		break;
	case 64:
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
		break;

	}

	bool IsFragile = false;

	// DelayEvent毛楼录 矛鈥毬犈?
	bRemoveFromDelayEventList(iNpcH, DEF_OWNERTYPE_NPC, 0);
	if ((m_pNpcList[iNpcH]->m_bIsSummoned == false) && (m_pNpcList[iNpcH]->m_bIsUnsummoned == false)) {
		// 矛鈥犈捗劉藴毛陋鹿矛聺麓 矛鈥⑩€灻€顾喢澛济┞?矛鈥斅懊杆?矛啪卢毛拢艗锚掳鈧?毛鈥毸溍溌€孤?
		pItem = new class CItem;
		ZeroMemory(cItemName, sizeof(cItemName));
		switch (m_pNpcList[iNpcH]->m_sType) {

		case 10: // Slime
			if (iDice(1, 25) == 1) iItemID = 220;  // SlimeJelly			
			break;

		case 11: // Skeleton
			switch (iDice(1, 2)) {
			case 1:if (iDice(1, 20) == 1) iItemID = 219; break; // SkeletonBones
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 12: // Stone-Golem
			switch (iDice(1, 2)) {
			case 1:	if (iDice(1, 30) == 1) iItemID = 221; break; // StoneGolemPiece
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 13: // Cyclops
			switch (iDice(1, 6)) {
			case 1:	if (iDice(1, 36) == 1) iItemID = 194; break; // CyclopsEye
			case 2:	if (iDice(1, 40) == 1) iItemID = 195; break; // CyclopsHandEdge
			case 3:	if (iDice(1, 30) == 1) iItemID = 196; break; // CyclopsHeart
			case 4:	if (iDice(1, 22) == 1) iItemID = 197; break; // CyclopsMeat
			case 5:	if (iDice(1, 40) == 1) iItemID = 198; break; // CyclopsLeather
			case 6:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 14: // Orc, Orc-Mage
			switch (iDice(1, 4)) {
			case 1:	if (iDice(1, 11) == 1) iItemID = 206; break; // OrcMeat
			case 2:	if (iDice(1, 20) == 1) iItemID = 207; break; // OrcLeather
			case 3:	if (iDice(1, 21) == 1) iItemID = 208; break; // OrcTeeth
			case 4: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 16: //Giant-Ant
			switch (iDice(1, 3)) {
			case 1:	if (iDice(1, 9) == 1) iItemID = 192; break; // AntLeg
			case 2:	if (iDice(1, 10) == 1) iItemID = 193; break; // AntFeeler
			case 3:	bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 17: //Scorpion
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 50) == 1) iItemID = 215; break; // ScorpionPincers
			case 2:	if (iDice(1, 20) == 1) iItemID = 216; break; // ScorpionMeat
			case 3: if (iDice(1, 50) == 1) iItemID = 217; break; // ScorpionSting
			case 4: if (iDice(1, 40) == 1) iItemID = 218; break; // ScorpionSkin
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 22: //Amphis
			switch (iDice(1, 5)) {
			case 1: if (iDice(1, 15) == 1) iItemID = 188; break; // SnakeMeat
			case 2:	if (iDice(1, 16) == 1) iItemID = 189; break; // SnakeSkin
			case 3:	if (iDice(1, 16) == 1) iItemID = 190; break; // SnakeTeeth
			case 4:	if (iDice(1, 17) == 1) iItemID = 191; break; // SnakeTongue
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 23: //Clay-Golem
			switch (iDice(1, 2)) {
			case 1: if (iDice(1, 30) == 1) iItemID = 205; break; // LumpofClay
			case 2: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;


		case 27: //Hellbound
			switch (iDice(1, 7)) {
			case 1:	if (iDice(1, 40) == 1) iItemID = 199; break; // HelboundHeart
			case 2:	if (iDice(1, 38) == 1) iItemID = 200; break; // HelboundLeather
			case 3:	if (iDice(1, 38) == 1) iItemID = 201; break; // HelboundTail
			case 4:	if (iDice(1, 36) == 1) iItemID = 202; break; // HelboundTeeth
			case 5:	if (iDice(1, 36) == 1) iItemID = 203; break; // HelboundClaw
			case 6:	if (iDice(1, 50) == 1) iItemID = 204; break; // HelboundTongue
			case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 28: //Troll
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 35) == 1) iItemID = 222; break; // TrollHeart
			case 2:	if (iDice(1, 23) == 1) iItemID = 223; break; // TrollMeat
			case 3:	if (iDice(1, 25) == 1) iItemID = 224; break; // TrollLeather
			case 4:	if (iDice(1, 27) == 1) iItemID = 225; break; // TrollClaw
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 29: //Orge
			switch (iDice(1, 7)) {
			case 1:	if (iDice(1, 20) == 1) iItemID = 209; break; // OgreHair
			case 2:	if (iDice(1, 22) == 1) iItemID = 210; break; // OgreHeart
			case 3:	if (iDice(1, 25) == 1) iItemID = 211; break; // OgreMeat
			case 4:	if (iDice(1, 25) == 1) iItemID = 212; break; // OgreLeather
			case 5:	if (iDice(1, 28) == 1) iItemID = 213; break; // OgreTeeth
			case 6:	if (iDice(1, 28) == 1) iItemID = 214; break; // OgreClaw
			case 7: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 31: //Demon
		case 93: //Black-Demon
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 400) == 123) iItemID = 541; break; // DemonHeart
			case 2:	if (iDice(1, 1000) == 123) iItemID = 542; break; // DemonMeat
			case 3:	if (iDice(1, 200) == 123) iItemID = 543; break; // DemonLeather
			case 4:	if (iDice(1, 300) == 123) iItemID = 540; break; // DemonEye
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 32: //Unicorn
			switch (iDice(1, 5)) {
			case 1:	if (iDice(1, 3000) == 396) iItemID = 544; break; // UnicornHeart
			case 2:	if (iDice(1, 500) == 3) iItemID = 545; break; // UnicornHorn
			case 3:	if (iDice(1, 100) == 3) iItemID = 546; break; // UnicornMeat
			case 4:	if (iDice(1, 200) == 3) iItemID = 547; break; // UnicornLeather
			case 5: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 33: //WereWolf
			switch (iDice(1, 8)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 551; break; // WerewolfTail
			case 2:	if (iDice(1, 28) == 3) iItemID = 548; break; // WerewolfHeart
			case 3:	if (iDice(1, 25) == 3) iItemID = 550; break; // WerewolfMeat
			case 4:	if (iDice(1, 35) == 3) iItemID = 553; break; // WerewolfLeather
			case 5:	if (iDice(1, 28) == 3) iItemID = 552; break; // WerewolfTeeth
			case 6:	if (iDice(1, 28) == 3) iItemID = 554; break; // WerewolfClaw
			case 7:	if (iDice(1, 38) == 3) iItemID = 549; break; // WerewolfNail
			case 8: bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); break;

			}
			break;

		case 48: //Stalker
		case 49: //Hellclaw
		case 50: //Tigerworm
		case 52: //Gagoyle
		case 53: //Beholder
		case 55: //Rabbit
		case 56: //Cat
		case 57: //Giant-Frog
		case 58: //Mountain-Giant
		case 59: //Ettin
		case 60: //Cannibal-Plant
		case 61: //Rudolph
		case 62: //DireBoar
		case 63: //Frost
		case 65: //Ice
		case 54: //Dark-Elf
		case 30: //Liche
		case 18: //Zombie
		case 92: // Eternal Dragon
			bGetItemNameWhenDeleteNpc(iItemID, m_pNpcList[iNpcH]->m_sType); 
			break;

		// new 05/10/2004
		case 66: // Wyvern
		case 73: // Fire-Wyvern
		
		case 94: // BlackWyv
		case 95: // LighWyvern
		case 96: // PoisonWyvern
		case 97: // HeavenWyvern
		case 98: // IllusionWyvern
			bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,	// NPC Type
				50,							// 脠庐路眉( 脙脰录脪 ~ 脙脰麓毛 禄莽脌脤脌脟 戮脝脌脤脜脹脌脤 鲁陋驴脙 脠庐路眉 , 100 脌脤赂茅 脙脰麓毛 掳鹿录枚 赂赂脜颅 鲁陋驴脗麓脵.)
				5,							// 鲁陋驴脥戮脽 脟脧麓脗 Item 脙脰录脪 掳鲁录枚
				9,							// 鲁陋驴脙录枚 脌脰麓脗 Item 脙脰麓毛 掳鲁录枚
				m_pNpcList[iNpcH]->m_sX,	// 卤芒脕脴 脕脗脟楼 X
				m_pNpcList[iNpcH]->m_sY,	// 卤芒脕脴 脕脗脟楼 Y
				DEF_ITEMSPREAD_FIXED,		// 戮脝脌脤脜脹 禄脩赂庐麓脗 鹿忙鹿媒 (RANDOM, FIXED)
				4,							// 戮脝脌脤脜脹 禄脩赂卤 鹿眉脌搂, DEF_ITEMSPREAD_RANDOM 脌脧露搂赂赂 禄莽驴毛
				iItemIDs,					// 鹿脼戮脝驴脙 戮脝脌脤脜脹 Id碌茅
				ItemPositions,				// 戮脝脌脤脜脹碌茅脌脟 脌搂脛隆
				&iNumItem);					// 戮脝脌脤脜脹脌脟 脙脩 掳鹿录枚
			
			break;

		case 81: // Abaddon
			bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,	// NPC Type
				50,							// 脠庐路眉( 脙脰录脪 ~ 脙脰麓毛 禄莽脌脤脌脟 戮脝脌脤脜脹脌脤 鲁陋驴脙 脠庐路眉 , 100 脌脤赂茅 脙脰麓毛 掳鹿录枚 赂赂脜颅 鲁陋驴脗麓脵.)
				12,							// 鲁陋驴脥戮脽 脟脧麓脗 Item 脙脰录脪 掳鲁录枚
				15,							// 鲁陋驴脙录枚 脌脰麓脗 Item 脙脰麓毛 掳鲁录枚
				m_pNpcList[iNpcH]->m_sX,	// 卤芒脕脴 脕脗脟楼 X
				m_pNpcList[iNpcH]->m_sY,	// 卤芒脕脴 脕脗脟楼 Y
				DEF_ITEMSPREAD_FIXED,		// 戮脝脌脤脜脹 禄脩赂庐麓脗 鹿忙鹿媒 (RANDOM, FIXED)
				65,							// 戮脝脌脤脜脹 禄脩赂卤 鹿眉脌搂, DEF_ITEMSPREAD_RANDOM 脌脧露搂赂赂 禄莽驴毛
				iItemIDs,					// 鹿脼戮脝驴脙 戮脝脌脤脜脹 Id碌茅
				ItemPositions,				// 戮脝脌脤脜脹碌茅脌脟 脌搂脛隆
				&iNumItem);
			
			break;

		case 99: // Ghost-Abaddon
			bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType,	// NPC Type
				50,							// 脠庐路眉( 脙脰录脪 ~ 脙脰麓毛 禄莽脌脤脌脟 戮脝脌脤脜脹脌脤 鲁陋驴脙 脠庐路眉 , 100 脌脤赂茅 脙脰麓毛 掳鹿录枚 赂赂脜颅 鲁陋驴脗麓脵.)
				12,							// 鲁陋驴脥戮脽 脟脧麓脗 Item 脙脰录脪 掳鲁录枚
				20,							// 鲁陋驴脙录枚 脌脰麓脗 Item 脙脰麓毛 掳鲁录枚
				m_pNpcList[iNpcH]->m_sX,	// 卤芒脕脴 脕脗脟楼 X
				m_pNpcList[iNpcH]->m_sY,	// 卤芒脕脴 脕脗脟楼 Y
				DEF_ITEMSPREAD_FIXED,		// 戮脝脌脤脜脹 禄脩赂庐麓脗 鹿忙鹿媒 (RANDOM, FIXED)
				65,							// 戮脝脌脤脜脹 禄脩赂卤 鹿眉脌搂, DEF_ITEMSPREAD_RANDOM 脌脧露搂赂赂 禄莽驴毛
				iItemIDs,					// 鹿脼戮脝驴脙 戮脝脌脤脜脹 Id碌茅
				ItemPositions,				// 戮脝脌脤脜脹碌茅脌脟 脌搂脛隆
				&iNumItem);
			
			break;

		}

		if (iItemID < 0) 
		{
			delete m_pNpcList[iNpcH];
			m_pNpcList[iNpcH] = 0;
			return;
		}

		dwCount = 1;
		if (iNumItem > 0) {
			GetLocalTime(&SysTime);
			wsprintf(cTemp, "%d%02d%", SysTime.wMonth, SysTime.wDay);
			for (int j = 0; j < iNumItem; j++) {
				if (pItem == 0) {
					pItem = new class CItem;
				}
				if (_bInitItemAttr(pItem, iItemIDs[j]) == false ||
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetIsMoveAllowedTile((short)ItemPositions[j].x, (short)ItemPositions[j].y) == false) {
					delete pItem;
					pItem = 0;
				}
				else {
					if (iItemIDs[j] == 90)
						pItem->m_dwCount = iDice(10, 10500);
					else
						pItem->m_dwCount = dwCount;
						
					pItem->m_sTouchEffectType = DEF_ITET_ID;
					pItem->m_sTouchEffectValue1 = iDice(1, 100000);
					pItem->m_sTouchEffectValue2 = iDice(1, 100000);
					pItem->m_sTouchEffectValue3 = (short)timeGetTime();
					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem((short)ItemPositions[j].x, (short)ItemPositions[j].y, pItem);
					
					//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					//	ItemPositions[j].x, ItemPositions[j].y, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
						(short)ItemPositions[j].x, (short)ItemPositions[j].y, pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

					_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
					pItem = 0;
				}
			}
		}
		else {
			if (_bInitItemAttr(pItem, iItemID) == false) {
				delete pItem;
				pItem = 0;
			}
			else {
				pItem->m_dwCount = dwCount;

				pItem->m_sTouchEffectType = DEF_ITET_ID;
				pItem->m_sTouchEffectValue1 = iDice(1, 100000);
				pItem->m_sTouchEffectValue2 = iDice(1, 100000);
				pItem->m_sTouchEffectValue3 = (short)timeGetTime();
				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);
				
				//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
				//	m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
				SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
					pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

				_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
			}
		}

		if (iDice(1, 100000) < 10) {
			pItem2 = new class CItem;
			switch (iDice(1, 4)) {
			case 1:	iSlateID = 868; break;
			case 2: iSlateID = 869; break;
			case 3: iSlateID = 870; break;
			case 4: iSlateID = 871; break;
			}
			if (_bInitItemAttr(pItem2, iSlateID) == false) {
				delete pItem2;
				pItem2 = 0;
			}
			else {
				pItem2->m_dwCount = 1;

				pItem2->m_sTouchEffectType = DEF_ITET_ID;
				pItem2->m_sTouchEffectValue1 = iDice(1, 100000);
				pItem2->m_sTouchEffectValue2 = iDice(1, 100000);
				pItem2->m_sTouchEffectValue3 = (short)timeGetTime();

				m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2);
				//SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
				//	m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem2->m_sSprite, pItem2->m_sSpriteFrame, pItem2->m_cItemColor);
				SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
					m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
					pItem2->m_sIDnum, pItem2->m_sSpriteFrame, pItem2->m_cItemColor, pItem2->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem2);
			}
		}
	}

	delete m_pNpcList[iNpcH];
	m_pNpcList[iNpcH] = 0;
}

int CGame::iGetFollowerNumber(short sOwnerH, char cOwnerType)
{
	int i, iTotal;

	iTotal = 0;

	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != 0) && (m_pNpcList[i]->m_cMoveType == DEF_MOVETYPE_FOLLOW)) {

			if ((m_pNpcList[i]->m_iFollowOwnerIndex == sOwnerH) && (m_pNpcList[i]->m_cFollowOwnerType == cOwnerType))
				iTotal++;
		}

	return iTotal;
}

void CGame::bSetNpcAttackMode(char* cName, int iTargetH, char cTargetType, bool bIsPermAttack)
{
	int i, iIndex;

	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, cName, 5) == 0)) {
			iIndex = i;
			goto NEXT_STEP_SNAM1;

		}
	// 掳掳脌潞 脌脤赂搂脌禄 掳隆脕酶 NPC掳隆 戮酶麓脵.
	return;

NEXT_STEP_SNAM1:;

	switch (cTargetType) {
	case DEF_OWNERTYPE_PLAYER:
		if (m_pClientList[iTargetH] == 0) return;
		break;

	case DEF_OWNERTYPE_NPC:
		if (m_pNpcList[iTargetH] == 0) return;
		break;
	}

	// 赂帽脟楼鹿掳脌禄 脟脪麓莽脟脩麓脵. 
	m_pNpcList[iIndex]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
	m_pNpcList[iIndex]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iIndex]->m_iTargetIndex = iTargetH;
	m_pNpcList[iIndex]->m_cTargetType = cTargetType;

	// 驴碌卤赂 掳酶掳脻 赂冒碌氓 驴漏潞脦 录鲁脕陇 
	m_pNpcList[iIndex]->m_bIsPermAttackMode = bIsPermAttack;

}

int CGame::iGetNpcRelationship(int iWhatH, int iRecvH)
{
	int iRet;

	if (m_pClientList[iRecvH] == 0) return 0;
	if (m_pNpcList[iWhatH] == 0) return 0;

	iRet = 0;
	switch (m_pNpcList[iWhatH]->m_cSide) {
	case 10: iRet |= 8; break;//Fixed, Original 15 
	case 1:  iRet = (iRet | 4) | 2; break;//Fixed, Original 1 
	case 2:  iRet |= 4; break;//Fixed, Orignal 8 
	}

	return iRet;
}



int CGame::iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH)
{
	int iRet;

	// 驴漏卤芒录颅 禄莽脌脤碌氓 掳猫禄锚脟脧麓脗 鹿忙鹿媒脌潞 掳眉脕隆脌脤 麓脵赂拢鹿脟路脦 脕脰脌脟脟脩麓脵. 
	// 鲁禄(NPC) 碌楼脌脤脜脥赂娄 脌脹录潞脟脧麓脗 掳脥脌脤 戮脝麓脧露贸 麓脵赂楼 脌脤驴隆掳脭 潞赂鲁禄麓脗 碌楼脌脤脜脥赂娄 脌脹录潞脟脧麓脗 掳脥脌脫脌禄 脕脰脌脟.
	if (m_pClientList[iOpponentH] == 0) return 0;
	if (m_pClientList[iOpponentH]->m_bIsInitComplete == false) return 0;

	if (m_pNpcList[iNpcH] == 0) return 0;

	iRet = 0;

	if (m_pClientList[iOpponentH]->m_iPKCount != 0) {
		// 禄贸麓毛鹿忙脌脤 鹿眉脕脣脌脷脌脤脟脧赂茅 脝铆脌脤 掳掳脌赂赂茅 脌搂脟霉脌赂路脦 戮脝麓脧赂茅 NPC 脌没脌赂路脦 脌脦陆脛碌脠麓脵.
		if (m_pNpcList[iNpcH]->m_cSide == m_pClientList[iOpponentH]->m_cSide)
			iRet = 7;
		else iRet = 2;
	}
	else {
		if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[iOpponentH]->m_cSide) {
			// 赂梅脌脟 脝铆掳煤 禄贸麓毛鹿忙脌脟 脝铆脌脤 麓脵赂拢麓脵.
			if (m_pNpcList[iNpcH]->m_cSide == 10)
				iRet = 2; // 赂梅脌脤 脌脧鹿脻 赂梅脌脤麓脵. 禄贸麓毛鹿忙驴隆掳脭麓脗 脌没脌赂路脦 脌脦陆脛碌脠麓脵.
			else
				if (m_pNpcList[iNpcH]->m_cSide == 0) iRet = 0; // 赂梅脌脤 脕脽赂鲁脌脤赂茅 鹿芦脕露掳脟 脕脽赂鲁 	
				else
					if (m_pClientList[iOpponentH]->m_cSide == 0)
						iRet = 0; // 禄贸麓毛鹿忙脌脤 脕脽赂鲁脌脤麓脵. 录脪脠炉赂梅掳煤 脕脽赂鲁掳煤脌脟 掳眉掳猫麓脗 脕脽赂鲁. 
					else iRet = 2; // 录脪脠炉赂梅掳煤 陆脙鹿脦 脌脤禄贸脌脦碌楼 脝铆脌脤 麓脵赂拢鹿脟路脦 脌没 

		}
		else iRet = 1; // 掳掳脌潞 脝铆 
	}

	return iRet;
}

void CGame::NpcRequestAssistance(int iNpcH)
{
	int ix, iy, sX, sY;
	short sOwnerH;
	char  cOwnerType;

	// iNpc 卤脵鹿忙驴隆 脌脰麓脗 脌眉脜玫脕脽脌脤 戮脝麓脩 NPC驴隆掳脭 碌碌驴貌脌禄 驴盲脙禄脟脩麓脵. 
	if (m_pNpcList[iNpcH] == 0) return;

	sX = m_pNpcList[iNpcH]->m_sX;
	sY = m_pNpcList[iNpcH]->m_sY;

	for (ix = sX - 8; ix <= sX + 8; ix++)
		for (iy = sY - 8; iy <= sY + 8; iy++) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
			if ((sOwnerH != 0) && (m_pNpcList[sOwnerH] != 0) && (cOwnerType == DEF_OWNERTYPE_NPC) &&
				(iNpcH != sOwnerH) && (m_pNpcList[sOwnerH]->m_cSide == m_pNpcList[iNpcH]->m_cSide) &&
				(m_pNpcList[sOwnerH]->m_bIsPermAttackMode == false) && (m_pNpcList[sOwnerH]->m_cBehavior == DEF_BEHAVIOR_MOVE)) {

				// 脕露掳脟驴隆 潞脦脟脮脟脧麓脗 NPC赂娄 脙拢戮脪麓脵. 
				m_pNpcList[sOwnerH]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
				m_pNpcList[sOwnerH]->m_sBehaviorTurnCount = 0;
				m_pNpcList[sOwnerH]->m_iTargetIndex = m_pNpcList[iNpcH]->m_iTargetIndex;
				m_pNpcList[sOwnerH]->m_cTargetType = m_pNpcList[iNpcH]->m_cTargetType;

				return;
			}
		}
}

void CGame::NpcTalkHandler(int iClientH, int iWho, int iQuest)
{
	char cRewardName[21], cTargetName[21];
	int iResMode, iQuestNum, iQuestType, iRewardType, iRewardAmount, iContribution, iX, iY, iRange, iTargetType, iTargetCount, iReward, i;

	iQuestNum = 0;
	/*ZeroMemory(cTargetName, sizeof(cTargetName));
	if (m_pClientList[iClientH] == 0) return;
	switch (iWho) {
	case 1: break;
	case 2:	break;
	case 3:	break;
	case 4:
		iQuestNum = _iTalkToNpcResult_Cityhall(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
		break;
	case 5: break;
	case 6:	break;
	case 32: break;
	case 21:
		iQuestNum = _iTalkToNpcResult_Guard(iClientH, &iQuestType, &iResMode, &iRewardType, &iRewardAmount, &iContribution, cTargetName, &iTargetType, &iTargetCount, &iX, &iY, &iRange);
		if (iQuestNum >= 1000) return;
		break;
	}*/

	ZeroMemory(cTargetName, sizeof(cTargetName));

	//Magn0S:: Quest List
	for (i = 0; i < DEF_MAXQUEST; i++) { // Magn0S:: Multi Quest
		if (m_pClientList[iClientH]->m_iQuest[i] == 0) {
			if (iWho == 4 && iQuest > 0)
			{
				iQuestNum = iQuest;

				iReward = iDice(1, 3);
				iResMode = m_pQuestConfigList[iQuestNum]->m_iResponseMode;
				iRewardType = m_pQuestConfigList[iQuestNum]->m_iRewardType[iReward];
				iRewardAmount = m_pQuestConfigList[iQuestNum]->m_iRewardAmount[iReward];
				iContribution = m_pQuestConfigList[iQuestNum]->m_iContribution;

				memcpy(cTargetName, m_pQuestConfigList[iQuestNum]->m_cTargetName, 20); // <<, Crash line?

				iX = m_pQuestConfigList[iQuestNum]->m_sX;
				iY = m_pQuestConfigList[iQuestNum]->m_sY;
				iRange = m_pQuestConfigList[iQuestNum]->m_iRange;

				iTargetType = m_pQuestConfigList[iQuestNum]->m_iTargetType;
				iTargetCount = m_pQuestConfigList[iQuestNum]->m_iMaxCount;
				iQuestType = m_pQuestConfigList[iQuestNum]->m_iType;
			}

			ZeroMemory(cRewardName, sizeof(cRewardName));
			if (iQuestNum > 0) {
				if (iRewardType > 1) {
					strcpy(cRewardName, m_pItemConfigList[iRewardType]->m_cName);
				}
				else {
					switch (iRewardType) {
						// SNOOPY Strange here !

					case -1: strcpy(cRewardName, "Exp"); break;
					}
				}

				m_pClientList[iClientH]->m_iAskedQuest = iQuestNum;
				//m_pClientList[iClientH]->m_iQuestRewardType[i] = iRewardType;
				//m_pClientList[iClientH]->m_iQuestRewardAmount[i] = iRewardAmount;
				//SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Send Quest Data");

				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, iQuestType, iResMode, iRewardAmount, cRewardName, iContribution,
					iTargetType, iTargetCount, iX, iY, iRange, cTargetName);
			}
			else {
				switch (iQuestNum) {
				case  0: SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (iWho + 130), 0, 0, 0, 0); break;
				case -1:
				case -2:
				case -3:
				case -4: SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, 104, 0, 0, 0, 0); break;
				case -5: break;
				}
			}
		}
	}
}

void CGame::SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char* pData)
{
	int i, iTargetIndex;
	char   seps[] = "= \t\n";
	char* token, cTargetName[11], cBuff[256];
	class  CStrTok* pStrTok;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_cSide == 0) return;
	switch (iMode) {
	case 8: // Mobilize
		if ((m_bIsHeldenianMode == true)
			&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0)
			&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1))
		{
			iRecoverFollowers(iClientH, true);
		}
		break;
	case 0: // Free ?	case 1: // Hold ?		for (i = 0; i < DEF_MAXNPCS; i++)
			if (m_pNpcList[i] != 0)
			{
				if ((m_pNpcList[i]->m_iFollowOwnerIndex == iClientH)
					&& ((m_pNpcList[i]->m_bIsSummoned == true) || (m_pNpcList[i]->m_dwTamingTime != 0))
					&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER))
				{
					m_pNpcList[i]->m_iSummonControlMode = iMode;
					m_pNpcList[i]->m_bIsPermAttackMode = false;
					m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
					m_pNpcList[i]->m_sBehaviorTurnCount = 0;
					m_pNpcList[i]->m_iTargetIndex = 0;
				}
			}
		break;
	case 2:	// target
		if ((dwMsgSize) <= 0) return;
		memcpy(cBuff, pData, dwMsgSize);
		pStrTok = new class CStrTok(cBuff, seps);
		token = pStrTok->pGet();
		token = pStrTok->pGet();

		iTargetIndex = 0;
		if (token != 0) {
			// token捞 梆 傍拜且 荤侩磊 捞抚 
			if (strlen(token) > 10)
				memcpy(cTargetName, token, 10);
			else memcpy(cTargetName, token, strlen(token));

			// 2002.8.17 厘柳龋 荐沥
			for (i = 0; i < DEF_MAXCLIENTS; i++)
			{
				if ((m_pClientList[i] != 0) &&
					(memcmp(m_pClientList[i]->m_cCharName, cTargetName, 10) == 0) &&
					(strcmp(m_pClientList[iClientH]->m_cMapName, m_pClientList[i]->m_cMapName) == 0)) // adamas(map捞 鞍酒具 茄促.)
				{
					// 格钎 某腐磐甫 茫疽促. 牢郸胶 历厘
					iTargetIndex = i;
					goto SSMA_SKIPSEARCH;
				}
			}
		}
		break;

	SSMA_SKIPSEARCH:;
		if ((iTargetIndex != 0) && (m_pClientList[iTargetIndex]->m_cSide != 0)
			&& (m_pClientList[iTargetIndex]->m_cSide != m_pClientList[iClientH]->m_cSide))
		{
			for (i = 0; i < DEF_MAXNPCS; i++)
				if (m_pNpcList[i] != 0)
				{
					if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER)
						&& ((m_pNpcList[i]->m_bIsSummoned == true) || (m_pNpcList[i]->m_dwTamingTime != 0))
						&& (m_pNpcList[i]->m_iFollowOwnerIndex == iClientH))
					{
						m_pNpcList[i]->m_iSummonControlMode = iMode;
						m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_ATTACK;
						m_pNpcList[i]->m_sBehaviorTurnCount = 0;
						m_pNpcList[i]->m_iTargetIndex = iTargetIndex;
						m_pNpcList[i]->m_cTargetType = DEF_OWNERTYPE_PLAYER;
						m_pNpcList[i]->m_bIsPermAttackMode = true;
					}
				}
		}
		delete pStrTok;
		break;
	}
}

int CGame::_iTalkToNpcResult_Guard(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	// 掳忙潞帽潞麓掳煤脌脟 麓毛脠颅. 脟脙路鹿脌脤戮卯脌脟 脌搂脛隆驴隆 脌没脌媒脟脩 麓毛脠颅 赂脼陆脙脕枚赂娄 碌冒陆潞脟脙路鹿脌脤脟脩麓脵.
	if (m_pClientList[iClientH] == 0) return 0;

	if (memcmp(m_pClientList[iClientH]->m_cLocation, "are", 3) == 0) {
		// 赂禄 掳脟 禄莽露梅脌脤 戮脝路鹿陆潞碌搂 录脪录脫 
		if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
			// 脌脷卤芒 赂露脌禄 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (200), 0, 0, 0, 0);
			return 1000;
		}
		else
			if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0) {
				// 麓脵赂楼 赂露脌禄 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆.	
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (201), 0, 0, 0, 0);
				return 1001;
			}
	}
	else
		if (memcmp(m_pClientList[iClientH]->m_cLocation, "elv", 3) == 0) {
			if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
				// 麓脵赂楼 赂露脌禄 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆.	
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (202), 0, 0, 0, 0);
				return 1002;
			}
			else
				if (memcmp(m_pClientList[iClientH]->m_cMapName, "elv", 3) == 0) {
					// 脌脷卤芒 赂露脌禄 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (203), 0, 0, 0, 0);
					return 1003;
				}
		}
		else
			if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) {
				if (memcmp(m_pClientList[iClientH]->m_cMapName, "aresden", 7) == 0) {
					// 脕脽赂鲁脌脤 戮脝路鹿陆潞碌搂 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (204), 0, 0, 0, 0);
					return 1004;
				}
				else
					if (memcmp(m_pClientList[iClientH]->m_cMapName, "elvine", 6) == 0) {
						// 脕脽赂鲁脌脤 驴陇鹿脵脌脦 掳忙潞帽驴隆掳脭 赂禄脌禄 掳脡戮煤脌陆.
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (205), 0, 0, 0, 0);
						return 1005;
					}
					else
						if (memcmp(m_pClientList[iClientH]->m_cMapName, "default", 7) == 0) {
							SendNotifyMsg(0, iClientH, DEF_NOTIFY_NPCTALK, (206), 0, 0, 0, 0);
							return 1006;
						}
			}

	return 0;
}

int CGame::_iTalkToNpcResult_GuildHall(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_GShop(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_BSmith(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_WHouse(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

int CGame::_iTalkToNpcResult_WTower(int iClientH, int* pQuestType, int* pMode, int* pRewardType, int* pRewardAmount, int* pContribution, char* pTargetName, int* pTargetType, int* pTargetCount, int* pX, int* pY, int* pRange)
{
	return -4;
}

// 05/21/2004 - Hypnotoad - Balances to drop status
void CGame::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)
{
	class CItem* pItem;
	char  cColor, cItemName[21];
	bool  bIsGold;
	int   iGenLevel, iResult, iItemID;
	DWORD dwType, dwValue;
	double dTmp1, dTmp2, dTmp3;
	short sElement;

	if (m_pNpcList[iNpcH] == 0) return;
	if ((cAttackerType != DEF_OWNERTYPE_PLAYER) || (m_pNpcList[iNpcH]->m_bIsSummoned == true)) return;
	if (m_pNpcList[iNpcH]->m_bIsUnsummoned == true) return;

	ZeroMemory(cItemName, sizeof(cItemName));
	bIsGold = false;

	switch (m_pNpcList[iNpcH]->m_sType) {
	// NPC not dropping Gold
	case 21: // Guard
	case 64: // Crop
		return;
	}

	// 6500 default; the lower the greater the Weapon/Armor/Wand Drop
	if (iDice(1, 100) >= m_iPrimaryDropRate) {
		// 35% Drop 60% of that is gold
		// 35% Chance of drop (35/100)
		if (iDice(1, 100) <= m_iGoldRate) { // Centuu : Agregado para controlar el drop de oro.
			iItemID = 90; // Gold: (35/100) * (60/100) = 21%
			
			// If a non-existing itemID is given create no item 
			pItem = new class CItem;
			if (_bInitItemAttr(pItem, iItemID) == false) {
				delete pItem;
				return;
			}

			pItem->m_dwCount = (DWORD)(iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin)) + m_pNpcList[iNpcH]->m_iGoldDiceMin);

			// v1.42 Gold 
			if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_iAddGold > 0)) {
				dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddGold;
				dTmp2 = (double)pItem->m_dwCount;
				dTmp3 = (dTmp1 / 100.0f) * dTmp2;
				pItem->m_dwCount += (int)dTmp3;
			}

			//Magn0S:: Multiplicador de Gold
			//pItem->m_dwCount *= 10;
		}
		else {

			switch (m_pNpcList[iNpcH]->m_sType) {
			case 34: // Dummy
			case 55: // Rabbit
			case 56: //	Cat
				return;
			}
			// 9000 default; the lower the greater the Weapon/Armor/Wand Drop
			// 35% Drop 40% of that is an Item 
			if (iDice(1, 100) <= m_iSecondaryDropRate) {
				// 40% Drop 90% of that is a standard drop
				// Standard Drop Calculation: (35/100) * (40/100) * (90/100) = 12.6%
				iResult = iDice(1, 1000);
				if ((iResult >= 1) && (iResult < 300))          dwValue = 1;
				else if ((iResult >= 300) && (iResult < 400))  dwValue = 2;
				else if ((iResult >= 400) && (iResult < 550))  dwValue = 3;
				else if ((iResult >= 550) && (iResult < 700))  dwValue = 4;
				else if ((iResult >= 700) && (iResult < 850))  dwValue = 5;
				else if ((iResult >= 850) && (iResult < 920))  dwValue = 6;
				else if ((iResult >= 920) && (iResult < 980))  dwValue = 7;
				else if ((iResult >= 980) && (iResult <= 1000)) dwValue = 8;
				
				switch (dwValue) {
				case 1: // Green Potion
				case 4: iItemID = 96; break; // Big Green Potion
				case 2: // Red Potion
				case 5: iItemID = 92; break; // Big Red Potion
				case 3: // Blue Potion
				case 6: iItemID = 94; break; // Big Blue Potion
				case 7: iItemID = 1000; break; // Kloness Potion
				case 8: 
					switch (iDice(1, 3)) {
						case 1: iItemID = 650; break; // Zemstone of Sacrifice
						case 2: iItemID = 656; break; // Xelima Stone
						case 3: iItemID = 657; break; // Merien Stone
					}
					break;

				}
				// If a non-existing item is created then delete the item
				pItem = new class CItem;
				if (_bInitItemAttr(pItem, iItemID) == false) {
					delete pItem;
					return;
				}
			}
			else {
				// Valuable Drop Calculation: (35/100) * (40/100) * (10/100) = 1.4%
				// Define iGenLevel using Npc.cfg#
				switch (m_pNpcList[iNpcH]->m_sType) {

				case 10: // Slime
				case 16: // Giant-Ant
				case 22: // Amphis
				//case 55: // Rabbit
				//case 56: //	Cat
					iGenLevel = 1;
					break;

				case 11: // Skeleton
				case 14: // Orc, Orc-Mage
				case 17: // Scorpion
				case 18: // Zombie
					iGenLevel = 2;
					break;

				case 12: // Stone-Golem
				case 23: // Clay-Golem
					iGenLevel = 3;
					break;

				case 27: // Hellbound
				case 61: // Rudolph
					iGenLevel = 4;
					break;

				
				case 76: // Giant-Plant
				case 74: // Giant-Crayfish
				case 13: // Cyclops
				case 28: // Troll
				case 53: // Beholder
				case 60: // Cannibal-Plant
				case 62: // DireBoar
					iGenLevel = 5;
					break;

				case 29: // Orge
				case 33: // WereWolf
				case 48: // Stalker
				case 54: // Dark-Elf
				case 65: // Ice-Golem
				
					iGenLevel = 6;
					break;

				
				case 30: // Liche
				case 63: // Frost
				
					iGenLevel = 7;
					break;

				case 31: // Demon
				case 32: // Unicorn
				case 49: // Hellclaw
				case 50: // Tigerworm
				case 52: // Gagoyle
					iGenLevel = 8;
					break;

				case 59: // Ettin
				case 58: // MountainGiant
				case 72: // Claw-Turtle
				case 78: // Minotaurus
					iGenLevel = 9;
					break;

				
				case 77: // MasterMage-Orc
				case 75: // Lizards
				case 70: // Balrogs
				case 71: // Centaurus
				case 79: // Nizie
					iGenLevel = 10;
					break;

				default:
					iGenLevel = 0;
					break;
				}

				if (iGenLevel == 0) return;

				// Weapon Drop: 
				// 1.4% chance Valuable Drop 60% that it is a Weapon
				if (iDice(1, 100) <= m_iPrimaryDropRate) {
					if (iDice(1, 100) <= m_iSecondaryDropRate) {
						// 70% the Weapon is Melee
						switch (iGenLevel) {

						//Nixu : aca se manejan los Drop..

						case 1: // Slime, Giant-Ant, Amphis
							switch (iDice(1, 5)) {
								case 1: iItemID = 1;  break; // Dagger
								case 2: iItemID = 8;  break; // ShortSword
								case 3: iItemID = 59; break; // LightAxe
								case 4: iItemID = 316;  break; // SilverNecklace
								case 5: iItemID = 332;  break; // SilverRing
							}
							break;

						case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
							switch (iDice(1, 8)) {
								case 1: iItemID = 12;  break; // MainGauche
								case 2: iItemID = 15;  break; // Gradius
								case 3: iItemID = 65;  break; // SexonAxe
								case 4: iItemID = 62;  break; // Tomahoc
								case 5: iItemID = 23;  break; // Sabre
								case 6: iItemID = 31;  break; // Esterk
								case 7: iItemID = 316;  break; // SilverNecklace
								case 8: iItemID = 332;  break; // SilverRing
							}
							break;

						case 3: // Stone-Golem, Clay-Golem
							switch (iDice(1, 6)) {
								case 1: iItemID = 17;  break; // LongSword
								case 2: iItemID = 68;  break; // DoubleAxe
								case 3: iItemID = 23;  break; // Sabre
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 316;  break; // SilverNecklace
								case 6: iItemID = 332;  break; // SilverRing
							}
							break;

						case 4: // Hellbound, Rudolph
							switch (iDice(1, 7)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 28;  break; // Falchion
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 316;  break; // SilverNecklace
								case 7: iItemID = 332;  break; // SilverRing
							}
							break;

						case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
							switch (iDice(1, 5)) {
								case 1: iItemID = 31;  break; // Esterk
								case 2: iItemID = 34;  break; // Rapier
								case 3: iItemID = 71;  break; // WarAxe
								case 4: iItemID = 315;  break; // GoldNecklace
								case 5: iItemID = 331;  break; // GoldRing
							}
							break;

						case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
							switch (iDice(1, 11)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 46;  break; // Claymore
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 617; break; // CompositeBow
								case 7: iItemID = 1044; break; // PlateBow
								case 8: iItemID = 1045; break; // KnightBow
								case 9: iItemID = 1046; break; // GiantBow
								case 10: iItemID = 315;  break; // GoldNecklace
								case 11: iItemID = 331;  break; // GoldRing
							}
							break;

						case 7: // Liche, Frost
							switch (iDice(1, 6)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 31;  break; // Esterk
								case 4: iItemID = 34;  break; // Rapier
								case 5: iItemID = 316;  break; // SilverNecklace
								case 6: iItemID = 332;  break; // SilverRing
							}
							break;

						case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
							switch (iDice(1, 10)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 560; break; // BattleAxe
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 55;  break; // Flameberge+1
								case 7: iItemID = 615; break; // GiantSword
								case 8: iItemID = 1045; break; // KnightBow
								case 9: iItemID = 315;  break; // GoldNecklace
								case 10: iItemID = 331;  break; // GoldRing
							}
							break;

						case 9: // MountainGiant
							switch (iDice(1, 8)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 28;  break; // Falchion
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								case 6: iItemID = 760; break; // Hammer
								case 7: iItemID = 316;  break; // SilverNecklace
								case 8: iItemID = 332;  break; // SilverRing
							}
							break;

						case 10: 
							switch (iDice(1, 7)) {
								case 1: iItemID = 46;  break; // Claymore
								case 2: iItemID = 31;  break; // Esterk
								case 3: iItemID = 34;  break; // Rapier
								case 4: iItemID = 760; break; // Hammer
								case 5: iItemID = 761; break; // BattleHammer
								case 6: iItemID = 315;  break; // GoldNecklace
								case 7: iItemID = 331;  break; // GoldRing
							}
							break;

						}
					}
					else {
						// 30% the weapon is a Wand
						switch (iGenLevel) {

						case 2:
						case 3:
						case 9:
						case 10:
							iItemID = 258; // MagicWand(MS0) 
							break; 
						case 4:
						case 5:
						case 6:
							iItemID = 257; // MagicWand(MS10) 
							break; 
						case 7:
						case 8:
							iItemID = 256; // MagicWand(MS20) 
							break; 
						}
					}
				}
				else {
					// 1.4% chance Valuable Drop 40% that drop is an Armor/Shield
					switch (iGenLevel) {

					case 1: // Slime, Giant-Ant, Amphis
					case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
						switch (iDice(1, 4)) {
							case 1: iItemID = 79;  break; // WoodShield
							case 2: iItemID = 80;  break; // LeatherShield
							case 3: iItemID = 450; break; // Shoes
							case 4: iItemID = 451; break; // LongBoots
						}
						break;

					case 3: // Stone-Golem, Clay-Golem
						iItemID = 81; // TargeShield
						break;

					case 4: // Hellbound, Rudolph
						switch (iDice(1, 5)) {
							case 1: iItemID = 454; break; // Hauberk(M)
							case 2: iItemID = 472; break; // Hauberk(W)
							case 3: iItemID = 461; break; // ChainHose(M)
							case 4: iItemID = 482; break; // ChainHose(W)
							case 5: iItemID = 83;  break; // BlondeShield
						}
						break;

					case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
						switch (iDice(1, 9)) {
							case 1: iItemID = 455; break; // LeatherArmor(M)
							case 2: iItemID = 475; break; // LeatherArmor(W)
							case 3: iItemID = 84;  break; // IronShield
							case 4: iItemID = 454; break; // Hauberk(M)
							case 5: iItemID = 472; break; // Hauberk(W)
							case 6: iItemID = 461; break; // ChainHose(M)
							case 7: iItemID = 482; break; // ChainHose(W)
							case 8: iItemID = 623; break; // Hood(M)
							case 9: iItemID = 624; break; // Hood(W)
						}
						break;

					case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
						switch (iDice(1, 21)) {
							case 1: iItemID = 456; break; // ChainMail(M)
							case 2: iItemID = 476; break; // ChainMail(W)
							case 3: iItemID = 458; break; // PlateMail(M)
							case 4: iItemID = 478; break; // PlateMail(W)
							case 5: iItemID = 85; break; // LagiShield
							case 6: iItemID = 750; break; // Horned-Helm(M)
							case 7: iItemID = 751; break; // Wings-Helm(M)
							case 8: iItemID = 754; break; // Horned-Helm(W)
							case 9: iItemID = 755; break; // Wings-Helm(W)
							case 10: iItemID = 752; break; // Wizard-Cap(M) 
							case 11: iItemID = 753; break; // Wizard-Hat(M)
							case 12: iItemID = 756; break; // Wizard-Cap(W) 
							case 13: iItemID = 757; break; // Wizard-Hat(W) 
							case 14: iItemID = 454; break; // Hauberk(M)
							case 15: iItemID = 472; break; // Hauberk(W)
							case 16: iItemID = 461; break; // ChainHose(M)
							case 17: iItemID = 482; break; // ChainHose(W)
							case 18: iItemID = 623; break; // Hood(M)
							case 19: iItemID = 624; break; // Hood(W)
							case 20: iItemID = 455; break; // LeatherArmor(M)
							case 21: iItemID = 475; break; // LeatherArmor(W)

						}
						break;

					case 7: // Liche, Frost
						switch (iDice(1, 12)) {
							case 1: iItemID = 457; break; // ScaleMail(M)
							case 2: iItemID = 477; break; // ScaleMail(W)
							case 3: iItemID = 458; break; // PlateMail(M)
							case 4: iItemID = 478; break; // PlateMail(W)
							case 5: iItemID = 86; break; // KnightShield
							case 6: iItemID = 87; break; // TowerShield
							case 7: iItemID = 600; break; // Helm(M)
							case 8: iItemID = 602; break; // Helm(W)
							case 9: iItemID = 601; break; // Full-Helm(M)
							case 10: iItemID = 603; break; // Full-Helm(W)
							case 11: iItemID = 623; break; // Hood(M)
							case 12: iItemID = 624; break; // Hood(W)

						}
						break;

					case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
						iItemID = 402; // Cape
						break;

					// Centu - Mountain-Giant, Ettin, MasterMage-Orc, Giant-Lizard
					case 9:
					
						switch (iDice(1, 23)) {
						case 1: iItemID = 456; break; // ChainMail(M)
						case 2: iItemID = 476; break; // ChainMail(W)
						case 3: iItemID = 458; break; // PlateMail(M)
						case 4: iItemID = 478; break; // PlateMail(W)
						case 5: iItemID = 85; break; // LagiShield
						case 6: iItemID = 600; break; // Horned-Helm(M)
						case 7: iItemID = 601; break; // Wings-Helm(M)
						case 8: iItemID = 602; break; // Horned-Helm(W)
						case 9: iItemID = 603; break; // Wings-Helm(W)
						case 10: iItemID = 752; break; // Wizard-Cap(M) 
						case 11: //break;
						case 12: iItemID = 756; break; // Wizard-Cap(W) 
						case 13: //break;
						case 14: iItemID = 454; break; // Hauberk(M)
						case 15: iItemID = 472; break; // Hauberk(W)
						case 16: iItemID = 461; break; // ChainHose(M)
						case 17: iItemID = 482; break; // ChainHose(W)
						case 18: iItemID = 623; break; // Hood(M)
						case 19: iItemID = 624; break; // Hood(W)
						case 20: iItemID = 455; break; // LeatherArmor(M)
						case 21: iItemID = 475; break; // LeatherArmor(W)
						case 22: iItemID = 462; break; // PlateLeggings(M)
						case 23: iItemID = 483; break; // PlateLeggings(W)

						}
						break;

					case 10:
						switch (iDice(1, 23)) {
						case 1: iItemID = 685; break; // WizardRobe(M)
						case 2: iItemID = 686; break; // WizardRobe(W)
						case 3: iItemID = 675; break; // KnightPlateMail(M)
						case 4: iItemID = 676; break; // KnightPlateMail(W)
						case 5: iItemID = 87; break; // TowerShield
						case 6: iItemID = 750; break; // Horned-Helm(M)
						case 7: iItemID = 751; break; // Wings-Helm(M)
						case 8: iItemID = 754; break; // Horned-Helm(W)
						case 9: iItemID = 755; break; // Wings-Helm(W)
						case 10: iItemID = 752; break; // Wizard-Cap(M) 
						case 11: iItemID = 753; break; // Wizard-Hat(M)
						case 12: iItemID = 756; break; // Wizard-Cap(W) 
						case 13: iItemID = 757; break; // Wizard-Hat(W) 
						case 14: iItemID = 681; break; // WizardHauberk(M)
						case 15: iItemID = 682; break; // WizardHauberk(W)
						case 16: iItemID = 687; break; // KnightHauberk(M)
						case 17: iItemID = 688; break; // KnightHauberk(W)
						case 18: iItemID = 662; break; // ArcherHood(M)
						case 19: iItemID = 664; break; // ArcherHood(W)
						case 20: iItemID = 661; break; // ArcherArmor(M)
						case 21: iItemID = 663; break; // ArcherArmor(W)
						case 22: iItemID = 677; break; // KnightPlateLeg(M)
						case 23: iItemID = 678; break; // KnightPlateLeg(W)

						}
						break;
					}
				}

				pItem = new class CItem;
				if (_bInitItemAttr(pItem, iItemID) == false) {
					delete pItem;
					return;
				}

				//Magn0S:: Begin to update Drop Management from .cfg file
				if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK || pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW) {
					iResult = iDice(1, 1000); 
					if ((iResult >= 1) && (iResult <= 29)) {
						dwType = ITEMSTAT_LIGHT;
						cColor = 2;
					}
					else if ((iResult >= 30) && (iResult <= 99)) {
						dwType = ITEMSTAT_STRONG;
						cColor = 3;
					}
					else if ((iResult >= 100) && (iResult <= 249)) {
						dwType = ITEMSTAT_CRITICAL;
						cColor = 5;
					}
					else if ((iResult >= 250) && (iResult <= 449)) {
						dwType = ITEMSTAT_AGILE;
						cColor = 1;
					}
					else if ((iResult >= 450) && (iResult <= 649)) {
						dwType = ITEMSTAT_RIGHTEOUS;
						cColor = 7;
					}
					else if ((iResult >= 650) && (iResult <= 809)) {
						dwType = ITEMSTAT_POISONING;
						cColor = 4;
					}
					else if ((iResult >= 810) && (iResult <= 969)) {
						dwType = ITEMSTAT_SHARP;
						cColor = 6;
					}
					else if ((iResult >= 970) && (iResult <= 1000)) {
						dwType = ITEMSTAT_ANCIENT;
						cColor = 8;
					}

					pItem->m_cItemColor = cColor;

					iResult = iDice(1, 3000);
					if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
					else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
					else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
					else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
					else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
					else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
					else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
					else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
					else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
					else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
					else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
					else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
					else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
					else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
					else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
					else dwValue = 1; // v2.03 906

					switch (dwType) {
					case ITEMSTAT_CRITICAL: // 
					case ITEMSTAT_POISONING: // Min +20
						if (dwValue <= 4) dwValue = 4;
						if (dwValue > m_iMaxHPCrit) dwValue = m_iMaxHPCrit;
						break;
					case ITEMSTAT_LIGHT: // Min +14%
					case ITEMSTAT_STRONG: // Min +14%					
						if (dwValue <= 2) dwValue = 2;
						break;
					}
					// Max 7
					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					// Main
					pItem->m_dwAttribute = 0;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					// Rare Drop 40%
					if (iDice(1, 100) <= m_iRareDropRate) {
						//  Hit Prob(50%),  CAD(35%),  Gold(10%), Exp(5%)
						iResult = iDice(1, 1000);
						if ((iResult >= 1) && (iResult <= 499))       dwType = ITEMSTAT2_HITPROB;
						else if ((iResult >= 500) && (iResult <= 849)) dwType = ITEMSTAT2_CAD;
						else if ((iResult >= 850) && (iResult <= 949)) dwType = ITEMSTAT2_GOLD;
						else if ((iResult >= 950) && (iResult <= 1000)) dwType = ITEMSTAT2_EXP;

						iResult = iDice(1, 3000);
						if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
						else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
						else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
						else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
						else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
						else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
						else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
						else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
						else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
						else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
						else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
						else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
						else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
						else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
						else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
						else dwValue = 1; // v2.03 906

						switch (dwType) {
						case ITEMSTAT2_HITPROB: // Min Hip Prob +21%
							if (dwValue <= 3) dwValue = 3;
							if (dwValue > m_iMaxHPCrit) dwValue = m_iMaxHPCrit;
							break;
						case ITEMSTAT2_CAD: // CAD Vai do +1  at?+7 s?							//if (dwValue > 7) dwValue = 7;
							break;
						case ITEMSTAT2_EXP: // Exp  +20%
							//dwValue = 2;
							break;
						case ITEMSTAT2_GOLD: // Gold +50%
							//dwValue = 5;
							break;
						}

						// Demais stats max ?7 (para mobs fracos iGenLevel)
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						dwType = dwType << 12;
						dwValue = dwValue << 8;

						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}
				else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_BIJOUTERIE) { // necks & rings
					iResult = iDice(1, 1000);
					if ((iResult >= 1) && (iResult <= 299))  dwType = ITEMSTAT_MANACONV;
					else if ((iResult >= 300) && (iResult <= 599)) dwType = ITEMSTAT_CRITICAL;
					else if ((iResult >= 600) && (iResult <= 799)) dwType = ITEMSTAT_CRITICAL2;
					else if ((iResult >= 800) && (iResult <= 1000)) dwType = ITEMSTAT_CASTPROB;

					iResult = iDice(1, 3000);
					if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
					else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
					else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
					else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
					else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
					else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
					else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
					else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
					else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
					else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
					else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
					else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
					else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
					else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
					else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
					else dwValue = 1; // v2.03 906

					switch (dwType) {
					case ITEMSTAT_CRITICAL: // 
						if (dwValue <= 5) dwValue = 5;
						if (dwValue > m_iMaxHPCrit) dwValue = m_iMaxHPCrit;
						break;
					case ITEMSTAT_MANACONV:
					case ITEMSTAT_CRITICAL2:
						/*dwValue = (dwValue + 1) / 2;
						if (dwValue < 1) dwValue = 1;*/
						if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
						break;
					}
					// Max = 7
					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					pItem->m_dwAttribute = 0;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					// 40%
					if (iDice(1, 100) <= m_iRareDropRate) {

						// Poison R.(1),  Hit Prob(2), DR(3), HP(4), SP(5), MP(6),  MR(7),  PA(8), MA(9), CAD(10),  Exp(11), Gold(12)
						iResult = iDice(1, 1000);
						if ((iResult >= 1) && (iResult <= 299))       dwType = ITEMSTAT2_HITPROB;
						else if ((iResult >= 300) && (iResult <= 399)) dwType = ITEMSTAT2_CAD;
						else if ((iResult >= 400) && (iResult <= 499)) dwType = ITEMSTAT2_GOLD;
						else if ((iResult >= 500) && (iResult <= 699)) dwType = ITEMSTAT2_EXP;
						else if ((iResult >= 700) && (iResult <= 799))  dwType = ITEMSTAT2_SPREC;	// 16
						else if ((iResult >= 800) && (iResult <= 899))  dwType = ITEMSTAT2_HPREC;	// 23
						else if ((iResult >= 900) && (iResult <= 1000))  dwType = ITEMSTAT2_MPREC;	// 23 

						iResult = iDice(1, 3000);
						if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
						else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
						else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
						else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
						else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
						else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
						else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
						else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
						else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
						else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
						else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
						else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
						else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
						else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
						else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
						else dwValue = 1; // v2.03 906

						switch (dwType) {
						case ITEMSTAT2_HITPROB: // Min Hip Prob +21%
							if (dwValue <= 3) dwValue = 3;
							if (dwValue > m_iMaxHPCrit) dwValue = m_iMaxHPCrit;
							break;
						case ITEMSTAT2_CAD: // CAD Vai do +1  at?+7 s?							//if (dwValue > 7) dwValue = 7;
							break;
						case ITEMSTAT2_EXP: // Exp  +20%
							//dwValue = 2;
							break;
						case ITEMSTAT2_GOLD: // Gold +50%
							//dwValue = 5;
							break;
						case ITEMSTAT2_SPREC:
						case ITEMSTAT2_MPREC:
						case ITEMSTAT2_HPREC:
							if (dwValue > m_iMaxRec) dwValue = m_iMaxRec;

							break;
						}

						// Max = 7
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						dwType = dwType << 12;
						dwValue = dwValue << 8;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}
				else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {

					dwType = ITEMSTAT_CASTPROB;
					cColor = 5;

					pItem->m_cItemColor = cColor;

					iResult = iDice(1, 3000);
					if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
					else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
					else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
					else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
					else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
					else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
					else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
					else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
					else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
					else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
					else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
					else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
					else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
					else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
					else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
					else dwValue = 1; // v2.03 906

					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					pItem->m_dwAttribute = 0;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					if (iDice(1, 100) <= m_iRareDropRate) {
						iResult = iDice(1, 1000);
						if ((iResult >= 1) && (iResult <= 499))      dwType = ITEMSTAT2_HITPROB;
						else if ((iResult >= 500) && (iResult <= 849)) dwType = ITEMSTAT2_CAD;
						else if ((iResult >= 850) && (iResult <= 949)) dwType = ITEMSTAT2_GOLD;
						else if ((iResult >= 950) && (iResult <= 1000)) dwType = ITEMSTAT2_EXP;

						iResult = iDice(1, 3000);
						if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
						else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
						else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
						else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
						else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
						else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
						else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
						else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
						else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
						else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
						else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
						else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
						else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
						else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
						else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
						else dwValue = 1; // v2.03 906

						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						switch (dwType) {
						case ITEMSTAT2_HITPROB: // Hit Prob Min +21%
							if (dwValue <= 3) dwValue = 3;
							if (dwValue > m_iMaxHPCrit) dwValue = m_iMaxHPCrit;
							break;
						case ITEMSTAT2_CAD: // Cad vai de +1 at?+7
							//if (dwValue > 7) dwValue = 7;
							break;
						case ITEMSTAT2_EXP: // Exp set +20%
							//dwValue = 2;
							break;
						case ITEMSTAT2_GOLD: // Gold Set +50%
							//dwValue = 5;
							break;
						}

						dwType = dwType << 12;
						dwValue = dwValue << 8;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}
				else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE) {

					//  Strong (60%), Light (30%), Mana Conv.(5%), Crit Increased(5%) (Drops prob.)
					iResult = iDice(1, 1000);
					if ((iResult >= 1) && (iResult <= 599))          dwType = ITEMSTAT_STRONG;
					else if ((iResult >= 600) && (iResult <= 899))  dwType = ITEMSTAT_LIGHT;
					else if ((iResult >= 900) && (iResult <= 954))  dwType = ITEMSTAT_MANACONV; 
					else if ((iResult >= 955) && (iResult <= 1000)) dwType = ITEMSTAT_CRITICAL2; 

					iResult = iDice(1, 3000);
					if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
					else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
					else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
					else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
					else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
					else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
					else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
					else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
					else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
					else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
					else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
					else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
					else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
					else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
					else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
					else dwValue = 1; // v2.03 906

					switch (dwType) {
					case ITEMSTAT_LIGHT: // Min +16%
						if (dwValue <= 4) dwValue = 4;
						break;
					case ITEMSTAT_STRONG: // Min +14%
						if (dwValue <= 2) dwValue = 2;
						break;

					case ITEMSTAT_MANACONV:
					case ITEMSTAT_CRITICAL2:
						/*dwValue = (dwValue + 1) / 2;
						if (dwValue < 1) dwValue = 1;*/
						if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
						break;
					}
					// Max = 7
					if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

					pItem->m_dwAttribute = 0;
					dwType = dwType << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

					// 40%
					if (iDice(1, 100) <= m_iRareDropRate) {

						// Poison R.(1),  Hit Prob(2), DR(3), HP(4), SP(5), MP(6),  MR(7),  PA(8), MA(9), CAD(10),  Exp(11), Gold(12)
						iResult = iDice(1, 1000);
						if ((iResult >= 1) && (iResult <= 99))       dwType = ITEMSTAT2_DEF;	// 10
						else if ((iResult >= 100) && (iResult <= 399))  dwType = ITEMSTAT2_PSNRES;		// 12
						else if ((iResult >= 400) && (iResult <= 549))  dwType = ITEMSTAT2_SPREC;	// 16
						else if ((iResult >= 550) && (iResult <= 649))  dwType = ITEMSTAT2_HPREC;	// 23
						else if ((iResult >= 650) && (iResult <= 749))  dwType = ITEMSTAT2_MPREC;	// 23 
						else if ((iResult >= 750) && (iResult <= 939))  dwType = ITEMSTAT2_MR;		// 12
						else if ((iResult >= 940) && (iResult <= 979))  dwType = ITEMSTAT2_PA;		// 3
						else if ((iResult >= 980) && (iResult <= 1000)) dwType = ITEMSTAT2_MA;		// 1

						//Magn0S:: Added to ability some stats drops
						if ((m_bNullDrop[DROP_MA] == false) && (dwType == ITEMSTAT2_MA))
							dwType = ITEMSTAT2_NONE;

						if ((m_bNullDrop[DROP_PA] == false) && (dwType == ITEMSTAT2_PA))
							dwType = ITEMSTAT2_NONE;

						iResult = iDice(1, 3000);
						if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
						else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
						else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
						else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
						else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
						else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
						else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
						else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
						else if ((iResult >= 2930) && (iResult < 2950))  dwValue = 9;  // 360/29348 = 1.2%
						else if ((iResult >= 2950) && (iResult < 2960))  dwValue = 10; // 237/29348 = 0.8%
						else if ((iResult >= 2960) && (iResult < 2970))  dwValue = 11; // 156/29348 = 0.5%
						else if ((iResult >= 2970) && (iResult < 2980))  dwValue = 12; // 103/29348 = 0.3%
						else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 13; // 68/29348 = 0.1%
						else if ((iResult >= 2990) && (iResult < 2995))  dwValue = 14; // 68/29348 = 0.1%
						else if ((iResult >= 2995) && (iResult <= 3000))  dwValue = 15; // 68/29348 = 0.1%
						else dwValue = 1; // v2.03 906

						switch (dwType) {
						case ITEMSTAT2_SPREC:
						case ITEMSTAT2_MPREC:
						case ITEMSTAT2_HPREC:
							if (dwValue > m_iMaxRec) dwValue = m_iMaxRec;

							break;

						case 1: // Poison min +21%
						case 3: // DR Min +21%
						case 7: // MR Min +21%
						
							if (dwValue <= 3) dwValue = 3;
							if (dwValue > m_iMaxResist) dwValue = m_iMaxResist;
							
							break;

						case 8: // PA Min +9%
						case 9: // MA Min +9%
							if (dwValue <= 3) dwValue = 3;

							if (dwValue > m_iMaxAbs) dwValue = m_iMaxAbs;

							//Magn0S:: Add Elements
							iResult = iDice(1, 1020);
							if ((iResult >= 1) && (iResult < 275)) sElement = 1;  //Earth 27%
							else if ((iResult >= 275) && (iResult < 550)) sElement = 2;  //Light 27%
							else if ((iResult >= 550) && (iResult < 650)) sElement = 3;  //Fire 10%
							else if ((iResult >= 650) && (iResult < 925)) sElement = 4;  //Water 27%
							else if ((iResult >= 925) && (iResult < 962)) sElement = 5;  //Holy 04%
							else if ((iResult >= 962) && (iResult < 1000)) sElement = 6;  //Unholy 04%
							else if ((iResult >= 1000) && (iResult <= 1020)) sElement = 7;  //Unatunned 02%
							else sElement = 7;
							pItem->m_sNewEffect1 = sElement;
							break;
							
						}
						// Max = 7
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						dwType = dwType << 12;
						dwValue = dwValue << 8;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
					}
				}

				_AdjustRareItemValue(pItem);
			}
		}

		pItem->m_sTouchEffectType = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = iDice(1, 100000);
		pItem->m_sTouchEffectValue2 = iDice(1, 100000);

		SYSTEMTIME SysTime;
		char cTemp[256];
		GetLocalTime(&SysTime);
		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
		pItem->m_sTouchEffectValue3 = atoi(cTemp);

		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX,
			m_pNpcList[iNpcH]->m_sY,
			pItem);

		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
			m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ //v1.4 color

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
			m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

		_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, 0, pItem);
	}
}

// October 19,2004 - 3.51 translated
void CGame::NpcBehavior_Dead(int iNpcH)
{
	DWORD dwTime;

	if (m_pNpcList[iNpcH] == 0) return;

	dwTime = timeGetTime();
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount++;
	if (m_pNpcList[iNpcH]->m_sBehaviorTurnCount > 5) {
		m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	}

	if ((dwTime - m_pNpcList[iNpcH]->m_dwDeadTime) > m_pNpcList[iNpcH]->m_dwRegenTime)
		DeleteNpc(iNpcH);
}


bool CGame::_bDecodeNpcItemConfigFileContents(char* cFn)
{
	FILE* pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	char* cp, * token, cGSMode[16] = "";
	char seps[] = "= \t\n";
	class CStrTok* pStrTok;
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iNpcConfigListIndex = 0, k = 0;
	class CNpcItem* pTempNpcItem = 0;

	hFile = CreateFile(cFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(cFn, "rt");
	if (pFile != 0) {

		PutLogList("(!) NpcItemConfig file contents received. Now decoding...");
		cp = new char[dwFileSize + 2];
		ZeroMemory(cp, dwFileSize + 2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();
		while (token != 0) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:

					switch (cReadModeB) {

					case 1:
						// NPC 捞抚
						if (strlen(token) > 20) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Too long Npc name.");
							delete[] cp;
							delete pStrTok;
							return false;
						}
						for (iNpcConfigListIndex = 0; iNpcConfigListIndex < DEF_MAXNPCTYPES; iNpcConfigListIndex++)
							if (m_pNpcConfigList[iNpcConfigListIndex] != 0) {
								if (strcmp(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, token) == 0) {
									ZeroMemory(G_cTxt, sizeof(G_cTxt));
									wsprintf(G_cTxt, "%s... Ok!", token);
									PutLogList(G_cTxt);
									break;
								}
							}
						if (iNpcConfigListIndex == DEF_MAXNPCTYPES) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - No exist Npc Name");
							delete[] cp;
							delete pStrTok;
							return false;
						}
						cReadModeB = 2;
						break;

					case 2:
						if (strlen(token) > 2) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Type Error.");
							delete[] cp;
							delete pStrTok;
							return false;
						}
						m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemType = atoi(token);
						cReadModeB = 3;
						break;

					case 3:
						if (strlen(token) > 20) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Too long Item name.");
							delete[] cp;
							delete pStrTok;
							return false;
						}
						if (pTempNpcItem == 0)
							pTempNpcItem = new class CNpcItem();

						if (memcmp(token, "[ENDITEM]", 9) == 0) {
							cReadModeA = 0;
							cReadModeB = 0;

							if (pTempNpcItem != 0) {
								delete pTempNpcItem;
								pTempNpcItem = 0;
							}
							break;
						}

						strcpy(pTempNpcItem->m_cName, token);
						if (!m_bReceivedItemList)	// ITEM List啊 坷瘤 臼疽促.
						{
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration error - Before Item List receiving.");
							delete[] cp;
							delete pStrTok;
							return false;
						}

						for (k = 0; k < DEF_MAXITEMTYPES; k++)
						{
							if (m_pItemConfigList[k] == 0)
								continue;

							if (strcmp(token, m_pItemConfigList[k]->m_cName) == 0) {
								pTempNpcItem->m_sItemID = m_pItemConfigList[k]->m_sIDnum;
								break;
							}
						}

						if (k == DEF_MAXITEMTYPES) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration error - Do Not exist in ITEM LIST");
							delete[] cp;
							delete pStrTok;
							return false;
						}

						cReadModeB = 4;
						break;

					case 4:
						// 霉锅掳 犬伏
						if (_bGetIsStringIsNumber(token) == false) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Wrong Data format.");
							delete[] cp;
							delete pStrTok;
							return false;
						}
						pTempNpcItem->m_sFirstProbability = atoi(token);

						if (pTempNpcItem->m_sFirstProbability <= 0) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - First probability have wrong value");
							delete[] cp;
							delete pStrTok;
							return false;
						}

						// 霉锅掳 犬伏俊 措茄 鸥百蔼阑 烙狼肺 霖促. 

						cReadModeB = 5;
						break;

					case 5:
						// 滴锅掳 犬伏
						if (_bGetIsStringIsNumber(token) == false) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Wrong Data format.");
							delete[] cp;
							delete pStrTok;
							return false;
						}

						pTempNpcItem->m_sSecondProbability = atoi(token);

						if (pTempNpcItem->m_sSecondProbability <= 0) {
							PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file error - Second probability have wrong value");
							delete[] cp;
							delete pStrTok;
							return false;
						}

						// 滴锅掳 犬伏俊 措茄 鸥百蔼阑 烙狼肺 霖促. 

						cReadModeB = 3;

						// vector俊 泅犁 鳖瘤 佬绢 靛赴 蔼阑 持绰促. 
						m_pNpcConfigList[iNpcConfigListIndex]->m_vNpcItem.push_back(*pTempNpcItem);

						// 2002-09-17 #1 NPCITEM Type 2老 版快
						/*if (m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemMax < pTempNpcItem->m_sSecondProbability)
							m_pNpcConfigList[iNpcConfigListIndex]->m_iNpcItemMax = pTempNpcItem->m_sSecondProbability;*/

						break;

					} // switch #2
					break;

				default:
					break;

				} // switch #1
			} // if
			else {

				if (memcmp(token, "NpcItem", 7) == 0) {
					cReadModeA = 1;
					cReadModeB = 1;

				}
			}
			token = pStrTok->pGet();
		}

		delete pStrTok;
		delete[] cp;
		fclose(pFile);
	}
	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! NPCITEM configuration file contents error!");
		return false;
	}
	ZeroMemory(G_cTxt, sizeof(G_cTxt));
	wsprintf(G_cTxt, "(!) NPC-Item(Total:%d) configuration - success!", iNpcConfigListIndex);
	PutLogList(G_cTxt);
	return true;

}
bool CGame::bGetItemNameWhenDeleteNpc(int& iItemID, short sNpcType)
{
	if (m_bNpcItemConfig) {

		class    CNpcItem	CTempNpcItem;
		int    iResult;
		int    iNpcIndex;

		bool	bFirstDice = false, bSecondDice = false;

		for (iNpcIndex = 0; iNpcIndex < DEF_MAXNPCTYPES; iNpcIndex++) {
			if (m_pNpcConfigList[iNpcIndex] != 0) {
				if (m_pNpcConfigList[iNpcIndex]->m_sType == sNpcType) break;
			}
		}

		if (iNpcIndex == DEF_MAXNPCTYPES) return false;

		if (m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size() <= 0)  return false;

		switch (m_pNpcConfigList[iNpcIndex]->m_iNpcItemType) {
		case 1:
			// 汲沥 颇老狼 咯矾 俺狼 酒捞袍 吝 窍唱甫 绊弗促.
			iResult = iDice(1, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size()) - 1;

			CTempNpcItem = m_pNpcConfigList[iNpcIndex]->m_vNpcItem.at(iResult);

			// centu - fixed que lea probabilidades
			if (iDice(1, 10000) == CTempNpcItem.m_sFirstProbability) bFirstDice = true;
			if (iDice(1, 10000) == CTempNpcItem.m_sSecondProbability) bSecondDice = true;
		
			if (bFirstDice && bSecondDice) {
				iItemID = CTempNpcItem.m_sItemID;

				wsprintf(G_cTxt, "NpcType 1 (%d) size(%d) %s(%d) (%d)", sNpcType, m_pNpcConfigList[iNpcIndex]->m_vNpcItem.size(), CTempNpcItem.m_cName, CTempNpcItem.m_sItemID, iItemID);
				PutLogList(G_cTxt);

			}
			else iItemID = -1;
			break;

		case 2:
			
			break;

		} // switch

		if (iItemID <= 0)
			return false;

		return true;

	}
	else {
		int iResult;
		// NPC毳?靷牅頃犽晫 韸轨垬 鞎勳澊韰滌潉 氚滌儩鞁滍偓 瓴冹澑歆€鞚?鞐秬毳?瓿勳偘頃滊嫟. 

		switch (sNpcType) {
		case 49: // Hellclaw 
			switch (iDice(1, 18)) {
			case 1: iItemID = 610; break; // XelimaBlade
			case 2: iItemID = 611; break; // XelimaAxe
			case 3: iItemID = 612; break; // XelimaRapier
			case 4: iItemID = 620; break; // MerienShield
			case 5: iItemID = 622; break; // MerienPlateMailW
			case 6: iItemID = 621; break; // MerienPlateMailM
			case 7: iItemID = 642; break; // KnecklaceOfIcePro
			case 8: iItemID = 640; break; // KnecklaceOfSufferent
			case 9: iItemID = 637; break; // KnecklaceOfLightPro
			case 10: iItemID = 614; break; // SwordofIceElemental
			case 11: iItemID = 634; break; // RingofWizard
			case 12: iItemID = 635; break; // RingofMage
			case 13: iItemID = 335; break; // EmeraldRing
			case 14: iItemID = 311; break; // MagicNecklace(DF+10)
			case 15: iItemID = 300; break; // MagicNecklace(RM10)
			case 16: iItemID = 337; break; // RubyRing
			case 17: iItemID = 308; break; // MagicNecklace(MS10)
			case 18: iItemID = 259; break; // MagicWand(M.Shield)
			}
			break;

		case 50: // Tigerworm
			switch (iDice(1, 19)) {
			case 1: iItemID = 610; break; // XelimaBlade
			case 2: iItemID = 611; break; // XelimaAxe
			case 3: iItemID = 612; break; // XelimaRapier
			case 4: iItemID = 620; break; // MerienShield
			case 5: iItemID = 622; break; // MerienPlateMailW
			case 6: iItemID = 621; break; // MerienPlateMailM
			case 7: iItemID = 642; break; // KnecklaceOfIcePro
			case 8: iItemID = 640; break; // KnecklaceOfSufferent
			case 9: iItemID = 637; break; // KnecklaceOfLightPro
			case 10: iItemID = 614; break; // SwordofIceElemental
			case 11: iItemID = 634; break; // RingofWizard
			case 12: iItemID = 635; break; // RingofMage
			case 13: iItemID = 335; break; // EmeraldRing
			case 14: iItemID = 311; break; // MagicNecklace(DF+10)
			case 15: iItemID = 300; break; // MagicNecklace(RM10)
			case 16: iItemID = 337; break; // RubyRing
			case 17: iItemID = 308; break; // MagicNecklace(MS10)
			case 18: iItemID = 259; break; // MagicWand(M.Shield)
			case 19: iItemID = 1037; break; // StunSB
			}
			break;
		}

		if (iDice(1, 45) == m_iRareDropRate) {
			switch (sNpcType) {
			case 11: if (iDice(1, 550) != 11) return false; 	  // Skeleton   2 * 100	
			case 12: if (iDice(1, 400) != 11) return false; 	  // Stone-Golem 2 * 100	
			case 13: if (iDice(1, 100) != 11) return false; 	  // Cyclops  6 * 100	
			case 14: if (iDice(1, 700) != 11) return false; 	  // Orc 4 * 100	
			case 17: if (iDice(1, 600) != 11) return false; 	  // Scorpoin 5 * 100	
			case 18: if (iDice(1, 850) != 11) return false; 	  // Zombie 1 * 100	
			case 22: if (iDice(1, 600) != 11) return false; 	  // Amphis 5 * 100	
			case 23: if (iDice(1, 400) != 11) return false; 	  // Clay-Golem 2 * 100	
			case 27: if (iDice(1, 100) != 11) return false; 	  // Hellhound 7 * 100	
			case 28: if (iDice(1, 100) != 11) return false; 	  // Troll 5 * 100	
			case 29: if (iDice(1, 150) != 11) return false; 	  // Orge  7 * 100	
			case 30: if (iDice(1, 120) != 11) return false; 	  // Liche 1 * 100   
															  // Demon 5 * 100	
			case 32: if (iDice(1, 200) != 11) return false; 	  // Unicorn 5 * 100	
			case 33: if (iDice(1, 300) != 11) return false; 	  // WereWolf 7 * 100
			case 48: if (iDice(1, 100) != 11) return false; 	  // Stalker 
			case 52: if (iDice(1, 300) != 11) return false;    // Gagoyle
			case 53: if (iDice(1, 500) != 11) return false; 	  // Beholder
			case 54: if (iDice(1, 200) != 11) return false; 	  // Dark-Elf
			case 57: if (iDice(1, 400) != 11) return false; 	  // Giant-Frog
			case 63: if (iDice(1, 300) != 11) return false; 	  // Frost
			case 79: if (iDice(1, 200) != 11) return false; 	  // Nizie
			case 70: if (iDice(1, 200) != 11) return false; 	  // Barlog
			case 71: if (iDice(1, 200) != 11) return false; 	  // Centaurus
			default: break;
			}
		}
		else return false;

		switch (sNpcType) {

		case 11: // Skeleton
		case 17: // Scorpoin
		case 14: // Orc
		case 28: // Troll
		case 57: // Giant-Frog
			switch (iDice(1, 7)) {
			case 1: iItemID = 334; break; // LuckyGoldRing
			case 2: iItemID = 336; break; // SapphireRing
			case 3: if (iDice(1, 15) == 3) iItemID = 335; break; // EmeraldRing
			case 4: iItemID = 337; break; // RubyRing
			case 5: iItemID = 333; break; // PlatinumRing
			case 6: if (iDice(1, 15) == 3) iItemID = 634; break; // RingofWizard
			case 7: if (iDice(1, 25) == 3) iItemID = 635; break; // RingofMage
			}
			return true;

		case 13: // Cyclops
		case 27: // Hellhound
		case 29: // Orge
			switch (iDice(1, 7)) {
			case 1: iItemID = 311; break; // MagicNecklace(DF+10)
			case 2: if (iDice(1, 20) == 13) iItemID = 308; break; // MagicNecklace(MS10)
			case 3: if (iDice(1, 10) == 13) iItemID = 305; break; // MagicNecklace(DM+1)
			case 4: iItemID = 300; break; // MagicNecklace(RM10)
			case 5: if (iDice(1, 30) == 13) iItemID = 632; break; // RingofOgrepower
			case 6: if (iDice(1, 30) == 13) iItemID = 637; break; // KnecklaceOfLightPro
			case 7: if (iDice(1, 30) == 13) iItemID = 638; break; // KnecklaceOfFirePro
			}
			return true;

		case 18: // Zombie
		case 22: // Amphis
			switch (iDice(1, 4)) {
			case 1: if (iDice(1, 75) == 13) iItemID = 613; break; // SwordofMedusa
			case 2: if (iDice(1, 20) == 13) iItemID = 639; break; // KnecklaceOfPoisonPro
			case 3: if (iDice(1, 40) == 13) iItemID = 641; break; // KnecklaceOfMedusa
			case 4: if (iDice(1, 30) == 13) iItemID = 640; break; // KnecklaceOfSufferent
			}
			return true;

		case 12: // Stone-Golem
			switch (iDice(1, 5)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			case 5: if (iDice(1, 20) == 11) iItemID = 647; break; // KnecklaceOfStoneGolem
			}
			return true;

		case 23: // Clay-Golem
			switch (iDice(1, 4)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield	
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			}
			return true;

		case 32: // Unicorn
			switch (iDice(1, 5)) {
			case 1: if (iDice(1, 40) == 13) iItemID = 620; break; // MerienShield	
			case 2: if (iDice(1, 40) == 13) iItemID = 621; break; // MerienPlateMail(M)
			case 3: if (iDice(1, 40) == 13) iItemID = 622; break; // MerienPlateMail(W)
			case 4: if (iDice(1, 20) == 11) iItemID = 644; break; // KnecklaceOfAirEle
			case 5: if (iDice(1, 20) == 11) iItemID = 848; break; // Lighting Blade
			}
			return true;

		case 33: // WereWolf
		case 48: // Stalker
			switch (iDice(1, 2)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 290; break; // Flameberge+3(LLF)
			case 2: iItemID = 292; break; // GoldenAxe(LLF)
			}
			return true;

		case 30: // Liche
			switch (iDice(1, 8)) {
			case 1: if (iDice(1, 10) == 3) iItemID = 380; break; // IceStormManual
			case 2: iItemID = 259; break; // MagicWand(M.Shield)
			case 3: if (iDice(1, 30) == 3) iItemID = 291; break; // MagicWand(MS30-LLF)
			case 4: if (iDice(1, 10) == 3) iItemID = 614; break; // SwordofIceElemental	
			case 5: if (iDice(1, 10) == 3) iItemID = 642; break; // KnecklaceOfIcePro
			case 6: if (iDice(1, 15) == 3) iItemID = 643; break; // KnecklaceOfIceEle	
			case 7: if (iDice(1, 30) == 3) iItemID = 636; break; // RingofGrandMage
			case 8: if (iDice(1, 30) == 3) iItemID = 734; break; // RingOfArcmage
			}
			return true;

		case 31: // Demon 
			switch (iDice(1, 8)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // BloodyShockW.Manual
			case 2: iItemID = 491; break; // BloodAxe
			case 3: if (iDice(1, 10) == 3) iItemID = 490; break; // BloodSword
			case 4: iItemID = 492; break; // BloodRapier
			case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // MassFireStrikeManual
			case 6: if (iDice(1, 30) == 3) iItemID = 633; break; // RingofDemonpower
			case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
			case 8: if (iDice(1, 20) == 3) iItemID = 616; break; // DemonSlayer
			}
			return true;

		case 52: // Gagoyle
			switch (iDice(1, 12)) {
			case 1: if (iDice(1, 30) == 3) iItemID = 382; break; // BloodyShockW.Manual	
			case 2: if (iDice(1, 20) == 3) iItemID = 610; break; // XelimaBlade	
			case 3: if (iDice(1, 20) == 3) iItemID = 611; break; // XelimaAxe	
			case 4: if (iDice(1, 20) == 3) iItemID = 612; break; // XelimaRapier
			case 5: if (iDice(1, 10) == 3) iItemID = 381; break; // MassFireStrikeManual
			case 6: break; // RingofDemonpower
			case 7: if (iDice(1, 10) == 3) iItemID = 645; break; // KnecklaceOfEfreet
			case 8: if (iDice(1, 40) == 3) iItemID = 630; break; // RingoftheXelima	
			case 9: break; // RingoftheAbaddon
			case 10: break; // RingOfDragonpower
			case 11: if (iDice(1, 40) == 3) iItemID = 20; break; // Excalibur
			case 12: break; // The_Devastator
			}
			return true;

		case 53: // Beholder
			if (iDice(1, 20) == 11) iItemID = 646;  // KnecklaceOfBeholder
			return true;


		case 54: // Dark-Elf
			if (iDice(1, 20) == 11) iItemID = 618;  // DarkElfBow
			return true;


		case 63: // Frost
			if (iDice(1, 40) == 11) iItemID = 845;  // StormBringer
			return true;


		case 79: // Nizie
			if (iDice(1, 20) == 11) iItemID = 845;  // StormBringer	
			return true;


		case 70: // Barlog
			if (iDice(1, 40) == 11) iItemID = 846;  // The_Devastator
			return true;


		case 71: // Centaurus
			if (iDice(1, 20) == 11) iItemID = 848;  // Lighting Blade
			return true;


		}

		return false;
	}
}

void CGame::RemoveEventNpc(int iNpcH)
{
	if (m_pNpcList[iNpcH] == 0) return;
	if (m_pNpcList[iNpcH]->m_bIsKilled == true) return;

	m_pNpcList[iNpcH]->m_bIsKilled = true;
	m_pNpcList[iNpcH]->m_iHP = 0;
	m_pNpcList[iNpcH]->m_iLastDamage = 0;
	m_pNpcList[iNpcH]->m_dwRegenTime = 0;
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

	ReleaseFollowMode(iNpcH);
	m_pNpcList[iNpcH]->m_iTargetIndex = 0;
	m_pNpcList[iNpcH]->m_cTargetType = 0;

	SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTDYING, 0, 1, 0);
	if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(10, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	}
	else {
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
	}
	m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetDeadOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
	m_pNpcList[iNpcH]->m_cBehavior = 4;
	m_pNpcList[iNpcH]->m_sBehaviorTurnCount = 0;
	m_pNpcList[iNpcH]->m_dwDeadTime = timeGetTime();

}

bool CGame::_bDecodeNpcConfigFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iNpcConfigListIndex = 0;
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
					// NPC
					if (strlen(token) > 20) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Too long Npc name.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					ZeroMemory(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, sizeof(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName));
					memcpy(m_pNpcConfigList[iNpcConfigListIndex]->m_cNpcName, token, strlen(token));
					cReadModeB = 2;
					break;
				case 2:
					// m_sType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_sType = atoi(token);
					cReadModeB = 3;
					break;
				case 3:
					// m_iHitDice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iHitDice = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					// m_iDefenseRatio
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iDefenseRatio = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					// m_iHitRatio
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iHitRatio = atoi(token);
					cReadModeB = 6;
					break;
				case 6:
					// m_iMinBravery
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMinBravery = atoi(token);
					cReadModeB = 7;
					break;
				case 7:
					// m_iExpDiceMin
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMin = atoi(token);
					cReadModeB = 8;
					break;

				case 8:
					// m_iExpDiceMax
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iExpDiceMax = atoi(token);
					cReadModeB = 9;
					break;

				case 9:
					// m_iGoldDiceMin
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMin = atoi(token);
					cReadModeB = 10;
					break;

				case 10:
					// m_iGoldDiceMax
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iGoldDiceMax = atoi(token);
					cReadModeB = 11;
					break;

				case 11:
					// m_cAttackDiceThrow
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceThrow = atoi(token);
					cReadModeB = 12;
					break;

				case 12:
					// m_cAttackDiceRange
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttackDiceRange = atoi(token);
					cReadModeB = 13;
					break;

				case 13:
					// m_cSize
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cSize = atoi(token);
					cReadModeB = 14;
					break;

				case 14:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cSide = atoi(token);
					cReadModeB = 15;
					break;

				case 15:
					// ActionLimit 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cActionLimit = atoi(token);
					cReadModeB = 16;
					break;

				case 16:
					// Action Time
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_dwActionTime = atoi(token);
					cReadModeB = 17;
					break;

				case 17:
					// ResistMagic
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cResistMagic = atoi(token);
					cReadModeB = 18;
					break;

				case 18:
					// cMagicLevel
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cMagicLevel = atoi(token);
					cReadModeB = 19;
					break;

				case 19:
					// cGenDayWeekLimit  // 漂沥 夸老俊父 积己登绰 阁胶磐咯何 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cDayOfWeekLimit = atoi(token);
					cReadModeB = 20;
					break;

				case 20:
					// cChatMsgPresence
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cChatMsgPresence = atoi(token);
					cReadModeB = 21;
					break;

				case 21:
					// m_cTargetSearchRange
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cTargetSearchRange = atoi(token);

					cReadModeB = 22;
					break;

				case 22:
					// Npc 犁 积己鳖瘤狼 矫埃
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_dwRegenTime = atoi(token);

					cReadModeB = 23;
					break;

				case 23:
					// Attribute
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_cAttribute = atoi(token);

					cReadModeB = 24;
					break;

				case 24:
					// Absorb Magic Damage
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iAbsDamage = atoi(token);

					cReadModeB = 25;
					break;

				case 25:
					// Maximum Mana Point
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMaxMana = atoi(token);

					cReadModeB = 26;
					break;

				case 26:
					// MagicHitRatio
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iMagicHitRatio = atoi(token);

					cReadModeB = 27;
					break;

				case 27:
					// AttackRange
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_iAttackRange = atoi(token);
					cReadModeB = 28;
					break;

				
				case 28:
					// AreaSize
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! NPC configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pNpcConfigList[iNpcConfigListIndex]->m_sAreaSize = atoi(token);
					cReadModeA = 0;
					cReadModeB = 0;
					iNpcConfigListIndex++;
					break;

				}
				break;

			default:
				break;
			}
		}
		else {
			if (memcmp(token, "Npc", 3) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
				m_pNpcConfigList[iNpcConfigListIndex] = new class CNpc(" ");
			}


		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! NPC configuration file contents error!");
		return false;
	}

	wsprintf(cTxt, "(!) NPC(Total:%d) configuration - success!", iNpcConfigListIndex);
	PutLogList(cTxt);

	return true;
}

void CGame::MobGenerator()
{
	int i, x, j, iNamingValue, iResult, iTotalMob;
	char cNpcName[21], cName_Master[11], cName_Slave[11], cWaypoint[11];
	char cSA;
	int  pX, pY, iMapLevel, iProbSA, iKindSA, iResultNum, iNpcID;
	bool bFirmBerserk, bIsSpecialEvent, bMaster = false;
	bool bIsGuard;

	if (m_bOnExitProcess == true) return;
	
	for (i = 0; i < DEF_MAXMAPS; i++) {

		if (m_pMapList[i] != 0) {
			iResultNum = m_pMapList[i]->m_iMaximumObject;
		}

		if ((m_pMapList[i] != 0) && (m_pMapList[i]->m_bRandomMobGenerator == true) && (iResultNum > m_pMapList[i]->m_iTotalActiveObject)) {
			if ((m_iMiddlelandMapIndex != -1) && (m_iMiddlelandMapIndex == i) && (m_bIsCrusadeMode == true)) continue;//break;

			iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
			if (iNamingValue != -1) {
				// Master Mob????????.
				ZeroMemory(cName_Master, sizeof(cName_Master));
				wsprintf(cName_Master, "XX%d", iNamingValue);
				cName_Master[0] = '_';
				cName_Master[1] = i + 65;

				ZeroMemory(cNpcName, sizeof(cNpcName));

				bFirmBerserk = false;
				iResult = iDice(1, 100);
				switch (m_pMapList[i]->m_cRandomMobGeneratorLevel) {

				case 1: // arefarm, elvfarm, aresden, elvine
					if ((iResult >= 1) && (iResult < 20)) {
						iResult = 1; // Slime
					}
					else if ((iResult >= 20) && (iResult < 40)) {
						iResult = 2; // Giant-Ant
					}
					else if ((iResult >= 40) && (iResult < 85)) {
						iResult = 24; // Rabbit
					}
					else if ((iResult >= 85) && (iResult < 95)) {
						iResult = 25; // Cat
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 3; // Orc
					}
					iMapLevel = 1;
					break;

				case 2:
					if ((iResult >= 1) && (iResult < 40)) {
						iResult = 1;
					}
					else if ((iResult >= 40) && (iResult < 80)) {
						iResult = 2;
					}
					else iResult = 10;
					iMapLevel = 1;
					break;

				case 3:     // v1.4334 ????????? ????
					if ((iResult >= 1) && (iResult < 20)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3;  break;
						case 2: iResult = 4;  break;
						}
					}
					else if ((iResult >= 20) && (iResult < 25)) {
						iResult = 30;
					}
					else if ((iResult >= 25) && (iResult < 50)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 5;  break;
						case 2: iResult = 6;  break;
						case 3:	iResult = 7;  break;
						}
					}
					else if ((iResult >= 50) && (iResult < 75)) {
						switch (iDice(1, 7)) {
						case 1:
						case 2:	iResult = 8;  break;
						case 3:	iResult = 11; break;
						case 4: iResult = 12; break;
						case 5:	iResult = 18; break;
						case 6:	iResult = 26; break;
						case 7: iResult = 28; break;
						}
					}
					else if ((iResult >= 75) && (iResult <= 100)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2: iResult = 9;  break;
						case 3:	iResult = 13; break;
						case 4: iResult = 14; break;
						case 5:	iResult = 27; break;
						}
					}
					iMapLevel = 3;
					break;

				case 4:
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 2;  break;
						case 2: iResult = 10; break;
						}
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 11; break;
						}
					}
					else if ((iResult >= 80) && (iResult <= 100)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 14; break;
						case 2:	iResult = 9;  break;
						}
					}
					iMapLevel = 2;
					break;

				case 5:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5: iResult = 2;  break;
						}
					}
					else if ((iResult >= 30) && (iResult < 60)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3;  break;
						case 2: iResult = 4;  break;
						}
					}
					else if ((iResult >= 60) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5;  break;
						case 2: iResult = 7;  break;
						}
					}
					else if ((iResult >= 80) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 8;  break;
						case 3:	iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 11; break;
						case 2: iResult = 14; break;
						case 3: iResult = 9;  break;
						}
					}
					iMapLevel = 3;
					break;

				case 6: // huntzone3, huntzone4
					if ((iResult >= 1) && (iResult < 60)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 5;  break; // Skeleton
						case 2:	iResult = 6;  break; // Orc-Mage
						case 3: iResult = 12; break; // Cyclops
						case 4: iResult = 11; break; // Troll
						}
					}
					else if ((iResult >= 60) && (iResult < 90)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2: iResult = 8;  break; // Stone-Golem
						case 3:	iResult = 11; break; // Troll
						case 4:	iResult = 12; break; // Cyclops 
						case 5:	iResult = 43; break; // Tentocle
						}
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						switch (iDice(1, 9)) {
						case 1:	iResult = 26; break;
						case 2:	iResult = 9;  break;
						case 3: iResult = 13; break;
						case 4: iResult = 14; break;
						case 5:	iResult = 18; break;
						case 6:	iResult = 28; break;
						case 7: iResult = 27; break;
						case 8: iResult = 29; break;
						}
					}
					iMapLevel = 4;
					break;

				case 7: // areuni, elvuni
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 5)) {
						case 1: iResult = 3;  break; // Orc
						case 2: iResult = 6;  break; // Orc-Mage
						case 3: iResult = 10; break; // Amphis
						case 4: iResult = 3;  break; // Orc
						case 5: iResult = 50; break; // Giant-Tree
						}
					}
					else if ((iResult >= 50) && (iResult < 60)) {
						iResult = 29; // Rudolph
					}
					else if ((iResult >= 60) && (iResult < 85)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 50; break; // Giant-Tree
						case 2:
						case 3: iResult = 6;  break; // Orc-Mage
						case 4: iResult = 12; break; // Troll
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 4)) {
						case 1: iResult = 12;  break; // Troll
						case 2:
						case 3:
							if (iDice(1, 100) < 3)
								iResult = 17; // Unicorn
							else iResult = 12; // Troll
							break;
						case 4: iResult = 29;  break; // Cannibal-Plant
						}
					}
					iMapLevel = 4;
					break;

				case 8:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 4;  break;
						case 2: iResult = 5;  break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 14; break;
					}
					iMapLevel = 4;
					break;

				case 9:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 2)) {
						case 1:	iResult = 4;  break;
						case 2: iResult = 5;  break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8;  break;
						case 2: iResult = 9;  break;
						case 3: iResult = 13; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 6)) {
						case 1:
						case 2:
						case 3: iResult = 9;  break;
						case 4:
						case 5: iResult = 14; break;
						case 6: iResult = 15; break;
						}
					}

					if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = true;
					iMapLevel = 5;
					break;

				case 10:
					if ((iResult >= 1) && (iResult < 70)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 9; break;
						case 2: iResult = 5; break;
						case 3: iResult = 8; break;
						}
					}
					else if ((iResult >= 70) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2:	iResult = 13; break;
						case 3: iResult = 14; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 14; break;
						case 3: iResult = 15; break;
						}
					}
					// Demon?? Berserk ????? ??? ?????
					if ((iDice(1, 3) == 1) && (iResult != 16)) bFirmBerserk = true;
					iMapLevel = 5;
					break;

				case 11:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 5)) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5: iResult = 2; break;
						}
					}
					else if ((iResult >= 30) && (iResult < 60)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 3; break;
						case 2: iResult = 4; break;
						}
					}
					else if ((iResult >= 60) && (iResult < 80)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5; break;
						case 2: iResult = 7; break;
						}
					}
					else if ((iResult >= 80) && (iResult < 95)) {
						switch (iDice(1, 3)) {
						case 1:
						case 2: iResult = 10;  break;
						case 3:	iResult = 11; break;
						}
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 11; break;
						case 2: iResult = 7; break;
						case 3: iResult = 8; break;
						}
					}
					iMapLevel = 4;
					break;

				case 12:
					if ((iResult >= 1) && (iResult < 50)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 1; break;
						case 2: iResult = 2; break;
						case 3: iResult = 10; break;
						}
					}
					else if ((iResult >= 50) && (iResult < 85)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5; break;
						case 2: iResult = 4; break;
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8; break;
						case 2: iResult = 11; break;
						case 3: iResult = 26; break;
						}
					}
					iMapLevel = 4;
					break;

				case 13:
					if ((iResult >= 1) && (iResult < 15)) {
						iResult = 4;
						bFirmBerserk = true;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 15) && (iResult < 40)) {
						iResult = 14;
						bFirmBerserk = true;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 40) && (iResult < 60)) {
						iResult = 9;
						bFirmBerserk = true;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 60) && (iResult < 75)) {
						iResult = 13;
						bFirmBerserk = true;
						iTotalMob = 4 - (iDice(1, 2) - 1);
						break;
					}
					else if ((iResult >= 75) && (iResult < 95)) {
						iResult = 23;
					}
					else if ((iResult >= 95) && (iResult <= 100)) {
						iResult = 22;
					}
					iMapLevel = 5;
					break;

				case 14: // icebound
					if ((iResult >= 1) && (iResult < 30)) {
						iResult = 23; // Dark-Elf
					}
					else if ((iResult >= 30) && (iResult < 50)) {
						iResult = 31; // Ice-Golem
					}
					else if ((iResult >= 50) && (iResult < 70)) {
						iResult = 22; // Beholder
						bFirmBerserk = true;
						iTotalMob = 4 - (iDice(1, 2) - 1);
					}
					else if ((iResult >= 70) && (iResult < 90)) {
						iResult = 32; // DireBoar
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 33; // Frost
					}
					iMapLevel = 5;
					break;

				case 15:
					if ((iResult >= 1) && (iResult < 35)) {
						iResult = 23;
						bFirmBerserk = true;
					}
					else if ((iResult >= 35) && (iResult < 50)) {
						iResult = 22;
						bFirmBerserk = true;
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						iResult = 15;
					}
					else if ((iResult >= 80) && (iResult <= 100)) {
						iResult = 21;
					}
					iMapLevel = 4;
					break;

				case 16: // 2ndmiddle, huntzone1, huntzone2, 
					if ((iResult >= 1) && (iResult < 40)) {
						switch (iDice(1, 3)) {
						case 1:	iResult = 7;  break; // Scorpion
						case 2: iResult = 2;  break; // Giant-Ant
						case 3: iResult = 10; break; // Amphis
						}
					}
					else if ((iResult >= 40) && (iResult < 50)) {
						iResult = 30; // Rudolph
					}
					else if ((iResult >= 50) && (iResult < 85)) {
						switch (iDice(1, 2)) {
						case 1: iResult = 5;  break; // Skeleton
						case 2: iResult = 4;  break; // Zombie
						}
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						switch (iDice(1, 3)) {
						case 1: iResult = 8;  break; // Stone-Golem
						case 2: iResult = 11; break; // Clay-Golem
						case 3: iResult = 7;  break; // Scorpion
						}
					}
					iMapLevel = 1;
					break;

				case 17:
					if ((iResult >= 1) && (iResult < 30)) {
						switch (iDice(1, 4)) {
						case 1:	iResult = 22;  break; // Giant-Frog
						case 2: iResult = 8;   break; // Stone-Golem
						case 3: iResult = 24;  break; // Rabbit
						case 4: iResult = 5;   break;
						}
					}
					else if ((iResult >= 30) && (iResult < 40)) {
						iResult = 30;
					}
					else if ((iResult >= 40) && (iResult < 70)) {
						iResult = 32;
					}
					else if ((iResult >= 70) && (iResult < 90)) {
						iResult = 31;
						if (iDice(1, 5) == 1) {
							bFirmBerserk = true;
						}
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 33;
					}
					iMapLevel = 1;
					break;

				case 18: // druncncity
					if ((iResult >= 1) && (iResult < 2)) {
						iResult = 39; // Tentocle
					}
					else if ((iResult >= 2) && (iResult < 12)) {
						iResult = 44; // ClawTurtle
					}
					else if ((iResult >= 12) && (iResult < 50)) {
						iResult = 48; // Nizie
					}
					else if ((iResult >= 50) && (iResult < 80)) {
						iResult = 45; // Giant-Crayfish
					}
					else if ((iResult >= 80) && (iResult < 90)) {
						iResult = 34; // Stalker
					}
					else if ((iResult >= 90) && (iResult <= 100)) {
						iResult = 26; // Giant-Frog
					}
					iMapLevel = 4;
					break;

				case 19:
					if ((iResult >= 1) && (iResult < 15)) {
						iResult = 44;
					}
					else if ((iResult >= 15) && (iResult < 25)) {
						iResult = 46;
					}
					else if ((iResult >= 25) && (iResult < 35)) {
						iResult = 21;
					}
					else if ((iResult >= 35) && (iResult < 60)) {
						iResult = 43;
					}
					else if ((iResult >= 60) && (iResult < 85)) {
						iResult = 23;
					}
					else if ((iResult >= 85) && (iResult <= 100)) {
						iResult = 22;
					}
					iMapLevel = 4;
					break;

				case 20:
					if ((iResult >= 1) && (iResult < 2)) {
						iResult = 41;
					}
					else if ((iResult >= 2) && (iResult < 3)) {
						iResult = 40;
					}
					else if ((iResult >= 3) && (iResult < 8)) {
						iResult = 53;
					}
					else if ((iResult >= 8) && (iResult < 9)) {
						iResult = 39;
					}
					else if ((iResult >= 9) && (iResult < 20)) {
						iResult = 21;
					}
					else if ((iResult >= 20) && (iResult < 35)) {
						iResult = 16;
					}
					else if ((iResult >= 35) && (iResult < 45)) {
						iResult = 44;
					}
					else if ((iResult >= 45) && (iResult < 55)) {
						iResult = 45;
					}
					else if ((iResult >= 55) && (iResult < 75)) {
						iResult = 28;
					}
					else if ((iResult >= 75) && (iResult < 95)) {
						iResult = 43;
					}
					else if ((iResult >= 95) && (iResult < 100)) {
						iResult = 22;
					}
					iMapLevel = 4;
					break;

				case 21:
					if ((iResult >= 1) && (iResult < 94)) {
						iResult = 17; // Unicorn
						bFirmBerserk = true;
					}
					else if ((iResult >= 94) && (iResult < 95)) {
						iResult = 36; // Wyvern
					}
					else if ((iResult >= 95) && (iResult < 96)) {
						iResult = 37; // Fire-Wyvern
					}
					else if ((iResult >= 96) && (iResult < 97)) {
						iResult = 47; // MasterMage-Orc
					}
					else if ((iResult >= 97) && (iResult < 98)) {
						iResult = 35; // Hellclaw
					}
					else if ((iResult >= 98) && (iResult < 99)) {
						iResult = 49; // Tigerworm
					}
					else if ((iResult >= 99) && (iResult <= 100)) {
						iResult = 51; // Abaddon
					}
					iMapLevel = 4;
					break;

				}

				pX = 0;
				pY = 0;

				if ((m_bIsSpecialEventTime == true) && (iDice(1, 10) == 3)) bIsSpecialEvent = true;

				if (bIsSpecialEvent == true) {
					switch (m_cSpecialEventType) {
					case 1:
						if (m_pMapList[i]->m_iMaxPx != 0) {
							pX = m_pMapList[i]->m_iMaxPx * 20 + 10;
							pY = m_pMapList[i]->m_iMaxPy * 20 + 10;

							if (pX < 0) pX = 0;
							if (pY < 0) pY = 0;

							if (m_bIsCrusadeMode == true) {
								if (strcmp(m_pMapList[i]->m_cName, "aresden") == 0) {
									switch (iDice(1, 6)) {
									case 1: iResult = 20; break;
									case 2: iResult = 53; break;
									case 3: iResult = 55; break;
									case 4: iResult = 57; break;
									case 5: iResult = 59; break;
									case 6: iResult = 61; break;
									}
								}
								else if (strcmp(m_pMapList[i]->m_cName, "elvine") == 0) {
									switch (iDice(1, 6)) {
									case 1: iResult = 19; break;
									case 2: iResult = 52; break;
									case 3: iResult = 54; break;
									case 4: iResult = 56; break;
									case 5: iResult = 58; break;
									case 6: iResult = 60; break;
									}
								}
							}
							wsprintf(G_cTxt, "(!) Mob-Event Map(%s)[%d (%d,%d)]", m_pMapList[i]->m_cName, iResult, pX, pY);
						}
						break;

					case 2:
						if (iDice(1, 3) == 2) {
							if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "middled1n", 9) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
								(memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0)) {
								if (iDice(1, 30) == 5)
									iResult = 16;
								else iResult = 5;
							}
							else iResult = 16;
						}
						else iResult = 17;

						m_bIsSpecialEventTime = false;
						break;
					}
				}

				ZeroMemory(cNpcName, sizeof(cNpcName));
				//Random Monster Spawns
				switch (iResult) {
				case 1:  strcpy(cNpcName, "Slime");				iNpcID = 10; iProbSA = 5;  iKindSA = 1; break;
				case 2:  strcpy(cNpcName, "Giant-Ant");			iNpcID = 16; iProbSA = 10; iKindSA = 2; break;
				case 3:  strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
				case 4:  strcpy(cNpcName, "Zombie");			iNpcID = 18; iProbSA = 15; iKindSA = 3; break;
				case 5:  strcpy(cNpcName, "Skeleton");			iNpcID = 11; iProbSA = 35; iKindSA = 8; break;
				case 6:  strcpy(cNpcName, "Orc-Mage");			iNpcID = 14; iProbSA = 30; iKindSA = 7; break;
				case 7:  strcpy(cNpcName, "Scorpion");			iNpcID = 17; iProbSA = 15; iKindSA = 3; break;
				case 8:  strcpy(cNpcName, "Stone-Golem");		iNpcID = 12; iProbSA = 25; iKindSA = 5; break;
				case 9:  strcpy(cNpcName, "Cyclops");			iNpcID = 13; iProbSA = 35; iKindSA = 8; break;
				case 10: strcpy(cNpcName, "Amphis");			iNpcID = 22; iProbSA = 20; iKindSA = 3; break;
				case 11: strcpy(cNpcName, "Clay-Golem");		iNpcID = 23; iProbSA = 20; iKindSA = 5; break;
				case 12: strcpy(cNpcName, "Troll");				iNpcID = 28; iProbSA = 25; iKindSA = 3; break;
				case 13: strcpy(cNpcName, "Orge");				iNpcID = 29; iProbSA = 25; iKindSA = 1; break;
				case 14: strcpy(cNpcName, "Hellbound");			iNpcID = 27; iProbSA = 25; iKindSA = 8; break;
				case 15: strcpy(cNpcName, "Liche");				iNpcID = 30; iProbSA = 30; iKindSA = 8; break;
				case 16: strcpy(cNpcName, "Demon");				iNpcID = 31; iProbSA = 20; iKindSA = 8; break;
				case 17: strcpy(cNpcName, "Unicorn");			iNpcID = 32; iProbSA = 35; iKindSA = 7; break;
				case 18: strcpy(cNpcName, "WereWolf");			iNpcID = 33; iProbSA = 25; iKindSA = 1; break;
				case 19: strcpy(cNpcName, "YB-Aresden");		iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
				case 20: strcpy(cNpcName, "YB-Elvine");			iNpcID = -1;  iProbSA = 15; iKindSA = 1; break;
				case 21: strcpy(cNpcName, "Gagoyle");			iNpcID = 52; iProbSA = 20; iKindSA = 8; break;
				case 22: strcpy(cNpcName, "Beholder");			iNpcID = 53; iProbSA = 20; iKindSA = 5; break;
				case 23: strcpy(cNpcName, "Dark-Elf");			iNpcID = 54; iProbSA = 20; iKindSA = 3; break;
				case 24: strcpy(cNpcName, "Rabbit");			iNpcID = -1; iProbSA = 5;  iKindSA = 1; break;
				case 25: strcpy(cNpcName, "Cat");				iNpcID = -1; iProbSA = 10; iKindSA = 2; break;
				case 26: strcpy(cNpcName, "Giant-Frog");		iNpcID = 57; iProbSA = 10; iKindSA = 2; break;
				case 27: strcpy(cNpcName, "Mountain-Giant");	iNpcID = 58; iProbSA = 25; iKindSA = 1; break;
				case 28: strcpy(cNpcName, "Ettin");				iNpcID = 59; iProbSA = 20; iKindSA = 8; break;
				case 29: strcpy(cNpcName, "Cannibal-Plant");	iNpcID = 60; iProbSA = 20; iKindSA = 5; break;
				case 30: strcpy(cNpcName, "Rudolph");			iNpcID = -1; iProbSA = 20; iKindSA = 5; break;
				case 31: strcpy(cNpcName, "Ice-Golem");			iNpcID = 65; iProbSA = 35; iKindSA = 8; break;
				case 32: strcpy(cNpcName, "DireBoar");			iNpcID = 62; iProbSA = 20; iKindSA = 5; break;
				case 33: strcpy(cNpcName, "Frost");				iNpcID = 63; iProbSA = 30; iKindSA = 8; break;
				case 34: strcpy(cNpcName, "Stalker");           iNpcID = 48; iProbSA = 20; iKindSA = 1; break;
				case 35: strcpy(cNpcName, "Hellclaw");			iNpcID = 49; iProbSA = 20; iKindSA = 1; break;
				case 36: strcpy(cNpcName, "Wyvern");			iNpcID = 66; iProbSA = 20; iKindSA = 1; break;
				case 37: strcpy(cNpcName, "Fire-Wyvern");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 38: strcpy(cNpcName, "Barlog");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 39: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 40: strcpy(cNpcName, "Centaurus");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 41: strcpy(cNpcName, "Giant-Lizard");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 42: strcpy(cNpcName, "Minotaurs");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 43: strcpy(cNpcName, "Tentocle");			iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 44: strcpy(cNpcName, "Claw-Turtle");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 45: strcpy(cNpcName, "Giant-Crayfish");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 46: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 47: strcpy(cNpcName, "MasterMage-Orc");	iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 48: strcpy(cNpcName, "Nizie");				iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 49: strcpy(cNpcName, "Tigerworm");			iNpcID = 50; iProbSA = 20; iKindSA = 1; break;
				case 50: strcpy(cNpcName, "Giant-Plant");		iNpcID = -1; iProbSA = 20; iKindSA = 1; break;
				case 51: strcpy(cNpcName, "Abaddon");			iNpcID = -1; iProbSA = 20; iKindSA = 9; break;
				case 52: strcpy(cNpcName, "YW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 53: strcpy(cNpcName, "YW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 54: strcpy(cNpcName, "YY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 55: strcpy(cNpcName, "YY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 56: strcpy(cNpcName, "XB-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 57: strcpy(cNpcName, "XB-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 58: strcpy(cNpcName, "XW-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 59: strcpy(cNpcName, "XW-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 60: strcpy(cNpcName, "XY-Aresden");		iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				case 61: strcpy(cNpcName, "XY-Elvine");			iNpcID = -1; iProbSA = 15; iKindSA = 1; break;
				default: strcpy(cNpcName, "Orc");				iNpcID = 14; iProbSA = 15; iKindSA = 1; break;
				}

				cSA = 0;
				if (iDice(1, 100) <= iProbSA) {
					cSA = _cGetSpecialAbility(iKindSA);
				}

				bMaster = bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, bFirmBerserk, true);

				if (!bMaster)
				{
					m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
					iTotalMob = 0;
				}

			}

			switch (iResult) {

			case 1:
			case 2:
			case 3:
			case 10:
			case 12:
			case 18:
			case 21:
			case 29:
				iTotalMob = iDice(1, 5) - 1;
				break;

			case 4:
			case 5:
			case 6:
			case 7:
			case 11:
			case 13:
			case 15:
			case 26:
			case 27:
			case 28:
			case 30:
			case 31:
			case 42:
			case 44:
				iTotalMob = iDice(1, 3) - 1;
				break;

			case 8:
			case 9:
			case 14:
			case 16:
			case 17:
			case 19:
			case 20:
			case 22:
			case 23:
			case 25:
				iTotalMob = iDice(1, 2) - 1;
				break;

			case 24:
				iTotalMob = iDice(1, 4) - 1;
				break;

			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 43:
			case 45:
				iTotalMob = 1;
				break;

			default:
				iTotalMob = 0;
				break;
			}


			if (iTotalMob > 2) {
				switch (iResult) {
				case 1:  // Slime 
				case 2:  // Giant-Ant
				case 3:  // Orc
				case 4:  // Zombie
				case 5:  // Skeleton
				case 6:  // Orc-Mage
				case 7:  // Scorpion
				case 8:  // Stone-Golem
				case 9:  // Cyclops
				case 10: // Amphis
				case 11: // Clay-Golem
				case 12: // Troll
				case 13: // Orge
				case 14: // Hellbound
				case 15: // Liche
				case 16: // Demon
				case 17: // Unicorn
				case 18: // WereWolf
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
					if (iDice(1, 5) == 1) iTotalMob = 0;  // 75% ???????? ???.
					break;

				case 33:
				case 34:
				case 35:
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41:
				case 42:
				case 44:
				case 45:
				case 46:
				case 47:
				case 48:
				case 49:
					if (iDice(1, 5) != 1) iTotalMob = 0;  // 75% ???????????????
					break;
				}
			}

			if (bIsSpecialEvent == true) {
				switch (m_cSpecialEventType) {
				case 1:
					if ((iResult != 35) && (iResult != 36) && (iResult != 37) && (iResult != 49)
						&& (iResult != 51) && (iResult != 15) && (iResult != 16) && (iResult != 21)) iTotalMob = 12;
					for (x = 0; x < DEF_MAXCLIENTS; x++)
						if ((iNpcID != -1) && (m_pClientList[x] != 0) && (m_pClientList[x]->m_bIsInitComplete == true)) {
							SendNotifyMsg(0, x, DEF_NOTIFY_SPAWNEVENT, pX, pY, iNpcID, 0, 0, 0);
						}
					break;

				case 2:
					if ((memcmp(m_pMapList[i]->m_cLocationName, "aresden", 7) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "elvine", 6) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "elvfarm", 7) == 0) ||
						(memcmp(m_pMapList[i]->m_cLocationName, "arefarm", 7) == 0)) {
						iTotalMob = 0;
					}
					break;
				}
				m_bIsSpecialEventTime = false;
			}

			for (j = 0; j < iTotalMob; j++) {
				iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
				if (iNamingValue != -1) {
					ZeroMemory(cName_Slave, sizeof(cName_Slave));
					wsprintf(cName_Slave, "XX%d", iNamingValue);
					cName_Slave[0] = 95; // original '_';
					cName_Slave[1] = i + 65;

					cSA = 0;

					if (iDice(1, 100) <= iProbSA) {
						cSA = _cGetSpecialAbility(iKindSA);
					}

					if (bCreateNewNpc(cNpcName, cName_Slave, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOM, &pX, &pY, cWaypoint, 0, 0, -1, false, false, bFirmBerserk) == false) {
						m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
					}
					else {
						bSetNpcFollowMode(cName_Slave, cName_Master, DEF_OWNERTYPE_NPC);
					}
				}
			}
		}

		if ((m_pMapList[i] != 0) && ((m_pMapList[i]->m_iMaximumObject) > m_pMapList[i]->m_iTotalActiveObject))
		{
			for (j = 1; j < DEF_MAXSPOTMOBGENERATOR; j++)
				if ((iDice(1, 3) == 2) && (m_pMapList[i]->m_stSpotMobGenerator[j].bDefined == true) &&
					(m_pMapList[i]->m_stSpotMobGenerator[j].iMaxMobs > m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs))
				{
					iNamingValue = m_pMapList[i]->iGetEmptyNamingValue();
					if (iNamingValue != -1)
					{
							bIsGuard = false;
							ZeroMemory(cNpcName, sizeof(cNpcName));
							switch (m_pMapList[i]->m_stSpotMobGenerator[j].iMobType) {
								// spot-mob-generator
							case 10:  strcpy(cNpcName, "Slime");				iProbSA = 5;  iKindSA = 1;  break;
							case 16:  strcpy(cNpcName, "Giant-Ant");			iProbSA = 10; iKindSA = 2;  break;
							case 14:  strcpy(cNpcName, "Orc");				iProbSA = 15; iKindSA = 1;  break;
							case 18:  strcpy(cNpcName, "Zombie");			iProbSA = 15; iKindSA = 3;  break;
							case 11:  strcpy(cNpcName, "Skeleton");			iProbSA = 35; iKindSA = 8;  break;
							case 6:   strcpy(cNpcName, "Orc-Mage");			iProbSA = 30; iKindSA = 7;  break;
							case 17:  strcpy(cNpcName, "Scorpion");			iProbSA = 15; iKindSA = 3;  break;
							case 12:  strcpy(cNpcName, "Stone-Golem");		iProbSA = 25; iKindSA = 5;  break;
							case 13:  strcpy(cNpcName, "Cyclops");			iProbSA = 35; iKindSA = 8;  break;
							case 22:  strcpy(cNpcName, "Amphis");			iProbSA = 20; iKindSA = 3;  break;
							case 23:  strcpy(cNpcName, "Clay-Golem");		iProbSA = 20; iKindSA = 5;  break;

								// centu - guards
							case 24:  strcpy(cNpcName, "Guard-Aresden");		bIsGuard = true; break;
							case 25:  strcpy(cNpcName, "Guard-Elvine");		bIsGuard = true; break;
							case 26:  strcpy(cNpcName, "Guard-Neutral");		bIsGuard = true; break;

							case 27:  strcpy(cNpcName, "Hellbound");			iProbSA = 20; iKindSA = 1;  break;
							case 29:  strcpy(cNpcName, "Orge");              iProbSA = 20; iKindSA = 1;  break;
							case 30:  strcpy(cNpcName, "Liche");				iProbSA = 30; iKindSA = 8;  break;
							case 31:  strcpy(cNpcName, "Demon");				iProbSA = 20; iKindSA = 8;  break;
							case 32:  strcpy(cNpcName, "Unicorn");			iProbSA = 35; iKindSA = 7;  break;
							case 33:  strcpy(cNpcName, "WereWolf");			iProbSA = 25; iKindSA = 1;  break;
							case 34:  strcpy(cNpcName, "Dummy");				iProbSA = 5;  iKindSA = 1;  break;
							case 35:  strcpy(cNpcName, "Attack-Dummy");		iProbSA = 5;  iKindSA = 1;  break;
							case 48:  strcpy(cNpcName, "Stalker");			iProbSA = 20; iKindSA = 3;  break;

							case 49:  strcpy(cNpcName, "Hellclaw");			iProbSA = 20; iKindSA = 8;  break;
							case 50:  strcpy(cNpcName, "Tigerworm");			iProbSA = 20; iKindSA = 8;  break;
							case 54:  strcpy(cNpcName, "Dark-Elf");			iProbSA = 20; iKindSA = 8;  break;
							case 53:  strcpy(cNpcName, "Beholder");			iProbSA = 20; iKindSA = 8;  break;
							case 52:  strcpy(cNpcName, "Gagoyle");			iProbSA = 20; iKindSA = 8;  break;

							case 57:  strcpy(cNpcName, "Giant-Frog");		iProbSA = 10; iKindSA = 2;  break;
							case 58:  strcpy(cNpcName, "Mountain-Giant");	iProbSA = 25; iKindSA = 1;  break;
							case 59:  strcpy(cNpcName, "Ettin");				iProbSA = 20; iKindSA = 8;  break;
							case 60:  strcpy(cNpcName, "Cannibal-Plant");	iProbSA = 20; iKindSA = 5;  break;
							case 61:  strcpy(cNpcName, "Rudolph");			iProbSA = 20; iKindSA = 1;  break;
							case 62:  strcpy(cNpcName, "DireBoar");			iProbSA = 20; iKindSA = 1;  break;
							case 63:  strcpy(cNpcName, "Frost");				iProbSA = 20; iKindSA = 8;  break;
							case 65:  strcpy(cNpcName, "Ice-Golem");			iProbSA = 20; iKindSA = 8;  break;
							case 66:  strcpy(cNpcName, "Wyvern");			iProbSA = 20; iKindSA = 1;  break;
							case 55:  strcpy(cNpcName, "Rabbit");			iProbSA = 20; iKindSA = 1;  break;
							case 67:  strcpy(cNpcName, "McGaffin");			iProbSA = 20; iKindSA = 1;  break;
							case 68:  strcpy(cNpcName, "Perry");				iProbSA = 20; iKindSA = 1;  break;
							case 69:  strcpy(cNpcName, "Devlin");			iProbSA = 20; iKindSA = 1;  break;
							case 73:  strcpy(cNpcName, "Fire-Wyvern");		iProbSA = 20; iKindSA = 1;  break;
							case 70:  strcpy(cNpcName, "Barlog");			iProbSA = 20; iKindSA = 1;  break;
							case 80:  strcpy(cNpcName, "Tentocle");			iProbSA = 20; iKindSA = 1;  break;
							case 71:  strcpy(cNpcName, "Centaurus");			iProbSA = 20; iKindSA = 1;  break;
							case 75:  strcpy(cNpcName, "Giant-Lizard");		iProbSA = 20; iKindSA = 1;  break;
							case 78:  strcpy(cNpcName, "Minotaurs");			iProbSA = 20; iKindSA = 1;  break;
							case 81:  strcpy(cNpcName, "Abaddon");			iProbSA = 20; iKindSA = 9;  break;
							case 72:  strcpy(cNpcName, "Claw-Turtle");		iProbSA = 20; iKindSA = 1;  break;
							case 74:  strcpy(cNpcName, "Giant-Crayfish");	iProbSA = 20; iKindSA = 1;  break;
							case 76:  strcpy(cNpcName, "Giant-Plant");		iProbSA = 20; iKindSA = 1;  break;
							case 77:  strcpy(cNpcName, "MasterMage-Orc");	iProbSA = 20; iKindSA = 1;  break;
							case 79:  strcpy(cNpcName, "Nizie");				iProbSA = 20; iKindSA = 1; break;
							//Magn0S:: New Npc's
							case 92:  strcpy(cNpcName, "Eternal-Dragon");			iProbSA = 20; iKindSA = 9;  break;
							case 93:  strcpy(cNpcName, "Black-Demon");			iProbSA = 20; iKindSA = 9;  break;
							case 94:  strcpy(cNpcName, "Black-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 95:  strcpy(cNpcName, "Light-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 96:  strcpy(cNpcName, "Poison-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 97:  strcpy(cNpcName, "Heaven-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 98:  strcpy(cNpcName, "Illusion-Wyvern");			iProbSA = 20; iKindSA = 9;  break;
							case 99:  strcpy(cNpcName, "Ghost-Abaddon");			iProbSA = 20; iKindSA = 9;  break;


							default:
								strcpy(cNpcName, "Orc");
								iProbSA = 15;
								iKindSA = 1;
								break;
							}
							/* NPCs not spawning in pits:

							// 15 ShopKeeper-W
							// 19 Gandlf
							// 20 Howard
							// 36 Arrow Gaurd Tower Kit - Aresden, Elvine
							// 37 Cannon Gaurd Tower Kit - Aresden, Elvine
							// 38 Mana Collector Kit - Aresden, Elvine
							// 39 Detector Constructor Kit - Aresden, Elvine
							// 40 Energy Shield Generator - Aresden, Elvine
							// 41 Grand Master Generator - Aresden Elvine
							// 43 Light War Beetle - Aresden, Elvine
							// 44 God's Hand Knight
							// 45 Mounted God's Hand Knight
							// 46 Temple Knight
							// 47 Battle Golem
							// 51 Catapult
							// 64 Crops
							*/
							bFirmBerserk = false;
							if ((iMapLevel == 5) && (iDice(1, 3) == 1)) bFirmBerserk = true;

							ZeroMemory(cName_Master, sizeof(cName_Master));
							wsprintf(cName_Master, "XX%d", iNamingValue);
							cName_Master[0] = 95; // original '_';
							cName_Master[1] = i + 65;

							cSA = 0;

							// centu - guards only Magic Destruction & Clairvoyrant
							if (bIsGuard)
							{
								cSA = _cGetSpecialAbility(10);
							}
							else
							{
								if ((m_pMapList[i]->m_stSpotMobGenerator[j].iMobType != 34) && (iDice(1, 100) <= iProbSA)) {
									cSA = _cGetSpecialAbility(iKindSA);
								}
							}

							switch (m_pMapList[i]->m_stSpotMobGenerator[j].cType) {
							case 1:
								if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMAREA, &pX, &pY, cWaypoint, &m_pMapList[i]->m_stSpotMobGenerator[j].rcRect, j, -1, false, false, bFirmBerserk) == false) {
									m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
								}
								else {
									m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
								}
								break;

							case 2:
								if (bCreateNewNpc(cNpcName, cName_Master, m_pMapList[i]->m_cName, (rand() % 3), cSA, DEF_MOVETYPE_RANDOMWAYPOINT, 0, 0, m_pMapList[i]->m_stSpotMobGenerator[j].cWaypoint, 0, j, -1, false, false, bFirmBerserk) == false) {
									m_pMapList[i]->SetNamingValueEmpty(iNamingValue);
								}
								else {
									m_pMapList[i]->m_stSpotMobGenerator[j].iCurMobs++;
								}
								break;
							}
						}
					}
		}
	}
}

// SNOOPY: reconnecting clients will recover summons 
void CGame::iRecoverFollowers(int  iClientH, bool bControlAll)
{
	if (m_pClientList[iClientH] == 0) return;
	int i, iTotal;
	char  cKillerMsg[128];
	iTotal = 0;
	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != 0)
			&& (m_pNpcList[i]->m_cFollowOwnerType > 7)	// Reduce cases to process
			&& ((m_pNpcList[i]->m_bIsSummoned == true) || (m_pNpcList[i]->m_dwTamingTime != 0))
			&& (m_pNpcList[i]->m_bIsKilled == false))
		{
			switch (m_pNpcList[i]->m_cFollowOwnerType) {
			case DEF_OWNERTYPE_PLAYER_FREE: // Anybody can take control
				if ((bControlAll == true)
					&& (m_pNpcList[i]->m_cSide == m_pClientList[iClientH]->m_cSide))
				{
					m_pNpcList[i]->m_iFollowOwnerIndex = iClientH;
					m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
					m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
					strcpy(m_pNpcList[i]->m_cFollowOwnerCharName, m_pClientList[iClientH]->m_cCharName);
					//Tell client				
					ZeroMemory(cKillerMsg, sizeof(cKillerMsg));
					wsprintf(cKillerMsg, "A %s soldier accepted to follow you.", m_pNpcList[i]->m_cNpcName);
					SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cKillerMsg);
				}
				break;
			case DEF_OWNERTYPE_PLAYER_WAITING:
				if (strcmp(m_pNpcList[i]->m_cFollowOwnerCharName, m_pClientList[iClientH]->m_cCharName) == 0)
				{
					m_pNpcList[i]->m_iFollowOwnerIndex = iClientH;
					m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
					m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
					//Tell client				
					ZeroMemory(cKillerMsg, sizeof(cKillerMsg));
					if ((m_bIsHeldenianMode == true)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != 0)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						wsprintf(cKillerMsg, "Recovered control of a %s soldier.", m_pNpcList[i]->m_cNpcName);
						SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cKillerMsg);
					}
					else
					{
						wsprintf(cKillerMsg, "Recovered control of a %s summon.", m_pNpcList[i]->m_cNpcName);
						SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cKillerMsg);
					}
				}
				break;
			}
		}
}

/*********************************************************************************************************************
**  bool CGame::bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType)							**
**  DESCRIPTION			:: set npc follow mode																		**
**  LAST_UPDATED		:: March 18, 2005; 9:43 AM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
/*****************************************************************************************************
**							  Follower mode functions:												**
**	bSetNpcFollowMode:	After creating a mob, gives him a master...									**
**  RemoveFromTarget:   If target or master Killed/TP/deco/Invi, check follow mode					**
**  ReleaseFollowMode:  Check if a killed npc had followers, and free them							**
**  iRecoverFollowers:  Restore follow mode after a client has disconnected the connected again		**
**  iGetFollowerNumber: Counts the follower numbers to prevent too much summons						**
*****************************************************************************************************/
bool CGame::bSetNpcFollowMode(char* pName, char* pFollowName, char cFollowOwnerType)
{
	int i, iIndex, iMapIndex, iFollowIndex;
	char cTmpName[11], cFollowSide;
	ZeroMemory(cTmpName, sizeof(cTmpName));// Snoopy: added m_cFollowOwnerCharName
	iIndex = -1;
	iMapIndex = -1;
	iFollowIndex = -1;
	for (i = 0; i < DEF_MAXNPCS; i++)
		if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, pName, 5) == 0))
		{
			iIndex = i;
			iMapIndex = m_pNpcList[i]->m_cMapIndex;
			break;
		}
	if (iIndex == -1) return false;

	switch (cFollowOwnerType) {
	case DEF_OWNERTYPE_NPC:
		for (i = 0; i < DEF_MAXNPCS; i++)
			if ((m_pNpcList[i] != 0) && (memcmp(m_pNpcList[i]->m_cName, pFollowName, 5) == 0))
			{
				if (m_pNpcList[i]->m_cMapIndex != iMapIndex) return false;
				m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
				m_pNpcList[iIndex]->m_cFollowOwnerType = DEF_OWNERTYPE_NPC;
				m_pNpcList[iIndex]->m_iFollowOwnerIndex = i;
				ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
				m_pNpcList[iIndex]->m_cSide = m_pNpcList[i]->m_cSide;
				return true;
			}
		break;

	case DEF_OWNERTYPE_PLAYER:
		for (i = 0; i < DEF_MAXCLIENTS; i++)
			if ((m_pClientList[i] != 0) && (memcmp(m_pClientList[i]->m_cCharName, pFollowName, 10) == 0))
			{
				if (m_pClientList[i]->m_cMapIndex != iMapIndex) return false;
				iFollowIndex = i;
				cFollowSide = m_pClientList[i]->m_cSide;
				m_pNpcList[iIndex]->m_cMoveType = DEF_MOVETYPE_FOLLOW;
				m_pNpcList[iIndex]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER;
				m_pNpcList[iIndex]->m_iFollowOwnerIndex = i;
				strcpy(m_pNpcList[iIndex]->m_cFollowOwnerCharName, m_pClientList[i]->m_cCharName);
				m_pNpcList[iIndex]->m_cSide = m_pClientList[i]->m_cSide;
				return true;
			}
		break;
	}
	return false;
}

/*********************************************************************************************************************
**  int CGame::bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType,**
** int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, bool bHideGenMode,**
** bool bIsSummoned, bool bFirmBerserk, bool bIsMaster, int iGuildGUID)												**
**  DESCRIPTION			:: creates a npc																			**
**  LAST_UPDATED		:: March 18, 2005; 6:01 PM; Hypnotoad														**
**	RETURN_VALUE		:: int																						**
**  NOTES				::	- sends 1x1 monsters to a different position verifier than 2x2/3x3						**
**							- new - expmin and expmax x 2 if npc is bFirmBerserk									**
**							- creates blocked spaces below 1x1, 2x2, and 3x3 monsters								**
**	MODIFICATION		::	- ExpDice MIN/MAX and bFirmBerserk requires testing										**
**********************************************************************************************************************/
int CGame::bCreateNewNpc(char* pNpcName, char* pName, char* pMapName, short sClass, char cSA, char cMoveType, int* poX, int* poY, char* pWaypointList, RECT* pArea, int iSpotMobIndex, char cChangeSide, bool bHideGenMode, bool bIsSummoned, bool bFirmBerserk, bool bIsMaster, int iGuildGUID)
{
	int i, t, j, k, iMapIndex;
	char  cTmpName[11], cTxt[120];
	short sX, sY, sRange;
	bool  bFlag;
	SYSTEMTIME SysTime;

	if (strlen(pName) == 0)   return false;
	if (strlen(pNpcName) == 0) return false;
	GetLocalTime(&SysTime);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pMapName);
	iMapIndex = -1;

	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] != 0) {
			if (memcmp(m_pMapList[i]->m_cName, cTmpName, 10) == 0)
				iMapIndex = i;
		}

	if (iMapIndex == -1) return false;

	for (i = 1; i < DEF_MAXNPCS; i++)
		if (m_pNpcList[i] == 0) {
			m_pNpcList[i] = new class CNpc(pName);
			if (_bInitNpcAttr(m_pNpcList[i], pNpcName, sClass, cSA) == false) {
				wsprintf(cTxt, "(!) Not existing NPC creation request! (%s) Ignored.", pNpcName);
				PutLogList(cTxt);

				delete m_pNpcList[i];
				m_pNpcList[i] = 0;
				break;
			}
			if (m_pNpcList[i]->m_cDayOfWeekLimit < 10) {
				if (m_pNpcList[i]->m_cDayOfWeekLimit != SysTime.wDayOfWeek) {
					delete m_pNpcList[i];
					m_pNpcList[i] = 0;
					break;
				}
			}
			switch (cMoveType) {
			case DEF_MOVETYPE_GUARD:
			case DEF_MOVETYPE_RANDOM:
				if ((poX != 0) && (poY != 0) && (*poX != 0) && (*poY != 0)) {
					sX = *poX;
					sY = *poY;
				}
				else {
					for (j = 0; j <= 30; j++) {
						// SNOOPY: Why -15 ?
						sX = (rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15;
						sY = (rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15;
						bFlag = true;
						for (k = 0; k < DEF_MAXMGAR; k++)
							if (m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left != -1) {
								if ((sX >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].left) &&
									(sX <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].right) &&
									(sY >= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].top) &&
									(sY <= m_pMapList[iMapIndex]->m_rcMobGenAvoidRect[k].bottom)) {
									bFlag = false;
								}
							}
						// SNOOPY: Added condition for accessible map place.
						// Secondary possible effect: Less spawns in small corridors ?
						if ((m_pMapList[iMapIndex]->bGetMoveable(sX, sY) == false) ||
							(m_pMapList[iMapIndex]->bGetIsTeleport(sX, sY) == true))
						{
							bFlag = false; // Not accessible place
						}
						if (bFlag == true) goto GET_VALIDLOC_SUCCESS;
					}
					delete m_pNpcList[i];
					m_pNpcList[i] = 0;
					return false;

				GET_VALIDLOC_SUCCESS:;
				}
				break;

			case DEF_MOVETYPE_RANDOMAREA:
				sRange = (short)(pArea->right - pArea->left);
				if (sRange <= 0) sRange = 1; // SNOOPY antibug
				sX = (short)((rand() % sRange) + pArea->left);
				sRange = (short)(pArea->bottom - pArea->top);
				if (sRange <= 0) sRange = 1; // SNOOPY antibug
				sY = (short)((rand() % sRange) + pArea->top);
				break;

			case DEF_MOVETYPE_RANDOMWAYPOINT:
				sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].x;
				sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[iDice(1, 10) - 1]].y;
				break;

			default:
				if ((poX != 0) && (poY != 0) && (*poX != 0) && (*poY != 0)) {
					sX = *poX;
					sY = *poY;
				}
				else {
					sX = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].x;
					sY = (short)m_pMapList[iMapIndex]->m_WaypointList[pWaypointList[0]].y;
				}
				break;
			}
			// new - sends 1x1 monsters to a different position checker than 2x2/3x3
			if (m_pNpcList[i]->m_sAreaSize == 0) {
				if (bGetEmptyPosition(&sX, &sY, iMapIndex) == false) {
					delete m_pNpcList[i];
					m_pNpcList[i] = 0;
					break;
				}
			}
			else {
				// new - checks if walk location is free for larger than 1x1 monsters
				if (bGetNpcMovementArea(i, sX, sY, iMapIndex, m_pNpcList[i]->m_sAreaSize) == false) {
					delete m_pNpcList[i];
					m_pNpcList[i] = 0;
					break;
				}
			}

			if ((bHideGenMode == true) && (_iGetPlayerNumberOnSpot(sX, sY, iMapIndex, 7) != 0)) {
				delete m_pNpcList[i];
				m_pNpcList[i] = 0;
				break;
			}

			if ((poX != 0) && (poY != 0)) {
				*poX = sX;
				*poY = sY;
			}

			m_pNpcList[i]->m_sX = sX;
			m_pNpcList[i]->m_sY = sY;
			m_pNpcList[i]->m_vX = sX;
			m_pNpcList[i]->m_vY = sY;
			for (t = 0; t < 10; t++) m_pNpcList[i]->m_iWayPointIndex[t] = pWaypointList[t];

			m_pNpcList[i]->m_cTotalWaypoint = 0;

			for (t = 0; t < 10; t++)
				if (m_pNpcList[i]->m_iWayPointIndex[t] != -1) m_pNpcList[i]->m_cTotalWaypoint++;

			if (pArea != 0) {
				SetRect(&m_pNpcList[i]->m_rcRandomArea, pArea->left, pArea->top, pArea->right, pArea->bottom);
			}

			switch (cMoveType) {
			case DEF_MOVETYPE_GUARD:
				m_pNpcList[i]->m_dX = m_pNpcList[i]->m_sX;
				m_pNpcList[i]->m_dY = m_pNpcList[i]->m_sY;
				break;

			case DEF_MOVETYPE_SEQWAYPOINT:
				m_pNpcList[i]->m_cCurWaypoint = 1;
				m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
				m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
				break;

			case DEF_MOVETYPE_RANDOMWAYPOINT:
				m_pNpcList[i]->m_cCurWaypoint = (rand() % (m_pNpcList[i]->m_cTotalWaypoint - 1)) + 1;
				m_pNpcList[i]->m_dX = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].x;
				m_pNpcList[i]->m_dY = (short)m_pMapList[iMapIndex]->m_WaypointList[m_pNpcList[i]->m_iWayPointIndex[m_pNpcList[i]->m_cCurWaypoint]].y;
				break;

			case DEF_MOVETYPE_RANDOMAREA:
				m_pNpcList[i]->m_cCurWaypoint = 0;
				sRange = (short)(m_pNpcList[i]->m_rcRandomArea.right - m_pNpcList[i]->m_rcRandomArea.left);
				m_pNpcList[i]->m_dX = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.left);
				sRange = (short)(m_pNpcList[i]->m_rcRandomArea.bottom - m_pNpcList[i]->m_rcRandomArea.top);
				m_pNpcList[i]->m_dY = (short)((rand() % sRange) + m_pNpcList[i]->m_rcRandomArea.top);
				break;

			case DEF_MOVETYPE_RANDOM:
				m_pNpcList[i]->m_dX = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeX - 50)) + 15);
				m_pNpcList[i]->m_dY = (short)((rand() % (m_pMapList[iMapIndex]->m_sSizeY - 50)) + 15);
				break;
			}
			m_pNpcList[i]->m_tmp_iError = 0;
			m_pNpcList[i]->m_cMoveType = cMoveType;

			switch (m_pNpcList[i]->m_cActionLimit) {
			case 2:
			case 3:
			case 5:
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_STOP;
				switch (m_pNpcList[i]->m_sType) {
				case 15: // ShopKeeper-W
				case 19: // Gandlf
				case 20: // Howard
				case 24: // Tom
				case 25: // William
				case 26: // Kennedy
					m_pNpcList[i]->m_cDir = 6; //iDice(1, 3) + 3;
					break;

				default:
					m_pNpcList[i]->m_cDir = iDice(1, 8);
					break;
				}
				break;

			case 8: // Heldenian gate
				m_pNpcList[i]->m_cDir = 3;
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_STOP;
				if (m_pNpcList[i]->m_cArea > 0)
				{
					for (short sx1 = (sX - 1); sx1 <= sX + 1; sx1++)
						for (short sy1 = (sY - 1); sy1 <= sY + 1; sy1++)
						{
							m_pMapList[iMapIndex]->SetTempMoveAllowedFlag(sx1, sy1, false);
						}
				}
				break;

			default:
				m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_MOVE;
				m_pNpcList[i]->m_cDir = 5;
				break;
			}

			m_pNpcList[i]->m_iFollowOwnerIndex = 0;
			ZeroMemory(m_pNpcList[i]->m_cFollowOwnerCharName, sizeof(m_pNpcList[i]->m_cFollowOwnerCharName));
			m_pNpcList[i]->m_iTargetIndex = 0;
			m_pNpcList[i]->m_cTurn = (rand() % 2);

			switch (m_pNpcList[i]->m_sType) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				m_pNpcList[i]->m_sAppr2 = (short)0xF000;
				m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | ((rand() % 13) << 4);
				m_pNpcList[i]->m_sAppr2 = m_pNpcList[i]->m_sAppr2 | (rand() % 9);
				break;

			case 36: // AGT-Aresden/AGT-Elvine
			case 37: // CGT-Aresden/CGT-Elvine
			case 38: // MS-Aresden/MS-Elvine
			case 39: // DT-Aresden/DT-Elvine
				m_pNpcList[i]->m_sAppr2 = 3;
				break;

			case 64: // Crop
				m_pNpcList[i]->m_sAppr2 = 1; // 1 bud; 2 grown; 3 large 
				break;

			default:
				m_pNpcList[i]->m_sAppr2 = 0;
				break;
			}
			m_pNpcList[i]->m_cMapIndex = (char)iMapIndex;
			m_pNpcList[i]->m_dwTime = timeGetTime() + (rand() % 10000);
			m_pNpcList[i]->m_dwActionTime += (rand() % 300);
			m_pNpcList[i]->m_dwMPupTime = timeGetTime();
			m_pNpcList[i]->m_dwHPupTime = m_pNpcList[i]->m_dwMPupTime;
			m_pNpcList[i]->m_sBehaviorTurnCount = 0;
			m_pNpcList[i]->m_bIsSummoned = bIsSummoned;
			m_pNpcList[i]->m_bIsMaster = bIsMaster;
			if (bIsSummoned == true)
			{
				m_pNpcList[i]->m_dwSummonedTime = timeGetTime();
				m_pNpcList[i]->m_dwSummonWaitTime = 0;
			}
			if (bFirmBerserk == true) {
				m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = 1;
				m_pNpcList[i]->m_iStatus = m_pNpcList[i]->m_iStatus | 0x20;
				m_pNpcList[i]->m_iExpDiceMin = (m_pNpcList[i]->m_iExpDiceMin * 2); // new - expmin x 2 if npc is bFirmBerserk
				m_pNpcList[i]->m_iExpDiceMax = (m_pNpcList[i]->m_iExpDiceMax * 2); // new - expmax x 10 if npc is bFirmBerserk

			}
			if (cChangeSide != -1) m_pNpcList[i]->m_cSide = cChangeSide;
			m_pNpcList[i]->m_cBravery = (rand() % 3) + m_pNpcList[i]->m_iMinBravery;
			m_pNpcList[i]->m_iSpotMobIndex = iSpotMobIndex;
			m_pNpcList[i]->m_iGuildGUID = iGuildGUID;
			if (iGuildGUID != 0) {
				wsprintf(G_cTxt, "Summon War Unit(%d) GUID(%d)", m_pNpcList[i]->m_sType, iGuildGUID);
				PutLogList(G_cTxt);
			}
			// new - creates blocked spaces below 1x1, 2x2, and 3x3 monsters
			if (m_pNpcList[i]->m_sAreaSize == 0) {
				m_pMapList[iMapIndex]->SetOwner(i, DEF_OWNERTYPE_NPC, sX, sY);
			}
			else {
				m_pMapList[iMapIndex]->SetBigOwner(i, DEF_OWNERTYPE_NPC, sX, sY, m_pNpcList[i]->m_sAreaSize);
			}
			if ((m_bIsHeldenianMode == true) // If any mob created as Helnian war not started,, ensure it's Neutral...
				&& (m_bHeldenianWarInitiated == false)
				&& (m_pMapList[iMapIndex]->m_bIsHeldenianMap == true)
				&& (m_pNpcList[i]->m_cSide > 2))
			{
				m_pNpcList[i]->m_cOriginalSide = m_pNpcList[i]->m_cSide;
				m_pNpcList[i]->m_cSide = 0;
			}
			m_pMapList[iMapIndex]->m_iTotalActiveObject++;
			m_pMapList[iMapIndex]->m_iTotalAliveObject++;
			switch (m_pNpcList[i]->m_sType) {
			case 36: // AGT-Aresden/AGT-Elvine
			case 37: // CGT-Aresden/CGT-Elvine
			case 38: // MS-Aresden/MS-Elvine
			case 39: // DT-Aresden/DT-Elvine
			case 42: // ManaStone
				m_pMapList[iMapIndex]->bAddCrusadeStructureInfo((char)m_pNpcList[i]->m_sType, sX, sY, m_pNpcList[i]->m_cSide);
				break;

			case 64:
				m_pMapList[iMapIndex]->bAddCropsTotalSum();
				break;
			}
			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM, 0, 0, 0);
			return true;
		}
	return false;
}

/*********************************************************************************************************************
**  void CGame::NpcProcess()																						**
**  DESCRIPTION			:: manages Npc variables																	**
**  LAST_UPDATED		:: March 18, 2005; 6:30 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- new Crop DeleteNpc after specific amount of time										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
void CGame::NpcProcess()
{
	int i, iMaxHP;
	DWORD dwTime, dwActionTime;
	// SNOOPY: Poisonned npcs
	int iHPup = 0;

	dwTime = timeGetTime();
	for (i = 0; i < DEF_MAXNPCS; i++) {
		if (m_pNpcList[i] != 0) {
			if (m_pNpcList[i]->m_cBehavior == DEF_BEHAVIOR_ATTACK) {
				switch (iDice(1, 7)) {
				case 1: dwActionTime = m_pNpcList[i]->m_dwActionTime; break;
				case 2: dwActionTime = m_pNpcList[i]->m_dwActionTime - 100; break;
				case 3: dwActionTime = m_pNpcList[i]->m_dwActionTime - 200; break;
				case 4: dwActionTime = m_pNpcList[i]->m_dwActionTime - 300; break;
				case 5: dwActionTime = m_pNpcList[i]->m_dwActionTime - 400; break;
				case 6: dwActionTime = m_pNpcList[i]->m_dwActionTime - 600; break;
				case 7: dwActionTime = m_pNpcList[i]->m_dwActionTime - 700; break;
				}
				if (dwActionTime < 600) dwActionTime = 600;
			}
			else dwActionTime = m_pNpcList[i]->m_dwActionTime;
			if (m_pNpcList[i]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] != 0)
				dwActionTime += (dwActionTime / 2);

			// SNOOPY: Added safety here
			if (m_pNpcList[i]->m_iHP <= 0) m_pNpcList[i]->m_cBehavior = DEF_BEHAVIOR_DEAD;
		}
		//SNOOPY Added here for ending Taming
		if ((m_pNpcList[i] != 0)
			&& (m_pNpcList[i]->m_dwTamingTime != 0)
			&& (m_pNpcList[i]->m_cOriginalSide != m_pNpcList[i]->m_cSide)
			&& (dwTime > m_pNpcList[i]->m_dwTamingTime))
		{
			m_pNpcList[i]->m_cSide = m_pNpcList[i]->m_cOriginalSide;
			m_pNpcList[i]->m_dwTamingTime = 0;
			m_pNpcList[i]->m_cMoveType = DEF_MOVETYPE_RANDOMWAYPOINT;
			SendEventToNearClient_TypeA(i, DEF_OWNERTYPE_NPC, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
		}
		if ((m_pNpcList[i] != 0) && ((dwTime - m_pNpcList[i]->m_dwTime) > dwActionTime)) {
			m_pNpcList[i]->m_dwTime = dwTime;
			if (abs(m_pNpcList[i]->m_cMagicLevel) > 0) {
				if ((dwTime - m_pNpcList[i]->m_dwMPupTime) > DEF_MPUPTIME) {
					m_pNpcList[i]->m_dwMPupTime = dwTime;
					m_pNpcList[i]->m_iMana += iDice(1, (m_pNpcList[i]->m_iMaxMana / 5));

					if (m_pNpcList[i]->m_iMana > m_pNpcList[i]->m_iMaxMana)
						m_pNpcList[i]->m_iMana = m_pNpcList[i]->m_iMaxMana;
				}
			}
			// HP up
			if (((dwTime - m_pNpcList[i]->m_dwHPupTime) > DEF_HPUPTIME) && (m_pNpcList[i]->m_bIsKilled == false))
			{
				m_pNpcList[i]->m_dwHPupTime = dwTime;
				iMaxHP = iDice(m_pNpcList[i]->m_iHitDice, 8) + m_pNpcList[i]->m_iHitDice;
				if ((m_pNpcList[i]->m_iHP < iMaxHP)
					&& (m_pNpcList[i]->m_cActionLimit != 5) && (m_pNpcList[i]->m_cActionLimit != 8)) // Building do not regenerate.					
				{
					if (m_pNpcList[i]->m_bIsSummoned == false)
					{
						iHPup = iDice(1, m_pNpcList[i]->m_iHitDice);
					}
					// SNOOPY: Added poison support for npcs...
					if (m_pNpcList[i]->m_iPoisonLevel != 0)
					{
						iHPup -= m_pNpcList[i]->m_iPoisonLevel;
						if (iHPup < 0) iHPup = 0;
						if (bCheckResistingPoisonSuccess(i, DEF_OWNERTYPE_NPC) == true)
						{
							m_pNpcList[i]->m_iPoisonLevel = 0;
							SetPoisonFlag(i, DEF_OWNERTYPE_NPC, false);
						}
					}
					m_pNpcList[i]->m_iHP += iHPup;
					if (m_pNpcList[i]->m_iHP > iMaxHP) m_pNpcList[i]->m_iHP = iMaxHP;
					if (m_pNpcList[i]->m_iHP <= 0)     m_pNpcList[i]->m_iHP = 1;
				}
			}
			switch (m_pNpcList[i]->m_cBehavior) {
			case DEF_BEHAVIOR_DEAD:
				NpcBehavior_Dead(i);
				break;
			case DEF_BEHAVIOR_STOP:
				NpcBehavior_Stop(i);
				break;
			case DEF_BEHAVIOR_MOVE:
				NpcBehavior_Move(i);
				break;
			case DEF_BEHAVIOR_ATTACK:
				NpcBehavior_Attack(i);
				break;
			case DEF_BEHAVIOR_FLEE:
				NpcBehavior_Flee(i);
				break;
			}
			//SNOOPY Added here for ending waiting for Tamed mobs
			if ((m_pNpcList[i] != 0) && (m_pNpcList[i]->m_iHP > 0)
				&& (m_pNpcList[i]->m_dwTamingTime != 0)
				&& (m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
				&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
			{
				m_pNpcList[i]->m_cFollowOwnerType = 0;
				m_pNpcList[i]->m_iSummonControlMode = 0;
			}
			// !!! m_pNpcList
			if ((m_pNpcList[i] != 0) && (m_pNpcList[i]->m_iHP > 0) && (m_pNpcList[i]->m_bIsSummoned == true))
			{
				switch (m_pNpcList[i]->m_sType) {
				case 29: // Ogre summon popo:10 min
				case 33: // WW summon popo		
				case 31: // Demon summon popo
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 10)
						NpcKilledHandler(0, 0, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				case 43: // LWB
				case 44: // GHK
				case 45: // GHKABS
				case 46: // TK
				case 47: // BG Crusade summons 10 min
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 10)
						NpcKilledHandler(0, 0, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				case 82: // Sorceress	
				case 83: // ATK	
				case 84: // MasterElf	
				case 85: // DSK	
				case 86: // HBT		
				case 88: // Barbarian
					if ((m_bIsHeldenianMode == true)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex] != 0)
						&& (m_pMapList[m_pNpcList[i]->m_cMapIndex]->m_bIsHeldenianMap == 1))
					{
						if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
							&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
						{
							switch (m_cHeldenianSummonMode) {
							case 0: // Master has not connected again, remove summon
								NpcKilledHandler(0, 0, i, 0);
								break;
							case 1: // Master has not connected again, wait forever			
								m_pNpcList[i]->m_iSummonControlMode = 0; // Un"/hold"
								break;
							case 2: // Master has not connected again, available for others to take over	
								m_pNpcList[i]->m_cFollowOwnerType = DEF_OWNERTYPE_PLAYER_FREE;
								m_pNpcList[i]->m_iSummonControlMode = 0; // Un"/hold"
								break;
							}
						}
						else break; // Keep them alive					
					}
					else if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > 1000 * 60 * 15)
						NpcKilledHandler(0, 0, i, 0);		// Heldenian soldiars limited to 15min if not Heldenian
					break;
				case 36: // AGT         Those will last forever when summonned
				case 37: // AGC
				case 38: // MS
				case 39: // DT
				case 40: // ESG
				case 41: // ManaStone
				case 87: // CT
				case 89: // AGC
				case 91: // gates
					if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				default: //other summons: 5 minutes
					if ((dwTime - m_pNpcList[i]->m_dwSummonedTime) > DEF_SUMMONTIME)
						NpcKilledHandler(0, 0, i, 0);
					else if ((m_pNpcList[i]->m_cFollowOwnerType == DEF_OWNERTYPE_PLAYER_WAITING)
						&& ((dwTime - m_pNpcList[i]->m_dwSummonWaitTime) > 1000 * 60)) // Wait 60sec for the character to connect again
					{
						m_pNpcList[i]->m_cFollowOwnerType = 0;
						m_pNpcList[i]->m_iSummonControlMode = 0;
					}
					break;
				}
			}
		}
	}
}

int CGame::iGetDangerValue(int iNpcH, short dX, short dY)
{
	int ix, iy, iDangerValue;
	short sOwner, sDOType;
	char  cOwnerType;
	DWORD dwRegisterTime;

	if (m_pNpcList[iNpcH] == 0) return false;

	iDangerValue = 0;

	for (ix = dX - 2; ix <= dX + 2; ix++)
		for (iy = dY - 2; iy <= dY + 2; iy++) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->GetOwner(&sOwner, &cOwnerType, ix, iy);
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetDynamicObject(ix, iy, &sDOType, &dwRegisterTime);

			if (sDOType == 1) iDangerValue++;

			switch (cOwnerType) {
			case 0:
				break;
			case DEF_OWNERTYPE_PLAYER:
				if (m_pClientList[sOwner] == 0) break;
				if (m_pNpcList[iNpcH]->m_cSide != m_pClientList[sOwner]->m_cSide)
					iDangerValue++;
				else iDangerValue--;
				break;
			case DEF_OWNERTYPE_NPC:
				if (m_pNpcList[sOwner] == 0) break;
				if (m_pNpcList[iNpcH]->m_cSide != m_pNpcList[sOwner]->m_cSide)
					iDangerValue++;
				else iDangerValue--;
				break;
			}
		}

	return iDangerValue;
}

// 3.51 - 05/17/2004 - Hypnotoad/[KLKS] - Monster Special Abilities
char CGame::_cGetSpecialAbility(int iKindSA)
{
	char cSA;

	switch (iKindSA) {
	case 1:
		// Slime, Orc, Orge, WereWolf, YB-, Rabbit, Mountain-Giant, Stalker, Hellclaw, 
		// Wyvern, Fire-Wyvern, Barlog, Tentocle, Centaurus, Giant-Lizard, Minotaurus,
		// Abaddon, Claw-Turtle, Giant-Cray-Fish, Giant-Plant, MasterMage-Orc, Nizie,
		// Tigerworm
		switch (iDice(1, 2)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		}
		break;

	case 2:
		// Giant-Ant, Cat, Giant-Frog, 
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		}
		break;

	case 3:
		// Zombie, Scorpion, Amphis, Troll, Dark-Elf
		switch (iDice(1, 4)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		case 4: cSA = 6; break; // Critical Poisonous
		}
		break;

	case 4:
		// no linked Npc
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 7; break; // Explosive
		}
		break;

	case 5:
		// Stone-Golem, Clay-Golem, Beholder, Cannibal-Plant, Rudolph, DireBoar
		switch (iDice(1, 4)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 7; break; // Explosive
		case 4: cSA = 8; break; // Critical-Explosive
		}
		break;

	case 6:
		// no linked Npc
		switch (iDice(1, 3)) {
		case 1: cSA = 3; break; // Anti-Physical Damage
		case 2: cSA = 4; break; // Anti-Magic Damage
		case 3: cSA = 5; break; // Poisonous
		}
		break;

	case 7:
		// Orc-Mage, Unicorn
		switch (iDice(1, 3)) {
		case 1: cSA = 1; break; // Clairvoyant
		case 2: cSA = 2; break; // Distruction of Magic Protection
		case 3: cSA = 4; break; // Anti-Magic Damage
		}
		break;

	case 8:
		// Frost, Ice-Golem, Ettin, Gagoyle, Demon, Liche, Hellbound, Cyclops, 
		// Skeleton
		switch (iDice(1, 5)) {
		case 1: cSA = 1; break; // Clairvoyant
		case 2: cSA = 2; break; // Distruction of Magic Protection
		case 3: cSA = 4; break; // Anti-Magic Damage
		case 4: cSA = 3; break; // Anti-Physical Damage
		case 5: cSA = 8; break; // Critical-Explosive
		}
		break;

	case 9:
		// no linked Npc
		cSA = iDice(1, 8); // All abilities available
		break;

	case 10: // centu - guards
		cSA = iDice(1, 2);
		break;
	}

	return cSA;
}

//Magn0S:: Coded Drop Manager from .cfg
bool CGame::bDecodeDropManagerFile(char* pFn)
{
	FILE* pFile;
	HANDLE hFile;
	char* cp, * token, cReadModeA;
	char seps[] = "= \t\n";
	int i = 0, j = 0, k = 0, f = 0, o = 0;
	char cTxt[250];

	DWORD  dwFileSize;
	class CStrTok* pStrTok;

	cReadModeA = 0;

	hFile = CreateFile(pFn, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	dwFileSize = GetFileSize(hFile, 0);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	pFile = fopen(pFn, "rt");
	if (pFile == 0) {

		PutLogList("(!) Cannot open Drop Manager file.");
		return false;
	}
	else {
		PutLogList("(!) Reading DropManager file...");
		cp = new char[dwFileSize + 2];
		ZeroMemory(cp, dwFileSize + 2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new class CStrTok(cp, seps);
		token = pStrTok->pGet();

		while (token) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR! Max Armor Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}

					m_iMaxArmorDrop = atoi(token);
					wsprintf(cTxt, "(*) iMaxArmorProbability drop rate: (%d)", m_iMaxArmorDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 2:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR! ArmorDrop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}
					if (i > 12) {
						PutLogList("(!) WARNING (A)! Too many armor probability added!");
						return true;
					}
					m_iArmorDrop[i] = atoi(token);
					wsprintf(cTxt, "(*) iArmorProb[%d] drop rate: (%d)", i, m_iArmorDrop[i]);
					PutLogList(cTxt);
					i++;
					cReadModeA = 0;
					break;
				//-------------------------------------------------------------------------------------------------------------
				case 3:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (B)! Max Weapon Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxWeaponDrop = atoi(token);
					wsprintf(cTxt, "(*) iMaxWeaponProbability drop rate: (%d)", m_iMaxWeaponDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 4:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (B)! WeaponDrop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (j > 12) {
						PutLogList("(!) WARNING (B)! Too many weapon probability added!");
						return true;
					}
					m_iWeaponDrop[j] = atoi(token);
					wsprintf(cTxt, "(*) iWeaponProb[%d] drop rate: (%d)", j, m_iWeaponDrop[j]);
					PutLogList(cTxt);
					j++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 5:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (C)! Max Drop Stated Armor Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxStatedArmor = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxStatedArmor drop rate: (%d)", m_iMaxStatedArmor);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 6:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (C)! Armor Stated Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (k > 8) {
						PutLogList("(!) WARNING (C)! Too many stated armor probability added!");
						return true;
					}
					m_iStatedArmorDrop[k] = atoi(token);
					wsprintf(cTxt, "(*) iStatedArmor[%d] drop rate: (%d)", k, m_iStatedArmorDrop[k]);
					PutLogList(cTxt);
					k++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 7:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (D)! Attr Weapon Drop Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxAttrWeaponDrop = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxAttrWeaponDrop rate: (%d)", m_iMaxAttrWeaponDrop);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 8:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (D)! Attr Weapon Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (f > 9) {
						PutLogList("(!) WARNING (D)! Too many attr weapon probability added!");
						return true;
					}
					m_iAttrWeaponDrop[f] = atoi(token);
					wsprintf(cTxt, "(*) iAttrWeaponDrop[%d] drop rate: (%d)", f, m_iAttrWeaponDrop[f]);
					PutLogList(cTxt);
					f++;
					cReadModeA = 0;
					break;
				//===========================================================================================================================
				case 9:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (E)! Max Drop Stated Weapon Prob. config. file error - Wrong Data format.");
						delete[] cp; return false;
					}
					m_iMaxStatedWeapon = atoi(token);
					wsprintf(cTxt, "(*) m_iMaxStatedWeapon rate: (%d)", m_iMaxStatedWeapon);
					PutLogList(cTxt);
					cReadModeA = 0;
					break;

				case 10:
					if (_bGetIsStringIsNumber(token) == false)
					{	PutLogList("(!!!) CRITICAL ERROR (E)! Armor Stated Drop configuration file error - Wrong Data format.");
						delete[] cp; return false;
					}

					if (o > 3) {
						PutLogList("(!) WARNING (E)! Too many stated weapon probability added!");
						return true;
					}
					m_iStatedWeaponDrop[o] = atoi(token);
					wsprintf(cTxt, "(*) iStatedWeaponDrop[%d] drop rate: (%d)", o, m_iStatedWeaponDrop[o]);
					PutLogList(cTxt);
					o++;
					cReadModeA = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "max-armor-prob", 14) == 0) {
					cReadModeA = 1;
				}
				if (memcmp(token, "p_armor", 7) == 0) {
					cReadModeA = 2;
				}
				if (memcmp(token, "max-weapon-prob", 15) == 0) {
					cReadModeA = 3;
				}
				if (memcmp(token, "p_weapon", 8) == 0) {
					cReadModeA = 4;
				}
				if (memcmp(token, "max-stated-armor", 16) == 0) {
					cReadModeA = 5;
				}
				if (memcmp(token, "stated-armor", 12) == 0) {
					cReadModeA = 6;
				}
				if (memcmp(token, "max-attr-weapon", 15) == 0) {
					cReadModeA = 7;
				}
				if (memcmp(token, "attr-weapon", 11) == 0) {
					cReadModeA = 8;
				}
				if (memcmp(token, "max-stated-weapon", 17) == 0) {
					cReadModeA = 9;
				}
				if (memcmp(token, "stated-weapon", 13) == 0) {
					cReadModeA = 10;
				}
			}

			token = pStrTok->pGet();
		}
		delete[] cp;
		fclose(pFile);
	}

	return true;
}
