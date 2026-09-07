// Item.cpp: implementation of the CItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Item.h"
#include <string>
#include "team.h"
using namespace std;
extern class Team* c_team;

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

CItem::CItem()
{
	ZeroMemory(m_cName, sizeof(m_cName));
	m_sSprite = 0;
	m_sSpriteFrame = 0;
											  
	m_sItemEffectValue1 = 0;
	m_sItemEffectValue2 = 0;
	m_sItemEffectValue3 = 0; 

	m_sItemEffectValue4 = 0;
	m_sItemEffectValue5 = 0;
	m_sItemEffectValue6 = 0; 

	m_dwCount = 1;
	m_sTouchEffectType   = 0;
	m_sTouchEffectValue1 = 0;
	m_sTouchEffectValue2 = 0;
	m_sTouchEffectValue3 = 0;
	
	m_cItemColor = 0;
	m_sItemSpecEffectValue1 = 0;
	m_sItemSpecEffectValue2 = 0;
	m_sItemSpecEffectValue3 = 0;

	m_sSpecialEffectValue1 = 0;
	m_sSpecialEffectValue2 = 0;
	
	m_wCurLifeSpan = 0;
	m_dwAttribute   = 0;

	m_cCategory = 0;
	m_sIDnum    = 0;

	m_bIsForSale = false;

	teamcape = false;
	teamboots = false;

	//Magn0S:: Add new itens effects
	m_sNewEffect1 = 0;
	m_sNewEffect2 = 0;
	m_sNewEffect3 = 0;
	m_sNewEffect4 = 0;

	m_iClass = 0;

	m_iReqStat = 0;
	m_iQuantStat = 0;

	bEkSale = false;
	bContrbSale = false;
	bCoinSale = false;

	m_bIsHero = false;
	m_iHeroBonus = 0;
}

CItem::~CItem()
{

}

void CGame::ItemDepleteHandler(int iClientH, short sItemIndex, bool bIsUseItemResult, bool bIsItemUsed)
{
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
	if ((bIsItemUsed == 1) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_MATERIAL) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 380) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 381) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 382)) 
	{
		_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, -1, m_pClientList[iClientH]->m_pItemList[sItemIndex], false);
	}
	else if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 247) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 248)) 
	{
		_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, -1, m_pClientList[iClientH]->m_pItemList[sItemIndex], false);
	}

	ReleaseItemHandler(iClientH, sItemIndex, true);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMDEPLETED_ERASEITEM, sItemIndex, (int)bIsUseItemResult, 0, 0);
	delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
	m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;
	m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	iCalcTotalWeight(iClientH);
}

void CGame::CalcTotalItemEffect(int iClientH, int iEquipItemID, bool bNotify)
{
	short sItemIndex;
	int  i, iPrevSAType, iTemp;
	char cEquipPos;
	double dV1, dV2, dV3;
	DWORD  dwSWEType, dwSWEValue;
	short  sTemp;
	int iShieldHPrec, iShieldSPrec, iShieldMPrec, iTotalAngelicstatas;

	iShieldHPrec = iShieldSPrec = iShieldMPrec = 0;

	if (m_pClientList[iClientH] == 0) return;

	if ((m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1) &&
		(m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)) {

		// 肋给等 公扁 厘馒 炼钦捞促. 笛 吝 窍唱甫 郴妨 初绰促. 
		if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] != 0) {
			// 茄颊 八狼 厘馒 惑怕甫 秦力茄促. 
			m_pClientList[iClientH]->m_bIsItemEquipped[m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND]] = false;
			m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] = -1;
		}
	}

	m_pClientList[iClientH]->m_iAngelicStr = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicInt = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicDex = 0; // By Snoopy81
	m_pClientList[iClientH]->m_iAngelicMag = 0; // By Snoopy81 
	SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 0, 0);

	//if (m_pClientList[iClientH]->m_iWantedLevel > 0) 
		//SetWantedFlag(iClientH, DEF_OWNERTYPE_PLAYER, 1); // Wanted System

	m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
	m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
	m_pClientList[iClientH]->m_cAttackBonus_SM = 0;

	m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
	m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
	m_pClientList[iClientH]->m_cAttackBonus_L = 0;

	m_pClientList[iClientH]->m_iHitRatio = 0; 

	m_pClientList[iClientH]->m_iDefenseRatio = (m_pClientList[iClientH]->m_iDex * 2);

	m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 0;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[i] = 0;

	m_pClientList[iClientH]->m_iManaSaveRatio = 0;
	m_pClientList[iClientH]->m_iAddResistMagic = 0;

	m_pClientList[iClientH]->m_iAddPhysicalDamage = 0;
	m_pClientList[iClientH]->m_iAddMagicalDamage = 0;

	m_pClientList[iClientH]->m_bIsLuckyEffect = false;
	m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = -1;
	m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = 0;

	m_pClientList[iClientH]->m_iAddAbsLight = 0;	// 加己喊 措固瘤 软荐
	m_pClientList[iClientH]->m_iAddAbsEarth = 0;
	m_pClientList[iClientH]->m_iAddAbsFire = 0;
	m_pClientList[iClientH]->m_iAddAbsWater = 0;
	//Magn0S:: New
	m_pClientList[iClientH]->m_iAddAbsHoly = 0;
	m_pClientList[iClientH]->m_iAddAbsUnholy = 0;
	m_pClientList[iClientH]->m_iAddElementAbs = 0;

	m_pClientList[iClientH]->m_iCustomItemValue_Attack = 0;
	m_pClientList[iClientH]->m_iCustomItemValue_Defense = 0;

	m_pClientList[iClientH]->m_iMinAP_SM = 0;
	m_pClientList[iClientH]->m_iMinAP_L = 0;

	m_pClientList[iClientH]->m_iMaxAP_SM = 0;
	m_pClientList[iClientH]->m_iMaxAP_L = 0;

	m_pClientList[iClientH]->m_iSpecialWeaponEffectType = 0;	// 锐蓖 酒捞袍 瓤苞 辆幅: 0-None 1-鞘混扁措固瘤眠啊 2-吝刀瓤苞 3-沥狼狼 4-历林狼
	m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = 0;	// 锐蓖 酒捞袍 瓤苞 蔼

	m_pClientList[iClientH]->m_iAddHP = m_pClientList[iClientH]->m_iAddSP = m_pClientList[iClientH]->m_iAddMP = 0;
	m_pClientList[iClientH]->m_iAddAR = m_pClientList[iClientH]->m_iAddPR = m_pClientList[iClientH]->m_iAddDR = 0;
	m_pClientList[iClientH]->m_iAddMR = m_pClientList[iClientH]->m_iAddAbsPD = m_pClientList[iClientH]->m_iAddAbsMD = 0;
	m_pClientList[iClientH]->m_iAddCD = m_pClientList[iClientH]->m_iAddExp = m_pClientList[iClientH]->m_iAddGold = 0;

	iPrevSAType = m_pClientList[iClientH]->m_iSpecialAbilityType;

	m_pClientList[iClientH]->m_iSpecialAbilityType = 0;
	m_pClientList[iClientH]->m_iSpecialAbilityLastSec = 0;
	m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = 0;

	m_pClientList[iClientH]->m_iAddTransMana = 0;
	m_pClientList[iClientH]->m_iAddChargeCritical = 0;

	// re introduced by SNOOPY
	m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM = 0;
	m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L = 0;
	m_pClientList[iClientH]->m_iMagicHitRatio_ItemEffect = 0;

	m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
	// 馒侩窍瘤 臼酒档 瓤苞啊 乐绰 酒捞袍.
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0) {
					// 荐疙捞 乐绢具 瓤苞啊 乐促.
					m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
				}
				break;
			}
		}

	// Hidden bonus to hit per weapon reintroduced by SNOOPY
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) &&
			(m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == true))
		{
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
			case DEF_ITEMEFFECTTYPE_ATTACK:
			case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:
			case DEF_ITEMEFFECTTYPE_DEFENSE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM:
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				break;
				// Use same (L) value for both SM & L
			case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
			case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				m_pClientList[iClientH]->m_iHitRatio_ItemEffect_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue2;
				break;
			}
		}

	// 馒侩阑 秦具 瓤苞啊 乐绰 酒捞袍 
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) &&
			(m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == true)) {

			cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {

			case DEF_ITEMEFFECTTYPE_MAGICDAMAGESAVE:
				m_pClientList[iClientH]->m_iMagicDamageSaveItemIndex = sItemIndex;
				break;

			case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
			case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
			case DEF_ITEMEFFECTTYPE_ATTACK:
			case DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM:
				m_pClientList[iClientH]->m_cAttackDiceThrow_SM = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
				m_pClientList[iClientH]->m_cAttackDiceRange_SM = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
				m_pClientList[iClientH]->m_cAttackBonus_SM = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				m_pClientList[iClientH]->m_cAttackDiceThrow_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
				m_pClientList[iClientH]->m_cAttackDiceRange_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
				m_pClientList[iClientH]->m_cAttackBonus_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
				// Xelima Upgraded weapons
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_cAttackBonus_SM += iTemp;
				m_pClientList[iClientH]->m_cAttackBonus_L += iTemp;
				// Notice: +1 on Magic damage as well
				m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;

				// Add skill to HitRatio
				m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];
				m_pClientList[iClientH]->m_sUsingWeaponSkill = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill;

				// v1.41 Custom-Made <==> Manuf items
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != 0)
				{
					m_pClientList[iClientH]->m_iCustomItemValue_Attack += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 2);
					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 100)
						m_pClientList[iClientH]->m_iCustomItemValue_Attack = 100;
					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < -100)
						m_pClientList[iClientH]->m_iCustomItemValue_Attack = -100;

					if (m_pClientList[iClientH]->m_iCustomItemValue_Attack > 0)
					{	// Completion >100%
						dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
						dV1 = (dV2 / 100.0f) * (5.0f);
						m_pClientList[iClientH]->m_iMinAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM +
							m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;
						m_pClientList[iClientH]->m_iMinAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L +
							m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;
						if (m_pClientList[iClientH]->m_iMinAP_SM < 1) m_pClientList[iClientH]->m_iMinAP_SM = 1;
						if (m_pClientList[iClientH]->m_iMinAP_L < 1)  m_pClientList[iClientH]->m_iMinAP_L = 1;
						if (m_pClientList[iClientH]->m_iMinAP_SM > (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
							m_pClientList[iClientH]->m_iMinAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);
						if (m_pClientList[iClientH]->m_iMinAP_L > (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
							m_pClientList[iClientH]->m_iMinAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
					}
					else if (m_pClientList[iClientH]->m_iCustomItemValue_Attack < 0)
					{	// Completion <100%
						dV2 = (double)m_pClientList[iClientH]->m_iCustomItemValue_Attack;
						dV1 = (dV2 / 100.0f) * (5.0f);
						m_pClientList[iClientH]->m_iMaxAP_SM = m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM
							+ m_pClientList[iClientH]->m_cAttackBonus_SM + (int)dV1;
						m_pClientList[iClientH]->m_iMaxAP_L = m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L
							+ m_pClientList[iClientH]->m_cAttackBonus_L + (int)dV1;
						if (m_pClientList[iClientH]->m_iMaxAP_SM < 1) m_pClientList[iClientH]->m_iMaxAP_SM = 1;
						if (m_pClientList[iClientH]->m_iMaxAP_L < 1)  m_pClientList[iClientH]->m_iMaxAP_L = 1;
						if (m_pClientList[iClientH]->m_iMaxAP_SM < (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM))
							m_pClientList[iClientH]->m_iMaxAP_SM = (m_pClientList[iClientH]->m_cAttackDiceThrow_SM * m_pClientList[iClientH]->m_cAttackDiceRange_SM + m_pClientList[iClientH]->m_cAttackBonus_SM);
						if (m_pClientList[iClientH]->m_iMaxAP_L < (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L))
							m_pClientList[iClientH]->m_iMaxAP_L = (m_pClientList[iClientH]->m_cAttackDiceThrow_L * m_pClientList[iClientH]->m_cAttackDiceRange_L + m_pClientList[iClientH]->m_cAttackBonus_L);
					}
				}
				// Enchanted weapons
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
					case ITEMSTAT_POISONING:  // Poison			-> Increase effect of 1st item, or replace a different effect
					case ITEMSTAT_CRITICAL:  // Crit +			-> Increase effect of 1st item, or replace a different effect
					case ITEMSTAT_CASTPROB: // CP (as wand)	-> Increase effect of 1st item, or replace a different effect
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
						}
						else
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						}
						break;
					case ITEMSTAT_SHARP:  // Sharp
						m_pClientList[iClientH]->m_cAttackDiceRange_SM += 10;
						m_pClientList[iClientH]->m_cAttackDiceRange_L += 10;
						break;
					case ITEMSTAT_ANCIENT:  // Ancient
						m_pClientList[iClientH]->m_cAttackDiceRange_SM += 15;
						m_pClientList[iClientH]->m_cAttackDiceRange_L += 15;
						break;
					case ITEMSTAT_MANACONV: // ManaConv 
						m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
						//if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
						break;
					case ITEMSTAT_CRITICAL2: // Crit Increase 
						m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
						//if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
						break;
					default: // All others
						m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
						m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						break;
					}
				}

				// Enchanted weapon, compute stats
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					switch (dwSWEType) {
					case 0:  break;
					case 1:  m_pClientList[iClientH]->m_iAddPR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[23]) / 100); break;
					case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // armes HitProba
					case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					case 4:  m_pClientList[iClientH]->m_iAddHP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iVit) / 100); break;
					case 5:  m_pClientList[iClientH]->m_iAddSP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iStr) / 100); break;
					case 6:  m_pClientList[iClientH]->m_iAddMP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iMag) / 100); break;
					case 7:  m_pClientList[iClientH]->m_iAddMR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[3]) / 100); break;
					
					case 8:  
						//m_pClientList[iClientH]->m_iAddAbsPD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100);
						//if (m_pClientList[iClientH]->m_iAddAbsPD > 80) m_pClientList[iClientH]->m_iAddAbsPD = 80;
						m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100);
						if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] = 80;
						break;

					/*case 9:
						m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
						if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;*/
					//Magn0S:: Magic Abs by elements
					case 9:  
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect1) {
						case 1:	
							m_pClientList[iClientH]->m_iAddAbsEarth += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsEarth > 80) m_pClientList[iClientH]->m_iAddAbsEarth = 80;
							break;
						case 2:	
							m_pClientList[iClientH]->m_iAddAbsLight += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsLight > 80) m_pClientList[iClientH]->m_iAddAbsLight = 80;
							break;
						case 3:	
							m_pClientList[iClientH]->m_iAddAbsFire += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsFire > 80) m_pClientList[iClientH]->m_iAddAbsFire = 80;
							break;
						case 4:	
							m_pClientList[iClientH]->m_iAddAbsWater += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsWater > 80) m_pClientList[iClientH]->m_iAddAbsWater = 80;
							break;
						case 5:	
							m_pClientList[iClientH]->m_iAddAbsHoly += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsHoly > 80) m_pClientList[iClientH]->m_iAddAbsHoly = 80;
							break;
						case 6:	
							m_pClientList[iClientH]->m_iAddAbsUnholy += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100); 
							if (m_pClientList[iClientH]->m_iAddAbsUnholy > 80) m_pClientList[iClientH]->m_iAddAbsUnholy = 80;
							break;
						default: 
							m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
							break;
						}
						break;

					case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case 11: m_pClientList[iClientH]->m_iAddExp += (int)(((dwSWEValue * 10) * m_pClientList[iClientH]->m_iExp) / 100); break;
					case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}
				}

				// Those Attack items where treated as a bundle above, see here special cases
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
				case DEF_ITEMEFFECTTYPE_ATTACK: // More combo for Flamberge
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue == 12)
					{
						m_pClientList[iClientH]->m_iAddCD += 5;
					}
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_MAXHPDOWN:
					m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					// Xelima Upgraded weapons
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iSideEffect_MaxHPdown -= iTemp;
					if (m_pClientList[iClientH]->m_iSideEffect_MaxHPdown < 2) m_pClientList[iClientH]->m_iSideEffect_MaxHPdown = 2;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE:
				case DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM:
					// SNOOPY: For wands, ignore m_sItemEffectValue4/5/6 for damage{
					m_pClientList[iClientH]->m_cAttackDiceThrow_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					m_pClientList[iClientH]->m_cAttackDiceRange_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					m_pClientList[iClientH]->m_cAttackBonus_L = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
					// Compute Mana save, max=80%
					m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
					if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
					//Snoopy: Reintroduced defence for staves
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					//Snoopy: Added Magic ToHit Bonus for some wands.
					m_pClientList[iClientH]->m_iMagicHitRatio_ItemEffect += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_DEFENSE: // PA for some weapons
					//m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					//m_pClientList[iClientH]->m_iAddAbsPD += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					//if (m_pClientList[iClientH]->m_iAddAbsPD > 80) m_pClientList[iClientH]->m_iAddAbsPD = 80;
					break;

				case DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY:
					// TYpe of special weapon, Duration of the effect
					m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
					m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;
					// SNOOPY: put this here for Xelima / IceElemental Bows
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					if ((bNotify == true) && (iEquipItemID == (int)sItemIndex))
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, 0);
					break;
				}
				break;

			case DEF_ITEMEFFECTTYPE_ADDEFFECT:
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
				case 1:	// MR
					m_pClientList[iClientH]->m_iAddResistMagic += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddResistMagic += (iTemp * 10);
					break;

				case 2:	// MS
					m_pClientList[iClientH]->m_iManaSaveRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iManaSaveRatio += iTemp;
					// MS max = 80%
					if (m_pClientList[iClientH]->m_iManaSaveRatio > 80) m_pClientList[iClientH]->m_iManaSaveRatio = 80;
					break;

				case 3:	// Adds physical damage 
					m_pClientList[iClientH]->m_iAddPhysicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddPhysicalDamage += iTemp;
					break;

				case 4:	// Rings Defence Ratio 
					m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;

					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);
					break;

				case 5:	// Lucky Gold Ring
					// SNOOPY: changed to use m_sItemEffectValue2 as Lucky % value
					//         and cumulative betxeen items
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 > 0)
						m_pClientList[iClientH]->m_bIsLuckyEffect += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					else m_pClientList[iClientH]->m_bIsLuckyEffect += 10;
					// Merien Upgraded items
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_bIsLuckyEffect += (iTemp * 10);
					break;

				case 6:	// Add Magic Damage. 
					m_pClientList[iClientH]->m_iAddMagicalDamage += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddMagicalDamage += iTemp;
					break;

				case 7: // Lightning protection
					m_pClientList[iClientH]->m_iAddAbsLight += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsLight >= 0)		m_pClientList[iClientH]->m_iAddAbsLight += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsLight -= (iTemp * 5);
					break;
				case 8: // Earth Protection
					m_pClientList[iClientH]->m_iAddAbsEarth += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsEarth >= 0)	m_pClientList[iClientH]->m_iAddAbsEarth += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsEarth -= (iTemp * 5);
					break;
				case 9: // Fire Protection
					m_pClientList[iClientH]->m_iAddAbsFire += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsFire >= 0)		m_pClientList[iClientH]->m_iAddAbsFire += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsFire -= (iTemp * 5);
					break;
				case 10: // Ice Protection
					m_pClientList[iClientH]->m_iAddAbsWater += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (m_pClientList[iClientH]->m_iAddAbsWater >= 0)	m_pClientList[iClientH]->m_iAddAbsWater += (iTemp * 5);
					else												m_pClientList[iClientH]->m_iAddAbsWater -= (iTemp * 5);
					break;

				case 11: // Poison protection
					m_pClientList[iClientH]->m_iAddPR += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAddPR += (iTemp * 10);
					break;

				case 12: // Adds To Hit Bonus
					m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iHitRatio += (iTemp * 5);
					break;

				case 13: // Magin Ruby  Characters Hp recovery rate(% applied) added by the purity formula.
					m_pClientList[iClientH]->m_iAddHP += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 5);
					break;

				case 14: // Magin Diamond Attack probability(physical&magic) added by the purity formula.
					m_pClientList[iClientH]->m_iAddAR += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 5);
					break;

				case 15: // Magin Emerald Magical damage decreased(% applied) by the purity formula.    
					m_pClientList[iClientH]->m_iAddAbsMD += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 10);
					if (m_pClientList[iClientH]->m_iAddAbsMD > DEF_MAXMAGICALABS) m_pClientList[iClientH]->m_iAddAbsMD = DEF_MAXMAGICALABS;
					break;

				case 30: // Magin Sapphire	Phisical damage decreased(% applied) by the purity formula.	
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 10);
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] += iTemp;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] += iTemp;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] += iTemp;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] += iTemp;

					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] = 80;
					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] = 80;
					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] = 80;
					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] = 80;
					//m_pClientList[iClientH]->m_iAddAbsPD += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 / 10);
					//if (m_pClientList[iClientH]->m_iAddAbsPD > DEF_MAXMAGICALABS) m_pClientList[iClientH]->m_iAddAbsPD = DEF_MAXMAGICALABS;
					break;

					/*
					Functions rates confirm.
					Magic Diamond:	Completion rate / 5		= Functions rate. ? Maximum 20. (not%)
					Magic Ruby:		Completion rate / 5		= Functions rate.(%) ? Maximum 20%.
					Magic Emerald:	Completion rate / 10	= Functions rate.(%) ? Maximum 10%.
					Magic Sapphire: Completion rate / 10	= Functions rate.(%) ? Maximum 10%.
					*/

					// ******* Angel Code - Begin ******* //   
				case 16: // Angel STR//AngelicPandent(STR)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicStr = iTemp;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 1, iTemp);
					//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);
					break;
				case 17: // Angel DEX //AngelicPandent(DEX)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicDex = iTemp;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 2, iTemp);
					//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);
					break;
				case 18: // Angel INT//AngelicPandent(INT)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicInt = iTemp;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 3, iTemp);
					//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);
					break;
				case 19: // Angel MAG//AngelicPandent(MAG)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iAngelicMag = iTemp;
					SetAngelFlag(iClientH, DEF_OWNERTYPE_PLAYER, 4, iTemp);
					//SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETTING_SUCCESS, 0, 0, 0, 0);
					break;
				}
				if (cEquipPos == DEF_EQUIPPOS_BACK || cEquipPos == DEF_EQUIPPOS_BOOTS)
				{	// Snoopy Special effect capes/ mantles still have small DefRatio & PA
					m_pClientList[iClientH]->m_iDefenseRatio += 3;	// Standard DR
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] ++;
					//m_pClientList[iClientH]->m_iAddAbsPD++;
					// Merien Upgraded diverse (capes?)
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iTemp != 0)
					{
						m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);
						m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += iTemp;
						//m_pClientList[iClientH]->m_iAddAbsPD += iTemp;
					}
					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] = 80;
				}
				break;

			case DEF_ITEMEFFECTTYPE_ATTACK_ARROW:
				if ((m_pClientList[iClientH]->m_cArrowIndex != -1)
					&& (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cArrowIndex] == 0))
				{
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);	// ArrowIndex
				}
				else if (m_pClientList[iClientH]->m_cArrowIndex == -1)
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

				if (m_pClientList[iClientH]->m_cArrowIndex == -1)
				{
					m_pClientList[iClientH]->m_cAttackDiceThrow_SM = 0;
					m_pClientList[iClientH]->m_cAttackDiceRange_SM = 0;
					m_pClientList[iClientH]->m_cAttackBonus_SM = 0;
					m_pClientList[iClientH]->m_cAttackDiceThrow_L = 0;
					m_pClientList[iClientH]->m_cAttackDiceRange_L = 0;
					m_pClientList[iClientH]->m_cAttackBonus_L = 0;
				}
				else
				{
					m_pClientList[iClientH]->m_cAttackDiceThrow_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					m_pClientList[iClientH]->m_cAttackDiceRange_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					m_pClientList[iClientH]->m_cAttackBonus_SM += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
					m_pClientList[iClientH]->m_cAttackDiceThrow_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue4;
					m_pClientList[iClientH]->m_cAttackDiceRange_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue5;
					m_pClientList[iClientH]->m_cAttackBonus_L += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6;
				}
				// Xelima Upgraded bows
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_cAttackBonus_SM += iTemp;
				m_pClientList[iClientH]->m_cAttackBonus_L += iTemp;

				// Standard HitRatio uses Skill 
				m_pClientList[iClientH]->m_iHitRatio += m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill];
				// iHitRatio3 discards skill
				// SNOOPY: Added support for enchanted bows. (1st stat)
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
						case ITEMSTAT_POISONING:  // Poison			-> Increase effect of 1st item, or replace a different effect
						case ITEMSTAT_CRITICAL:  // Crit +			-> Increase effect of 1st item, or replace a different effect
						case ITEMSTAT_CASTPROB: // CP (as wand)	-> Increase effect of 1st item, or replace a different effect
							if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
							{
								m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
							}
							else
							{
								m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
								m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
							}
							break;
						case ITEMSTAT_SHARP:  // Sharp
							m_pClientList[iClientH]->m_cAttackDiceRange_SM += 10;
							m_pClientList[iClientH]->m_cAttackDiceRange_L += 10;
							break;
						case ITEMSTAT_ANCIENT:  // Ancient
							m_pClientList[iClientH]->m_cAttackDiceRange_SM += 15;
							m_pClientList[iClientH]->m_cAttackDiceRange_L += 15;
							break;
						case ITEMSTAT_MANACONV: // ManaConv 
							m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
							if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
							break;
						case ITEMSTAT_CRITICAL2: // Crit Increase 
							m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
							if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
							break;
						default: // All others
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
							break;
					}
				}

				// SNOOPY: Added support for enchanted bows. (2nd stat)
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					switch (dwSWEType) {
					case 0:  break;
					case 1:  m_pClientList[iClientH]->m_iAddPR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[23]) / 100); break;
					case 2:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // adds Hit Proba
					case 3:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					case 4:  m_pClientList[iClientH]->m_iAddHP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iVit) / 100); break;
					case 5:  m_pClientList[iClientH]->m_iAddSP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iStr) / 100); break;
					case 6:  m_pClientList[iClientH]->m_iAddMP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iMag) / 100); break;
					case 7:  m_pClientList[iClientH]->m_iAddMR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[3]) / 100); break;
					case 8:  
						m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100);
						//m_pClientList[iClientH]->m_iAddAbsPD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100);
						if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] = 80;
						break;
					/*case 9:
						m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
						if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;*/
					//Magn0S:: Add elemental abs
					case 9:
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect1) {
						case 1:
							m_pClientList[iClientH]->m_iAddAbsEarth += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsEarth > 80) m_pClientList[iClientH]->m_iAddAbsEarth = 80;
							break;
						case 2:
							m_pClientList[iClientH]->m_iAddAbsLight += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsLight > 80) m_pClientList[iClientH]->m_iAddAbsLight = 80;
							break;
						case 3:
							m_pClientList[iClientH]->m_iAddAbsFire += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsFire > 80) m_pClientList[iClientH]->m_iAddAbsFire = 80;
							break;
						case 4:
							m_pClientList[iClientH]->m_iAddAbsWater += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsWater > 80) m_pClientList[iClientH]->m_iAddAbsWater = 80;
							break;
						case 5:
							m_pClientList[iClientH]->m_iAddAbsHoly += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsHoly > 80) m_pClientList[iClientH]->m_iAddAbsHoly = 80;
							break;
						case 6:
							m_pClientList[iClientH]->m_iAddAbsUnholy += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsUnholy > 80) m_pClientList[iClientH]->m_iAddAbsUnholy = 80;
							break;
						default:
							m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
							break;
						}
						break;

					case 10: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case 11: m_pClientList[iClientH]->m_iAddExp += (int)(((dwSWEValue * 10) * m_pClientList[iClientH]->m_iExp) / 100); break;
					case 12: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}
				}
				break;

			case DEF_ITEMEFFECTTYPE_DEFENSE: // Notice that MainGauche is here as well .....			
			case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
				// SNOOPY: 2 values for Defence ratio fot PvP mode HIGH
				m_pClientList[iClientH]->m_iDefenseRatio += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;


				// Merien Upgraded armors, add DefRatio for all defence items
				iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				m_pClientList[iClientH]->m_iDefenseRatio += (iTemp * 5);

				// Manufs armors 
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) != 0)
				{
					m_pClientList[iClientH]->m_iCustomItemValue_Defense += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					dV2 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					dV3 = (double)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					dV1 = (double)(dV2 / 100.0f) * dV3;
					dV1 = dV1 / 2.0f;
					m_pClientList[iClientH]->m_iDefenseRatio += (int)dV1;
					if (m_pClientList[iClientH]->m_iDefenseRatio <= 0) m_pClientList[iClientH]->m_iDefenseRatio = 1;
				}

				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x000F0000) >> 16;
					switch (dwSWEType) {
					case ITEMSTAT_CRITICAL:  // Crit +			-> May increase effect of 1st item or give effect if no other effect present
					case ITEMSTAT_POISONING:  // Poison			-> May Increase effect of 1st item or give effect if no other effect present
					case ITEMSTAT_CASTPROB: // CP (as wand)	-> May Increase effect of 1st item or give effect if no other effect present				
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == dwSWEType)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue += dwSWEValue;
						}
						else {
							if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == 0)
							{
								m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
								m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
							}
						}
						break;
					case ITEMSTAT_RIGHTEOUS: // Rite -> Give effect if no other effect present	
						if (m_pClientList[iClientH]->m_iSpecialWeaponEffectType == 0)
						{
							m_pClientList[iClientH]->m_iSpecialWeaponEffectType = dwSWEType;
							m_pClientList[iClientH]->m_iSpecialWeaponEffectValue = dwSWEValue;
						}
						break;
					case ITEMSTAT_SHARP: // Sharp MG have effect in iCalculatteAttackEffect function
					case ITEMSTAT_ANCIENT: // Ancient MG have effect in iCalculatteAttackEffect function
						break;
					case ITEMSTAT_MANACONV: // ManaConv  Total max 20
						m_pClientList[iClientH]->m_iAddTransMana += dwSWEValue;	// SNOOPY changed to 20 as for Crit increase
						//if (m_pClientList[iClientH]->m_iAddTransMana > 20) m_pClientList[iClientH]->m_iAddTransMana = 20;
						break;
					case ITEMSTAT_CRITICAL2: // Crit Increase Total max 20
						m_pClientList[iClientH]->m_iAddChargeCritical += dwSWEValue;
						//if (m_pClientList[iClientH]->m_iAddChargeCritical > 20) m_pClientList[iClientH]->m_iAddChargeCritical = 20;
						break;
					default:
						break;
					}
				}
				// stated armors/shields
				if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) != 0)
				{
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000F00) >> 8;
					// HP or Rep+ will have effect on Armor or MainGauche !
					switch (dwSWEType) {
					case 0:  break;
					case ITEMSTAT2_PSNRES:  m_pClientList[iClientH]->m_iAddPR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[23]) / 100); break;
					case ITEMSTAT2_HITPROB:  m_pClientList[iClientH]->m_iAddAR += (int)dwSWEValue * 7; break; // Armes Hit Proba
					case ITEMSTAT2_DEF:  m_pClientList[iClientH]->m_iAddDR += (int)dwSWEValue * 7; break;
					
					case ITEMSTAT2_HPREC:  // HPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldHPrec += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iVit) / 100);
						else m_pClientList[iClientH]->m_iAddHP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iVit) / 100);
						break;
					case ITEMSTAT2_SPREC:  // SPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldSPrec += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iStr) / 100);
						else m_pClientList[iClientH]->m_iAddSP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iStr) / 100);
						break;
					case ITEMSTAT2_MPREC:   // MPrec
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
							iShieldMPrec += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iMag) / 100);
						else m_pClientList[iClientH]->m_iAddMP += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_iMag) / 100);
						break;
					case ITEMSTAT2_MR:
						m_pClientList[iClientH]->m_iAddMR += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_cSkillMastery[3]) / 100);
						break;
					case ITEMSTAT2_PA: // PA
						if (cEquipPos == DEF_EQUIPPOS_LHAND)
						{
							m_pClientList[iClientH]->m_iDamageAbsorption_Shield += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100);
							if (m_pClientList[iClientH]->m_iDamageAbsorption_Shield > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 80;
						}
						else
						{
							m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) / 100); //m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (int)dwSWEValue * 3;
							if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] = 80;

						}
						break;
					case ITEMSTAT2_MA:
					
						switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect1) {
						case 1:
							m_pClientList[iClientH]->m_iAddAbsEarth += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsEarth > 80) m_pClientList[iClientH]->m_iAddAbsEarth = 80;
							break;
						case 2:
							m_pClientList[iClientH]->m_iAddAbsLight += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsLight > 80) m_pClientList[iClientH]->m_iAddAbsLight = 80;
							break;
						case 3:
							m_pClientList[iClientH]->m_iAddAbsFire += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsFire > 80) m_pClientList[iClientH]->m_iAddAbsFire = 80;
							break;
						case 4:
							m_pClientList[iClientH]->m_iAddAbsWater += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsWater > 80) m_pClientList[iClientH]->m_iAddAbsWater = 80;
							break;
						case 5:
							m_pClientList[iClientH]->m_iAddAbsHoly += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsHoly > 80) m_pClientList[iClientH]->m_iAddAbsHoly = 80;
							break;
						case 6:
							m_pClientList[iClientH]->m_iAddAbsUnholy += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsUnholy > 80) m_pClientList[iClientH]->m_iAddAbsUnholy = 80;
							break;
						default:
							m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
							if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
							break;
						}
						
						//m_pClientList[iClientH]->m_iAddAbsMD += (int)(((dwSWEValue * 7) * m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3) / 100);
						//if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
						break;
					case ITEMSTAT2_CAD: m_pClientList[iClientH]->m_iAddCD += (int)dwSWEValue; break;
					case ITEMSTAT2_EXP: m_pClientList[iClientH]->m_iAddExp += (int)(((dwSWEValue * 10) * m_pClientList[iClientH]->m_iExp) / 100); break;
					case ITEMSTAT2_GOLD: m_pClientList[iClientH]->m_iAddGold += (int)dwSWEValue * 10; break;
					}

				}
				switch (cEquipPos) { // Sub-cases of DEF_ITEMEFFECTTYPE_DEFENSE
				/*case DEF_EQUIPPOS_LHAND:
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 != 0)
					{
						m_pClientList[iClientH]->m_iDamageAbsorption_Shield = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					}
					else // Snoopy individual PA setting for shields...
					{
						m_pClientList[iClientH]->m_iDamageAbsorption_Shield = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) - (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) / 3;
					}
					// Merien Upgraded shield +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Shield += iTemp;

					break;*/
				case DEF_EQUIPPOS_TWOHAND:

				case DEF_EQUIPPOS_RHAND:

					break;
				
				case DEF_EQUIPPOS_LHAND:
					m_pClientList[iClientH]->m_iDamageAbsorption_Shield += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					
					// Merien Upgraded shield +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Shield += iTemp;

					if (m_pClientList[iClientH]->m_iDamageAbsorption_Shield > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Shield = 80;

					break;

				case DEF_EQUIPPOS_LEGGINGS:
				case DEF_EQUIPPOS_ARMS:
				case DEF_EQUIPPOS_BODY:
				case DEF_EQUIPPOS_HEAD:	// PA on Head
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					//m_pClientList[iClientH]->m_iAddAbsPD += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					m_pClientList[iClientH]->m_iAddAbsMD += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3);

					// Merien Upgraded helmet +1 PA
					iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += iTemp;
					//m_pClientList[iClientH]->m_iAddAbsPD += iTemp;
					m_pClientList[iClientH]->m_iAddAbsMD += iTemp;

					//if (m_pClientList[iClientH]->m_iAddAbsPD > 80) m_pClientList[iClientH]->m_iAddAbsPD = 80;
					if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] > 80) m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] = 80;
					if (m_pClientList[iClientH]->m_iAddAbsMD > 80) m_pClientList[iClientH]->m_iAddAbsMD = 80;
					break;

				default:
					// SNOOPY: Kept this here so we can use Damage absorbtion for something else (ie: Cape)
					//m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
					// Merien Upgraded diverse (capes?)
					//iTemp = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					//m_pClientList[iClientH]->m_iDamageAbsorption_Armor[cEquipPos] += iTemp;
					break;
				}

				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
				case DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY:
					m_pClientList[iClientH]->m_iSpecialAbilityType = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect;
					m_pClientList[iClientH]->m_iSpecialAbilityLastSec = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffectValue1;
					m_pClientList[iClientH]->m_iSpecialAbilityEquipPos = (int)cEquipPos;
					if ((bNotify == true) && (iEquipItemID == (int)sItemIndex))
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 2, m_pClientList[iClientH]->m_iSpecialAbilityType, m_pClientList[iClientH]->m_iSpecialAbilityTime, 0);
					break;
				}
				break;
			}
		}

	//v1.432
	if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType == 0) && (bNotify == true))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SPECIALABILITYSTATUS, 4, 0, 0, 0);
		if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true)
		{
			m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = false;
			m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp & 0xFF0F;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
		}
	}
	if ((iPrevSAType != 0) && (m_pClientList[iClientH]->m_iSpecialAbilityType != 0)
		&& (iPrevSAType != m_pClientList[iClientH]->m_iSpecialAbilityType) && (bNotify == true))
	{
		if (m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled == true)
		{
			SendNotifyMsg(0, i, DEF_NOTIFY_SPECIALABILITYSTATUS, 3, 0, 0, 0); // 20min
			m_pClientList[iClientH]->m_bIsSpecialAbilityEnabled = false;
			m_pClientList[iClientH]->m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp & 0xFF0F;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
		}
	}
			
	m_pClientList[iClientH]->m_iAddHP += iShieldHPrec;
	m_pClientList[iClientH]->m_iAddSP += iShieldSPrec;
	m_pClientList[iClientH]->m_iAddMP += iShieldMPrec;
	if (m_pClientList[iClientH]->m_iAddHP > 100) m_pClientList[iClientH]->m_iAddHP = 100;
	if (m_pClientList[iClientH]->m_iAddSP > 100) m_pClientList[iClientH]->m_iAddSP = 100;
	if (m_pClientList[iClientH]->m_iAddMP > 100) m_pClientList[iClientH]->m_iAddMP = 100;
	
	m_pClientList[iClientH]->m_cHeroArmourBonus = _cCheckHeroItemEquipped(iClientH);
	//m_pClientList[iClientH]->m_cHeroArmourBonus = 0;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_HEROBONUS, m_pClientList[iClientH]->m_cHeroArmourBonus, 0, 0, 0);
	
	// Snoopy: Bonus for Angels	
	m_pClientList[iClientH]->m_iAddMR += m_pClientList[iClientH]->m_iAngelicInt;
	m_pClientList[iClientH]->m_iAddDR += m_pClientList[iClientH]->m_iAngelicDex;
	m_pClientList[iClientH]->m_iAddMagicalDamage += m_pClientList[iClientH]->m_iAngelicMag;
	m_pClientList[iClientH]->m_iAddPhysicalDamage += m_pClientList[iClientH]->m_iAngelicStr;

	if (m_pClientList[iClientH]->m_iHP > iGetMaxHP(iClientH)) m_pClientList[iClientH]->m_iHP = iGetMaxHP(iClientH, false);
	if (m_pClientList[iClientH]->m_iMP > iGetMaxMP(iClientH)) m_pClientList[iClientH]->m_iMP = iGetMaxMP(iClientH);
	if (m_pClientList[iClientH]->m_iSP > iGetMaxSP(iClientH)) m_pClientList[iClientH]->m_iSP = iGetMaxSP(iClientH);

	// Maximum value for  PA, here instead of iCalculateAttackEffect
	/*if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] > DEF_MAXPHYSICALABS)	// Torse,  max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BODY] = DEF_MAXPHYSICALABS;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] > DEF_MAXPHYSICALABS)	// T阾e    max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_HEAD] = DEF_MAXPHYSICALABS;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] > DEF_MAXPHYSICALABS)	// Membres max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_ARMS] = DEF_MAXPHYSICALABS;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BOOTS] > DEF_MAXPHYSICALABS)	// Membres max 60 
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BOOTS] = DEF_MAXPHYSICALABS;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] > DEF_MAXPHYSICALABS)	// Pieds   max 60 
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_LEGGINGS] = DEF_MAXPHYSICALABS;
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] > DEF_MAXPHYSICALABS)	// Dos     max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Armor[DEF_EQUIPPOS_BACK] = DEF_MAXPHYSICALABS;*/
	if (m_pClientList[iClientH]->m_iDamageAbsorption_Shield > DEF_MAXPHYSICALABS)						// Shield  max 60
		m_pClientList[iClientH]->m_iDamageAbsorption_Shield = DEF_MAXPHYSICALABS;

	NotifyPlayerAttributes(iClientH);
	//SendNotifyMsg(0, iClientH, DEF_NOTIFY_ARMOURVALUES, 0, 0, 0, 0);
}

bool CGame::_bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID)
{
	bool bRet;

	// 脌搂脛隆赂娄 脕枚脕陇脟脧掳铆 禄莽驴毛 脠脛 禄莽露贸脕枚麓脗 戮脝脌脤脜脹 脠驴掳煤 脙鲁赂庐 潞脦潞脨 
	if (m_pClientList[iClientH] == 0) return false;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;

	switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
	case DEF_ITEMEFFECTTYPE_OCCUPYFLAG:
		bRet = __bSetOccupyFlag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
			0, iClientH, 0);
		if (bRet == true) {
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTFLAGSPOT, 0, 0, 0, 0);
		}
		return bRet;

		// crusade
	case DEF_ITEMEFFECTTYPE_CONSTRUCTIONKIT:
		// 掳脟录鲁 脜掳脝庐脌脤麓脵. 鹿脤碌茅路拢碌氓驴隆录颅赂赂 禄莽驴毛 掳隆麓脡脟脭. m_sItemEffectValue1: 掳脟脙脿鹿掳 脕戮路霉, m_sItemEffectValue2: 掳脟脙脿 陆脙掳拢 
		bRet = __bSetConstructionKit(m_pClientList[iClientH]->m_cMapIndex, dX, dY,                                 // 
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,        //
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,        //
			iClientH); // 录鲁脛隆脌脷 
		if (bRet == true) {
			// 掳脟录鲁 陆脙脌脹	
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			// 掳脟录鲁 陆脟脝脨		
			// Centuu : No se puede construir.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTCONSTRUCT, 0, 0, 0, 0);
		}
		return bRet;

	case DEF_ITEMEFFECTTYPE_DYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) {
				if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 11) ||
					(m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 12))
				{
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, 0, 0);
					return true;
				}
				else 
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, 0, 0);
					return false;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_ATTRIBUTEPOTION: // kazin
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) 
		{
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) 
			{
				auto attr = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute;
				auto dwType1 = (attr & 0x00F00000) >> 20;
				auto dwValue1 = (attr & 0x000F0000) >> 16;
				auto dwType2 = (attr & 0x0000F000) >> 12;
				auto dwValue2 = (attr & 0x00000F00) >> 8;

				auto& dst_attr = m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_dwAttribute;

				auto dst_dwType1 = (dst_attr & 0x00F00000) >> 20;
				auto dst_dwValue1 = (dst_attr & 0x000F0000) >> 16;
				auto dst_dwType2 = (dst_attr & 0x0000F000) >> 12;
				auto dst_dwValue2 = (dst_attr & 0x00000F00) >> 8;

				if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_dwAttribute & 0x0000F000) == 0)
				{
					return false;
				}

				if (dst_dwType2 != dwType2)
				{
					return false;
				}

				auto dst_value = dst_dwValue2 + dwValue2;

				switch (dwType2) {
				case ITEMSTAT2_PSNRES: // 1
				case ITEMSTAT2_HPREC: // 4
				case ITEMSTAT2_MPREC: // 6
				case ITEMSTAT2_SPREC: // 5
				case ITEMSTAT2_MR: // 7
				case ITEMSTAT2_PA: // 8
				case ITEMSTAT2_MA: // 9
				case ITEMSTAT2_DEF: // 3
				case ITEMSTAT2_HITPROB: // 2

					// Centuu : limite maximo para las attribute potion
					if (dwType2 == ITEMSTAT2_PA || dwType2 == ITEMSTAT2_MA)
					{
						if (dst_value > 15) dst_value = 15;
					}
					else
					{
						if (dst_value > 28) dst_value = 28;
					}
					dst_attr = 0;
					dst_dwType1 = dst_dwType1 << 20;
					dst_dwValue1 = dst_dwValue1 << 16;
					dwType2 = dwType2 << 12;
					dwValue2 = dst_value << 8;
					dst_attr = dst_attr | dst_dwType1 | dst_dwValue1;
					dst_attr = dst_attr | dwType2 | dwValue2;
					break;

				default:
					return false;
				}

				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_dwAttribute, 0, 0);
				return true;
				
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_ARMORDYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) {
				if (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 6) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, 0, 0);
					return true;
				}
				else {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, 0, 0);
					return false;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_WEAPONDYE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) {
				if (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 1) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor = (char)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cItemColor, 0, 0);
					return true;
				}
				else {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMCOLORCHANGE, sDestItemID, -1, 0, 0);
					return false;
				}
			}
		}
		break;

	//Magn0S:: Repair item by a "ticket"
	case DEF_ITEMEFFECTTYPE_REPAIR:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) {
				if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 1) || (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 6)) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_wMaxLifeSpan;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_wCurLifeSpan, 0, 0);
					return true;
				}
				else {
					//SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_wCurLifeSpan, 0, 0);
					return false;
				}
			}
		}
		break;

	//Magn0S:: Attune Pots
	case DEF_ITEMEFFECTTYPE_ATTUNE:
		if ((sDestItemID >= 0) && (sDestItemID < DEF_MAXITEMS)) {
			if (m_pClientList[iClientH]->m_pItemList[sDestItemID] != 0) {
				if ((m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_cCategory == 6) && (m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_sNewEffect1 == 0)) {
					m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_sNewEffect1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ATTUNEARMOUR, sDestItemID, m_pClientList[iClientH]->m_pItemList[sDestItemID]->m_sNewEffect1, 0, 0);
					//RequestUpdateItem(iClientH);
					return true;
				}
				else {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ATTUNEARMOUR, sDestItemID, -1, 0, 0);
					return false;
				}
			}
		}
		break;

	case DEF_ITEMEFFECTTYPE_FARMING:
		bRet = bPlantSeedBag(m_pClientList[iClientH]->m_cMapIndex, dX, dY,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1,
			m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2,
			iClientH);

		if (bRet == true) {
			// 掳脟录鲁 陆脙脌脹	
			GetExp(iClientH, (iDice(1, (m_pClientList[iClientH]->m_iLevel)))); // centu
		}
		else {
			// 掳脟录鲁 陆脟脝脨	
			// Centuu : No se puede plantar.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOMOREAGRICULTURE, 0, 0, 0, 0);
		}

		return bRet;

	default:
		break;
	}

	return true;
}

// v1.4311-3 脙脽掳隆  脟脭录枚 脌脭脌氓卤脟脌禄 脕脰麓脗 脟脭录枚   GetFightzoneTicketHandler 
void CGame::GetFightzoneTicketHandler(int iClientH)
{
	int   iRet, iEraseReq, iMonth, iDay, iHour;
	char* cp, cData[256], cItemName[21];
	class CItem* pItem;
	DWORD* dwp;
	short* sp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;

	if (m_pClientList[iClientH]->m_iFightZoneTicketNumber <= 0) {
		// 脌脭脌氓卤脟脌禄 麓脵 禄莽驴毛脟脽脌陆脌禄 戮脣赂掳麓脵.
		// 禄莽脜玫脌氓 鹿酶脠拢掳隆 脌陆录枚赂茅 驴鹿戮脿脌潞 脟脽麓脗碌楼 脌脭脌氓卤脟脌禄 麓脵 鹿脼脌潞掳忙驴矛 ..
		m_pClientList[iClientH]->m_iFightzoneNumber *= -1;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_FIGHTZONERESERVE, -1, 0, 0, 0);
		return;
	}

	ZeroMemory(cItemName, sizeof(cItemName));

	if (m_pClientList[iClientH]->m_iFightzoneNumber == 1)
		strcpy(cItemName, "ArenaTicket");
	else  wsprintf(cItemName, "ArenaTicket(%d)", m_pClientList[iClientH]->m_iFightzoneNumber);

	pItem = new class CItem;
	if (_bInitItemAttr(pItem, cItemName) == false) {
		delete pItem;
		return;
	}

	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
		// 驴隆路炉 鹿忙脕枚驴毛 脛脷碌氓
		if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

		// 戮脝脌脤脜脹脌禄 鹿脼脌禄 录枚 脌脰麓脵麓脗 掳脥脌脤 脠庐脕陇 碌脟戮煤脌赂鹿脟路脦 脌脭脌氓卤脟掳鲁录枚赂娄 禄漏碌碌 碌脠麓脵. 
		m_pClientList[iClientH]->m_iFightZoneTicketNumber = m_pClientList[iClientH]->m_iFightZoneTicketNumber - 1;

		pItem->m_sTouchEffectType = DEF_ITET_DATE;

		iMonth = m_pClientList[iClientH]->m_iReserveTime / 10000;
		iDay = (m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000) / 100;
		iHour = m_pClientList[iClientH]->m_iReserveTime - iMonth * 10000 - iDay * 100;

		pItem->m_sTouchEffectValue1 = iMonth;
		pItem->m_sTouchEffectValue2 = iDay;
		pItem->m_sTouchEffectValue3 = iHour;


		wsprintf(G_cTxt, "(*) Get FIGHTZONETICKET : Char(%s) TICKENUMBER (%d)(%d)(%d)", m_pClientList[iClientH]->m_cCharName, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
		PutLogFileList(G_cTxt);
		PutLogList(G_cTxt);

		ZeroMemory(cData, sizeof(cData));

		// 戮脝脌脤脜脹 戮貌戮煤麓脵麓脗 赂脼陆脙脕枚赂娄 脌眉录脹脟脩麓脵.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		// 1掳鲁 脠鹿碌忙脟脽麓脵.
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 戮貌脌潞 戮脝脌脤脜脹脌脤鹿脟路脦 脌氓脗酶碌脟脕枚 戮脢戮脪麓脵.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		if (iEraseReq == 1) delete pItem;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 53);

		iCalcTotalWeight(iClientH);

		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			DeleteClient(iClientH, true, true);
			break;
		}
	}
	else {
		delete pItem;

		iCalcTotalWeight(iClientH);

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
			DeleteClient(iClientH, true, true);
			break;
		}
	}
}

//Hero Code by Zabuza - fixed by Centu
void CGame::GetHeroMantleHandler(int iClientH, int iItemID, char* pString)
{
	int   i, iNum, iEraseReq;
	char cItemName[21];
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return;
	//if (m_pClientList[iClientH]->m_iEnemyKillCount < 100) return;
	//if (m_pClientList[iClientH]->m_iContribution < 10) return;
	if (m_pClientList[iClientH]->m_cSide == 0) return;

	if (_iGetItemSpaceLeft(iClientH) == 0) {
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, 0, 0);
		return;
	}

	//Prevents a crash if item dosent exist
	if (m_pItemConfigList[iItemID] == 0)  return;

	switch (iItemID) {
		// Hero Cape
	case 400: //Aresden HeroCape
	case 401: //Elvine HeroCape
		if (m_pClientList[iClientH]->m_iEnemyKillCount < 300) return;
		m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
		break;
	}

	// Da el item correspondiente dependiendo el sexo del personaje, centu.
	switch (m_pClientList[iClientH]->m_cSex) {
	case 1:
		switch (iItemID) {
		// Hero Hood
		case 392:
		case 394:
		// Hero Helm
		case 403: //Aresden HeroHelm(M)
		case 405: //Elvine HeroHelm(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
			// Hero Cap
		case 407: //Aresden HeroCap(M)
		case 409: //Elvine HeroCap(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
			// Hero Armour
		case 411: //Aresden HeroArmour(M)
		case 413: //Elvine HeroArmour(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 300 || m_pClientList[iClientH]->m_iContribution < 300) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
			m_pClientList[iClientH]->m_iContribution -= 300;
			break;
			// Hero Robe
		case 415: //Aresden HeroRobe(M)
		case 417: //Elvine HeroRobe(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 200 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 200;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
			// Hero Hauberk
		case 419: //Aresden HeroHauberk(M)
		case 421: //Elvine HeroHauberk(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 100) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 100;
			break;
			// Hero Leggings
		case 423: //Aresden HeroLeggings(M)
		case 425: //Elvine HeroLeggings(M)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 150) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 150;
			break;
		default:
			break;
		}
		break;

	case 2:
		switch (iItemID) {
			// Hero Hood
		case 393:
		case 395:
		case 404: //Aresden HeroHelm(W)
		case 406: //Elvine HeroHelm(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
		case 408: //Aresden HeroCap(W)
		case 410: //Elvine HeroCap(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
		case 412: //Aresden HeroArmour(W)
		case 414: //Elvine HeroArmour(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 300 || m_pClientList[iClientH]->m_iContribution < 300) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 300;
			m_pClientList[iClientH]->m_iContribution -= 300;
			break;
		case 416: //Aresden HeroRobe(W)
		case 418: //Elvine HeroRobe(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 200 || m_pClientList[iClientH]->m_iContribution < 200) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 200;
			m_pClientList[iClientH]->m_iContribution -= 200;
			break;
		case 420: //Aresden HeroHauberk(W)
		case 422: //Elvine HeroHauberk(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 100 || m_pClientList[iClientH]->m_iContribution < 100) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 100;
			m_pClientList[iClientH]->m_iContribution -= 100;
			break;
		case 424: //Aresden HeroLeggings(W)
		case 426: //Elvine HeroLeggings(W)
			if (m_pClientList[iClientH]->m_iEnemyKillCount < 150 || m_pClientList[iClientH]->m_iContribution < 150) return;
			m_pClientList[iClientH]->m_iEnemyKillCount -= 150;
			m_pClientList[iClientH]->m_iContribution -= 150;
			break;

		default:
			break;
		}
		break;
	}

	ZeroMemory(cItemName, sizeof(cItemName));
	memcpy(cItemName, m_pItemConfigList[iItemID]->m_cName, 20);
	// ReqPurchaseItemHandler
	iNum = 1;
	for (i = 1; i <= iNum; i++) {
		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == false) {
			delete pItem;
		}
		else {
			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				wsprintf(G_cTxt, "(*) Get HeroItem : Char(%s) Player-EK(%d) Player-Contr(%d) Hero Obtained(%s)", m_pClientList[iClientH]->m_cCharName, m_pClientList[iClientH]->m_iEnemyKillCount, m_pClientList[iClientH]->m_iContribution, cItemName);
				PutLogFileList(G_cTxt);

				pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;

				SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);
			
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
				calcularTop15HB(iClientH);
				// centu - manage contrib
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONTRIBPOINTS, m_pClientList[iClientH]->m_iContribution, 0, 0, 0);
				
				if (iEraseReq == 1) delete pItem;

				iCalcTotalWeight(iClientH);
			}
			else {
				delete pItem;

				iCalcTotalWeight(iClientH);

				SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, 0, 0);
			}
		}
	}
}

void CGame::_SetItemPos(int iClientH, char* pData)
{
	char* cp, cItemIndex;
	short* sp, sX, sY;

	if (m_pClientList[iClientH] == 0) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cItemIndex = *cp;
	cp++;

	sp = (short*)cp;
	sX = *sp;
	cp += 2;

	sp = (short*)cp;
	sY = *sp;
	cp += 2;

	// 脌脽赂酶碌脠 脕脗脟楼掳陋 潞赂脕陇 
	if (sY < -10) sY = -10;

	if ((cItemIndex < 0) || (cItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemIndex] != 0) {
		m_pClientList[iClientH]->m_ItemPosList[cItemIndex].x = sX;
		m_pClientList[iClientH]->m_ItemPosList[cItemIndex].y = sY;
	}
}

void CGame::CheckUniqueItemEquipment(int iClientH)
{
	int i, iDamage;

	if (m_pClientList[iClientH] == 0) return;
	// v1.3 脌炉麓脧脜漏 戮脝脌脤脜脹脌脟 脌氓脗酶 禄贸脜脗赂娄 脝脟麓脺脟脩麓脵. 脝炉脕陇脌脦赂赂脌脤 脗酶驴毛 掳隆麓脡脟脩 戮脝脌脤脜脹脌禄 脌氓脗酶脟脩 掳忙驴矛 鹿镁掳脺脕酶麓脵.

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
			if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER) &&
				(m_pClientList[iClientH]->m_bIsItemEquipped[i] == true)) {
				// Touch Effect Type脌脤 DEF_ITET_OWNER脌脤赂茅 Touch Effect Value 1, 2, 3脌脤 脕脰脌脦 脛鲁赂炉脜脥脌脟 掳铆脌炉掳陋脌禄 掳庐麓脗麓脵. 

				if ((m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
					// ID 掳陋脌脤 赂脗麓脵.

					// 脌脷陆脜脌脟 掳脥脌脤 戮脝麓脧鹿脟路脦 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[i]->m_cEquipPos, i, 0, 0);
					ReleaseItemHandler(iClientH, i, true);
					// v1.4 麓毛鹿脤脕枚赂娄 戮貌麓脗麓脵. 
					iDamage = iDice(1, 10); // Centuu
					m_pClientList[iClientH]->m_iHP -= iDamage;
					if (m_pClientList[iClientH]->m_iHP <= 0) {
						ClientKilledHandler(iClientH, 0, 0, iDamage);
					}
				}
			}
		}
}

/*********************************************************************************************************************
** bool CGame::GetAngelHandler(int iClientH, char * pData, DWORD dwMsgSize)											**
** description   :: Reversed and coded by Snoopy																	**
*********************************************************************************************************************/
void CGame::GetAngelHandler(int iClientH, char* pData, DWORD dwMsgSize)
{
	char* cp, cData[256], cTmpName[5];
	int   iAngel, iItemNbe;
	class CItem* pItem;
	int   iRet, iEraseReq;
	
	WORD* wp;
	int* ip;
	DWORD* dwp;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	// Centuu: Razones distintas por la cu醠 no puedes obtener un angel.
	if (_iGetItemSpaceLeft(iClientH) == 0)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ANGEL_FAILED, 1, 0, 0, 0); //"Impossible to get a Tutelary Angel."
		return;
	}
	if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft < 5)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ANGEL_FAILED, 2, 0, 0, 0); //"You need additional Majesty Points."	
		return;
	}
	/*if (m_pClientList[iClientH]->m_iLevel < m_iPlayerMaxLevel)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ANGEL_FAILED, 3, 0, 0, 0); //"Only Majesty characters can receive Tutelary Angel."
		return;
	}*/
	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, cp);
	cp += 5;
	ip = (int*)cp;
	iAngel = *ip; // 0x00 l a i
	cp += 4;
	switch (iAngel) {
	case 1: // STR
		iItemNbe = 908;
		break;
	case 2: // DEX
		iItemNbe = 909;
		break;
	case 3: // INT
		iItemNbe = 910;
		break;
	case 4: // MAG
		iItemNbe = 911;
		break;
	default:
		PutLogList("Gail asked to create a wrong item!");
		break;
	}
	pItem = new class CItem;
	if (pItem == 0) return;
	if ((_bInitItemAttr(pItem, iItemNbe) == true))
	{
		pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
		pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
		pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
		pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true)
		{	
			SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= 5;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			// Centuu : Recibe el angel.
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ANGEL_RECEIVED, 0, 0, 0, 0); //"You have received the Tutelary Angel."
		}
		else
		{
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
			/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ // v1.4 
			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;
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
	else
	{
		delete pItem;
		pItem = 0;
	}
}

/*********************************************************************************************************************
**  void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)											**
**  DESCRIPTION			:: set exchange item																		**
**  LAST_UPDATED		:: March 20, 2005; 6:31 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- make sure there that the item player places in trade exists							**
**							- stores item number in other character													**
**							- counts total item numbers sent														**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)
{
	int iExH, i;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
	if (m_pClientList[iClientH]->m_iExchangeCount > 8) return;	//only 4 items trade

	//no admin trade
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) {
		_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
		_ClearExchangeStatus(iClientH);
	}

	if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0)) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;
		if ((m_pClientList[iExH] == 0) || (memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0)) {
			// 背券且 惑措规捞 绝芭唱 背券窍绊磊 沁带 弊 某腐磐啊 酒聪促. 	
		}
		else {
			// 背券窍绊磊 窍绰 惑措规俊霸 酒捞袍阑 舅妨霖促. 
			// 秦寸 酒捞袍捞 粮犁窍绰瘤, 荐樊捞 嘎绰瘤 眉农茄促. 
			if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount < iAmount) return;

			//No Duplicate items
			for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount;i++) {
				if (m_pClientList[iClientH]->m_cExchangeItemIndex[i] == (char)iItemIndex) {
					_ClearExchangeStatus(iExH);
					_ClearExchangeStatus(iClientH);
					return;
				}
			}

			// 背券窍绊磊 窍绰 酒捞袍 牢郸胶, 荐樊 历厘 
			m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->m_iExchangeCount] = (char)iItemIndex;
			m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->m_iExchangeCount] = iAmount;

			memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->m_iExchangeCount], m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, 20);



			m_pClientList[iClientH]->m_iExchangeCount++;
			SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect1,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect3,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect4);

			SendNotifyMsg(iClientH, iExH, DEF_NOTIFY_SETEXCHANGEITEM, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 + 100,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect1,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect3,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect4);
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ConfirmExchangeItem(int iClientH)																	**
**  DESCRIPTION			:: confirms exchange																		**
**  LAST_UPDATED		:: March 20, 2005; 5:51 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- trade cancels if player changes HGServer												**
**							- trade cancels if player is disconnected or in waiting process							**
**							- trade cancels if player is trading with himself										**
**							- trade cancels if player is trading with noone											**
**							- trade cancels if player is recieves an item which hasnt been traded					**
**							- trade cancels if other player receives an item which you didn't trade					**
**							- changed weight calculations to support multiple items									**
**							- trade cancels if there is not enough item space for trade to occur					**
**  MODIFICATION		::	- GM can't trade if admin-security = true												**
**							- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::ConfirmExchangeItem(int iClientH)
{
	int iExH, i;
	int iItemWeightA, iItemWeightB, iWeightLeftA, iWeightLeftB, iAmountLeft;
	class CItem* pItemA[8], * pItemB[8], * pItemAcopy[8], * pItemBcopy[8];

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;

	//Magn0S:: Cancelado a珲es de char bloqueado.
	if (m_pClientList[iClientH]->m_iPenaltyBlockYear != 0) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Blocked characters can't Exchange items.");
		return;
	}

	if ((m_pClientList[iClientH]->m_bIsExchangeMode == true) && (m_pClientList[iClientH]->m_iExchangeH != 0)) {
		iExH = m_pClientList[iClientH]->m_iExchangeH;

		// v1.42 汲付 捞繁 版快啊?
		if (iClientH == iExH) return;

		if (m_pClientList[iExH] != 0) {
			if ((memcmp(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[iExH]->m_cCharName, 10) != 0) ||
				(m_pClientList[iExH]->m_bIsExchangeMode != true) ||
				(memcmp(m_pClientList[iExH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName, 10) != 0)) {
				// 背券窍绊磊 沁带 弊 某腐磐啊 酒聪促. 背券 惑怕绰 秒家.
				_ClearExchangeStatus(iClientH);
				_ClearExchangeStatus(iExH);
				return;
			}
			else {
				m_pClientList[iClientH]->m_bIsExchangeConfirm = true;
				if (m_pClientList[iExH]->m_bIsExchangeConfirm == true) {
					// 惑措规档 背券狼荤甫 灌躯促. 背券捞 己赋且 荐 乐绰瘤 拌魂茄促. 酒捞袍 肮荐, 公霸 力茄阑 魄窜.

					//Check all items
					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] == 0) ||
							(memcmp(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iClientH]->m_cExchangeItemName[i], 20) != 0)) {
							_ClearExchangeStatus(iClientH);
							_ClearExchangeStatus(iExH);
							return;
						}
					}
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] == 0) ||
							(memcmp(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName, m_pClientList[iExH]->m_cExchangeItemName[i], 20) != 0)) {
							_ClearExchangeStatus(iClientH);
							_ClearExchangeStatus(iExH);
							return;
						}
					}

					iWeightLeftA = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);
					iWeightLeftB = _iCalcMaxLoad(iExH) - iCalcTotalWeight(iExH);

					//Calculate weight for items
					iItemWeightA = 0;
					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						iItemWeightA = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]],
							m_pClientList[iClientH]->m_iExchangeItemAmount[i]);
					}
					iItemWeightB = 0;
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						iItemWeightB = iGetItemWeight(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]],
							m_pClientList[iExH]->m_iExchangeItemAmount[i]);
					}

					//See if the other person can take the item weightload
					if ((iWeightLeftA < iItemWeightB) || (iWeightLeftB < iItemWeightA)) {
						// 背券窍绊磊 窍绰 酒捞袍阑 罐阑 公霸傍埃捞 绝促. 背券 阂啊瓷. 
						_ClearExchangeStatus(iClientH);
						_ClearExchangeStatus(iExH);
						return;
					}

					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						// 荐樊窜困啊 乐绰 酒捞袍狼 版快绰 货肺 积己阑 矫难具 唱喘荐 乐促.
						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {

							if (m_pClientList[iClientH]->m_iExchangeItemAmount[i] > m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount) {
								// 背券窍绊磊 沁带 荐樊焊促 酒捞袍捞 利促. 弊悼救 临绢甸菌促.
								_ClearExchangeStatus(iClientH);
								_ClearExchangeStatus(iExH);
								return;
							}
							pItemA[i] = new class CItem;
							_bInitItemAttr(pItemA[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];

							// 肺弊甫 巢扁扁 困茄 酒捞袍 汗荤拱 
							pItemAcopy[i] = new class CItem;
							_bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemAcopy[i], pItemA[i]);
							pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
						}
						else {
							pItemA[i] = (class CItem*)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]];
							pItemA[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];

							// 肺弊甫 巢扁扁 困茄 酒捞袍 汗荤拱 
							pItemAcopy[i] = new class CItem;
							_bInitItemAttr(pItemAcopy[i], m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemAcopy[i], pItemA[i]);
							pItemAcopy[i]->m_dwCount = m_pClientList[iClientH]->m_iExchangeItemAmount[i];
						}
					}

					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {

							if (m_pClientList[iExH]->m_iExchangeItemAmount[i] > m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount) {
								// 背券窍绊磊 沁带 荐樊焊促 酒捞袍捞 利促. 弊悼救 临绢甸菌促.
								_ClearExchangeStatus(iClientH);
								_ClearExchangeStatus(iExH);
								return;
							}
							pItemB[i] = new class CItem;
							_bInitItemAttr(pItemB[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];

							// 肺弊甫 巢扁扁 困茄 酒捞袍 汗荤拱 
							pItemBcopy[i] = new class CItem;
							_bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemBcopy[i], pItemB[i]);
							pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
						}
						else {
							pItemB[i] = (class CItem*)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]];
							pItemB[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];

							// 肺弊甫 巢扁扁 困茄 酒捞袍 汗荤拱 
							pItemBcopy[i] = new class CItem;
							_bInitItemAttr(pItemBcopy[i], m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cName);
							bCopyItemContents(pItemBcopy[i], pItemB[i]);
							pItemBcopy[i]->m_dwCount = m_pClientList[iExH]->m_iExchangeItemAmount[i];
						}
					}

					// 刚历 酒捞袍阑 持篮 饶 
					for (i = 0; i < m_pClientList[iExH]->m_iExchangeCount; i++) {
						bAddItem(iClientH, pItemB[i], 0);
						_bItemLog(DEF_ITEMLOG_EXCHANGE, iExH, iClientH, pItemBcopy[i]);
						delete pItemBcopy[i];
						pItemBcopy[i] = 0;
						if ((m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
							//
							iAmountLeft = (int)m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iExH]->m_iExchangeItemAmount[i];
							if (iAmountLeft < 0) iAmountLeft = 0;
							// v1.41 !!!
							SetItemCount(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], iAmountLeft);

						}
						else {
							// 父距 厘馒等 酒捞袍捞扼搁 秦力茄促.
							ReleaseItemHandler(iExH, m_pClientList[iExH]->m_cExchangeItemIndex[i], true);
							SendNotifyMsg(0, iExH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iExH]->m_cExchangeItemIndex[i], m_pClientList[iExH]->m_iExchangeItemAmount[i], 0, m_pClientList[iClientH]->m_cCharName);
							m_pClientList[iExH]->m_pItemList[m_pClientList[iExH]->m_cExchangeItemIndex[i]] = 0;
						}
					}

					for (i = 0; i < m_pClientList[iClientH]->m_iExchangeCount; i++) {
						bAddItem(iExH, pItemA[i], 0);
						_bItemLog(DEF_ITEMLOG_EXCHANGE, iClientH, iExH, pItemAcopy[i]);
						delete pItemAcopy[i];
						pItemAcopy[i] = 0;

						if ((m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
							(m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
							//
							iAmountLeft = (int)m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]]->m_dwCount - m_pClientList[iClientH]->m_iExchangeItemAmount[i];
							if (iAmountLeft < 0) iAmountLeft = 0;
							// v1.41 !!!
							SetItemCount(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], iAmountLeft);

						}
						else {
							// 父距 厘馒等 酒捞袍捞扼搁 秦力茄促.
							ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_cExchangeItemIndex[i], true);
							SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, m_pClientList[iClientH]->m_cExchangeItemIndex[i], m_pClientList[iClientH]->m_iExchangeItemAmount[i], 0, m_pClientList[iExH]->m_cCharName);
							m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_cExchangeItemIndex[i]] = 0;
						}
					}

					m_pClientList[iClientH]->m_bIsExchangeMode = false;
					m_pClientList[iClientH]->m_bIsExchangeConfirm = false;
					ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
					m_pClientList[iClientH]->m_iExchangeH = 0;
					m_pClientList[iClientH]->m_iExchangeCount = 0;

					m_pClientList[iExH]->m_bIsExchangeMode = false;
					m_pClientList[iExH]->m_bIsExchangeConfirm = false;
					ZeroMemory(m_pClientList[iExH]->m_cExchangeName, sizeof(m_pClientList[iExH]->m_cExchangeName));
					m_pClientList[iExH]->m_iExchangeH = 0;
					m_pClientList[iExH]->m_iExchangeCount = 0;

					for (i = 0;i < 8;i++) {
						m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
						m_pClientList[iExH]->m_cExchangeItemIndex[i] = -1;
					}

					SendNotifyMsg(0, iClientH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);
					SendNotifyMsg(0, iExH, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, 0, 0, 0, 0);


					// 公霸 犁汲沥
					iCalcTotalWeight(iClientH);
					iCalcTotalWeight(iExH);
					return;
				}
			}
		}
		else {
			// 背券且 惑措规捞 绝促. 背券篮 秒家 等促.
			_ClearExchangeStatus(iClientH);
			return;
		}
	}
}

/*********************************************************************************************************************
**  void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount,									**
**									short dX, short dY, WORD wObjectID, char *pItemName)							**
**  DESCRIPTION			:: exchange item handler																	**
**  LAST_UPDATED		:: March 20, 2005; 6:31 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- stores item number																	**
**							- counts total item numbers																**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char* pItemName)
{
	short sOwnerH;
	char  cOwnerType;
	int i;

	if (m_pClientList[iClientH] == 0) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount < iAmount) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
	if (m_pClientList[iClientH]->m_bIsExchangeMode == true) return;
	if (wObjectID >= DEF_MAXCLIENTS) return;

	// 酒捞袍阑 穿备客 背券窍摆促绰 皋矫瘤啊 档馒沁促. 惑措规俊霸 舅府绊 剧率 葛滴 背券芒阑 凯档废 茄促. 
	//捞力 dX, dY俊 乐绰 坷宏璃飘俊霸 家厚己 酒捞袍阑 扒匙霖促. 
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

	if ((sOwnerH != 0) && (cOwnerType == DEF_OWNERTYPE_PLAYER)) {

		if ((m_bAdminSecurity == true) && (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0 && m_pClientList[sOwnerH]->m_iAdminUserLevel < 7)) return;

		// v1.4 林绊磊 茄 按眉客 嘎绰瘤 魄窜茄促.
		if (wObjectID != 0) {
			if (wObjectID < 10000) {
				// 敲饭捞绢 
				if (m_pClientList[wObjectID] != 0) {
					if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
				}
			}
			else sOwnerH = 0;
		}

		if ((sOwnerH == 0) || (m_pClientList[sOwnerH] == 0)) {
			//戮脝脌脤脜脹 卤鲁脠炉脌禄 驴盲脙禄脟脩 脟脙路鹿脌脤戮卯驴隆掳脭 脟脴麓莽 脌搂脛隆驴隆 脟脙路鹿脌脤戮卯掳隆 戮酶脌陆脌禄 戮脣赂掳麓脵. 
			_ClearExchangeStatus(iClientH);
		}
		else {
			if ((m_pClientList[sOwnerH]->m_bIsExchangeMode == true) || (m_pClientList[sOwnerH]->m_sAppr2 & 0xF000) ||
				(m_pMapList[m_pClientList[sOwnerH]->m_cMapIndex]->m_bIsFightZone == true)) {
				// 禄贸麓毛鹿忙脌脤 脌脤鹿脤 卤鲁脠炉 脕脽脌脤掳脜鲁陋 脌眉脜玫赂冒碌氓, 脠陇脌潞 禄莽脜玫脌氓脌脤麓脵. 卤鲁脠炉赂冒碌氓路脦 碌茅戮卯掳楼 录枚 戮酶麓脵. 
				_ClearExchangeStatus(iClientH);
			}
			else {
				// 卤鲁脠炉赂冒碌氓掳隆 陆脙脌脹碌脟戮煤麓脵. 脌脦碌娄陆潞, 脌脤赂搂 脌煤脌氓  
				m_pClientList[iClientH]->m_bIsExchangeMode = true;
				m_pClientList[iClientH]->m_iExchangeH = sOwnerH;
				ZeroMemory(m_pClientList[iClientH]->m_cExchangeName, sizeof(m_pClientList[iClientH]->m_cExchangeName));
				strcpy(m_pClientList[iClientH]->m_cExchangeName, m_pClientList[sOwnerH]->m_cCharName);

				//Clear items in the list
				m_pClientList[iClientH]->m_iExchangeCount = 0;
				m_pClientList[sOwnerH]->m_iExchangeCount = 0;
				for (i = 0; i < 8; i++) {
					//Clear the trader
					ZeroMemory(m_pClientList[iClientH]->m_cExchangeItemName[i], sizeof(m_pClientList[iClientH]->m_cExchangeItemName[i]));
					m_pClientList[iClientH]->m_cExchangeItemIndex[i] = -1;
					m_pClientList[iClientH]->m_iExchangeItemAmount[i] = 0;
					//Clear the guy we're trading with
					ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeItemName[i], sizeof(m_pClientList[sOwnerH]->m_cExchangeItemName[i]));
					m_pClientList[sOwnerH]->m_cExchangeItemIndex[i] = -1;
					m_pClientList[sOwnerH]->m_iExchangeItemAmount[i] = 0;
				}

				// 卤鲁脠炉脟脧掳铆脌脷 脟脧麓脗 戮脝脌脤脜脹 脌脦碌娄陆潞, 录枚路庐 脌煤脌氓 
				m_pClientList[iClientH]->m_cExchangeItemIndex[m_pClientList[iClientH]->m_iExchangeCount] = (char)sItemIndex;
				m_pClientList[iClientH]->m_iExchangeItemAmount[m_pClientList[iClientH]->m_iExchangeCount] = iAmount;

				memcpy(m_pClientList[iClientH]->m_cExchangeItemName[m_pClientList[iClientH]->m_iExchangeCount], m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, 20);

				m_pClientList[sOwnerH]->m_bIsExchangeMode = true;
				m_pClientList[sOwnerH]->m_iExchangeH = iClientH;
				ZeroMemory(m_pClientList[sOwnerH]->m_cExchangeName, sizeof(m_pClientList[sOwnerH]->m_cExchangeName));
				strcpy(m_pClientList[sOwnerH]->m_cExchangeName, m_pClientList[iClientH]->m_cCharName);

				m_pClientList[iClientH]->m_iExchangeCount++;
				SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex + 1000, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute, 0,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect1,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect2,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect3,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect4);

				SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_OPENEXCHANGEWINDOW, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, iAmount, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2 + 100,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute, 0,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect1,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect2,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect3,
					m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sNewEffect4);
			}
		}
	}
	else {
		// NPC驴脥麓脗 鹿掳掳脟脌禄 卤鲁脠炉脟脪 录枚 戮酶麓脵.
		_ClearExchangeStatus(iClientH);
	}
}

/*********************************************************************************************************************
**  void CGame::_ClearExchangeStatus(int iToH)																		**
**  DESCRIPTION			:: clear exchange status																	**
**  LAST_UPDATED		:: March 20, 2005; 7:15 PM; Hypnotoad														**
**	RETURN_VALUE		:: void																						**
**  NOTES				::	- clears all trade variables															**
**  MODIFICATION		::	- fixed exchange bug by Rambox															**
**********************************************************************************************************************/
void CGame::_ClearExchangeStatus(int iToH)
{
	if ((iToH <= 0) || (iToH >= DEF_MAXCLIENTS)) return;
	if (m_pClientList[iToH] == 0) return;

	if (m_pClientList[iToH]->m_cExchangeName != false)
		SendNotifyMsg(0, iToH, DEF_NOTIFY_CANCELEXCHANGEITEM, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	m_pClientList[iToH]->m_dwInitCCTime = false;
	m_pClientList[iToH]->m_iAlterItemDropIndex = 0;
	m_pClientList[iToH]->m_iExchangeH = 0;

	m_pClientList[iToH]->m_bIsExchangeMode = false;
	m_pClientList[iToH]->m_bIsExchangeConfirm = false;

	ZeroMemory(m_pClientList[iToH]->m_cExchangeName, sizeof(m_pClientList[iToH]->m_cExchangeName));
}

void CGame::CancelExchangeItem(int iClientH)
{
	_ClearExchangeStatus(m_pClientList[iClientH]->m_iExchangeH);
	_ClearExchangeStatus(iClientH);
}

void CGame::RequestItemUpgradeHandler(int iClientH, int iItemIndex)
{
	int i, iItemX, iItemY, iSoM, iSoX, iSomH, iSoxH, iValue; // v2.172
	DWORD dwTemp, dwSWEType;
	double dV1, dV2, dV3;
	int sItemUpgrade = 2;

	if (m_pClientList[iClientH] == 0) return;
	if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;

	iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;

	// HeroBow
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 1010)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1011) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// HeroSword & HeroWand
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 1003)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1004) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 988)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1005) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Helm (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 403 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 405)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1061) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Helm (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 404 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 406)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1062) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Cap (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 407 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 409)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1063) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Cap (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 408 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 410)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1064) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Armor (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 411 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 413)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1065) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Armor (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 412 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 414)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1066) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Robe (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 415 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 417)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1067) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Robe (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 416 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 418)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1068) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Hauberk (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 419 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 421)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1069) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Hauberk (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 420 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 422)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1070) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Leggings (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 423 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 425)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1071) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Leggings (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 424 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 426)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1072) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Hood (M)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 392 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 394)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 147) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	// Hero Hood (W)
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 393 || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 395)
	{
		if (iUpgradeHeroItemRequirements(iClientH, iItemIndex))
		{
			iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
			iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
			delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
			m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
			m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
			if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 148) == false)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		}
		else
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	if (HeroItemChecker(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum, 0, 0, 0) != 0) 
	{
		if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
			(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
			(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			return;
		}
		if (iUpgradeHeroCapeRequirements(iClientH, iItemIndex)) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_UPGRADEHEROCAPE, iItemIndex,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		else {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, -1, m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
		}
		return;
	}

	switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cCategory) {
	case 46: // Pendants are category 46
		if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType != 1) 
		{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);			
			return; // Pendants are type 1
		}
		if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cEquipPos < 11) 
		{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);			
			return; // Pendants are left finger or more
		}
		if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType != 14) 
		{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);			
			return; // Pendants are EffectType 14
		}
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectValue1) {
		default: // Other items are not upgradable
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);			
			return; // Pendants are EffectType 14

		case 16: // AngelicPandent(STR)
		case 17: // AngelicPandent(DEX)
		case 18: // AngelicPandent(INT)
		case 19: // AngelicPandent(MAG)
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) 
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}
			if (iValue == 10)
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
				return;
			}
			switch (iValue) {
				case 0: sItemUpgrade = 10*2; break;
				case 1: sItemUpgrade = 11*2; break;
				case 2: sItemUpgrade = 13*2; break;
				case 3: sItemUpgrade = 16*2; break;
				case 4: sItemUpgrade = 20*2; break;
				case 5: sItemUpgrade = 25*2; break;
				case 6: sItemUpgrade = 31*2; break;
				case 7: sItemUpgrade = 38*2; break;
				case 8: sItemUpgrade = 46*2; break;
				case 9: sItemUpgrade = 55*2; break;
			}
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1)
				|| (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2)
				|| (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3))
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
				return;
			}
			if (( m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade ) < 0) 
			{	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return; 
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade; 
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			iValue++;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int) -1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			break;
		}
		break;

	case 1:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
		{
		case 709: // DarkKnightFlameberge
		case 737: // DarkKnightGiantSword
		case 745: // BlackKnightTemple
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) 
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}

			if (iValue == 15) 
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
				return;
			}

			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType != 0) 
			{
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
					return;
				}
			}

			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}

			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

			if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 709) )
			{ // DarkKnightFlameberge
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 737) == false) 
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 737))  // DarkKnightGiantSword
			{
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 745) == false) 
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 745))  //BlackKnightTemple
			{
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			
			else 
			{
				iValue += 2;
				if (iValue > 15) iValue = 15;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			break;

		case 717: // DarkKnightRapier
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}
			if (iValue == 7) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
				return;
			}
			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
				return;
			}
			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			iValue++;
			dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
			dwTemp = dwTemp & 0x0FFFFFFF;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
			_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			break;

		default:
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
				if (dwSWEType == 9) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
					return;
				}
			}
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break;
					case 657: iSoM++; iSomH = i; break;
					}
				}
			if (iSoX > 0) {
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false, true);
					ItemDepleteHandler(iClientH, iSoxH, false, true);
					return;
				}
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) {
					iValue++;
					if (iValue > 10) {
						iValue = 10;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, false, true);
					}
				}
				else {
					iValue++;
					if (iValue > 7) {
						iValue = 7;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, false, true);
					}
				}
			}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
			break;
		}
		return;

	case 3:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum)
		{
		case 138: // DarkKnightFlameberge
		case 139: // DarkKnightGiantSword
		case 140: // BlackKnightTemple
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}

			if (iValue == 15)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
				return;
			}

			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType != 0)
			{
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
					(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
					return;
				}
			}

			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0)
			{
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}

			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);

			if ((iValue == 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 138))
			{ // DarkKnightFlameberge
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 139) == false)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 139))  // DarkKnightGiantSword
			{
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 140) == false)
				{
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			else if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 140))  //BlackKnightTemple
			{
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}

			else
			{
				iValue += 2;
				if (iValue > 15) iValue = 15;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
			}
			break;
		
		default:
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
				if (dwSWEType == 9) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
					return;
				}
			}
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break;
					case 657: iSoM++; iSomH = i; break;
					}
				}
			if (iSoX > 0) {
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false, true);
					ItemDepleteHandler(iClientH, iSoxH, false, true);
					return;
				}
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) {
					iValue++;
					if (iValue > 10) {
						iValue = 10;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, false, true);
					}
				}
				else {
					iValue++;
					if (iValue > 7) {
						iValue = 7;
					}
					else {
						dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
						dwTemp = dwTemp & 0x0FFFFFFF;
						m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
						ItemDepleteHandler(iClientH, iSoxH, false, true);
					}
				}
			}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
			break;
		}
		return;

	case 5:
		if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
			dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
			if (dwSWEType == 8) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
				return;
			}
		}
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
		case 623:
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			return;
		default:
			break;
		}
		iSoX = iSoM = 0;
		for (i = 0; i < DEF_MAXITEMS; i++)
			if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
				switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
				case 656: iSoX++; iSoxH = i; break;
				case 657: iSoM++; iSomH = i; break;
				}
			}
		if (iSoM > 0) {
			if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, true) == false) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
				if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false, true);
				ItemDepleteHandler(iClientH, iSomH, false, true);
				return;
			}
			iValue++;
			if (iValue > 10) {
				iValue = 10;
			}
			else {
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) {
					dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					dV2 = 0.2f * dV1;
					dV3 = dV1 + dV2;
				}
				else {
					dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					dV2 = 0.15f * dV1;
					dV3 = dV1 + dV2;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
				if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0) m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan += (short)dV2;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, 0, 0);
				ItemDepleteHandler(iClientH, iSomH, false, true);
			}
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);
		return;

	case  6:
	case 15:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
		case 621: // MerienPlateMailM
		case 622: // MerienPlateMailW 
		case 700: // SangAhHauberk 
		case 701: // SangAhFullHel
		case 702: // SangAhLeggings  
		case 704: // SangAhPlateMail 
		case 706: // DarkKnightHauberk
		case 707: // DarkKnightFullHelm 
		case 708: // DarkKnightLeggings  
		case 710: // DarkKnightPlateMail 
		case 711: // DarkMageHauberk 
		case 712: // DarkMageChainMail
		case 713: // DarkMageLeggings  
		case 716: // DarkMageLedderArmor
		case 719: // DarkMageScaleMail
		case 724: // DarkKnightHauberkW
		case 725: // DarkKnightFullHelmW 
		case 726: // DarkKnightLeggingsW 
		case 728: // DarkKnightPlateMailW 
		case 729: // DarkMageHauberkW 
		case 730: // DarkMageChainMailW 
		case 731: // DarkMageLeggingsW 
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
			return;

		default:
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) != 0) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00F00000) >> 20;
				if (dwSWEType == 8) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
					return;
				}
			}
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break;
					case 657: iSoM++; iSomH = i; break;
					}
				}
			if (iSoM > 0) {
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSomH, true) == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28;
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false, true);
					ItemDepleteHandler(iClientH, iSomH, false, true);
					return;
				}
				iValue++;
				if (iValue > 10) {
					iValue = 10;
				}
				else {
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
					if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) {
						dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
						dV2 = 0.2f * dV1;
						dV3 = dV1 + dV2;
					}
					else {
						dV1 = (double)m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
						dV2 = 0.15f * dV1;
						dV3 = dV1 + dV2;
					}
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = (short)dV3;
					if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 < 0) m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1 = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wMaxLifeSpan = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan += (short)dV2;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, 0, 0);
					ItemDepleteHandler(iClientH, iSomH, false, true);
				}
			}
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue1, 0, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2);

			break;
		}
		return;

	case 8:
		switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
			// centu - MagicWand(MS30-LLF) ??? 
		case 714: // DarkMageMagicStaff 
		case 732: // DarkMageMagicStaffW 
		case 738: // DarkMageMagicWand
		case 746: // BlackMageTemple
			if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 != m_pClientList[iClientH]->m_sCharIDnum1) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 != m_pClientList[iClientH]->m_sCharIDnum2) ||
				(m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 != m_pClientList[iClientH]->m_sCharIDnum3)) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 2, 0, 0, 0);
				return;
			}
			if (m_pClientList[iClientH]->m_iGizonItemUpgradeLeft <= 0) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}
			if (iValue == 15) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 1, 0, 0, 0);
				return;
			}
			sItemUpgrade = (iValue * (iValue + 6) / 8) + 2;
			if ((m_pClientList[iClientH]->m_iGizonItemUpgradeLeft - sItemUpgrade) < 0) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 3, 0, 0, 0);
				return;
			}
			m_pClientList[iClientH]->m_iGizonItemUpgradeLeft -= sItemUpgrade;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMUPGRADELEFT, m_pClientList[iClientH]->m_iGizonItemUpgradeLeft, 0, 0, 0);
			if (iValue == 0) {
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
			}
			if ((iValue >= 4) && ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 714) || (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 732))) {
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 738) == false) { // DarkMageMagicWand
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			if ((iValue >= 6) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 738)) {
				iItemX = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x;
				iItemY = m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y;
				delete m_pClientList[iClientH]->m_pItemList[iItemIndex];
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = 0;
				m_pClientList[iClientH]->m_pItemList[iItemIndex] = new class CItem;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].x = iItemX;
				m_pClientList[iClientH]->m_ItemPosList[iItemIndex].y = iItemY;
				if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], 746) == false) { // BlackMageTemple 
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					return;
				}
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				iValue += 2;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			if ((iValue >= 12) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum == 746)) {
				iValue += 2;
				if (iValue > 15) {
					iValue = 15;
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = 9;
				}
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF;
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIZONITEMCANGE, iItemIndex,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemType,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_wCurLifeSpan,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cName,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSprite,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sSpriteFrame,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2,
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			else {
				iValue += 2;
				if (iValue > 15) iValue = 15;
				dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
				dwTemp = dwTemp & 0x0FFFFFFF; // 潞帽脝庐 脜卢赂庐戮卯 
				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28); // 戮梅卤脳路鹿脌脤碌氓碌脠 潞帽脝庐掳陋 脌脭路脗
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
				_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
				break;
			}
			break;

		default:
			// 脌脧鹿脻 戮脝脌脤脜脹
			// v2.16 2002-5-21 掳铆卤陇脟枚录枚脕陇
			iSoX = iSoM = 0;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
					switch (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum) {
					case 656: iSoX++; iSoxH = i; break; // 陆潞脜忙 驴脌潞锚 脕娄赂庐赂露 
					case 657: iSoM++; iSomH = i; break; // 陆潞脜忙 驴脌潞锚 赂脼赂庐驴拢 
					}
				}
			// 陆潞脜忙 驴脌潞锚 脕娄赂庐赂露掳隆 脌脰麓脵.
			if (iSoX > 0) {
				// 戮梅卤脳路鹿脌脤碌氓 录潞掳酶 脠庐路眉 掳猫禄锚.
				if (bCheckIsItemUpgradeSuccess(iClientH, iItemIndex, iSoxH) == false) {
					// 戮梅卤脳路鹿脌脤碌氓 陆脟脝脨 
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
					iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0xF0000000) >> 28; // v2.172
					if (iValue >= 1) ItemDepleteHandler(iClientH, iItemIndex, false, true); // v2.172 +1 -> +2 麓脺掳猫驴隆录颅 陆脟脝脨脟脧赂茅 禄莽露贸脕眉 
					// 陆潞脜忙 驴脌潞锚 脕娄赂庐赂露 戮酶戮脴麓脵.
					ItemDepleteHandler(iClientH, iSoxH, false, true);
					return;
				}

				iValue++;
				if (iValue > 7)
					iValue = 7;
				else {
					// 戮梅卤脳路鹿脌脤碌氓 录潞掳酶. 戮脝脌脤脜脹 脝炉录潞 鹿脵虏脵掳铆
					dwTemp = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
					dwTemp = dwTemp & 0x0FFFFFFF; // 潞帽脝庐 脜卢赂庐戮卯 
					m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = dwTemp | (iValue << 28); // 戮梅卤脳路鹿脌脤碌氓碌脠 潞帽脝庐掳陋 脌脭路脗	
					// 陆潞脜忙 驴脌潞锚 脕娄赂庐赂露 戮酶戮脴麓脵.
					ItemDepleteHandler(iClientH, iSoxH, false, true);
				}
			}

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);

			break;
		}
		break;

	default:
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
		break;
	}
}

// Enchanting System by Centuu
void CGame::RequestItemEnchantHandler(int iClientH, int iItemIndex, int iAttributeType)
{
	DWORD dwType, dwValue;
	char cColor;
	int iResult, iSkillLevel = (int)m_pClientList[iClientH]->m_cSkillMastery[20]; 
	short sElement;

	if (m_pClientList[iClientH] == 0) return;
	if ((iItemIndex < 0) || (iItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return;
	
	if (iSkillLevel < 1) iSkillLevel = 1;

	if ((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1)
		&& (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2)
		&& (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 9, 0, 0, 0);
		return;
	}

	auto attr = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute;
	auto dwType1 = (attr & 0x00F00000) >> 20;
	auto dwValue1 = (attr & 0x000F0000) >> 16;
	auto dwType2 = (attr & 0x0000F000) >> 12;
	auto dwValue2 = (attr & 0x00000F00) >> 8;

	if (dwType1 == ITEMSTAT_NONE && dwType2 == ITEMSTAT2_NONE)
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 9, 0, 0, 0);
		return;
	}

	if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK || m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_ARROW) {
		if (iAttributeType == 1) {
			iResult = iDice(1, iSkillLevel * 10);
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

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = cColor;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			switch (dwType) {
			case ITEMSTAT_CRITICAL: // 
				if (dwValue <= 5) dwValue = 5;
				break;
			case ITEMSTAT_POISONING: // Min +20
				if (dwValue <= 4) dwValue = 4;
				break;
			case ITEMSTAT_LIGHT: // Min +16%
				if (dwValue <= 4) dwValue = 4;
				break;
			case ITEMSTAT_STRONG: // Min +14%					
				if (dwValue <= 2) dwValue = 2;
				break;
			}
			// Max 7
			//if (dwValue > 7) dwValue = 7;

			// Main
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;
			dwType = dwType << 20;
			dwValue = dwValue << 16;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;
		
			dwType2 = dwType2 << 12;
			dwValue2 = dwValue2 << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType2 | dwValue2;
		}
		else {
			//  Hit Prob(50%),  CAD(35%),  Gold(10%), Exp(5%)
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 499))       dwType = ITEMSTAT2_HITPROB;
			else if ((iResult >= 500) && (iResult <= 849)) dwType = ITEMSTAT2_CAD;
			else if ((iResult >= 850) && (iResult <= 949)) dwType = ITEMSTAT2_GOLD;
			else if ((iResult >= 950) && (iResult <= 1000)) dwType = ITEMSTAT2_EXP;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			switch (dwType) {
			case ITEMSTAT2_HITPROB: // Min Hip Prob +21%
				if (dwValue <= 3) dwValue = 3;
				break;
			case ITEMSTAT2_CAD: // CAD Vai do +1  at?+7 s?				//if (dwValue > 7) dwValue = 7;
				break;
			case ITEMSTAT2_EXP: // Exp  +20%
				//dwValue = 2;
				break;
			case ITEMSTAT2_GOLD: // Gold +50%
				//dwValue = 5;
				break;
			}

			// Demais stats max ?7 (para mobs fracos iGenLevel)
			//if (dwValue > 7) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;

			dwType1 = dwType1 << 20;
			dwValue1 = dwValue1 << 16;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType1 | dwValue1;

			dwType = dwType << 12;
			dwValue = dwValue << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;
		}
	}
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {

		if (iAttributeType == 1) {
			dwType = ITEMSTAT_CASTPROB;
			cColor = 5;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_cItemColor = cColor;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			//if (dwValue > 7) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;
			dwType = dwType << 20;
			dwValue = dwValue << 16;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;

			dwType2 = dwType2 << 12;
			dwValue2 = dwValue2 << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType2 | dwValue2;
		}
		else {
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 499))      dwType = ITEMSTAT2_HITPROB;
			else if ((iResult >= 500) && (iResult <= 849)) dwType = ITEMSTAT2_CAD;
			else if ((iResult >= 850) && (iResult <= 949)) dwType = ITEMSTAT2_GOLD;
			else if ((iResult >= 950) && (iResult <= 1000)) dwType = ITEMSTAT2_EXP;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			//if (dwValue > 7) dwValue = 7;

			switch (dwType) {
			case ITEMSTAT2_HITPROB: // Hit Prob Min +21%
				if (dwValue <= 3) dwValue = 3;
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

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;

			dwType1 = dwType1 << 20;
			dwValue1 = dwValue1 << 16;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType1 | dwValue1;

			dwType = dwType << 12;
			dwValue = dwValue << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;
		}
	}
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE) {

		if (iAttributeType == 1) {
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 599))          dwType = ITEMSTAT_STRONG;
			else if ((iResult >= 600) && (iResult <= 899))  dwType = ITEMSTAT_LIGHT;
			else if ((iResult >= 900) && (iResult <= 954))  dwType = ITEMSTAT_MANACONV;
			else if ((iResult >= 955) && (iResult <= 1000)) dwType = ITEMSTAT_CRITICAL2;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
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
				if (dwValue < 1) dwValue = 1;
				if (dwValue > 2) dwValue = 2;*/
				break;
			}
			// Max = 7
			//if (dwValue > 7) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;
			dwType = dwType << 20;
			dwValue = dwValue << 16;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;

			dwType2 = dwType2 << 12;
			dwValue2 = dwValue2 << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType2 | dwValue2;
		}
		else {
			// Poison R.(1),  Hit Prob(2), DR(3), HP(4), SP(5), MP(6),  MR(7),  PA(8), MA(9), CAD(10),  Exp(11), Gold(12)
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 99))       dwType = ITEMSTAT2_DEF;	// 10
			else if ((iResult >= 100) && (iResult <= 399))  dwType = ITEMSTAT2_PSNRES;	// 12
			else if ((iResult >= 400) && (iResult <= 549))  dwType = ITEMSTAT2_SPREC;	// 16
			else if ((iResult >= 550) && (iResult <= 649))  dwType = ITEMSTAT2_HPREC;	// 23
			else if ((iResult >= 650) && (iResult <= 749))  dwType = ITEMSTAT2_MPREC;	// 23 
			else if ((iResult >= 750) && (iResult <= 939))  dwType = ITEMSTAT2_MR;		// 12
			else if ((iResult >= 940) && (iResult <= 979))  dwType = ITEMSTAT2_PA;		// 3
			else if ((iResult >= 980) && (iResult <= 1000)) dwType = ITEMSTAT2_MA;		// 1

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			switch (dwType) {
			case 1: // Poison min +21%
			case 3: // DR Min +21%
			case 7: // MR Min +21%
			case 8: // PA Min +9%
				if (dwValue <= 3) dwValue = 3;
				break;
			case 9: // MA Min +9%
				if (dwValue <= 3) dwValue = 3;
				//Magn0S:: Add Elements
				iResult = iDice(1, 1020);
				if ((iResult >= 1) && (iResult < 275)) sElement = 1;  //Earth 27%
				else if ((iResult >= 275) && (iResult < 550)) sElement = 2;  //Light 27%
				else if ((iResult >= 550) && (iResult < 650)) sElement = 3;  //Fire 10%
				else if ((iResult >= 650) && (iResult < 925)) sElement = 4;  //Water 27%
				else if ((iResult >= 925) && (iResult < 970)) sElement = 5;  //Holy 04%
				else if ((iResult >= 970) && (iResult < 1000)) sElement = 6;  //Unholy 04%
				else if ((iResult >= 1000) && (iResult <= 1020)) sElement = 7;  //Unatunned 02%
				else sElement = 7;

				m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sNewEffect1 = sElement;
				break;

			}
			// Max = 7
			//if (dwValue > 7) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;

			dwType1 = dwType1 << 20;
			dwValue1 = dwValue1 << 16;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType1 | dwValue1;

			dwType = dwType << 12;
			dwValue = dwValue << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;
		}
	}
	else if (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_BIJOUTERIE) { // necks & rings

		if (iAttributeType == 1) {
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 299))  dwType = ITEMSTAT_MANACONV;
			else if ((iResult >= 300) && (iResult <= 599)) dwType = ITEMSTAT_CRITICAL;
			else if ((iResult >= 600) && (iResult <= 799)) dwType = ITEMSTAT_CRITICAL2;
			else if ((iResult >= 800) && (iResult <= 1000)) dwType = ITEMSTAT_CASTPROB;

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			switch (dwType) {
			case ITEMSTAT_CRITICAL: // 
				if (dwValue <= 5) dwValue = 5;
				break;
			case ITEMSTAT_MANACONV:
			case ITEMSTAT_CRITICAL2:
				/*dwValue = (dwValue + 1) / 2;
				if (dwValue < 1) dwValue = 1;*/
				//if ((iGenLevel <= 3) && (dwValue > 2)) dwValue = 2;
				break;
			}
			// Max = 7
			//if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;
			dwType = dwType << 20;
			dwValue = dwValue << 16;
			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;

			dwType2 = dwType2 << 12;
			dwValue2 = dwValue2 << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType2 | dwValue2;
		}
		else {
			// Poison R.(1),  Hit Prob(2), DR(3), HP(4), SP(5), MP(6),  MR(7),  PA(8), MA(9), CAD(10),  Exp(11), Gold(12)
			iResult = iDice(1, iSkillLevel * 10);
			if ((iResult >= 1) && (iResult <= 299))       dwType = ITEMSTAT2_HITPROB;
			else if ((iResult >= 300) && (iResult <= 399)) dwType = ITEMSTAT2_CAD;
			else if ((iResult >= 400) && (iResult <= 499)) dwType = ITEMSTAT2_GOLD;
			else if ((iResult >= 500) && (iResult <= 699)) dwType = ITEMSTAT2_EXP;
			else if ((iResult >= 700) && (iResult <= 799))  dwType = ITEMSTAT2_SPREC;	// 16
			else if ((iResult >= 800) && (iResult <= 899))  dwType = ITEMSTAT2_HPREC;	// 23
			else if ((iResult >= 900) && (iResult <= 1000))  dwType = ITEMSTAT2_MPREC;	// 23 

			iResult = iDice(1, iSkillLevel * 30);
			if ((iResult >= 1) && (iResult < 1000))           dwValue = 1;  // 10000/29348 = 34%
			else if ((iResult >= 1000) && (iResult < 1740))  dwValue = 2;  // 6600/29348 = 22.4%
			else if ((iResult >= 1740) && (iResult < 2240))  dwValue = 3;  // 4356/29348 = 14.8%
			else if ((iResult >= 2240) && (iResult < 2540))  dwValue = 4;  // 2874/29348 = 9.7%
			else if ((iResult >= 2540) && (iResult < 2740))  dwValue = 5;  // 1897/29348 = 6.4%
			else if ((iResult >= 2740) && (iResult < 2840))  dwValue = 6;  // 1252/29348 = 4.2%
			else if ((iResult >= 2840) && (iResult < 2890))  dwValue = 7;  // 826/29348 = 2.8%
			else if ((iResult >= 2890) && (iResult < 2930))  dwValue = 8;  // 545/29348 = 1.85%
			else if ((iResult >= 2930) && (iResult < 2960))  dwValue = 9;  // 360/29348 = 1.2%
			else if ((iResult >= 2960) && (iResult < 2980))  dwValue = 10; // 237/29348 = 0.8%
			else if ((iResult >= 2980) && (iResult < 2990))  dwValue = 11; // 156/29348 = 0.5%
			else if ((iResult >= 2990) && (iResult < 2997))  dwValue = 12; // 103/29348 = 0.3%
			else if ((iResult >= 2997) && (iResult <= 3000))  dwValue = 13; // 68/29348 = 0.1%
			else dwValue = 1; // v2.03 906

			switch (dwType) {
			case ITEMSTAT2_HITPROB: // Min Hip Prob +21%
				if (dwValue <= 3) dwValue = 3;
				break;
			case ITEMSTAT2_CAD: // CAD Vai do +1  at?+7 s?				//if (dwValue > 7) dwValue = 7;
				break;
			case ITEMSTAT2_EXP: // Exp  +20%
				//dwValue = 2;
				break;
			case ITEMSTAT2_GOLD: // Gold +50%
				//dwValue = 5;
				break;
			}

			// Max = 7
			//if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = 0;

			dwType1 = dwType1 << 20;
			dwValue1 = dwValue1 << 16;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType1 | dwValue1;

			dwType = dwType << 12;
			dwValue = dwValue << 8;

			m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute | dwType | dwValue;
		}
	}
	else
	{
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMUPGRADEFAIL, 9, 0, 0, 0);
		return;
	}

	GetExp(iClientH, iDice(1, iSkillLevel*100));

	CalculateSSN_SkillIndex(iClientH, 20, 1);
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMATTRIBUTECHANGE, iItemIndex, m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute, 0, 0);
}

int CGame::HeroItemChecker(int iItemID, short sEnemyKill, char cContribution, char cSide)
{

	switch (iItemID) {

	case 400: // AresdenHeroCape 
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 0;
		if (cSide != 0) cSide = 1;
		break;

	case 401: // ElvineHeroCape 
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 0;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroHelm
	case 403: // aHeroHelm(M)
	case 404: // aHeroHelm(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 405: // aHeroHelm(M)
	case 406: // aHeroHelm(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroCap
	case 407: // aHeroCap(M)
	case 408: // aHeroCap(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 409: // aHeroCap(M)
	case 410: // aHeroCap(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// eHeroArmor
	case 411: // eHeroArmor(M)
	case 412: // eHeroArmor(W)
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 30;
		if (cSide != 0) cSide = 1;
		break;
	case 413: // eHeroArmor(M)
	case 414: // eHeroArmor(W)
		if (sEnemyKill != 0) sEnemyKill = 300;
		if (cContribution != 0) cContribution = 30;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroRobe
	case 415: // aHeroRobe(M)
	case 416: // aHeroRobe(W)
		if (sEnemyKill != 0) sEnemyKill = 200;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 1;
		break;
	case 417: // aHeroRobe(M)
	case 418: // aHeroRobe(W)
		if (sEnemyKill != 0) sEnemyKill = 200;
		if (cContribution != 0) cContribution = 20;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroHauberk
	case 419: // aHeroHauberk(M)
	case 420: // aHeroHauberk(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 10;
		if (cSide != 0) cSide = 1;
		break;
	case 421: // eHeroHauberk(M)
	case 422: // eHeroHauberk(W)
		if (sEnemyKill != 0) sEnemyKill = 100;
		if (cContribution != 0) cContribution = 10;
		if (cSide != 0) cSide = 2;
		break;

		// aHeroLeggings
	case 423: // aHeroLeggings(M)
	case 424: // aHeroLeggings(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 15;
		if (cSide != 0) cSide = 1;
		break;
	case 425: // eHeroLeggings(M)
	case 426: // eHeroLeggings(W)
		if (sEnemyKill != 0) sEnemyKill = 150;
		if (cContribution != 0) cContribution = 15;
		if (cSide != 0) cSide = 2;
		break;

	case 427: // AresdenHeroCape+1 
		if (cSide != 0) cSide = 1;
		break;

	case 428: // ElvineHeroCape+1
		if (cSide != 0) cSide = 2;
		break;

	default:
		return 0;
	}
	return iItemID;

}

bool CGame::iUpgradeHeroCapeRequirements(int iClientH, int iItemIndex)
{
	int iAfterItemID, iRequiredEnemyKills, iRequiredContribution, iStoneNumber, i;
	int iBeforeItemID;

	iAfterItemID = 0;
	iRequiredEnemyKills = 30;
	iRequiredContribution = 50;
	iStoneNumber = 657;
	i = 0;
	iBeforeItemID = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum;
	if (iBeforeItemID == 400) {
		
		iAfterItemID = 427;
	}
	else if (iBeforeItemID == 401) {
		
		iAfterItemID = 428;
	}
	else if (iBeforeItemID == 427) {
		
		iAfterItemID = 131;
		iRequiredEnemyKills = 3000;
		iRequiredContribution = 200;
	}
	else if (iBeforeItemID == 428) {
		
		iAfterItemID = 132;
		iRequiredEnemyKills = 3000;
		iRequiredContribution = 200;
	}
	else {
		return false;
	}
	_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)m_pClientList[iClientH]->m_pItemList[iItemIndex], false);
	if (iRequiredEnemyKills > m_pClientList[iClientH]->m_iEnemyKillCount) return false;
	if (iRequiredContribution > m_pClientList[iClientH]->m_iContribution) return false;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == iStoneNumber)) break;

	if ((i == DEF_MAXITEMS) || (iStoneNumber == 0)) return false;
	if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], iAfterItemID) == false) return false;
	m_pClientList[iClientH]->m_iEnemyKillCount -= iRequiredEnemyKills;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
	calcularTop15HB(iClientH);
	m_pClientList[iClientH]->m_iContribution -= iRequiredContribution;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONTRIBPOINTS, m_pClientList[iClientH]->m_iContribution, 0, 0, 0);
	if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
		ItemDepleteHandler(iClientH, i, false, true);
		return true;
	}
	return false;
}

bool CGame::iUpgradeHeroItemRequirements(int iClientH, int iItemIndex)
{
	int iRequiredEnemyKills, iStoneNumber, i;
	int iBeforeItemID, iAfterItemID;

	iAfterItemID = 0;
	iRequiredEnemyKills = 0;
	iStoneNumber = 0;

	iBeforeItemID = m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum;
	
	if (iBeforeItemID == 1003) {
		iAfterItemID = 1004;
		iStoneNumber = 656;
		iRequiredEnemyKills = 20000;
	}
	else if (iBeforeItemID == 988) {
		
		iAfterItemID = 1005;
		iStoneNumber = 656;
		iRequiredEnemyKills = 20000;
	}

	else if (iBeforeItemID == 1010) {
		iAfterItemID = 1011;
		iStoneNumber = 656;
		iRequiredEnemyKills = 20000;
	}

	// Hood M
	else if (iBeforeItemID == 392 || iBeforeItemID == 394) {

		iAfterItemID = 147;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Hood W
	else if (iBeforeItemID == 393 || iBeforeItemID == 395) {

		iAfterItemID = 148;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Helm M
	else if (iBeforeItemID == 403 || iBeforeItemID == 405) {

		iAfterItemID = 1061;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Helm W
	else if (iBeforeItemID == 404 || iBeforeItemID == 406) {

		iAfterItemID = 1062;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Cap M
	else if (iBeforeItemID == 407 || iBeforeItemID == 409) {

		iAfterItemID = 1063;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Cap W
	else if (iBeforeItemID == 408 || iBeforeItemID == 410) {

		iAfterItemID = 1064;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Armor M
	else if (iBeforeItemID == 411 || iBeforeItemID == 413) {

		iAfterItemID = 1065;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Armor W
	else if (iBeforeItemID == 412 || iBeforeItemID == 414) {

		iAfterItemID = 1066;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Robe M
	else if (iBeforeItemID == 415 || iBeforeItemID == 417) {

		iAfterItemID = 1067;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Robe W
	else if (iBeforeItemID == 416 || iBeforeItemID == 418) {

		iAfterItemID = 1068;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Hauberk M
	else if (iBeforeItemID == 419 || iBeforeItemID == 421) {

		iAfterItemID = 1069;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Hauberk W
	else if (iBeforeItemID == 420 || iBeforeItemID == 422) {

		iAfterItemID = 1070;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	// Leggings M
	else if (iBeforeItemID == 423 || iBeforeItemID == 425) {

		iAfterItemID = 1071;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}
	// Leggings W
	else if (iBeforeItemID == 424 || iBeforeItemID == 426) {

		iAfterItemID = 1072;
		iStoneNumber = 657;
		iRequiredEnemyKills = 30000;
	}

	else {
		return false;
	}

	_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)m_pClientList[iClientH]->m_pItemList[iItemIndex], false);

	if (iRequiredEnemyKills > m_pClientList[iClientH]->m_iEnemyKillCount) return false;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (m_pClientList[iClientH]->m_pItemList[i]->m_sIDnum == iStoneNumber)) break;

	if ((i == DEF_MAXITEMS) || (iStoneNumber == 0)) return false;
	if (_bInitItemAttr(m_pClientList[iClientH]->m_pItemList[iItemIndex], iAfterItemID) == false) return false;
	m_pClientList[iClientH]->m_iEnemyKillCount -= iRequiredEnemyKills;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
	calcularTop15HB(iClientH);
	if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
		ItemDepleteHandler(iClientH, i, false, true);
		return true;
	}
	return false;
}

//50Cent - Repair All
void CGame::RequestRepairAllItemsHandler(int iClientH)
{
	int i, price;
	double d1, d2, d3;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	m_pClientList[iClientH]->totalItemRepair = 0;

	for (i = 0; i < DEF_MAXITEMS; i++){
		if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
			//Magn0S:: Cant repair fragile item type 2
			if (m_pClientList[iClientH]->m_pItemList[i]->m_sNewEffect1 == DEF_FRAGILEITEM) {
				continue;
			}
			if (((m_pClientList[iClientH]->m_pItemList[i]->m_cCategory >= 1) && (m_pClientList[iClientH]->m_pItemList[i]->m_cCategory <= 12)) ||
				((m_pClientList[iClientH]->m_pItemList[i]->m_cCategory >= 43) && (m_pClientList[iClientH]->m_pItemList[i]->m_cCategory <= 50)))
			{
				if (m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan == m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan)
					continue;
				if (m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan <= 0)
					price = (m_pClientList[iClientH]->m_pItemList[i]->m_wPrice / 2);
				else
				{
					d1 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wCurLifeSpan);
					if (m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan != 0)
						d2 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wMaxLifeSpan);
					else
						d2 = 1.0f;
					d3 = ((d1 / d2) * 0.5f);
					d2 = (double)(m_pClientList[iClientH]->m_pItemList[i]->m_wPrice);
					d3 = (d3 * d2);
					price = ((m_pClientList[iClientH]->m_pItemList[i]->m_wPrice / 2) - (short)(d3));
				}
				m_pClientList[iClientH]->m_stRepairAll[m_pClientList[iClientH]->totalItemRepair].index = i;
				m_pClientList[iClientH]->m_stRepairAll[m_pClientList[iClientH]->totalItemRepair].price = price;
				m_pClientList[iClientH]->totalItemRepair++;
			}
		}
	}
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPAIRALLPRICES, 0, 0, 0, 0);
}

void CGame::RequestRepairAllItemsDeleteHandler(int iClientH, int index)
{
	int i;
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	for (i = index; i < m_pClientList[iClientH]->totalItemRepair; i++) {
		m_pClientList[iClientH]->m_stRepairAll[i] = m_pClientList[iClientH]->m_stRepairAll[i + 1];
	}
	m_pClientList[iClientH]->totalItemRepair--;
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPAIRALLPRICES, 0, 0, 0, 0);
}

void CGame::RequestRepairAllItemsConfirmHandler(int iClientH)
{
	char* cp, cData[120];
	DWORD* dwp;
	WORD* wp;
	int      iRet, i, totalPrice = 0;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == false) return;

	for (i = 0; i < m_pClientList[iClientH]->totalItemRepair; i++) {
		totalPrice += m_pClientList[iClientH]->m_stRepairAll[i].price;
	}

	if (dwGetItemCount(iClientH, "Gold") < (DWORD)totalPrice)
	{
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_NOTENOUGHGOLD;
		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
		*cp = 0;
		cp++;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
		switch (iRet) {
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
		for (i = 0; i < m_pClientList[iClientH]->totalItemRepair; i++)
		{
			//Magn0S:: Cant repair fragile item type 2
			if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index] != 0) {
				if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_sNewEffect1 == DEF_FRAGILEITEM) {
					continue;
				}
				m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wMaxLifeSpan;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMREPAIRED, m_pClientList[iClientH]->m_stRepairAll[i].index, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_stRepairAll[i].index]->m_wCurLifeSpan, 0, 0);
			}
		}
		iCalcTotalWeight(SetItemCount(iClientH, "Gold", dwGetItemCount(iClientH, "Gold") - totalPrice));
	}
}

// Magn0S:: Revamped Trade Contrib, Coins and Eks
void CGame::RequestPurchaseItemHandler2(int iClientH, char* pItemName, int iNum, int iPurchase)
{
	class CItem* pItem;
	char cItemName[21], cData[100];
	
	DWORD* dwp, dwItemCount;
	WORD* wp;
	int   i, iRet, iEraseReq, iCost = 0;

	bool bIsCoin = false, bIsPotion = false;

	DWORD dwType1, dwType2, dwValue;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cItemName, sizeof(cItemName));

	if (iPurchase == 3)
	{
		if (memcmp(pItemName, "10Coins", 7) == 0) {
			strcpy(cItemName, "Coin");
			dwItemCount = 10;
			bIsCoin = true;
		}
		else if (memcmp(pItemName, "100Coins", 8) == 0) {
			strcpy(cItemName, "Coin");
			dwItemCount = 100;
			bIsCoin = true;
		}
		else if (memcmp(pItemName, "Coin", 4) == 0) {
			strcpy(cItemName, "Coin");
			dwItemCount = 1;
			bIsCoin = true;
		}
		else if (memcmp(pItemName, "PAPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_PA;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "MAPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_MA;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "DRPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_DEF;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "PRPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_PSNRES;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "HitPPotion", 10) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_HITPROB;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "HPPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_HPREC;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "SPPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_SPREC;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "MPPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_MPREC;
			bIsPotion = true;
		}
		else if (memcmp(pItemName, "MRPotion", 8) == 0) {
			strcpy(cItemName, "AttributePotion");
			dwItemCount = 1;
			dwType2 = ITEMSTAT2_MR;
			bIsPotion = true;
		}
		else {
			memcpy(cItemName, pItemName, 20);
			dwItemCount = 1;
		}
	}
	else {
		memcpy(cItemName, pItemName, 20);
		dwItemCount = 1;
	}

	for (i = 1; i <= iNum; i++) {

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == false) {
			delete pItem;
		}
		else {

			switch (iPurchase) {
			case 1:
				if (pItem->bEkSale == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "This item is not for sale for Ek's. Update your CONTENTS.");
					delete pItem;
					return;
				}

				if (m_pClientList[iClientH]->m_iEnemyKillCount < pItem->m_wEkPrice) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Not enought Enemy Kill Points to purchase this item.");
					return;
				}
				iCost = pItem->m_wEkPrice* dwItemCount;
				break;
			case 2:
				if (pItem->bContrbSale == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "This item is not for sale for Contributions. Update your CONTENTS.");
					delete pItem;
					return;
				}

				if (m_pClientList[iClientH]->m_iContribution < pItem->m_wContribPrice) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Not enought Contribution Points to purchase this item.");
					return;
				}
				iCost = pItem->m_wContribPrice* dwItemCount;
				break;
			case 3:
				if (pItem->bCoinSale == false) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "This item is not for sale for Coin Points. Update your CONTENTS.");
					delete pItem;
					return;
				}

				if (m_pClientList[iClientH]->m_iCoinPoints < pItem->m_wCoinPrice) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, "Not enought Coins to purchase this item.");
					return;
				}
				iCost = pItem->m_wCoinPrice* dwItemCount;
				break;
			default:
				return;
				break;
			}

			pItem->m_dwCount = dwItemCount;

			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				if (bIsCoin)
				{
					pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
					pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
					pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
					pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				}

				if (bIsPotion)
				{
					dwType1 = ITEMSTAT_STRONG;
					dwValue = 1;

					pItem->m_dwAttribute = 0;
					dwType1 = dwType1 << 20;
					dwValue = dwValue << 16;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType1 | dwValue;

					//dwType2 = ITEMSTAT2_DEF;
					dwValue = 1;

					dwType2 = dwType2 << 12;
					dwValue = dwValue << 8;
					pItem->m_dwAttribute = pItem->m_dwAttribute | dwType2 | dwValue;
					
					_AdjustRareItemValue(pItem);

					pItem->m_sTouchEffectType = DEF_ITET_ID;
					pItem->m_sTouchEffectValue1 = iDice(1, 100000);
					pItem->m_sTouchEffectValue2 = iDice(1, 100000);

					SYSTEMTIME SysTime;
					char cTemp[256];
					GetLocalTime(&SysTime);
					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
					pItem->m_sTouchEffectValue3 = atoi(cTemp);

					_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, 0, pItem);
				}

				SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMPURCHASED, pItem, iCost);

				if (iEraseReq == 1) delete pItem;

				switch (iPurchase) {
				case 1:
					m_pClientList[iClientH]->m_iEnemyKillCount -= iCost;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ENEMYKILLS, m_pClientList[iClientH]->m_iEnemyKillCount, 0, 0, 0);
					calcularTop15HB(iClientH);
					break;
				case 2:
					m_pClientList[iClientH]->m_iContribution -= iCost;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CONTRIBPOINTS, m_pClientList[iClientH]->m_iContribution, 0, 0, 0);
					break;
				case 3:
					m_pClientList[iClientH]->m_iCoinPoints -= iCost;
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_COINPOINTS, m_pClientList[iClientH]->m_iCoinPoints, 0, 0, 0);
					break;
				default:
					return;
					break;
				}

				iCalcTotalWeight(iClientH);
			}
			else
			{
				delete pItem;
				iCalcTotalWeight(iClientH);

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

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

//dkset by diuude and update by drawjer and modified by Ovl
void CGame::GetDkSet(int iClientH)
{
	class CItem* pItem;
	int dkM[] = {706,707,708,710,709};
	int dkW[] = {724,725,726,728,709};
	int dmM[] = {711,712,713,913,714};
	int dmW[] = {729,730,731,914,714};
	int daM[] = {134,135,136,137,138};
	int daW[] = {143,144,145,146,138};
	int iItemID, i, iEraseReq;
	char cTxt[120];
	DWORD  dwGoldCount = dwGetItemCount(iClientH, "Gold");

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_iLevel < m_iPlayerMaxLevel) return;
	if (_iGetItemSpaceLeft(iClientH) == 0) 
	{
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, 0, 0);
		return;
	}
	if (dwGoldCount < m_iDKCost) 
	{
		wsprintf(cTxt, "Not enough gold. You need %d gold.", m_iDKCost);
		SendNotifyMsg(iClientH, iClientH, DEF_NOTIFY_NOTICEMSG, 0, 0, 0, cTxt);
		return; // centu - el dk set cuesta gold
	}
	SetItemCount(iClientH, "Gold", (dwGoldCount - m_iDKCost));
		for (i = 0; i < 5; i++)
		{
			pItem = new class CItem;
			iItemID = -1;
			if (m_pClientList[iClientH]->m_cSex == 1)
			{
				if (m_pClientList[iClientH]->m_iClass == 1)
				{
					iItemID = dkM[i];
				}
				else if (m_pClientList[iClientH]->m_iClass == 2)
				{
					iItemID = dmM[i];
				}
				else if (m_pClientList[iClientH]->m_iClass == 3)
				{
					iItemID = daM[i];
				}
			}
			else
			{
				if (m_pClientList[iClientH]->m_iClass == 1)
				{
					iItemID = dkW[i];
				}
				else if (m_pClientList[iClientH]->m_iClass == 2)
				{
					iItemID = dmW[i];
				}
				else if (m_pClientList[iClientH]->m_iClass == 3)
				{
					iItemID = daW[i];
				}
			}
			if (iItemID != -1) //if any error occures, dont crash character
			{
				_bInitItemAttr(pItem, iItemID);
				pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				pItem->m_sTouchEffectValue1 = m_pClientList[iClientH]->m_sCharIDnum1;
				pItem->m_sTouchEffectValue2 = m_pClientList[iClientH]->m_sCharIDnum2;
				pItem->m_sTouchEffectValue3 = m_pClientList[iClientH]->m_sCharIDnum3;
				_bAddClientItemList(iClientH, pItem, &iEraseReq);
				SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);
			}
		}
	
}

int CGame::_iGetArrowItemIndex(int iClientH)
{
	int i;

	if (m_pClientList[iClientH] == 0) return -1;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0) {
			if ((m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_ARROW) &&
				(m_pClientList[iClientH]->m_pItemList[i]->m_dwCount > 0))
				return i;
		}

	return -1;
}

void CGame::UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID)
{
	int iMax, iV1, iV2, iV3, iSEV1, iEffectResult = 0;
	DWORD dwTime, dwGUID;
	short sTemp, sTmpType, sTmpAppr1;
	char cSlateType[20];
	bool bDepleteNow = true;

	dwTime = timeGetTime();
	ZeroMemory(cSlateType, sizeof(cSlateType));

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;

	if ((string(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == "Coin") ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_EAT)) {

		// Centuu - class
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iClass > 0)
		{
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iClass != m_pClientList[iClientH]->m_iClass)
			{
				return;
			}
		}

		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
		case DEF_ITEMEFFECTTYPE_WARM:
			if (m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] == 1) {

				bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_ICE);
				bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_ICE, dwTime + (1 * 1000),
					iClientH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);

			}

			m_pClientList[iClientH]->m_dwWarmEffectTime = dwTime;
			break;

		case DEF_ITEMEFFECTTYPE_LOTTERY:

			break;

		case DEF_ITEMEFFECTTYPE_SLATES:
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
				// Full Ancient Slate ??
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 867) {
					// Slates dont work on Heldenian Map
					if ((m_bIsHeldenianMode == true) && (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == true)) {
						ItemDepleteHandler(iClientH, sItemIndex, true, false);
						return;
					}
					switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2) {
					case 2: // Bezerk slate
						m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_BERSERK] = true;
						SetBerserkFlag(iClientH, DEF_OWNERTYPE_PLAYER, true);
						bRegisterDelayEvent(DEF_DELAYEVENTTYPE_MAGICRELEASE, DEF_MAGICTYPE_BERSERK, dwTime + (1000 * 600),
							iClientH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTON, DEF_MAGICTYPE_BERSERK, 1, 0, 0);
						
						break;

					case 1: // Invincible slate
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_INVINCIBLE, 0, 0, 0, 0);
						break;
					case 3: // Mana slate
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_MANA, 0, 0, 0, 0);
						break;
					case 4: // Exp slate
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_EXP, 0, 0, 0, 0);
						break;
					}

					SetSlateFlag(iClientH, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2, true);
					bRegisterDelayEvent(DEF_DELAYEVENTTYPE_ANCIENT_TABLET, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2,
						dwTime + (1000 * 600), iClientH, DEF_OWNERTYPE_PLAYER, 0, 0, 0, 1, 0, 0);

				}
				if (strlen(cSlateType) > 0)
					_bItemLog(DEF_ITEMLOG_USE, iClientH, strlen(cSlateType), m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			}
			break;

		case DEF_ITEMEFFECTTYPE_HP:
			iMax = iGetMaxHP(iClientH);
			if (m_pClientList[iClientH]->m_iHP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iHP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iHP > iMax) m_pClientList[iClientH]->m_iHP = iMax;
				if (m_pClientList[iClientH]->m_iHP <= 0)   m_pClientList[iClientH]->m_iHP = 1;

				SendNotifyMsg(0, iClientH, DEF_NOTIFY_HP, 0, 0, 0, 0);
			}
			break;

		case DEF_ITEMEFFECTTYPE_MP:
			iMax = iGetMaxMP(iClientH);

			if (m_pClientList[iClientH]->m_iMP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else
				{
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iMP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iMP > iMax)
					m_pClientList[iClientH]->m_iMP = iMax;

				SendNotifyMsg(0, iClientH, DEF_NOTIFY_MP, 0, 0, 0, 0);
			}
			break;

		case DEF_ITEMEFFECTTYPE_SP:
			iMax = iGetMaxSP(iClientH);

			if (m_pClientList[iClientH]->m_iSP < iMax) {

				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1 == 0) {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;
				}
				else {
					iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
					iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue2;
					iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue3;
				}

				m_pClientList[iClientH]->m_iSP += (iDice(iV1, iV2) + iV3);
				if (m_pClientList[iClientH]->m_iSP > iMax)
					m_pClientList[iClientH]->m_iSP = iMax;

				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SP, 0, 0, 0, 0);
			}

			if (m_pClientList[iClientH]->m_bIsPoisoned == true) {
				m_pClientList[iClientH]->m_bIsPoisoned = false;
				SetPoisonFlag(iClientH, DEF_OWNERTYPE_PLAYER, false); // removes poison aura when using a revitalizing potion
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_MAGICEFFECTOFF, DEF_MAGICTYPE_POISON, 0, 0, 0);
			}
			break;

		case DEF_ITEMEFFECTTYPE_HPSTOCK:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
			iV3 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue3;

			m_pClientList[iClientH]->m_iHPstock += iDice(iV1, iV2) + iV3;
			if (m_pClientList[iClientH]->m_iHPstock < 0)   m_pClientList[iClientH]->m_iHPstock = 0;
			if (m_pClientList[iClientH]->m_iHPstock > 500) m_pClientList[iClientH]->m_iHPstock = 500;
			m_pClientList[iClientH]->m_iHungerStatus += iDice(iV1, iV2) + iV3;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_HUNGER, m_pClientList[iClientH]->m_iHungerStatus, 0, 0, 0); // MORLA2 - Muestra nuevamente
			if (m_pClientList[iClientH]->m_iHungerStatus > 100) m_pClientList[iClientH]->m_iHungerStatus = 100;
			if (m_pClientList[iClientH]->m_iHungerStatus < 0)   m_pClientList[iClientH]->m_iHungerStatus = 0;
			break;


		case DEF_ITEMEFFECTTYPE_STUDYSKILL: //Manual
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			iV2 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2;
			iSEV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1;
			if (iSEV1 == 0) 
			{
				TrainSkillResponse(true, iClientH, iV1, iV2);
			}
			else 
			{
				TrainSkillResponse(true, iClientH, iV1, iSEV1);
			}
			break;

		case DEF_ITEMEFFECTTYPE_STUDYMAGIC:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			
			if (m_pMagicConfigList[iV1] != 0) {
				if (!RequestStudyMagicHandler(iClientH, m_pMagicConfigList[iV1]->m_cName, bDepleteNow, false)) {
					return;
				}
			}

			if (bDepleteNow)
				ItemDepleteHandler(iClientH, sItemIndex, true, true);

			return; // break;

		//Magn0S:: Reputation Increased Potion.
		case DEF_ITEMEFFECTTYPE_REP:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			m_pClientList[iClientH]->m_iRating += iV1;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPDGDEATHS, 0, 0, m_pClientList[iClientH]->m_iRating, 0); // MORLA 2.4 - Actualiza la REP

			if (m_pClientList[iClientH]->m_iRating > 5000)  m_pClientList[iClientH]->m_iRating = 5000;
			if (m_pClientList[iClientH]->m_iRating < -5000)  m_pClientList[iClientH]->m_iRating = -5000;
			break;

		case DEF_ITEMEFFECTTYPE_COINS:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount; //m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			m_pClientList[iClientH]->m_iCoinPoints += iV1;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_COINPOINTS, m_pClientList[iClientH]->m_iCoinPoints, 0, 0, 0);
			break;
		
		//End Magn0S --------------------------------------------------------------------------------------------------

		// New 15/05/2004 Changed
		case DEF_ITEMEFFECTTYPE_MAGIC:
			if (m_pClientList[iClientH]->m_iAdminUserLevel == 0) {
				SetInvisibilityFlag(iClientH, DEF_OWNERTYPE_PLAYER, false);

				bRemoveFromDelayEventList(iClientH, DEF_OWNERTYPE_PLAYER, DEF_MAGICTYPE_INVISIBILITY);
				m_pClientList[iClientH]->m_cMagicEffectStatus[DEF_MAGICTYPE_INVISIBILITY] = 0;
			}

			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
			case 1:
				// Recall
				// testcode
				//50Cent - Capture The Flag
				if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0)
				{
					ShowClientMsg(iClientH, " You can not use that item being a flag carrier.");
					//goto MAGIC_NOEFFECT;
				}
				else RequestTeleportHandler(iClientH, "1  ");
				break;

			case 2:
				// 脜玫赂铆 赂露鹿媒脠驴掳煤掳隆 脌脰麓脗 戮脝脌脤脜脹. 
				//50Cent - Capture The Flag
				if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0)
				{
					ShowClientMsg(iClientH, " You can not use that item being a flag carrier.");
					//goto MAGIC_NOEFFECT;
				}
				else PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 32, true);
				break;

			case 3:
				// 脜玫赂铆 赂露鹿媒 脙拢卤芒. 禄莽脜玫脌氓 鲁禄潞脦赂茅 录脪驴毛戮酶麓脵. 
				if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsFightZone == false)
					PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 34, true);
				break;

			case 4:
				// fixed location teleportation: 脌脭脌氓卤脟 碌卯碌卯
				switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2) {
				case 1:
					//50Cent - Capture The Flag
					if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0)
					{
						ShowClientMsg(iClientH, " You can not use that item being a flag carrier.");
						//goto MAGIC_NOEFFECT;
					}
					else {
						// 潞铆赂庐碌霉 戮脝脌脧路脦 掳拢麓脵 
						if (memcmp(m_pClientList[iClientH]->m_cMapName, "bisle", 5) != 0) {
							//v1.42
							RequestTeleportHandler(iClientH, "2   ", "bisle", -1, -1);
						}
					}
					break;

				
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					//50Cent - Capture The Flag
					if (m_bIsCTFEvent && (m_pClientList[iClientH]->m_iStatus & 0x80000) != 0)
					{
						ShowClientMsg(iClientH, " You can not use that item being a flag carrier.");
						//goto MAGIC_NOEFFECT;
					}
					else {
						// 掳谩脜玫脌氓脌赂路脦 掳拢麓脵. 
						SYSTEMTIME SysTime;

						GetLocalTime(&SysTime);
						// v1.4311-3 潞炉掳忙 脌脭脌氓卤脟 脙录脜漏 麓脼/鲁炉脗楼/陆脙掳拢脌赂路脦 脙录脜漏脟脩麓脵. 
						// 脌脭脌氓 掳隆麓脡脟脩 陆脙掳拢潞赂麓脵 脌脹掳脜鲁陋 掳掳脌赂赂茅 脌脭脌氓卤脟脌脤 禄莽露贸脕酶麓脵.
						if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue1 == SysTime.wMonth) ||
							(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue2 == SysTime.wDay) ||
							(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectValue3 > SysTime.wHour)) {
							// 鲁炉脗楼掳隆 脕陇脠庐脟脧脕枚 戮脢麓脵. 戮脝鹿芦路卤 脠驴掳煤掳隆 戮酶掳铆 脌脭脌氓卤脟脌潞 禄莽露贸脕酶麓脵.

							char cDestMapName[11];
							ZeroMemory(cDestMapName, sizeof(cDestMapName));
							wsprintf(cDestMapName, "fightzone%d", m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 - 10);
							if (memcmp(m_pClientList[iClientH]->m_cMapName, cDestMapName, 10) != 0) {
								//v1.42
								RequestTeleportHandler(iClientH, "2   ", cDestMapName, -1, -1);
							}
						}
					}
					break;
				}
				break;

			case 5: // Heldenians scroll must be defined as DEF_ITEMTYPE_USE_PERM
					// Only Helnenian scrolls can be defined as DEF_ITEMTYPE_USE_PERM 
					// Scrolls not belonging to the user are depleted when entering function 
				if ((m_bIsHeldenianMode == true)
					&& (m_bHeldenianWarInitiated == true)
					&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0)
					&& (m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_bIsHeldenianMap == 1)
					&& (strcmp(m_pClientList[iClientH]->m_cMapName, "GodH") != 0))
				{
					memcpy(&dwGUID, &m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemSpecEffectValue1, 4);
					if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sTouchEffectType == DEF_ITET_UNIQUE_OWNER)
					{
						if (dwGUID == m_dwHeldenianGUID)
						{	// Scrolls from current Heldenian can summon
							if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6 == -1)// -1 allows using with weapon shield equiped
								PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, true,
									m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
							else
								PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, true,
									m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 * -1);
							m_pClientList[iClientH]->m_iWarContribution += 300;

						}
						
					}
					else
					{	// Not personal scrolls can summon and deplete scroll now
						if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue6 == -1)// -1 allows using with weapon shield equiped
							PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, true,
								m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2);
						else
							PlayerMagicHandler(iClientH, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, 31, true,
								m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue2 * -1);
					}
				}
				break;
			}
			break;

		case DEF_ITEMEFFECTTYPE_FIRMSTAMINAR:
			m_pClientList[iClientH]->m_iTimeLeft_FirmStaminar += m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			break;

		case DEF_ITEMEFFECTTYPE_CHANGEATTR:
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1) {
			case 1:
				// 赂脫赂庐 禄枚脌禄 鹿脵虏脹麓脵. 
				m_pClientList[iClientH]->m_cHairColor++;
				if (m_pClientList[iClientH]->m_cHairColor > 15) m_pClientList[iClientH]->m_cHairColor = 0;

				sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
				m_pClientList[iClientH]->m_sAppr1 = sTemp;
				break;

			case 2:
				// 赂脫赂庐 陆潞脜赂脌脧脌禄 鹿脵虏脹麓脵.
				m_pClientList[iClientH]->m_cHairStyle++;
				if (m_pClientList[iClientH]->m_cHairStyle > 7) m_pClientList[iClientH]->m_cHairStyle = 0;

				sTemp = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
				m_pClientList[iClientH]->m_sAppr1 = sTemp;
				break;

			case 3:
				// 脟脟潞脦禄枚脌禄 鹿脵虏脹麓脵.
				// 鲁禄驴毛脌禄 鹿脵脜脕脌赂路脦 Appearance赂娄 掳猫禄锚, 脟脪麓莽脟脩麓脵.
				m_pClientList[iClientH]->m_cSkin++;
				if (m_pClientList[iClientH]->m_cSkin > 3)
					m_pClientList[iClientH]->m_cSkin = 1;

				if (m_pClientList[iClientH]->m_cSex == 1)      sTemp = 1;
				else if (m_pClientList[iClientH]->m_cSex == 2) sTemp = 4;

				switch (m_pClientList[iClientH]->m_cSkin) {
				case 2:	sTemp++; break;
				case 3:	sTemp += 2; break;
				}
				m_pClientList[iClientH]->m_sType = sTemp;
				break;

			case 4:
				// 录潞脌眉脠炉 - 赂赂戮脿 驴脢脌禄 脌脭掳铆 脌脰麓脵赂茅 陆脟脝脨脟脩麓脵. 
				sTemp = m_pClientList[iClientH]->m_sAppr3 & 0xFF0F;
				if (sTemp == 0) {
					// sTemp掳隆 0脌脤 戮脝麓脧露贸赂茅 掳脩驴脢, 录脫驴脢, 鹿脵脕枚脕脽 脟脩掳隆脕枚赂娄 脌脭掳铆 脌脰麓脗 掳脥脌脤麓脵. 录潞脌眉脠炉脌禄 脟脪 录枚 戮酶麓脵. 
					if (m_pClientList[iClientH]->m_cSex == 1)
						m_pClientList[iClientH]->m_cSex = 2;
					else m_pClientList[iClientH]->m_cSex = 1;

					// 鲁禄驴毛脌禄 鹿脵脜脕脌赂路脦 Appearance赂娄 掳猫禄锚, 脟脪麓莽脟脩麓脵.
					if (m_pClientList[iClientH]->m_cSex == 1) {
						// 鲁虏脌脷脌脤麓脵. 
						sTmpType = 1;
					}
					else if (m_pClientList[iClientH]->m_cSex == 2) {
						// 驴漏脌脷脌脤麓脵.
						sTmpType = 4;
					}

					switch (m_pClientList[iClientH]->m_cSkin) {
					case 1:
						// 鹿茅脌脦脌脤赂茅 卤脳麓毛路脦.
						break;
					case 2:
						sTmpType++;
						break;
					case 3:
						sTmpType += 2;
						break;
					}

					sTmpAppr1 = (m_pClientList[iClientH]->m_cHairStyle << 8) | (m_pClientList[iClientH]->m_cHairColor << 4) | (m_pClientList[iClientH]->m_cUnderwear);
					m_pClientList[iClientH]->m_sType = sTmpType;
					m_pClientList[iClientH]->m_sAppr1 = sTmpAppr1;
					//
				}
				break;
			}

			SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
			break;

		default:
			return;
		}
		// *** Request Teleport Handler掳隆 脌脹碌驴碌脟赂茅 脌脤鹿脤 碌楼脌脤脜脥 脌煤脌氓脌脤 驴盲脙禄碌脠 禄贸脜脗脌脤鹿脟路脦 脌脤脠脛驴隆 戮脝脌脤脜脹脌禄 戮酶戮脰潞脕戮脽 录脪驴毛脌脤 戮酶麓脵. 
		// 戮脝脌脤脜脹脌禄 赂脮脌煤 戮酶戮脴麓脵.
		ItemDepleteHandler(iClientH, sItemIndex, true, true);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_DEPLETE_DEST) {
		// 禄莽驴毛脟脧赂茅录颅 赂帽脟楼脕枚脕隆脌禄 脕枚脕陇脟脧麓脗 戮脝脌脤脜脹.
		// dX, dY脌脟 脕脗脟楼掳隆 脌炉脠驴 鹿眉脌搂 鲁禄驴隆 脌脰麓脗脕枚 脠庐脌脦脟脴戮脽 脟脩麓脵.
		if (_bDepleteDestTypeItemUseEffect(iClientH, dX, dY, sItemIndex, sDestItemID) == true)
			ItemDepleteHandler(iClientH, sItemIndex, true, true);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW) {
		// 脠颅禄矛脌禄 脟脪麓莽脟脩麓脵. 
		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_PERM) {
		// 驴碌卤赂脠梅 戮碌 录枚 脌脰麓脗 戮脝脌脤脜脹. 脕茂 戮虏掳铆鲁陋碌碌 戮酶戮卯脕枚脕枚 戮脢麓脗 戮脝脌脤脜脹. (ex: 脕枚碌碌) 
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
		case DEF_ITEMEFFECTTYPE_SHOWLOCATION:
			iV1 = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectValue1;
			switch (iV1) {
			case 1:
				// 脟枚脌莽 脌脷陆脜脌脟 脌搂脛隆赂娄 潞赂驴漏脕脴麓脵. 
				if (strcmp(m_pClientList[iClientH]->m_cMapName, "aresden") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 1, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvine") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 2, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "middleland") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 3, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "default") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 4, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone2") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 5, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone1") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 6, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone4") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 7, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "huntzone3") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 8, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "arefarm") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 9, 0, 0);
				else if (strcmp(m_pClientList[iClientH]->m_cMapName, "elvfarm") == 0)
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 10, 0, 0);
				else SendNotifyMsg(0, iClientH, DEF_NOTIFY_SHOWMAP, iV1, 0, 0, 0);
				break;
			}
			break;
		}
	}
	else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_USE_SKILL) {
		// 卤芒录煤掳煤 掳眉路脙碌脠 戮脝脌脤脜脹脌禄 禄莽驴毛脟脩麓脵. 戮脝脌脤脜脹脌脟 录枚赂铆脌禄 鲁路脙脽掳铆 碌么路鹿脌脤 脌脤潞楼脝庐驴隆 碌卯路脧脟脩麓脵. 

		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) ||
			(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) ||
			(m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] == true)) {
			// 戮脝脌脤脜脹脌脟 录枚赂铆脌脤 麓脵 脟脽掳脜鲁陋 戮酶掳脜鲁陋 掳眉路脙 陆潞脜鲁脌禄 禄莽驴毛脕脽脌脤露贸赂茅 鹿芦陆脙 
			return;
		}
		else {
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wMaxLifeSpan != 0) {
				// 脙脰麓毛 录枚赂铆脌脤 0脌脤赂茅 禄莽驴毛脟脴碌碌 录枚赂铆脌脤 脕脵脕枚 戮脢麓脗麓脵.
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan--;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) {
					// 戮脝脌脤脜脹脌脟 录枚赂铆脌脤 麓脵 碌脟戮煤麓脵.
					// 戮脝脌脤脜脹脌脤 赂脕掳隆脕鲁麓脵麓脗 赂脼陆脙脕枚 <- 脌脤掳脡 鹿脼脌赂赂茅 脌氓脗酶脠颅赂茅驴隆录颅 脟脴脕娄陆脙脛脩戮脽 脟脩麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMLIFESPANEND, DEF_EQUIPPOS_NONE, sItemIndex, 0, 0);
				}
				else {
					// 卤芒录煤 禄莽驴毛 陆脙掳拢 ID掳陋脌禄 卤赂脟脩麓脵. v1.12
					int iSkillUsingTimeID = (int)timeGetTime();

					bRegisterDelayEvent(DEF_DELAYEVENTTYPE_USEITEM_SKILL, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill,
						dwTime + m_pSkillConfigList[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill]->m_sValue2 * 1000,
						iClientH, DEF_OWNERTYPE_PLAYER, m_pClientList[iClientH]->m_cMapIndex, dX, dY,
						m_pClientList[iClientH]->m_cSkillMastery[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill], iSkillUsingTimeID, 0);

					// 卤芒录煤 禄莽驴毛脕脽 
					m_pClientList[iClientH]->m_bSkillUsingStatus[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = true;
					m_pClientList[iClientH]->m_iSkillUsingTimeID[m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sRelatedSkill] = iSkillUsingTimeID; //v1.12
				}
			}
		}
	}
}

bool CGame::bSetItemToBankItem(int iClientH, short sItemIndex)
{
	int i, iRet, *ip;
	DWORD* dwp;
	WORD* wp;
	char* cp;
	short* sp;
	char cData[100];
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return false;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;
	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == false) return false;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0) {

			m_pClientList[iClientH]->m_pItemInBankList[i] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
			pItem = m_pClientList[iClientH]->m_pItemInBankList[i];
			m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;

			iCalcTotalWeight(iClientH);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMTOBANK;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			*cp = i;
			cp++;

			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0;
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			// v1.432
			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			// v1.42
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			//SNOOPY: Completion/Purity fix ?
			sp = (short*)cp;
			*sp = pItem->m_sItemSpecEffectValue2;
			cp += 2;

			//------------------------------------
			sp = (short*)cp;
			*sp = pItem->m_sNewEffect1;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect2;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect3;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect4;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue1;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue3;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue4;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue5;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue6;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectType;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wMaxLifeSpan;
			cp += 2;

			ip = (int*)cp;
			*ip = pItem->m_iClass;
			cp += 4;

			ip = (int*)cp;
			*ip = pItem->m_iReqStat;
			cp += 4;

			ip = (int*)cp;
			*ip = pItem->m_iQuantStat;
			cp += 4;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 55 + 28+8);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				break;
			}

			return true;
		}
	return false;
}
bool CGame::bSetItemToBankItem(int iClientH, class CItem* pItem)
{
	int i, iRet, *ip;
	DWORD* dwp;
	WORD* wp;
	char* cp;
	short* sp;
	char cData[100];

	if (m_pClientList[iClientH] == 0) return false;
	if (pItem == 0) return false;
	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == false) return false;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0) {

			m_pClientList[iClientH]->m_pItemInBankList[i] = pItem;

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_ITEMTOBANK;

			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

			*cp = i;
			cp++;

			*cp = 1;
			cp++;

			memcpy(cp, pItem->m_cName, 20);
			cp += 20;

			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwCount;
			cp += 4;

			*cp = pItem->m_cItemType;
			cp++;

			*cp = pItem->m_cEquipPos;
			cp++;

			*cp = (char)0;
			cp++;

			sp = (short*)cp;
			*sp = pItem->m_sLevelLimit;
			cp += 2;

			*cp = pItem->m_cGenderLimit;
			cp++;

			wp = (WORD*)cp;
			*wp = pItem->m_wCurLifeSpan;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wWeight;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSprite;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sSpriteFrame;
			cp += 2;

			*cp = pItem->m_cItemColor;
			cp++;

			// v1.432
			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			// v1.42
			dwp = (DWORD*)cp;
			*dwp = pItem->m_dwAttribute;
			cp += 4;

			//SNOOPY: Completion/Purity fix ?
			sp = (short*)cp;
			*sp = pItem->m_sItemSpecEffectValue2;
			cp += 2;

			//------------------------------------
			sp = (short*)cp;
			*sp = pItem->m_sNewEffect1;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect2;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect3;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sNewEffect4;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue1;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue2;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue3;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue4;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue5;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectValue6;
			cp += 2;

			sp = (short*)cp;
			*sp = pItem->m_sItemEffectType;
			cp += 2;

			wp = (WORD*)cp;
			*wp = pItem->m_wMaxLifeSpan;
			cp += 2;

			ip = (int*)cp;
			*ip = pItem->m_iClass;
			cp += 4;

			ip = (int*)cp;
			*ip = pItem->m_iReqStat;
			cp += 4;

			ip = (int*)cp;
			*ip = pItem->m_iQuantStat;
			cp += 4;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 55 + 28+8);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				break;
			}

			return true;
		}

	return false;
}

void CGame::ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char* pItemName)
{
	char cItemCategory, cItemName[21];
	short sRemainLife;
	int   iPrice;
	double d1, d2, d3;
	bool   bNeutral;
	DWORD  dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2, dwMul1, dwMul2;
	CItem* m_pGold;

	// 禄莽驴毛脌脷脌脟 戮脝脌脤脜脹 脝脠卤芒 驴盲卤赂.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;
	if (iNum <= 0) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

	iCalcTotalWeight(iClientH);

	m_pGold = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(m_pGold, cItemName);

	// v1.42
	bNeutral = false;
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = true;
	// v2.13 录潞脠脛麓脧 录枚脕陇 鹿掳掳脟脌禄 戮卯碌冒录颅鲁陋 脝脠掳脭 录枚脕陇脟脧驴漏录颅 脝脠露搂麓脗 NPC 卤赂潞脨脌脤 脟脢驴盲戮酶麓脵. 
	// 麓脺 脛芦脜脳掳铆赂庐赂娄 卤芒脕脴脌赂路脦 戮脝脌脤脜脹 掳隆掳脻脌禄 掳谩脕陇脟脩麓脵.
	switch (cSellToWhom) {
	case 15: 		// 禄贸脕隆 戮脝脕脺赂露 
	case 24:        // 麓毛脌氓掳拢 脕脰脌脦 
		cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;
		// 12-22 录潞脠脛麓脧 录枚脕陇 戮卯碌冒录颅碌莽 脝脠录枚 脌脰掳脭 录枚脕陇 
		// 禄贸脕隆戮脝脌脤脜脹 
		if ((cItemCategory >= 11) && (cItemCategory <= 50)) {

			// 脌没脟脮脟脧麓脵. 鹿芦脕露掳脟 鹿脻掳陋 
			iPrice = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) * iNum;
			sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;


			//v1.42 脕脽赂鲁脌脦 掳忙驴矛 鹿脻脌脟 鹿脻掳陋.
			if (bNeutral == true) iPrice = iPrice / 2;
			if (iPrice <= 0)    iPrice = 1;
			if (iPrice > 1000000) iPrice = 1000000;

			if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH)) {
				// v2.12 脝脠 掳忙驴矛 鹿芦掳脭掳隆 脙脢掳煤碌脟戮卯录颅 脝脠 录枚 戮酶麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			}
			else SendNotifyMsg(0, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
		}
		// 麓毛脌氓掳拢 戮脝脌脤脜脹
		else if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
			// 驴酶路隆 戮脝脌脤脜脹脌脟 录枚赂铆掳煤 潞帽卤鲁脟脴录颅 掳篓掳隆 禄贸掳垄脌禄 掳猫禄锚, 戮脝脌脤脜脹脌脟 掳隆掳脻脌禄 赂脜卤盲麓脵.
			sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

			if (sRemainLife == 0) {
				// 掳铆脌氓鲁颅 戮脝脌脤脜脹脌潞 脝脠 录枚 戮酶麓脵
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			}
			else {
				d1 = (double)sRemainLife;
				if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
					d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
				else d2 = 1.0f;
				d3 = (d1 / d2) * 0.5f;
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 驴酶路隆 掳隆掳脻 
				d3 = d3 * d2; // 脙脽禄锚碌脠 掳隆掳脻 

				iPrice = (int)d3;
				iPrice = iPrice * iNum;

				dwAddPrice1 = 0;
				dwAddPrice2 = 0;
				// 戮脝脌脤脜脹 脝炉录潞脛隆驴隆 碌没赂楼 掳隆掳脻 禄贸陆脗 
				if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != 0) {
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

					switch (dwSWEType) {
					case 6: dwMul1 = 2; break;  // 掳隆潞颅驴卯 
					case 8: dwMul1 = 2; break;  // 掳颅脠颅碌脠
					case 5: dwMul1 = 3; break;  // 鹿脦脙赂脌脟
					case 1: dwMul1 = 4; break;  // 脟脢禄矛脌脟 
					case 7: dwMul1 = 5; break;  // 驴鹿赂庐脟脩
					case 2: dwMul1 = 6; break;  // 脕脽碌露脌脟
					case 3: dwMul1 = 15; break; // 脕陇脌脟脌脟 
					case 9: dwMul1 = 20; break; // 掳铆麓毛鹿庐赂铆 
					default: dwMul1 = 1; break;
					}

					d1 = (double)iPrice * dwMul1;
					switch (dwSWEValue) {
					case 1: d2 = 10.0f; break;
					case 2: d2 = 20.0f; break;
					case 3: d2 = 30.0f; break;
					case 4: d2 = 35.0f; break;
					case 5: d2 = 40.0f; break;
					case 6: d2 = 50.0f; break;
					case 7: d2 = 100.0f; break;
					case 8: d2 = 200.0f; break;
					case 9: d2 = 300.0f; break;
					case 10: d2 = 400.0f; break;
					case 11: d2 = 500.0f; break;
					case 12: d2 = 700.0f; break;
					case 13: d2 = 900.0f; break;
					default: d2 = 0.0f; break;
					}
					d3 = d1 * (d2 / 100.0f);

					dwAddPrice1 = (int)(d1 + d3);
				}

				// v1.42 脠帽卤脥 戮脝脌脤脜脹脌脤露贸赂茅 Sub 脠驴掳煤赂娄 录鲁脕陇脟脩麓脵. 掳酶掳脻鹿芦卤芒麓脗 1掳鲁赂赂 脌氓脗酶碌脠麓脵掳铆 脟脽脌禄露搂赂赂 脌炉脠驴脟脭.
				if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != 0) {
					dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
					dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

					switch (dwSWEType) {
					case 1:
					case 12: dwMul2 = 2; break;

					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7: dwMul2 = 4; break;

					case 8:
					case 9:
					case 10:
					case 11: dwMul2 = 6; break;
					}

					d1 = (double)iPrice * dwMul2;
					switch (dwSWEValue) {
					case 1: d2 = 10.0f; break;
					case 2: d2 = 20.0f; break;
					case 3: d2 = 30.0f; break;
					case 4: d2 = 35.0f; break;
					case 5: d2 = 40.0f; break;
					case 6: d2 = 50.0f; break;
					case 7: d2 = 100.0f; break;
					case 8: d2 = 200.0f; break;
					case 9: d2 = 300.0f; break;
					case 10: d2 = 400.0f; break;
					case 11: d2 = 500.0f; break;
					case 12: d2 = 700.0f; break;
					case 13: d2 = 900.0f; break;
					default: d2 = 0.0f; break;
					}
					d3 = d1 * (d2 / 100.0f);

					dwAddPrice2 = (int)(d1 + d3);
				}

				// v2.03 925 脝炉录枚 戮脝脌脤脜脹 掳隆掳脻 掳隆脕脽脛隆赂娄 77%录枚脕脴脌赂路脦 麓脵驴卯 
				iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

				//v1.42 脕脽赂鲁脌脦 掳忙驴矛 鹿脻脌脟 鹿脻掳陋.
				if (bNeutral == true) iPrice = iPrice / 2;
				if (iPrice <= 0)    iPrice = 1;
				if (iPrice > 1000000) iPrice = 1000000;

				if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(m_pGold, iPrice) > (DWORD)_iCalcMaxLoad(iClientH)) {
					// v2.12 脝脠 掳忙驴矛 鹿芦掳脭掳隆 脙脢掳煤碌脟戮卯录颅 脝脠 录枚 戮酶麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 4, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
				}
				else SendNotifyMsg(0, iClientH, DEF_NOTIFY_SELLITEMPRICE, cItemID, sRemainLife, iPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName, iNum);
			}
		}
		else SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTSELLITEM, cItemID, 1, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
		break;


	default:
		break;
	}
	if (m_pGold != 0) delete m_pGold;
}

void CGame::ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char* pString)
{
	class CItem* pItemGold;
	short sRemainLife;
	int   iPrice;
	double d1, d2, d3;
	char cItemName[21], cData[120], cItemCategory;
	DWORD* dwp, dwMul1, dwMul2, dwSWEType, dwSWEValue, dwAddPrice1, dwAddPrice2;
	WORD* wp;
	int    iEraseReq, iRet;
	
	bool   bNeutral;


	// 戮脝脌脤脜脹脌禄 脝脠掳脷麓脵麓脗 掳脥脌脤 掳谩脕陇碌脟戮煤麓脵.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;
	if (iNum <= 0) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount < iNum) return;

	// New 18/05/2004
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == false) return;

	iCalcTotalWeight(iClientH);
	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// v1.42
	bNeutral = false;
	if (memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) == 0) bNeutral = true;

	iPrice = 0;
	// 戮脝脌脤脜脹脌脟 脕戮路霉驴隆 碌没露贸 掳隆掳脻 掳猫禄锚.
	if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
		// 鹿芦卤芒路霉麓脵
		// 驴酶路隆 戮脝脌脤脜脹脌脟 录枚赂铆掳煤 潞帽卤鲁脟脴录颅 掳篓掳隆 禄贸掳垄脌禄 掳猫禄锚, 戮脝脌脤脜脹脌脟 掳隆掳脻脌禄 赂脜卤盲麓脵.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;

		if (sRemainLife > 0) {
			// 赂脕掳隆脕酶 戮脝脌脤脜脹脌潞 脝脠脕枚 赂酶脟脩麓脵.	

			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 驴酶路隆 掳隆掳脻 
			d3 = d3 * d2; // 脙脽禄锚碌脠 掳隆掳脻 

			iPrice = (short)d3;
			iPrice = iPrice * iNum;

			dwAddPrice1 = 0;
			dwAddPrice2 = 0;
			// 戮脝脌脤脜脹 脝炉录潞脛隆驴隆 碌没赂楼 掳隆掳脻 禄贸陆脗 
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != 0) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
				dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;

				// 脠帽卤脥 戮脝脌脤脜脹 脠驴掳煤 脕戮路霉: 
				// 0-None 1-脟脢禄矛卤芒麓毛鹿脤脕枚脙脽掳隆 2-脕脽碌露脠驴掳煤 3-脕陇脌脟脌脟 4-脌煤脕脰脌脟 
				// 5-鹿脦脙赂脌脟 6-掳隆潞颅驴卯 7-驴鹿赂庐脟脩 8-掳颅脠颅碌脠 9-掳铆麓毛鹿庐赂铆脌脟
				switch (dwSWEType) {
				case 6: dwMul1 = 2; break;  // 掳隆潞颅驴卯 
				case 8: dwMul1 = 2; break;  // 掳颅脠颅碌脠
				case 5: dwMul1 = 3; break;  // 鹿脦脙赂脌脟
				case 1: dwMul1 = 4; break;  // 脟脢禄矛脌脟 
				case 7: dwMul1 = 5; break;  // 驴鹿赂庐脟脩
				case 2: dwMul1 = 6; break;  // 脕脽碌露脌脟
				case 3: dwMul1 = 15; break; // 脕陇脌脟脌脟 
				case 9: dwMul1 = 20; break; // 掳铆麓毛鹿庐赂铆 
				default: dwMul1 = 1; break;
				}

				d1 = (double)iPrice * dwMul1;
				switch (dwSWEValue) {
				case 1: d2 = 10.0f; break;
				case 2: d2 = 20.0f; break;
				case 3: d2 = 30.0f; break;
				case 4: d2 = 35.0f; break;
				case 5: d2 = 40.0f; break;
				case 6: d2 = 50.0f; break;
				case 7: d2 = 100.0f; break;
				case 8: d2 = 200.0f; break;
				case 9: d2 = 300.0f; break;
				case 10: d2 = 400.0f; break;
				case 11: d2 = 500.0f; break;
				case 12: d2 = 700.0f; break;
				case 13: d2 = 900.0f; break;
				default: d2 = 0.0f; break;
				}
				d3 = d1 * (d2 / 100.0f);
				dwAddPrice1 = (int)(d1 + d3);
			}

			// v1.42 脠帽卤脥 戮脝脌脤脜脹脌脤露贸赂茅 Sub 脠驴掳煤赂娄 录鲁脕陇脟脩麓脵. 掳酶掳脻鹿芦卤芒麓脗 1掳鲁赂赂 脌氓脗酶碌脠麓脵掳铆 脟脽脌禄露搂赂赂 脌炉脠驴脟脭.
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) != 0) {
				dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x0000F000) >> 12;
				dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00000F00) >> 8;

				// 脠帽卤脥 戮脝脌脤脜脹 脠驴掳煤 脕戮路霉: 
				//脙脽掳隆 碌露录潞脌煤脟脳(1), 脙脽掳隆 赂铆脕脽掳陋(2), 脙脽掳隆 鹿忙戮卯掳陋(3), HP 脠赂潞鹿路庐 脙脽掳隆(4), SP 脠赂潞鹿路庐 脙脽掳隆(5)
				//MP 脠赂潞鹿路庐 脙脽掳隆(6), 脙脽掳隆 赂露鹿媒脌煤脟脳(7), 鹿掳赂庐 麓毛鹿脤脕枚 脠铆录枚(8), 赂露鹿媒 麓毛鹿脤脕枚 脠铆录枚(9)
				//驴卢脜赂 麓毛鹿脤脕枚 脙脽掳隆(10), 麓玫 赂鹿脌潞 掳忙脟猫脛隆(11), 麓玫赂鹿脌潞 Gold(12)
				switch (dwSWEType) {
				case 1:
				case 12: dwMul2 = 2; break;

				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7: dwMul2 = 4; break;

				case 8:
				case 9:
				case 10:
				case 11: dwMul2 = 6; break;
				}

				d1 = (double)iPrice * dwMul2;
				switch (dwSWEValue) {
				case 1: d2 = 10.0f; break;
				case 2: d2 = 20.0f; break;
				case 3: d2 = 30.0f; break;
				case 4: d2 = 35.0f; break;
				case 5: d2 = 40.0f; break;
				case 6: d2 = 50.0f; break;
				case 7: d2 = 100.0f; break;
				case 8: d2 = 200.0f; break;
				case 9: d2 = 300.0f; break;
				case 10: d2 = 400.0f; break;
				case 11: d2 = 500.0f; break;
				case 12: d2 = 700.0f; break;
				case 13: d2 = 900.0f; break;
				default: d2 = 0.0f; break;
				}
				d3 = d1 * (d2 / 100.0f);
				dwAddPrice2 = (int)(d1 + d3);
			}

			iPrice = iPrice + (dwAddPrice1 - (dwAddPrice1 / 3)) + (dwAddPrice2 - (dwAddPrice2 / 3));

			//v1.42 脕脽赂鲁脌脦 掳忙驴矛 鹿脻脌脟 鹿脻掳陋.
			if (bNeutral == true) iPrice = iPrice / 2;
			if (iPrice <= 0) iPrice = 1;
			if (iPrice > 1000000) iPrice = 1000000; // New 06/05/2004

			// 戮脝脌脤脜脹脌禄 脝脠戮脪麓脵麓脗 赂脼陆脙脕枚 脌眉录脹 (麓脵脌脤戮贸路脦卤脳 鹿脷陆潞 潞帽脠掳录潞脠颅驴毛)
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, 0, 0, 0);

			_bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

			// 脝脠 戮脝脌脤脜脹脌禄 禄猫脕娄 
			if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
				(m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
				// 录枚路庐 掳鲁鲁盲脌脤 脌脰麓脵赂茅 掳鹿录枚赂娄 脕脵脌脦麓脵.
				// v1.41 !!!
				SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
			}
			else ItemDepleteHandler(iClientH, cItemID, false, false);
		}
	}
	else if ((cItemCategory >= 11) && (cItemCategory <= 50)) {
		// 陆脛脟掳, 脌芒脠颅碌卯脌脟 鹿脻掳陋鹿掳掳脟碌茅 
		iPrice = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		iPrice = iPrice * iNum;

		//v1.42 脕脽赂鲁脌脦 掳忙驴矛 鹿脻脌脟 鹿脻掳陋.
		if (bNeutral == true) iPrice = iPrice / 2;
		if (iPrice <= 0) iPrice = 1;
		if (iPrice > 1000000) iPrice = 1000000; // New 06/05/2004

		// 戮脝脌脤脜脹脌禄 脝脠戮脪麓脵麓脗 赂脼陆脙脕枚 脌眉录脹 (麓脵脌脤戮贸路脦卤脳 鹿脷陆潞 潞帽脠掳录潞脠颅驴毛)
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMSOLD, cItemID, 0, 0, 0);

		_bItemLog(DEF_ITEMLOG_SELL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[cItemID]);

		// 戮脝脌脤脜脹脌脟 脕戮路霉驴隆 碌没露贸 脌没脌媒脟脩 脙鲁赂庐赂娄 脟脩麓脵.
		if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pClientList[iClientH]->m_pItemList[cItemID]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
			// 录枚路庐 掳鲁鲁盲脌脤 脌脰麓脵赂茅 掳鹿录枚赂娄 脕脵脌脦麓脵.
			// v1.41 !!!
			SetItemCount(iClientH, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwCount - iNum);
		}
		else ItemDepleteHandler(iClientH, cItemID, false, true);
	}

	// Gold赂娄 脕玫掳隆陆脙脜虏麓脵. 赂赂戮脿 脝脟 掳隆掳脻脌脤 0 脠陇脌潞 赂露脌脤鲁脢陆潞脌脤赂茅 卤脻脌禄 脕脰脕枚 戮脢麓脗麓脵.
	if (iPrice <= 0) return;

	pItemGold = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(pItemGold, cItemName);

	pItemGold->m_dwCount = iPrice;

	if (_bAddClientItemList(iClientH, pItemGold, &iEraseReq) == true) {
		SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItemGold, 0);

		if (iEraseReq == 1)
			delete pItemGold;

		iCalcTotalWeight(iClientH);
	}
	else {
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY, pItemGold);

		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItemGold->m_sSprite, pItemGold->m_sSpriteFrame, pItemGold->m_cItemColor);*/ // v1.4 color

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItemGold->m_sIDnum, pItemGold->m_sSpriteFrame, pItemGold->m_cItemColor, pItemGold->m_dwAttribute);

		iCalcTotalWeight(iClientH);

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		switch (iRet) {
		case DEF_XSOCKEVENT_QUENEFULL:
		case DEF_XSOCKEVENT_SOCKETERROR:
		case DEF_XSOCKEVENT_CRITICALERROR:
		case DEF_XSOCKEVENT_SOCKETCLOSED:
			// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
			DeleteClient(iClientH, true, true);
			break;
		}
	}
}

void CGame::ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char* pString)
{
	char cItemCategory;
	short sRemainLife, sPrice;
	double d1, d2, d3;

	// 戮脝脌脤脜脹脌禄 掳铆脛隆掳脷麓脵麓脗 驴盲卤赂.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;

	//Magn0S:: Cant repair fragile item type 2
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_sNewEffect1 == DEF_FRAGILEITEM) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You can't repair Fragile Item.");
		return;
	}

	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// 戮脝脌脤脜脹脌脟 脕戮路霉驴隆 碌没露贸 掳隆掳脻 掳猫禄锚.
	if ((cItemCategory >= 1) && (cItemCategory <= 10)) {
		// 鹿芦卤芒路霉麓脵

		// 赂赂戮脿 鹿芦卤芒赂娄 麓毛脌氓掳拢 脕脰脌脦脌脤 戮脝麓脩 脌脤驴隆掳脭 掳铆脙脛麓脼露贸掳铆 脟脩麓脵赂茅 脟脪 录枚 戮酶麓脵. 
		if (cRepairWhom != 24) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			return;
		}

		// 驴酶路隆 戮脝脌脤脜脹脌脟 录枚赂铆掳煤 潞帽卤鲁脟脴录颅 掳篓掳隆 禄贸掳垄脌禄 掳猫禄锚, 戮脝脌脤脜脹脌脟 录枚赂庐 潞帽驴毛脌禄 赂脜卤盲麓脵.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 驴脧脌眉脠梅 赂脕掳隆脕酶 掳脥脌脤露贸赂茅 驴酶路隆掳隆掳脻脌脟 脌媒鹿脻脌脤 碌莽麓脵. 
			sPrice = (short)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 驴酶路隆 掳隆掳脻 
			d3 = d3 * d2; // 脙脽禄锚碌脠 掳隆掳脻 

			sPrice = (short)(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
	else if (((cItemCategory >= 43) && (cItemCategory <= 50)) || ((cItemCategory >= 11) && (cItemCategory <= 12))) {
		// 鲁卢陆脙麓毛, 掳卯卤陋脌脤 碌卯掳煤 掳掳脌潞 潞帽鹿芦卤芒路霉 录枚赂庐掳隆麓脡 戮脝脌脤脜脹. 驴脢, 潞脦脙梅路霉

		// 赂赂戮脿 禄贸脕隆 脕脰脌脦脌脤 戮脝麓脩 脌脤驴隆掳脭 掳铆脙脛麓脼露贸掳铆 脟脩麓脵赂茅 脟脪 录枚 戮酶麓脵. 
		if (cRepairWhom != 15) {
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 2, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
			return;
		}

		// 驴酶路隆 戮脝脌脤脜脹脌脟 录枚赂铆掳煤 潞帽卤鲁脟脴录颅 掳篓掳隆 禄贸掳垄脌禄 掳猫禄锚, 戮脝脌脤脜脹脌脟 录枚赂庐 潞帽驴毛脌禄 赂脜卤盲麓脵.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 驴脧脌眉脠梅 赂脕掳隆脕酶 掳脥脌脤露贸赂茅 驴酶路隆掳隆掳脻脌脟 脌媒鹿脻脌脤 碌莽麓脵. 
			sPrice = (short)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)sRemainLife;
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 驴酶路隆 掳隆掳脻 
			d3 = d3 * d2; // 脙脽禄锚碌脠 掳隆掳脻 

			sPrice = (short)(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_REPAIRITEMPRICE, cItemID, sRemainLife, sPrice, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
	else {
		// 掳铆脛楼录枚 戮酶麓脗 戮脝脌脤脜脹脌脤麓脵.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTREPAIRITEM, cItemID, 1, 0, m_pClientList[iClientH]->m_pItemList[cItemID]->m_cName);
	}
}

void CGame::ReqRepairItemCofirmHandler(int iClientH, char cItemID, char* pString)
{
	short    sRemainLife, sPrice;
	char* cp, cItemCategory, cData[120];
	double   d1, d2, d3, dV1, dV2, dV3;
	DWORD* dwp, dwGoldCount, dwSWEType, dwSWEValue;
	WORD* wp;
	int      iRet, iGoldWeight;

	// 戮脝脌脤脜脹脌禄 录枚赂庐脟脧掳脷麓脵麓脗 掳脥脌脤 掳谩脕陇碌脟戮煤麓脵.
	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	if ((cItemID < 0) || (cItemID >= 50)) return;
	if (m_pClientList[iClientH]->m_pItemList[cItemID] == 0) return;

	//Magn0S:: Cant repair fragile item type 2
	if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_sNewEffect1 == DEF_FRAGILEITEM) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You can't repair Fragile Item.");
		return;
	}

	// New 18/05/2004
	if (m_pClientList[iClientH]->m_pIsProcessingAllowed == false) return;

	cItemCategory = m_pClientList[iClientH]->m_pItemList[cItemID]->m_cCategory;

	// 戮脝脌脤脜脹脌脟 脕戮路霉驴隆 碌没露贸 掳隆掳脻 掳猫禄锚.
	if (((cItemCategory >= 1) && (cItemCategory <= 10)) || ((cItemCategory >= 43) && (cItemCategory <= 50)) ||
		((cItemCategory >= 11) && (cItemCategory <= 12))) {
		// 鹿芦卤芒路霉 脠陇脌潞 鲁卢陆脙麓毛, 掳卯卤陋脌脤驴脥 掳掳脌潞 戮脝脌脤脜脹, 驴脢, 陆脜鹿脽 

		// 驴酶路隆 戮脝脌脤脜脹脌脟 录枚赂铆掳煤 潞帽卤鲁脟脴录颅 掳篓掳隆 禄贸掳垄脌禄 掳猫禄锚, 戮脝脌脤脜脹脌脟 录枚赂庐 潞帽驴毛脌禄 赂脜卤盲麓脵.
		sRemainLife = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan;
		if (sRemainLife == 0) {
			// 驴脧脌眉脠梅 赂脕掳隆脕酶 掳脥脌脤露贸赂茅 驴酶路隆掳隆掳脻脌脟 脌媒鹿脻脌脤 碌莽麓脵. 
			sPrice = (short)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2;
		}
		else {
			d1 = (double)abs(sRemainLife);
			if (m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan != 0)
				d2 = (double)abs(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan);
			else d2 = 1.0f;
			d3 = (d1 / d2) * 0.5f;
			d2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice; // 驴酶路隆 掳隆掳脻 
			d3 = d3 * d2; // 脙脽禄锚碌脠 掳隆掳脻 

			sPrice = (short)(m_pClientList[iClientH]->m_pItemList[cItemID]->m_wPrice / 2) - (short)d3;
		}

		// sPrice赂赂脜颅脌脟 碌路脌脤 碌脟赂茅 掳铆脛楼 录枚 脌脰脌赂鲁陋 潞脦脕路脟脧赂茅 掳铆脛楼 录枚 戮酶麓脵. 
		dwGoldCount = dwGetItemCount(iClientH, "Gold");

		if (dwGoldCount < (DWORD)sPrice) {
			// 脟脙路鹿脌脤戮卯掳隆 掳庐掳铆脌脰麓脗 Gold掳隆 戮脝脌脤脜脹 录枚赂庐 潞帽驴毛驴隆 潞帽脟脴 脌没麓脵. 掳铆脛楼 录枚 戮酶脌陆.
			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_NOTENOUGHGOLD;
			cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
			*cp = cItemID;
			cp++;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
				DeleteClient(iClientH, true, true);
				break;
			}
			return;
		}
		else {
			//碌路脌脤 脙忙潞脨脟脧麓脵. 掳铆脛楼 录枚 脌脰麓脵. 

			// Centuu : Repair Fix (Recalculation of maxlifespan) - HB2
			for (int i = 0; i < DEF_MAXITEMTYPES; i++) {
				if ((m_pItemConfigList[i] != 0) &&
					(m_pItemConfigList[i]->m_sIDnum == m_pClientList[iClientH]->m_pItemList[cItemID]->m_sIDnum)) {
					m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
					if ((m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) != 0) {
						dwSWEType = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x00F00000) >> 20;
						dwSWEValue = (m_pClientList[iClientH]->m_pItemList[cItemID]->m_dwAttribute & 0x000F0000) >> 16;
						switch (dwSWEType) {
						case 8: // Strong
						case 9: // Ancient
							dV2 = (double)m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
							dV3 = (double)(dwSWEValue * 7);
							dV1 = (dV3 / 100.0f) * dV2;
							m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan += (int)dV1;
							break;
						}
					}
				}
			}

			// 戮脝脌脤脜脹脌脟 录枚赂铆脌禄 麓脙赂庐掳铆 脜毛潞赂脟脩麓脵. !BUG POINT 脌搂脛隆掳隆 脕脽驴盲脟脧麓脵. 赂脮脌煤 录枚赂铆脌禄 麓脙赂庐掳铆 碌路脌脟 脛芦驴卯脝庐赂娄 鲁路脙谩麓脵.
			m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan = m_pClientList[iClientH]->m_pItemList[cItemID]->m_wMaxLifeSpan;
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMREPAIRED, cItemID, m_pClientList[iClientH]->m_pItemList[cItemID]->m_wCurLifeSpan, 0, 0);

			iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - sPrice);

			// 录脪脕枚脟掳 脙脩 脕脽路庐 脌莽 掳猫禄锚 
			iCalcTotalWeight(iClientH);


		}
	}
}

int CGame::iCalcTotalWeight(int iClientH)
{
	int i, iWeight;
	short sItemIndex;

	if (m_pClientList[iClientH] == 0) return 0;

	m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
	// 脗酶驴毛脟脧脕枚 戮脢戮脝碌碌 脠驴掳煤掳隆 脌脰麓脗 戮脝脌脤脜脹 脠驴掳煤. 录脪脕枚脟脧掳铆 脌脰戮卯碌碌 脠驴掳煤掳隆 脌脰卤芒 露搂鹿庐驴隆 驴漏卤芒录颅 掳脣禄莽.
	for (sItemIndex = 0; sItemIndex < DEF_MAXITEMS; sItemIndex++)
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
			switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType) {
			case DEF_ITEMEFFECTTYPE_ALTERITEMDROP:
				if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan > 0) {
					// 录枚赂铆脌脤 脌脰戮卯戮脽 脠驴掳煤掳隆 脌脰麓脵.
					m_pClientList[iClientH]->m_iAlterItemDropIndex = sItemIndex;
				}
				break;
			}
		}

	iWeight = 0;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0) {

			iWeight += iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], m_pClientList[iClientH]->m_pItemList[i]->m_dwCount);
		}

	m_pClientList[iClientH]->m_iCurWeightLoad = iWeight;

	return iWeight;
}

// 05/29/2004 - Hypnotoad - Purchase Dicount updated to take charisma into consideration
void CGame::RequestPurchaseItemHandler(int iClientH, char* pItemName, int iNum)
{
	class CItem* pItem;
	char* cp, cItemName[21], cData[100];
	
	DWORD* dwp, dwGoldCount, dwItemCount;
	WORD* wp;
	int   i, iRet, iEraseReq, iGoldWeight;
	int   iCost;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	ZeroMemory(cData, sizeof(cData));
	ZeroMemory(cItemName, sizeof(cItemName));


	if (memcmp(pItemName, "10Arrows", 8) == 0) {
		strcpy(cItemName, "Arrow");
		dwItemCount = 10;
	}
	else if (memcmp(pItemName, "100Arrows", 9) == 0) {
		strcpy(cItemName, "Arrow");
		dwItemCount = 100;
	}
	else {
		memcpy(cItemName, pItemName, 20);
		dwItemCount = 1;
	}

	for (i = 1; i <= iNum; i++) {

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, cItemName) == false) {
			delete pItem;
		}
		else {

			if (pItem->m_bIsForSale == false) {
				delete pItem;
				return;
			}

			pItem->m_dwCount = dwItemCount;

			//Heldenian Price Fix Thing
			/*if (m_pClientList[iClientH]->m_cSide == m_sLastHeldenianWinner) 
			{
				iCost = (int)((double)(pItem->m_wPrice) * 0.9f + 0.5f);
				iCost = iCost * pItem->m_dwCount;
				iCost2 = pItem->m_wPrice * pItem->m_dwCount;
			}
			else 
			{
				iCost2 = iCost = (pItem->m_wPrice * pItem->m_dwCount); //LifeX Fix gold price 11/2
			}

			dwGoldCount = dwGetItemCount(iClientH, "Gold");

			// Charisma
			iDiscountRatio = (int)((m_pClientList[iClientH]->m_iCharisma - 10) / 4);

			dTmp1 = (double)(iDiscountRatio);
			dTmp2 = dTmp1 / 100.0f;
			dTmp1 = (double)iCost;
			dTmp3 = dTmp1 * dTmp2;
			iDiscountCost = (int)dTmp3;

			//Fix For Heldenian Price =x	
			if ((iCost - iDiscountCost) <= (int)(iCost2 / 2)) 
			{
				iDiscountCost = (int)(iCost - (iCost2 / 2) + 1);
			}*/

			dwGoldCount = dwGetItemCount(iClientH, "Gold");

			iCost = (pItem->m_wPrice * pItem->m_dwCount);

			//if (dwGoldCount < (DWORD)(iCost - iDiscountCost)) {
			if (dwGoldCount < (DWORD)(iCost)) {
				delete pItem;

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_NOTENOUGHGOLD;
				cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);
				*cp = -1;
				cp++;

				iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 7);
				switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					DeleteClient(iClientH, true, true);
					break;
				}
				return;
			}

			if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
				if (m_pClientList[iClientH]->m_iCurWeightLoad < 0) m_pClientList[iClientH]->m_iCurWeightLoad = 0;

				//wTempPrice = (iCost - iDiscountCost);
				//wTempPrice = iCost;
				SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMPURCHASED, pItem, iCost);

				if (iEraseReq == 1) delete pItem;

				iGoldWeight = SetItemCount(iClientH, "Gold", dwGoldCount - (DWORD)iCost);
				iCalcTotalWeight(iClientH);
			}
			else
			{
				delete pItem;

				iCalcTotalWeight(iClientH);

				dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
				*dwp = MSGID_NOTIFY;
				wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

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

void CGame::GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char* pItemName)
{
	int iRet, iEraseReq;
	short sOwnerH;
	char cOwnerType, cData[100], cCharName[21];
	DWORD* dwp;
	WORD* wp;
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (iAmount <= 0) return;

	if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) {
		PutLogList("(X) GiveItemHandler - Not matching Item name");
		return;
	}

	ZeroMemory(cCharName, sizeof(cCharName));

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount > (DWORD)iAmount)) {

		pItem = new class CItem;
		if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == false) {
			// 潞脨脟脪脟脧掳铆脌脷 脟脧麓脗 戮脝脌脤脜脹脌脤 赂庐陆潞脝庐驴隆 戮酶麓脗 掳脜麓脵. 脌脤路卤 脌脧脌潞 脌脧戮卯鲁炉 录枚掳隆 戮酶脕枚赂赂 
			delete pItem;
			return;
		}
		else {
			pItem->m_dwCount = iAmount;
		}

		m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

		SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
		if (wObjectID != 0) {
			if (wObjectID < 10000) {
				if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS)) {
					if (m_pClientList[wObjectID] != 0) {
						if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
					}
				}
			}
			else {
				// NPC
				if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS)) {
					if (m_pNpcList[wObjectID - 10000] != 0) {
						if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = 0;
					}
				}
			}
		}

		// Snoopy: Bock attemps to give item to a far character.
		/*if (((abs(m_pClientList[iClientH]->m_sX) - dX) > 5)
			|| ((abs(m_pClientList[iClientH]->m_sY) - dY) > 5))
		{
			sOwnerH = 0;
		}*/

		if (sOwnerH == 0) {
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

			// v1.411  
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, pItem);

			/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ //v1.4 color

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
		}
		else {
			if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
				memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);

				//Magn0S:: Cancel to give itens for GM.
				if (m_pClientList[sOwnerH]->m_iAdminUserLevel > 0) {
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "You can't give items to GM.");
					return;
				}

				if (sOwnerH == iClientH) {
					delete pItem;
					return;
				}

				if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == true) {
					SendItemNotifyMsg(sOwnerH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED, sItemIndex, iAmount, 0, cCharName);
				
				}
				else {
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						pItem);

					// v1.411  
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, pItem);

					/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ //v1.4 color

					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 6);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
						DeleteClient(sOwnerH, true, true);
						break;
					}

					// v1.4 录枚路庐麓脺脌搂脌脟 戮脝脌脤脜脹脌禄 脌眉麓脼驴隆 陆脟脝脨脟脽脌陆脌禄 戮脣赂掳麓脵.
					SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTGIVEITEM, sItemIndex, iAmount, 0, cCharName);
				}

			}
			else {
				// NPC驴隆掳脭 戮脝脌脤脜脹脌禄 脕脰戮煤麓脵.
				memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

				if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0) {
					// NPC掳隆 脙垄掳铆 脕脰脌脦脌脤戮煤麓脵赂茅 鹿掳掳脟脌禄 潞赂掳眉脟脧掳脷麓脵麓脗 脌脟鹿脤脌脤麓脵. 
					if (bSetItemToBankItem(iClientH, pItem) == false) {
						// 鹿掳掳脟脌禄 赂脙卤芒麓脗碌楼 陆脟脝脨脟脧驴麓麓脵.	
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, 0, 0, 0, 0);

						// 陆脟脝脨脟脽脌赂鹿脟路脦 鹿脵麓脷驴隆 露鲁卤潞麓脵.
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

						// v1.411  
						_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, pItem);

						// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
						/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ // v1.4 color

						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
					}
				}
				
				else {
					// 脌脧鹿脻 NPC驴隆掳脭 戮脝脌脤脜脹脌禄 脕脰赂茅 戮脝脌脤脜脹脌禄 录颅脌脰麓脗 脌搂脛隆驴隆 鹿枚路脕戮脽 脟脩麓脵. 
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);

					// v1.411  
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, pItem);

					// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
					/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ // v1.4 color

					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);
				}
			}
		}
	}
	else {
		// 戮脝脌脤脜脹 脌眉潞脦赂娄 脕脰戮煤麓脵.

		// 脌氓脗酶脕脽脌脦 戮脝脌脤脜脹脌禄 脕脰戮煤麓脵赂茅 戮脝脌脤脜脹 脌氓脗酶脠驴掳煤赂娄 脟脴脕娄脟脴戮脽 脟脧鹿脟路脦.
		ReleaseItemHandler(iClientH, sItemIndex, true);

		// 脙鲁赂庐碌碌脕脽 驴隆路炉掳隆 鹿脽禄媒脟脪露搂赂娄 麓毛潞帽脟脴录颅 -1路脦 脟脪麓莽脟脴 鲁玫麓脗麓脵.
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)
			m_pClientList[iClientH]->m_cArrowIndex = -1;

		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY); // 戮脝脌脤脜脹脌禄 dX, dY驴隆 脌脰麓脗 脛鲁赂炉脜脥驴隆掳脭 脕脴麓脵. 赂赂戮脿 鹿脼脌禄 脛鲁赂炉脜脥掳隆 戮脝脌脤脜脹脌禄 鹿脼脕枚 赂酶脟脪 禄贸脠虏脌脤露贸赂茅 露楼驴隆 露鲁戮卯脕酶麓脵.  

		if (wObjectID != 0) {
			if (wObjectID < 10000) {
				// 脟脙路鹿脌脤戮卯 
				if ((wObjectID > 0) && (wObjectID < DEF_MAXCLIENTS)) {
					if (m_pClientList[wObjectID] != 0) {
						if ((WORD)sOwnerH != wObjectID) sOwnerH = 0;
					}
				}
			}
			else {
				// NPC
				if ((wObjectID - 10000 > 0) && (wObjectID - 10000 < DEF_MAXNPCS)) {
					if (m_pNpcList[wObjectID - 10000] != 0) {
						if ((WORD)sOwnerH != (wObjectID - 10000)) sOwnerH = 0;
					}
				}
			}
		}

		// Snoopy: Bock attemps to give item to a far character.
		/*if (((abs(m_pClientList[iClientH]->m_sX) - dX) > 5)
			|| ((abs(m_pClientList[iClientH]->m_sY) - dY) > 5))
		{
			sOwnerH = 0;
		}*/

		if (sOwnerH == 0) {
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
				m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ // v1.4 color

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, 0);
		}
		else {

			if (cOwnerType == DEF_OWNERTYPE_PLAYER) {
				memcpy(cCharName, m_pClientList[sOwnerH]->m_cCharName, 10);
				pItem = m_pClientList[iClientH]->m_pItemList[sItemIndex];

				if (pItem->m_sIDnum == 88) {
					if ((m_pClientList[iClientH]->m_iGuildRank == -1) &&
						(memcmp(m_pClientList[iClientH]->m_cLocation, "NONE", 4) != 0) &&
						(memcmp(m_pClientList[iClientH]->m_cLocation, m_pClientList[sOwnerH]->m_cLocation, 10) == 0) &&
						(m_pClientList[sOwnerH]->m_iGuildRank == 0 || m_pClientList[sOwnerH]->m_iGuildRank == 1)) {
						SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION, 0, 0, 0, 0);
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, 0, cCharName);

						_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

						goto REMOVE_ITEM_PROCEDURE;
					}
				}
				if ((m_bIsCrusadeMode == false) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89)) {
					if ((memcmp(m_pClientList[iClientH]->m_cGuildName, m_pClientList[sOwnerH]->m_cGuildName, 20) == 0) &&
						(m_pClientList[iClientH]->m_iGuildRank != -1) &&
						(m_pClientList[sOwnerH]->m_iGuildRank == 0 || m_pClientList[sOwnerH]->m_iGuildRank == 1)) {
						// 辨靛 付胶磐捞促.	辨靛 付胶磐俊霸绰 呕硼犬牢 夸没 皋矫瘤甫 傈价茄促.
						SendNotifyMsg(iClientH, sOwnerH, DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION, 0, 0, 0, 0);
						// 捞力 酒捞袍阑 霖 夯牢俊霸 酒捞袍阑 林菌栏骨肺 府胶飘俊辑 昏力且巴阑 烹焊茄促.
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, 1, 0, cCharName);

						_bItemLog(DEF_ITEMLOG_DEPLETE, iClientH, (int)-1, pItem);

						goto REMOVE_ITEM_PROCEDURE;
					}
				}

				if (_bAddClientItemList(sOwnerH, pItem, &iEraseReq) == true) {

					_bItemLog(DEF_ITEMLOG_GIVE, iClientH, sOwnerH, pItem);
					SendItemNotifyMsg(sOwnerH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

					if (iEraseReq == 1) delete pItem;
				}
				else {
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]);
					_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ // v1.4 color

					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

					dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
					*dwp = MSGID_NOTIFY;
					wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
					*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

					iRet = m_pClientList[sOwnerH]->m_pXSock->iSendMsg(cData, 6);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						// 赂脼陆脙脕枚赂娄 潞赂鲁戮露搂 驴隆路炉掳隆 鹿脽禄媒脟脽麓脵赂茅 脕娄掳脜脟脩麓脵.
						DeleteClient(sOwnerH, true, true);
						break;
					}

					// v1.4 戮脝脌脤脜脹 脌眉麓脼脌脤 陆脟脝脨脟脽脌陆脌禄 戮脣赂庐麓脗 鹿忙鹿媒 
					ZeroMemory(cCharName, sizeof(cCharName));
				}
			}
			else {
				// NPC驴隆掳脭 戮脝脌脤脜脹脌禄 脕脰戮煤麓脵.
				memcpy(cCharName, m_pNpcList[sOwnerH]->m_cNpcName, 20);

				if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Howard", 6) == 0) {
					// NPC掳隆 脙垄掳铆 脕脰脌脦脌脤戮煤麓脵赂茅 鹿掳掳脟脌禄 潞赂掳眉脟脧掳脷麓脵麓脗 脌脟鹿脤脌脤麓脵. 
					if (bSetItemToBankItem(iClientH, sItemIndex) == false) {
						// 戮脝脌脤脜脹脌禄 潞赂掳眉脟脧麓脗碌楼 陆脟脝脨脟脧驴漏 鹿脵麓脷驴隆 露鲁戮卯脕鲁麓脵.
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTITEMTOBANK, 0, 0, 0, 0);

						// 陆脟脝脨脟脽脌赂鹿脟路脦 鹿脵麓脷驴隆 露鲁卤潞麓脵.
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
							m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// v1.41 脠帽卤脥 戮脝脌脤脜脹脌禄 露鲁戮卯露脽赂掳 掳脥脌脤露贸赂茅 路脦卤脳赂娄 鲁虏卤盲麓脵. 
						_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
						/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ // v1.4 color

						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
					}
				}
				else if (memcmp(m_pNpcList[sOwnerH]->m_cNpcName, "Kennedy", 7) == 0) {
					// 卤忙碌氓 禄莽鹿芦脌氓驴隆掳脭 卤忙碌氓 脜禄脜冒 陆脜脙禄录颅赂娄 脕脰戮煤麓脵赂茅 卤忙碌氓赂娄 脜禄脜冒脟脩麓脵. 
					if ((m_bIsCrusadeMode == false) && (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 89)) {

						if ((m_pClientList[iClientH]->m_iGuildRank != 0) && (m_pClientList[iClientH]->m_iGuildRank != -1)) {
							// 脜禄脜冒 陆脜脙禄脌脷驴隆掳脭 脜禄脜冒 录潞掳酶脟脽脌陆脌禄 戮脣赂庐麓脗 赂脼陆脙脕枚赂娄 潞赂鲁禄脕脴麓脵.
							SendNotifyMsg(iClientH, iClientH, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, 0, 0, 0, 0);

							// 卤忙碌氓 脜禄脜冒. 
							ZeroMemory(m_pClientList[iClientH]->m_cGuildName, sizeof(m_pClientList[iClientH]->m_cGuildName));
							memcpy(m_pClientList[iClientH]->m_cGuildName, "NONE", 4);
							m_pClientList[iClientH]->m_iGuildRank = -1;
							m_pClientList[iClientH]->m_iGuildGUID = -1;

							// 脝炉录潞脌脤 鹿脵虏卯鹿脟路脦 驴脺戮莽脌禄 禄玫路脦 潞赂鲁陆麓脵. 
							SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);


						}

						// 脌脤 戮脝脌脤脜脹脌潞 禄猫脕娄陆脙脛脩戮脽 脟脩麓脵. 
						delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
					}
					else {
						// 卤忙碌氓 脜禄脜冒 陆脜脙禄录颅掳隆 戮脝麓脧露贸赂茅 鹿脵麓脷驴隆 露鲁卤潞麓脵. 脜漏路莽录录脌脤碌氓 赂冒碌氓脌脧露搂碌碌 卤忙碌氓 掳眉路脙 驴卢禄锚 潞脪掳隆 
						m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
							m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

						/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ // v1.4 color

						SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
							m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
							m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

						ZeroMemory(cCharName, sizeof(cCharName));

					}
				}
				else {
					m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
						m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					_bItemLog(DEF_ITEMLOG_DROP, iClientH, 0, m_pClientList[iClientH]->m_pItemList[sItemIndex]);

					/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ // v1.4 color

					SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
						m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
						m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);

					ZeroMemory(cCharName, sizeof(cCharName));
				}
			}

			SendNotifyMsg(0, iClientH, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, cCharName);
		}

	REMOVE_ITEM_PROCEDURE:;
		if (m_pClientList[iClientH] == 0) return;

		m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
		m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;

		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}

	iCalcTotalWeight(iClientH);
}

DWORD CGame::dwGetItemCount(int iClientH, char* pName)
{
	int i;
	char cTmpName[21];

	if (m_pClientList[iClientH] == 0) return 0;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pName);

	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0)) {

			return m_pClientList[iClientH]->m_pItemList[i]->m_dwCount;
		}

	return 0;
}

int CGame::SetItemCount(int iClientH, char* pItemName, DWORD dwCount)
{
	int i;
	char cTmpName[21];
	WORD wWeight;

	if (m_pClientList[iClientH] == 0) return -1;

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pItemName);

	//m_pClientList[iClientH]->m_dwGold = dwCount;
	//SendNotifyMsg(0, iClientH, DEF_NOTIFY_GOLD, m_pClientList[iClientH]->m_dwGold, 0, 0, 0);

	for (i = 0; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, cTmpName, 20) == 0)) {

			wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[i], 1);

			// 脛芦驴卯脝庐掳隆 0脌脤赂茅 赂冒碌脦 录脪赂冒碌脠 掳脥脌脤鹿脟路脦 赂庐陆潞脝庐驴隆录颅 禄猫脕娄脟脩麓脵.
			if (dwCount == 0) {
				ItemDepleteHandler(iClientH, i, false, true);
			}
			else {
				// 戮脝脌脤脜脹脌脟 录枚路庐脌脤 潞炉掳忙碌脟戮煤脌陆脌禄 戮脣赂掳麓脵. 
				m_pClientList[iClientH]->m_pItemList[i]->m_dwCount = dwCount;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETITEMCOUNT, i, dwCount, (char)true, 0);
			}

			return wWeight;
		}

	return -1;
}


int CGame::SetItemCount(int iClientH, int iItemIndex, DWORD dwCount)
{
	WORD wWeight;

	if (m_pClientList[iClientH] == 0) return -1;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return -1;

	wWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemList[iItemIndex], 1);

	// 脛芦驴卯脝庐掳隆 0脌脤赂茅 赂冒碌脦 录脪赂冒碌脠 掳脥脌脤鹿脟路脦 赂庐陆潞脝庐驴隆录颅 禄猫脕娄脟脩麓脵.
	if (dwCount == 0) {
		ItemDepleteHandler(iClientH, iItemIndex, false, true);
	}
	else {
		// 戮脝脌脤脜脹脌脟 录枚路庐脌脤 潞炉掳忙碌脟戮煤脌陆脌禄 戮脣赂掳麓脵. 
		m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwCount = dwCount;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SETITEMCOUNT, iItemIndex, dwCount, (char)true, 0);
	}

	return wWeight;
}

// New 16/05/2004
void CGame::ReqCreateSlateHandler(int iClientH, char* pData)
{
	int i, iRet;
	char cItemID[4], ctr[4];
	char* cp, cSlateColour, cData[120];
	bool bIsSlatePresent = false;
	CItem* pItem;
	int iSlateType, iEraseReq;
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;

	for (i = 0; i < 4; i++) {
		cItemID[i] = 0;
		ctr[i] = 0;
	}

	cp = (char*)pData;
	cp += 10;

	// 14% chance of creating slates
	if (iDice(1, 100) <= m_sSlateSuccessRate) bIsSlatePresent = true;

	try {
		// make sure slates really exist
		for (i = 0; i < 4; i++) {
			cItemID[i] = *cp;
			cp++;

			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] == 0 || cItemID[i] > DEF_MAXITEMS) {
				bIsSlatePresent = false;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
				return;
			}

			//No duping
			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 868) // LU
				ctr[0] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 869) // LD
				ctr[1] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 870) // RU
				ctr[2] = 1;
			else if (m_pClientList[iClientH]->m_pItemList[cItemID[i]]->m_sIDnum == 871) // RD
				ctr[3] = 1;
		}
	}
	catch (...) {
		//Crash Hacker Caught
		bIsSlatePresent = false;
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
		wsprintf(G_cTxt, "TSearch Slate Hack: (%s) Player: (%s) - creating slates without correct item!", m_pClientList[iClientH]->m_cIPaddress, m_pClientList[iClientH]->m_cCharName);
		PutHackLogFileList(G_cTxt);
		//DeleteClient(iClientH, true, true);
		return;
	}

	// Are all 4 slates present ??
	if (ctr[0] != 1 || ctr[1] != 1 || ctr[2] != 1 || ctr[3] != 1) {
		bIsSlatePresent = false;
		return;
	}

	if (m_pClientList[iClientH]->m_iAdminUserLevel > 3) bIsSlatePresent = true;

	// if we failed, kill everything
	if (!bIsSlatePresent) {
		for (i = 0; i < 4; i++) {
			if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != 0) {
				ItemDepleteHandler(iClientH, cItemID[i], false, false);
			}
		}
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_CREATEFAIL, 0, 0, 0, 0);
		return;
	}

	// make the slates
	for (i = 0; i < 4; i++) {
		if (m_pClientList[iClientH]->m_pItemList[cItemID[i]] != 0) {
			ItemDepleteHandler(iClientH, cItemID[i], false, false);
		}
	}

	pItem = new class CItem;

	i = iDice(1, 1000);

	if (i < 50) { // Hp slate
		iSlateType = 1;
		cSlateColour = 32;
	}
	else if (i >= 50 && i < 250) { // Bezerk slate
		iSlateType = 2;
		cSlateColour = 3;
	}
	else if (i >= 250 && i < 750) { // Exp slate
		iSlateType = 4;
		cSlateColour = 7;
	}
	else if (i >= 750 && i < 950) { // Mana slate
		iSlateType = 3;
		cSlateColour = 37;
	}
	else if (i >= 950) { // Hp slate
		iSlateType = 1;
		cSlateColour = 32;
	}

	// Notify client
	SendNotifyMsg(0, iClientH, DEF_NOTIFY_SLATE_CREATESUCCESS, iSlateType, 0, 0, 0);

	ZeroMemory(cData, sizeof(cData));

	// Create slates
	if (_bInitItemAttr(pItem, 867) == false) {
		delete pItem;
		return;
	}
	else {
		pItem->m_sTouchEffectType = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = iDice(1, 100000);
		pItem->m_sTouchEffectValue2 = iDice(1, 100000);
		pItem->m_sTouchEffectValue3 = (short)timeGetTime();

		_bItemLog(DEF_ITEMLOG_GET, iClientH, -1, pItem);

		pItem->m_sItemSpecEffectValue2 = iSlateType;
		pItem->m_cItemColor = cSlateColour;
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
			SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

			if (iEraseReq == 1) delete pItem;
		}
		else {
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem);
			/*SendEventToNearClient_TypeB(MSGID_MAGICCONFIGURATIONCONTENTS, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame,
				pItem->m_cItemColor);*/

			SendEventToNearClient_TypeB(MSGID_MAGICCONFIGURATIONCONTENTS, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, pItem->m_sIDnum, pItem->m_sSpriteFrame,
				pItem->m_cItemColor, pItem->m_dwAttribute);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
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
}

void CGame::SetSlateFlag(int iClientH, short sType, bool bFlag)
{
	if (m_pClientList[iClientH] == 0) return;

	if (sType == DEF_NOTIFY_SLATECLEAR) {
		m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
		m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
		m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
		return;
	}

	if (bFlag == true) {
		if (sType == 1) { // Invincible slate
			m_pClientList[iClientH]->m_iStatus |= 0x400000;
		}
		else if (sType == 3) { // Mana slate
			m_pClientList[iClientH]->m_iStatus |= 0x800000;
		}
		else if (sType == 4) { // Exp slate
			m_pClientList[iClientH]->m_iStatus |= 0x10000;
		}
	}
	else {
		if ((m_pClientList[iClientH]->m_iStatus & 0x400000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFFBFFFFF;
		}
		else if ((m_pClientList[iClientH]->m_iStatus & 0x800000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFF7FFFFF;
		}
		else if ((m_pClientList[iClientH]->m_iStatus & 0x10000) != 0) {
			m_pClientList[iClientH]->m_iStatus &= 0xFFFEFFFF;
		}
	}

	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
}

// v2.16 2002-5-21 掳铆卤陇脟枚 脙脽掳隆
bool CGame::bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, bool bBonus)
{
	int iValue, iProb;

	if (m_pClientList[iClientH]->m_pItemList[iSomH] == 0) return false;
	iValue = (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x0F0000000) >> 28;

	switch (iValue) {
	case 0: iProb = 30; break;  // +1 :90%     +1~+2
	case 1: iProb = 25; break;  // +2 :80%      +3
	case 2: iProb = 20; break;  // +3 :48%      +4 
	case 3: iProb = 15; break;  // +4 :24%      +5
	case 4: iProb = 10; break;  // +5 :9.6%     +6
	case 5: iProb = 10; break;  // +6 :2.8%     +7
	case 6: iProb = 8; break;  // +7 :0.57%    +8
	case 7: iProb = 8; break;  // +8 :0.05%    +9
	case 8: iProb = 5; break;  // +9 :0.004%   +10
	case 9: iProb = 3; break;  // +10:0.00016%
	default: iProb = 1; break;
	}

	if (((m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_dwAttribute & 0x00000001) != 0) && (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 > 100)) {
		if (iProb > 20)
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 10);
		else if (iProb > 7)
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 20);
		else
			iProb += (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sItemSpecEffectValue2 / 40);
	}
	if (bBonus == true) iProb *= 2;

	iProb *= 100;


	if (iProb >= iDice(1, 10000)) {
		_bItemLog(DEF_ITEMLOG_UPGRADESUCCESS, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);
		return true;
	}

	_bItemLog(DEF_ITEMLOG_UPGRADEFAIL, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[iItemIndex]);

	return false;
}

/*********************************************************************************************************************
**  bool CGame::_bDecodeItemConfigFileContents(char * pData, DWORD dwMsgSize)										**
**  DESCRIPTION			:: decodes Item.cfg file																	**
**  LAST_UPDATED		:: March 17, 2005; 2:09 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	n/a																						**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
bool CGame::_bDecodeItemConfigFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iItemConfigListIndex, iTemp;
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
					// m_sIDnum
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemIDnumber");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iItemConfigListIndex = atoi(token);
					if (m_pItemConfigList[iItemConfigListIndex] != 0) {
						wsprintf(cTxt, "(!!!) CRITICAL ERROR! Duplicate ItemIDnum(%d)", iItemConfigListIndex);
						PutLogList(cTxt);
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex] = new class CItem;
					m_pItemConfigList[iItemConfigListIndex]->m_sIDnum = iItemConfigListIndex;
					cReadModeB = 2;
					break;
				case 2:
					// m_cName 
					ZeroMemory(m_pItemConfigList[iItemConfigListIndex]->m_cName, sizeof(m_pItemConfigList[iItemConfigListIndex]->m_cName));
					memcpy(m_pItemConfigList[iItemConfigListIndex]->m_cName, token, strlen(token));
					cReadModeB = 3;
					break;
				case 3:
					// m_cItemType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemType");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cItemType = atoi(token);
					cReadModeB = 4;
					break;
				case 4:
					// m_cEquipPos
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - EquipPos");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cEquipPos = atoi(token);
					cReadModeB = 5;
					break;
				case 5:
					// m_sItemEffectType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectType");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectType = atoi(token);
					cReadModeB = 6;
					break;
				case 6:
					// m_sItemEffectValue1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue1");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue1 = atoi(token);
					cReadModeB = 7;
					break;
				case 7:
					// m_sItemEffectValue2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue2");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue2 = atoi(token);
					cReadModeB = 8;
					break;
				case 8:
					// m_sItemEffectValue3
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue3");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue3 = atoi(token);
					cReadModeB = 9;
					break;
				case 9:
					// m_sItemEffectValue4
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue4");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue4 = atoi(token);
					cReadModeB = 10;
					break;
				case 10:
					// m_sItemEffectValue5
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue5");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue5 = atoi(token);
					cReadModeB = 11;
					break;
				case 11:
					// m_sItemEffectValue6
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ItemEffectValue6");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sItemEffectValue6 = atoi(token);
					cReadModeB = 12;
					break;
				case 12:
					// m_wMaxLifeSpan
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxLifeSpan");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_wMaxLifeSpan = (WORD)atoi(token);
					cReadModeB = 13;
					break;
				case 13:
					// m_sSpecialEffect
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - MaxFixCount");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffect = atoi(token);
					cReadModeB = 14;
					break;
				case 14:
					// m_sSprite
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Sprite");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSprite = atoi(token);
					cReadModeB = 15;
					break;
				case 15:
					// m_sSpriteFrame
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - SpriteFrame");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpriteFrame = atoi(token);
					cReadModeB = 16;
					break;
				case 16:
					// m_wPrice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Price");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = false;
					else m_pItemConfigList[iItemConfigListIndex]->m_bIsForSale = true;

					m_pItemConfigList[iItemConfigListIndex]->m_wPrice = abs(iTemp);
					cReadModeB = 17;
					break;
				case 17:
					// m_wWeight
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Weight");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_wWeight = atoi(token);
					cReadModeB = 18;
					break;
				case 18:
					// Appr Value
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - ApprValue");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cApprValue = atoi(token);
					cReadModeB = 19;
					break;
				case 19:
					// m_cSpeed
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Speed");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cSpeed = atoi(token);
					cReadModeB = 20;
					break;

				case 20:
					// m_sLevelLimit
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - LevelLimit");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sLevelLimit = atoi(token);
					cReadModeB = 21;
					break;

				case 21:
					// m_cGederLimit
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - GenderLimit");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cGenderLimit = atoi(token);
					cReadModeB = 22;
					break;

				case 22:
					// m_sSpecialEffectValue1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - SM_HitRatio");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue1 = atoi(token);
					cReadModeB = 23;
					break;

				case 23:
					// m_sSpecialEffectValue2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - L_HitRatio");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sSpecialEffectValue2 = atoi(token);
					cReadModeB = 24;
					break;

				case 24:
					// m_sRelatedSkill
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - RelatedSkill");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_sRelatedSkill = atoi(token);
					cReadModeB = 25;
					break;

				case 25:
					// m_cCategory
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cCategory = atoi(token);
					cReadModeB = 26;
					break;

				case 26:
					// m_cItemColor
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Category");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					m_pItemConfigList[iItemConfigListIndex]->m_cItemColor = atoi(token);
					//cReadModeA = 0;
					cReadModeB = 27;
					break;

				//Magn0S:: New variables for trades
				case 27:
					// m_wContribPrice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Contrib Cost");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->bContrbSale = false;
					else m_pItemConfigList[iItemConfigListIndex]->bContrbSale = true;

					m_pItemConfigList[iItemConfigListIndex]->m_wContribPrice = abs(iTemp);
					cReadModeB = 28;
					break;
				case 28:
					// m_wEkPrice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Ek Cost");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->bEkSale = false;
					else m_pItemConfigList[iItemConfigListIndex]->bEkSale = true;

					m_pItemConfigList[iItemConfigListIndex]->m_wEkPrice = abs(iTemp);
					cReadModeB = 29;
					break;
				case 29:
					// m_wCoinPrice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Coin Cost");
						delete[] pContents;
						delete pStrTok;
						return false;
					}
					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->bCoinSale = false;
					else m_pItemConfigList[iItemConfigListIndex]->bCoinSale = true;

					m_pItemConfigList[iItemConfigListIndex]->m_wCoinPrice = abs(iTemp);
					cReadModeB = 30;
					break;

				// Centuu - class
				case 30:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Class");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->m_iClass = 0;
					else m_pItemConfigList[iItemConfigListIndex]->m_iClass = iTemp;
					
					cReadModeB = 31;
					break;

				case 31:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Req. Stat");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->m_iReqStat = 0;
					else m_pItemConfigList[iItemConfigListIndex]->m_iReqStat = iTemp;

					cReadModeB = 32;
					break;
				case 32:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Quant. Stat");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					iTemp = atoi(token);
					if (iTemp < 0)
						m_pItemConfigList[iItemConfigListIndex]->m_iQuantStat = 0;
					else m_pItemConfigList[iItemConfigListIndex]->m_iQuantStat = iTemp;

					
					cReadModeB = 33;
					break;

				case 33:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Is Hero");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					iTemp = atoi(token);
					if (iTemp <= 0)
						m_pItemConfigList[iItemConfigListIndex]->m_bIsHero = false;
					else m_pItemConfigList[iItemConfigListIndex]->m_bIsHero = true;

					cReadModeB = 34;
					break;

				case 34:
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file error - Hero Bonus");
						delete[] pContents;
						delete pStrTok;
						return false;
					}

					iTemp = atoi(token);
					if (iTemp <= 0)
						m_pItemConfigList[iItemConfigListIndex]->m_iHeroBonus = 0;
					else m_pItemConfigList[iItemConfigListIndex]->m_iHeroBonus = iTemp;

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
			if (memcmp(token, "Item", 4) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}
			if (memcmp(token, "[ENDITEMLIST]", 13) == 0) {
				cReadModeA = 0;
				cReadModeB = 0;
				break;
			}
		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;
	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) CRITICAL ERROR! ITEM configuration file contents error!");
		return false;
	}
	wsprintf(cTxt, "(!) ITEM(Total:%d) configuration - success!", iItemConfigListIndex);
	PutLogList(cTxt);
	m_bReceivedItemList = true;
	return true;
}

/*********************************************************************************************************************
**  bool CGame::_bInitItemAttr(class CItem * pItem, char * pItemName)												**
**  DESCRIPTION			:: initializes item variables - by ItemName													**
**  LAST_UPDATED		:: March 17, 2005; 2:10 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	- overloaded function - see other _bInitItemAttr										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
bool CGame::_bInitItemAttr(class CItem* pItem, char* pItemName)
{
	int i;
	char cTmpName[22];

	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pItemName);

	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		if (m_pItemConfigList[i] != 0) {
			if (memcmp(cTmpName, m_pItemConfigList[i]->m_cName, 20) == 0) {
				ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
				strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
				pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
				pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
				pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
				pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
				pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
				pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
				pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
				pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
				pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
				pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_wCurLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;
				pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
				pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
				pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
				pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
				pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
				pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
				pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
				pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;
				pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
				pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;
				pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
				pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
				pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;
				pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
				pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
				//Magn0S:: New itens variables--------------------------------------
				pItem->m_wContribPrice = m_pItemConfigList[i]->m_wContribPrice;
				pItem->m_wEkPrice = m_pItemConfigList[i]->m_wEkPrice;
				pItem->m_wCoinPrice = m_pItemConfigList[i]->m_wCoinPrice;
				pItem->bEkSale = m_pItemConfigList[i]->bEkSale;
				pItem->bContrbSale = m_pItemConfigList[i]->bContrbSale;
				pItem->bCoinSale = m_pItemConfigList[i]->bCoinSale;
				pItem->m_sNewEffect1 = m_pItemConfigList[i]->m_sNewEffect1;
				pItem->m_sNewEffect2 = m_pItemConfigList[i]->m_sNewEffect2;
				pItem->m_sNewEffect3 = m_pItemConfigList[i]->m_sNewEffect3;
				pItem->m_sNewEffect4 = m_pItemConfigList[i]->m_sNewEffect4;

				pItem->m_iClass = m_pItemConfigList[i]->m_iClass;

				pItem->m_iReqStat = m_pItemConfigList[i]->m_iReqStat;
				pItem->m_iQuantStat = m_pItemConfigList[i]->m_iQuantStat;
				return true;
			}
		}
	return false;
}

/*********************************************************************************************************************
**  bool CGame::_bInitItemAttr(class CItem * pItem, char * pItemName)												**
**  DESCRIPTION			:: initializes item variables - by ItemID													**
**  LAST_UPDATED		:: March 17, 2005; 2:10 PM; Hypnotoad														**
**	RETURN_VALUE		:: bool																						**
**  NOTES				::	- overloaded function - see other _bInitItemAttr										**
**	MODIFICATION		::	n/a																						**
**********************************************************************************************************************/
bool CGame::_bInitItemAttr(class CItem* pItem, int iItemID)
{
	int i;

	for (i = 0; i < DEF_MAXITEMTYPES; i++)
		if (m_pItemConfigList[i] != 0) {
			if (m_pItemConfigList[i]->m_sIDnum == iItemID) {
				ZeroMemory(pItem->m_cName, sizeof(pItem->m_cName));
				strcpy(pItem->m_cName, m_pItemConfigList[i]->m_cName);
				pItem->m_cItemType = m_pItemConfigList[i]->m_cItemType;
				pItem->m_cEquipPos = m_pItemConfigList[i]->m_cEquipPos;
				pItem->m_sItemEffectType = m_pItemConfigList[i]->m_sItemEffectType;
				pItem->m_sItemEffectValue1 = m_pItemConfigList[i]->m_sItemEffectValue1;
				pItem->m_sItemEffectValue2 = m_pItemConfigList[i]->m_sItemEffectValue2;
				pItem->m_sItemEffectValue3 = m_pItemConfigList[i]->m_sItemEffectValue3;
				pItem->m_sItemEffectValue4 = m_pItemConfigList[i]->m_sItemEffectValue4;
				pItem->m_sItemEffectValue5 = m_pItemConfigList[i]->m_sItemEffectValue5;
				pItem->m_sItemEffectValue6 = m_pItemConfigList[i]->m_sItemEffectValue6;
				pItem->m_wMaxLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_wCurLifeSpan = m_pItemConfigList[i]->m_wMaxLifeSpan;
				pItem->m_sSpecialEffect = m_pItemConfigList[i]->m_sSpecialEffect;
				pItem->m_sSprite = m_pItemConfigList[i]->m_sSprite;
				pItem->m_sSpriteFrame = m_pItemConfigList[i]->m_sSpriteFrame;
				pItem->m_wPrice = m_pItemConfigList[i]->m_wPrice;
				pItem->m_wWeight = m_pItemConfigList[i]->m_wWeight;
				pItem->m_cApprValue = m_pItemConfigList[i]->m_cApprValue;
				pItem->m_cSpeed = m_pItemConfigList[i]->m_cSpeed;
				pItem->m_sLevelLimit = m_pItemConfigList[i]->m_sLevelLimit;
				pItem->m_cGenderLimit = m_pItemConfigList[i]->m_cGenderLimit;
				pItem->m_sSpecialEffectValue1 = m_pItemConfigList[i]->m_sSpecialEffectValue1;
				pItem->m_sSpecialEffectValue2 = m_pItemConfigList[i]->m_sSpecialEffectValue2;
				pItem->m_sRelatedSkill = m_pItemConfigList[i]->m_sRelatedSkill;
				pItem->m_cCategory = m_pItemConfigList[i]->m_cCategory;
				pItem->m_sIDnum = m_pItemConfigList[i]->m_sIDnum;
				pItem->m_bIsForSale = m_pItemConfigList[i]->m_bIsForSale;
				pItem->m_cItemColor = m_pItemConfigList[i]->m_cItemColor;
				//Magn0S:: New itens variables
				pItem->m_wContribPrice = m_pItemConfigList[i]->m_wContribPrice;
				pItem->m_wEkPrice = m_pItemConfigList[i]->m_wEkPrice;
				pItem->m_wCoinPrice = m_pItemConfigList[i]->m_wCoinPrice;
				pItem->bEkSale = m_pItemConfigList[i]->bEkSale;
				pItem->bContrbSale = m_pItemConfigList[i]->bContrbSale;
				pItem->bCoinSale = m_pItemConfigList[i]->bCoinSale;
				pItem->m_sNewEffect1 = m_pItemConfigList[i]->m_sNewEffect1;
				pItem->m_sNewEffect2 = m_pItemConfigList[i]->m_sNewEffect2;
				pItem->m_sNewEffect3 = m_pItemConfigList[i]->m_sNewEffect3;
				pItem->m_sNewEffect4 = m_pItemConfigList[i]->m_sNewEffect4;

				pItem->m_iClass = m_pItemConfigList[i]->m_iClass;

				pItem->m_iReqStat = m_pItemConfigList[i]->m_iReqStat;
				pItem->m_iQuantStat = m_pItemConfigList[i]->m_iQuantStat;

				return true;
			}
		}
	return false;
}

int CGame::_iGetItemSpaceLeft(int iClientH)
{
	int i, iTotalItem;

	iTotalItem = 0;
	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] != 0) iTotalItem++;

	return (DEF_MAXITEMS - iTotalItem);
}

bool CGame::bAddItem(int iClientH, CItem* pItem, char cMode)
{
	char* cp, cData[256];
	DWORD* dwp;
	WORD* wp;
	short* sp;
	int iRet, iEraseReq, *ip;


	ZeroMemory(cData, sizeof(cData));
	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
		// 戮脝脌脤脜脹脌禄 脠鹿碌忙脟脽麓脵.
		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_ITEMOBTAINED;

		cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

		// 1掳鲁 脠鹿碌忙脟脽麓脵. Amount掳隆 戮脝麓脧麓脵!
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;	// 录枚路庐脌禄 脌脭路脗 
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0; // 戮貌脌潞 戮脝脌脤脜脹脌脤鹿脟路脦 脌氓脗酶碌脟脕枚 戮脢戮脪麓脵.
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		//Magn0S::------------------------------------
		sp = (short*)cp;
		*sp = pItem->m_sNewEffect1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue5;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue6;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectType;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wMaxLifeSpan;
		cp += 2;

		ip = (int*)cp;
		*ip = pItem->m_iClass;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iReqStat;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iQuantStat;
		cp += 4;

		if (iEraseReq == 1) {
			delete pItem;
			pItem = 0;
		}

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 81+8); //Original = 53

		return true;
	}
	else {
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY,
			pItem);

		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ //v1.4 color

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute);

		dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
		*dwp = MSGID_NOTIFY;
		wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
		*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);

		return true;
	}

	return false;
}

void CGame::SendItemNotifyMsg(int iClientH, WORD wMsgType, CItem* pItem, int iV1)
{
	char* cp, cData[512];
	DWORD* dwp;
	WORD* wp;
	short* sp;
	int     iRet, *ip;

	if (m_pClientList[iClientH] == 0) return;

	dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_NOTIFY;
	wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = wMsgType;
	cp = (char*)(cData + DEF_INDEX2_MSGTYPE + 2);

	switch (wMsgType) {
	case DEF_NOTIFY_ITEMOBTAINED:
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0;
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor; // v1.4
		cp++;

		*cp = (char)pItem->m_sItemSpecEffectValue2; // v1.41 
		cp++;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		//------------------------------------
		sp = (short*)cp;
		*sp = pItem->m_sNewEffect1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue5;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue6;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectType;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wMaxLifeSpan;
		cp += 2;

		ip = (int*)cp;
		*ip = pItem->m_iClass;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iReqStat;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iQuantStat;
		cp += 4;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 81+8); // 53
		break;

	case DEF_NOTIFY_ITEMPURCHASED:
		*cp = 1;
		cp++;

		memcpy(cp, pItem->m_cName, 20);
		cp += 20;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwCount;
		cp += 4;

		*cp = pItem->m_cItemType;
		cp++;

		*cp = pItem->m_cEquipPos;
		cp++;

		*cp = (char)0;
		cp++;

		sp = (short*)cp;
		*sp = pItem->m_sLevelLimit;
		cp += 2;

		*cp = pItem->m_cGenderLimit;
		cp++;

		wp = (WORD*)cp;
		*wp = pItem->m_wCurLifeSpan;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wWeight;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSprite;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sSpriteFrame;
		cp += 2;

		*cp = pItem->m_cItemColor;
		cp++;

		ip = (int*)cp;
		*ip = iV1;
		cp += 4;

		//------------------------------------
		sp = (short*)cp;
		*sp = pItem->m_sNewEffect1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sNewEffect4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue1;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue2;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue3;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue4;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue5;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectValue6;
		cp += 2;

		sp = (short*)cp;
		*sp = pItem->m_sItemEffectType;
		cp += 2;

		wp = (WORD*)cp;
		*wp = pItem->m_wMaxLifeSpan;
		cp += 2;

		ip = (int*)cp;
		*ip = pItem->m_iClass;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iReqStat;
		cp += 4;

		ip = (int*)cp;
		*ip = pItem->m_iQuantStat;
		cp += 4;

		dwp = (DWORD*)cp;
		*dwp = pItem->m_dwAttribute;
		cp += 4;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 78+8+4); // 48
		break;

	case DEF_NOTIFY_CANNOTCARRYMOREITEM:
		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
		break;
	}
}

bool CGame::_bCheckItemReceiveCondition(int iClientH, CItem* pItem)
{
	int i;

	if (m_pClientList[iClientH] == 0) return false;


	if (m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount) > (DWORD)_iCalcMaxLoad(iClientH))
		return false;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == 0) return true;

	return false;
}

// New 07/05/2004
void CGame::DropItemHandler(int iClientH, short sItemIndex, int iAmount, char* pItemName, bool bByPlayer)
{
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsOnServerChange == true) return;
	if ((m_bAdminSecurity == true) && (m_pClientList[iClientH]->m_iAdminUserLevel > 0 && m_pClientList[iClientH]->m_iAdminUserLevel < 7)) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
	if ((iAmount != -1) && (iAmount < 0)) return;

	if (m_pClientList[iClientH]->IsInMap("team") || m_pClientList[iClientH]->IsInMap("fightzone1"))
		return;

	// Amount掳隆 -1脌脤掳铆 录脪赂冒脟掳脌脤赂茅 录枚路庐脌脟 脌眉潞脦赂娄 露鲁戮卯露脽赂掳麓脵.
	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(iAmount == -1))
		iAmount = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount;

	// 戮脝脌脤脜脹 脌脤赂搂脌脤 脌脧脛隆脟脧脕枚 戮脢戮脝碌碌 鹿芦陆脙 
	if (memcmp(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName, pItemName, 20) != 0) return;

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) &&
		(((int)m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount - iAmount) > 0)) {
		// 录脪潞帽录潞 戮脝脌脤脜脹脌脤戮煤掳铆 录枚路庐赂赂脜颅 掳篓录脪陆脙脜掳掳铆 鲁虏脌潞 掳脭 脌脰麓脵赂茅 
		pItem = new class CItem;
		if (_bInitItemAttr(pItem, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cName) == false) {
			// 潞脨脟脪脟脧掳铆脌脷 脟脧麓脗 戮脝脌脤脜脹脌脤 赂庐陆潞脝庐驴隆 戮酶麓脗 掳脜麓脵. 脌脤路卤 脌脧脌潞 脌脧戮卯鲁炉 录枚掳隆 戮酶脕枚赂赂 
			delete pItem;
			return;
		}
		else {
			if (iAmount <= 0) {
				// 戮脝脌脤脜脹脌脟 录枚路庐脌脤 0潞赂麓脵 脌脹脌赂赂茅 驴隆路炉禄贸脠虏. 赂庐脜脧 
				delete pItem;
				return;
			}
			pItem->m_dwCount = (DWORD)iAmount;
		}

		// 录枚路庐 掳篓录脪 

		// 驴隆路炉. 掳篓录脪陆脙脜掳掳铆脌脷 脟脧麓脗 戮莽脌脤 麓玫 赂鹿麓脵. 
		if ((DWORD)iAmount > m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount) {
			delete pItem;
			return;
		}

		m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount -= iAmount;

		// 潞炉掳忙碌脠 录枚路庐脌禄 录鲁脕陇脟脧掳铆 戮脣赂掳麓脵.
		// v1.41 !!!
		SetItemCount(iClientH, sItemIndex, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwCount);

		// 戮脝脌脤脜脹脌禄 录颅脌脰麓脗 脌搂脛隆驴隆 鹿枚赂掳麓脵. 
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
			m_pClientList[iClientH]->m_sY, pItem);

		// v1.411 脠帽卤脥 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌脦脕枚 脙录脜漏  
		// v2.17 2002-7-31 脟脙路鹿脌脤戮卯掳隆 脕脳戮卯录颅 露鲁戮卯脕酶掳脟 赂冒碌莽 路脦卤脳掳隆 鲁虏麓脗麓脵. 
		if (bByPlayer == true)
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem);
		else
			_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, pItem, true);

		// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
		/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);*/ // v1.4 color

		SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
			m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
			pItem->m_sIDnum, pItem->m_sSpriteFrame, pItem->m_cItemColor, pItem->m_dwAttribute); 

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, sItemIndex, iAmount, 0, 0);
	}
	else {
		// 脌脧鹿脻 戮脝脌脤脜脹脌脤鲁陋 录脪潞帽录潞 戮脝脌脤脜脹脌禄 赂冒碌脦 鹿枚路脠麓脵.

		// 赂脮脌煤 脌氓脗酶碌脟戮卯 脌脰麓脵赂茅 脟脴脕娄陆脙脜虏麓脵.

		ReleaseItemHandler(iClientH, sItemIndex, true);

		// v2.17 戮脝脌脤脜脹脌脤 脌氓脗酶碌脟戮卯 脌脰脌赂赂茅 脟脴脕娄脟脩麓脵.
		if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == true)
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);

		// v1.432
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) &&
			(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0)) {
			// 脠帽禄媒脌脟 碌鹿脌脦 掳忙驴矛 录枚赂铆脌脤 0脌脦 禄贸脜脗路脦 露鲁戮卯脕枚赂茅 禄莽露贸脕酶麓脵.
			delete m_pClientList[iClientH]->m_pItemList[sItemIndex];
			m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
		}
		else {
			// 戮脝脌脤脜脹脌禄 录颅脌脰麓脗 脌搂脛隆驴隆 鹿枚赂掳麓脵. 
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(m_pClientList[iClientH]->m_sX,
				m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]);

			// v1.41 脠帽卤脥 戮脝脌脤脜脹脌禄 露鲁戮卯露脽赂掳 掳脥脌脤露贸赂茅 路脦卤脳赂娄 鲁虏卤盲麓脵. 
			// v2.17 2002-7-31 脟脙路鹿脌脤戮卯掳隆 脕脳戮卯录颅 露鲁戮卯脕酶掳脟 赂冒碌莽 路脦卤脳掳隆 鲁虏麓脗麓脵. 
			if (bByPlayer == true)
				_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex]);
			else
				_bItemLog(DEF_ITEMLOG_DROP, iClientH, (int)-1, m_pClientList[iClientH]->m_pItemList[sItemIndex], true);

			// 麓脵赂楼 脜卢露贸脌脤戮冒脝庐驴隆掳脭 戮脝脌脤脜脹脌脤 露鲁戮卯脕酶 掳脥脌禄 戮脣赂掳麓脵. 
			/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSprite,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor);*/ //v1.4 color

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_ITEMDROP, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpriteFrame,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor,
				m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute);
		}

		// 戮脝脌脤脜脹脌禄 delete脟脧脕枚 戮脢掳铆 NULL路脦 脟脪麓莽脟脩麓脵. delete 脟脧脕枚 戮脢麓脗 脌脤脌炉麓脗 鹿脵麓脷驴隆 露鲁戮卯脕庐 脌脰卤芒 露搂鹿庐 
		m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
		m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;

		// 脌脤脕娄 戮脝脌脤脜脹脌禄 赂庐陆潞脝庐驴隆录颅 禄猫脕娄脟脪掳脥脌禄 脜毛潞赂脟脩麓脵.
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, sItemIndex, iAmount, 0, 0);

		// 脌脦碌娄陆潞掳隆 鹿脵虏卯戮煤脌赂鹿脟路脦 脌莽脟脪麓莽
		m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);
	}

	// 录脪脕枚脟掳 脙脩 脕脽路庐 脌莽 掳猫禄锚 
	iCalcTotalWeight(iClientH);

}

/////////////////////////////////////////////////////////////////////////////////////
//  int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
//  description			:: check if player is dropping item or picking up item
//  last updated		:: October 29, 2004; 7:12 PM; Hypnotoad
//	return value		:: int
/////////////////////////////////////////////////////////////////////////////////////
int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)
{
	DWORD* dwp, dwRemainItemAttr;
	WORD* wp;
	short sRemainItemID;
	char  cRemainItemColor, cData[100];
	int   iRet, iEraseReq;
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return 0;
	if ((cDir <= 0) || (cDir > 8))       return 0;
	if (m_pClientList[iClientH]->m_bIsKilled == true) return 0;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return 0;

	if ((sX != m_pClientList[iClientH]->m_sX) || (sY != m_pClientList[iClientH]->m_sY)) return 2;

	int iStX, iStY;
	if (m_pMapList[m_pClientList[iClientH]->m_cMapIndex] != 0) {
		iStX = m_pClientList[iClientH]->m_sX / 20;
		iStY = m_pClientList[iClientH]->m_sY / 20;
		m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iPlayerActivity++;

		switch (m_pClientList[iClientH]->m_cSide) {
		case 0: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iNeutralActivity++; break;
		case 1: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iAresdenActivity++; break;
		case 2: m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iElvineActivity++;  break;
		}
	}

	ClearSkillUsingStatus(iClientH);

	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->ClearOwner(0, iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);
	m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->SetOwner(iClientH, DEF_OWNERTYPE_PLAYER, sX, sY);

	//pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(sX, sY, &sRemainItemSprite, &sRemainItemSpriteFrame, &cRemainItemColor);
	pItem = m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->pGetItem(sX, sY, &sRemainItemID/*, &sRemainItemSpriteFrame*/, &cRemainItemColor, &dwRemainItemAttr); // v1.4
	if (pItem != 0) {
		if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
			_bItemLog(DEF_ITEMLOG_GET, iClientH, 0, pItem);
			SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

			if (iEraseReq == 1) delete pItem;

			/*SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				sRemainItemSprite, sRemainItemSpriteFrame, cRemainItemColor);*/

			SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_SETITEM, m_pClientList[iClientH]->m_cMapIndex,
				m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY,
				sRemainItemID, 0, cRemainItemColor, dwRemainItemAttr);
		}
		else
		{
			m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bSetItem(sX, sY, pItem);

			dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, true, true);
				return 0;
			}
		}
	}

	dwp = (DWORD*)(cData + DEF_INDEX4_MSGID);
	*dwp = MSGID_RESPONSE_MOTION;
	wp = (WORD*)(cData + DEF_INDEX2_MSGTYPE);
	*wp = DEF_OBJECTMOTION_CONFIRM;

	iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cData, 6);
	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true);
		return 0;
	}

	return 1;
}

bool CGame::_bAddClientItemList(int iClientH, class CItem* pItem, int* pDelReq)
{
	int i;

	if (m_pClientList[iClientH] == 0) return false;
	if (pItem == 0) return false;

	// 脕媒脌禄 戮脝脌脤脜脹 脕脽路庐掳猫禄锚  
	if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)) {
		// 录枚路庐掳鲁鲁盲脌脤 脌脰麓脗 戮脝脌脤脜脹 
		if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, pItem->m_dwCount)) > (DWORD)_iCalcMaxLoad(iClientH))
			return false;
	}
	else {
		// 录枚路庐 掳鲁鲁盲脌脤 戮酶麓脗 戮脝脌脤脜脹 
		if ((m_pClientList[iClientH]->m_iCurWeightLoad + iGetItemWeight(pItem, 1)) > (DWORD)_iCalcMaxLoad(iClientH))
			return false;
	}

	// 录脪潞帽录潞 戮脝脌脤脜脹脌禄 脌脤鹿脤 录脪脕枚脟脧掳铆 脌脰麓脵赂茅 录枚路庐赂赂 脕玫掳隆陆脙脜虏麓脵. 
	if ((pItem->m_cItemType == DEF_ITEMTYPE_CONSUME) || (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)) {
		for (i = 0; i < DEF_MAXITEMS; i++)
			if ((m_pClientList[iClientH]->m_pItemList[i] != 0) &&
				(memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, pItem->m_cName, 20) == 0)) {
				// 掳掳脌潞 脌脤赂搂脌禄 脙拢戮脪麓脵. 
				m_pClientList[iClientH]->m_pItemList[i]->m_dwCount += pItem->m_dwCount;

				*pDelReq = 1;

				// 录脪脕枚脟掳 脙脩 脕脽路庐 脌莽 掳猫禄锚 
				iCalcTotalWeight(iClientH);

				return true;
			}
	}

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == 0) {

			m_pClientList[iClientH]->m_pItemList[i] = pItem;
			// v1.3 戮脝脌脤脜脹脌脟 脌搂脛隆麓脗 鹿芦脕露掳脟 100, 100
			m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
			m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

			*pDelReq = 0;

			// 赂赂戮脿 脠颅禄矛路霉 戮脝脌脤脜脹脌脤露贸赂茅 脠颅禄矛脌禄 脟脪麓莽脟脩麓脵. 
			if (pItem->m_cItemType == DEF_ITEMTYPE_ARROW)
				m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

			// 录脪脕枚脟掳 脙脩 脕脽路庐 脌莽 掳猫禄锚 
			iCalcTotalWeight(iClientH);

			return true;
		}

	// 麓玫脌脤禄贸 戮脝脌脤脜脹脌禄 脕媒脌禄 掳酶掳拢脌脤 戮酶麓脵.
	return false;
}

bool CGame::bEquipItemHandler(int iClientH, short sItemIndex, bool bNotify)
{
	char  cEquipPos;
	short   sSpeed;
	short sTemp;
	int iTemp;

	if (m_pClientList[iClientH] == 0) return false;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return false;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return false;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return false;

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_wCurLifeSpan <= 0) return false;

	if ((m_pMapList[m_pClientList[iClientH]->m_cMapIndex]->bMapEquip == false) && (m_pClientList[iClientH]->m_iAdminUserLevel == 0)) {
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_IPACCOUNTINFO, 0, 0, 0, "Equip Items is not allowed in this Map.");
		return false;
	}

	// Centuu - class
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iClass > 0)
	{
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iClass != m_pClientList[iClientH]->m_iClass)
		{
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
			ReleaseItemHandler(iClientH, sItemIndex, true);
			return false;
		}
	}

	if (((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_dwAttribute & 0x00000001) == 0) &&
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sLevelLimit > m_pClientList[iClientH]->m_iLevel)) return false;

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 0) {
		switch (m_pClientList[iClientH]->m_sType) {
		case 1:
		case 2:
		case 3:
			// 鲁虏录潞脌脤麓脵.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 1) return false;
			break;
		case 4:
		case 5:
		case 6:
			// 驴漏录潞脌脤麓脵.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cGenderLimit != 2) return false;
			break;
		}
	}

	//if (iGetItemWeight(m_pClientList[iClientH]->m_pItemList[sItemIndex], 1) > (m_pClientList[iClientH]->m_iStr + m_pClientList[iClientH]->m_iAngelicStr) * 100) return false;

	cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;

	if (cEquipPos == DEF_EQUIPPOS_NONE) return false;

	if ((cEquipPos != DEF_EQUIPPOS_RFINGER) && (cEquipPos != DEF_EQUIPPOS_BACK) &&
		(cEquipPos != DEF_EQUIPPOS_BOOTS) && (cEquipPos != DEF_EQUIPPOS_LFINGER) &&
		(cEquipPos != DEF_EQUIPPOS_NECK)) {
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iReqStat) {
		case 1: // Str 脕娄脟脩 
			if ((m_pClientList[iClientH]->m_iStr) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		case 2: // Dex
			if ((m_pClientList[iClientH]->m_iDex) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		case 3: // Vit
			if (m_pClientList[iClientH]->m_iVit < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		case 4: // Int
			if ((m_pClientList[iClientH]->m_iInt) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		case 5: // Mag
			if ((m_pClientList[iClientH]->m_iMag) < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		case 6: // Agi
			if (m_pClientList[iClientH]->m_iCharisma < m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) {
				// 脜卢露贸脌脤戮冒脝庐 禄贸驴隆录颅麓脗 脗酶驴毛碌脠 禄贸脜脗脌脤鹿脟路脦 鹿镁掳脺戮脽 脟脩麓脵. 脗酶驴毛脌脤 脟脴脕娄碌脠麓脵.
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos, sItemIndex, 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], true);
				return false;
			}
			break;
		}
	}

	//Magn0S:: Update to put SelfSafe for bmages.
	if ((cEquipPos == DEF_EQUIPPOS_TWOHAND) || (cEquipPos == DEF_EQUIPPOS_RHAND)) {
		m_pClientList[iClientH]->m_bIsSelfSafe = false;
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM) {
			m_pClientList[iClientH]->m_bIsSelfSafe = true;
		}
	}

	if (cEquipPos == DEF_EQUIPPOS_TWOHAND) 
	{
		// Centuu : Fixed las armas Blood by KaoZureS
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 490) { // Sword
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMBLOOD, 0, 0, 0, 0);
			
		}
	}

	if (cEquipPos == DEF_EQUIPPOS_RHAND) 
	{
		// Resurrection wand(MS.10) or Resurrection wand(MS.20)
		if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866)) {
			if (((m_pClientList[iClientH]->m_iInt) >= m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) && ((m_pClientList[iClientH]->m_iMag) >= m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_iQuantStat) && (m_pClientList[iClientH]->m_iSpecialAbilityTime < 1)) {
				m_pClientList[iClientH]->m_cMagicMastery[94] = 1;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_RESUR_ON, 0, 0, 0, 0);
			}
		}
		// Centuu : Fixed las armas Blood by KaoZureS
		else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 491 || m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 492) { // Axe
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMBLOOD, 0, 0, 0, 0);
			
		}
	}

	if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) ||
		(m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY)) {

		if ((m_pClientList[iClientH]->m_iSpecialAbilityType != 0)) {
			// 脌脤鹿脤 脝炉录枚麓脡路脗脌脤 录鲁脕陇碌脟戮卯 脌脰麓脵. 脌脤脌眉驴隆 脌氓脗酶碌脟戮卯 脌脰麓脗 戮脝脌脤脜脹 脗酶驴毛 脟脴脕娄:
			// 麓脺, 脗酶驴毛 潞脦脌搂掳隆 掳掳麓脵赂茅 脟脴脕娄 赂脼陆脙脕枚 潞赂鲁禄脕枚 戮脢麓脗麓脵.
			if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos != m_pClientList[iClientH]->m_iSpecialAbilityEquipPos) {
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iClientH]->m_iSpecialAbilityEquipPos, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], 0, 0);
				// 脟脴麓莽 脗酶驴毛 潞脦脌搂脌脟 戮脝脌脤脜脹脠驴掳煤赂娄 脕娄掳脜.
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[m_pClientList[iClientH]->m_iSpecialAbilityEquipPos], true);
			}
		}
	}

	if (cEquipPos == DEF_EQUIPPOS_TWOHAND) {
		// 戮莽录脮脌禄 禄莽驴毛脟脧麓脗 鹿芦卤芒掳隆 脌氓脗酶碌脟戮卯 脌脰麓脵赂茅  
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
		else {
			// 驴脌赂楼录脮, 驴脼录脮驴隆 戮脝脌脤脜脹脌脤 脌脰麓脵赂茅 赂冒碌脦 脌氓脗酶 脟脴脕娄 
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RHAND], false);
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LHAND], false);
		}
	}
	else {
		// 戮莽录脮 戮脝脌脤脜脹脌脤 脌氓脗酶碌脟戮卯 脌脰麓脵赂茅 脟脴脕娄陆脙脜虏麓脵.
		if ((cEquipPos == DEF_EQUIPPOS_LHAND) || (cEquipPos == DEF_EQUIPPOS_RHAND)) {
			// 鹿芦卤芒鲁陋 鹿忙脝脨路霉露贸赂茅 戮莽录脮鹿芦卤芒赂娄 脟脴脕娄 
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND] != -1)
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_TWOHAND], false);
		}

		// 脌氓脗酶脌搂脛隆驴隆 戮脝脌脤脜脹脌脤 脌脤鹿脤 脌脰脌赂赂茅 禄猫脕娄.
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
	}


	if (cEquipPos == DEF_EQUIPPOS_RELEASEALL) {
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS], false);
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK] != -1) {
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BACK], false);
		}
	}
	else {
		if (cEquipPos == DEF_EQUIPPOS_HEAD || cEquipPos == DEF_EQUIPPOS_BODY || cEquipPos == DEF_EQUIPPOS_ARMS ||
			cEquipPos == DEF_EQUIPPOS_LEGGINGS || cEquipPos == DEF_EQUIPPOS_BOOTS || cEquipPos == DEF_EQUIPPOS_BACK) {
			if (m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL] != -1) {
				ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_RELEASEALL], false);
			}
		}
		if (m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] != -1)
			ReleaseItemHandler(iClientH, m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos], false);
	}


	m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = sItemIndex;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = true;

	switch (cEquipPos) {

	case DEF_EQUIPPOS_HEAD:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFF0F;	// 脜玫卤赂 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFFF0; // 潞脦脙梅 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.				
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor));
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	//case DEF_EQUIPPOS_PANTS:
	case DEF_EQUIPPOS_LEGGINGS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xF0FF;	// 鹿脵脕枚 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFF0FF; // 鹿脵脕枚 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 8);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	//case DEF_EQUIPPOS_LEGGINGS:
	case DEF_EQUIPPOS_BOOTS:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0x0FFF;	// 潞脦脙梅 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFF0F; // 潞脦脙梅 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 4);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BODY:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 掳脩驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.

		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue < 100) {
			// 驴脺脟眉掳陋脌脤 100 脌脤脟脧. 脕陇禄贸脌没脌脦 掳陋 
			sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 掳陋脌禄 录录脝脙. 
			m_pClientList[iClientH]->m_sAppr3 = sTemp;
		}
		else {
			// 驴脺脟眉掳陋脌脤 100 脌脤禄贸脌脤赂茅 脠庐脌氓 脟脙路隆卤脳赂娄 禄莽驴毛脟脩麓脵. 
			sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue - 100) << 12); // Appr 掳陋脌禄 录录脝脙. 
			m_pClientList[iClientH]->m_sAppr3 = sTemp;
			// 掳脩驴脢 脠庐脌氓 脝炉录潞脛隆 潞帽脝庐赂娄 录录脝庐.
			sTemp = m_pClientList[iClientH]->m_sAppr4;
			sTemp = sTemp | 0x080;
			m_pClientList[iClientH]->m_sAppr4 = sTemp;
		}

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFF0FFFFF; // 掳脩驴脢(掳漏驴脢) 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 20);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_ARMS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFFF0;	// 录脫驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue)); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFFFF; // 录脫驴脢 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯. // New 09/05/2004
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 12);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_LHAND:
		// 驴脼录脮驴隆 脌氓脗酶脟脧麓脗 戮脝脌脤脜脹. 鹿忙脝脨路霉脌脤麓脵. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xFFF0;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue)); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xF0FFFFFF; // 鹿忙脝脨 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 24);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RHAND:
		// 驴脌赂楼录脮驴隆 脌氓脗酶脟脧麓脗 戮脝脌脤脜脹. 鹿芦卤芒路霉脌脤麓脵. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFFF; // 鹿芦卤芒 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// 鹿芦卤芒 录脫碌碌赂娄 鲁陋脜赂鲁禄麓脗 Status脌脟 潞帽脝庐赂娄 录鲁脕陇脟脩麓脵.
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
		// Str驴隆 碌没露贸 鹿芦卤芒 录脫碌碌赂娄 脕脵脌脦麓脵. 
		// Centuu : full swing for archers
		if (m_pClientList[iClientH]->m_iClass == 3)
			sSpeed -= (m_pClientList[iClientH]->m_iCharisma / 13);
		else
			sSpeed -= ((m_pClientList[iClientH]->m_iStr) / 13);
		if (sSpeed < 0) sSpeed = 0;
		iTemp = iTemp | (int)sSpeed;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		m_pClientList[iClientH]->m_iComboAttackCount = 0;
		break;

	case DEF_EQUIPPOS_TWOHAND:
		// 戮莽录脮脜赂脌脭脌脤脕枚赂赂 禄莽陆脟 驴脌赂楼录脮驴隆 脌氓脗酶碌脠麓脵. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 4); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFFF; // 鹿芦卤芒 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 28);
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// 鹿芦卤芒 录脫碌碌赂娄 鲁陋脜赂鲁禄麓脗 Status脌脟 潞帽脝庐赂娄 录鲁脕陇脟脩麓脵.
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		sSpeed = (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cSpeed);
		// Str驴隆 碌没露贸 鹿芦卤芒 录脫碌碌赂娄 脕脵脌脦麓脵. 
		
		// Centuu : full swing for archers
		if (m_pClientList[iClientH]->m_iClass == 3)
			sSpeed -= (m_pClientList[iClientH]->m_iCharisma / 13);
		else
			sSpeed -= ((m_pClientList[iClientH]->m_iStr) / 13);
		
		if (sSpeed < 0) sSpeed = 0;
		iTemp = iTemp | (int)sSpeed;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		m_pClientList[iClientH]->m_iComboAttackCount = 0;
		break;

	case DEF_EQUIPPOS_BACK:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xF0FF;	// 赂脕脜盲 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 8); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFF0FFFF; // 赂脕脜盲 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		iTemp = iTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemColor) << 16);
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RELEASEALL:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 赂脕脜盲 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = sTemp | ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cApprValue) << 12); // Appr 掳陋脌禄 录录脝脙. 
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFF0FFFF; // 赂脕脜盲 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) {
		// 掳酶掳脻 脝炉录枚 麓脡路脗 潞帽脝庐 脜卢赂庐戮卯
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect) {
		case 0: break;
		case 1: // 掳酶掳脻陆脙 50% HP 掳篓录脪
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0004;
			break;

		case 2: // 鲁脙碌驴 脠驴掳煤 
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x000C;
			break;

		case 3: // 赂露潞帽 脠驴掳煤 
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0008;
			break;
		}
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY) {
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
		switch (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sSpecialEffect) {
		case 0:
			break;
		case 50:
		case 51:
		case 52:
		case 60: // Magn0S:: Bloody Armors
			m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0002;
			break;
		default:
			if (m_pClientList[iClientH]->m_iAdminUserLevel > 0) m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 | 0x0001;

			break;
		}
	}

	SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);
	CalcTotalItemEffect(iClientH, sItemIndex, bNotify);
	return true;

}

void CGame::ReleaseItemHandler(int iClientH, short sItemIndex, bool bNotice)
{
	char cEquipPos;
	short  sTemp;
	int   iTemp;

	if (m_pClientList[iClientH] == 0) return;
	if ((sItemIndex < 0) || (sItemIndex >= DEF_MAXITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex] == 0) return;
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cItemType != DEF_ITEMTYPE_EQUIP) return;

	if (m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] == false) return;

	cEquipPos = m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_cEquipPos;
	//Magn0S:: Update to put SelfSafe for bmages.
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MAGICITEM) {
		if (m_pClientList[iClientH]->m_bIsSelfSafe == true)
			m_pClientList[iClientH]->m_bIsSelfSafe = true;
		else m_pClientList[iClientH]->m_bIsSelfSafe = false;
	}
	if (cEquipPos == DEF_EQUIPPOS_RHAND) {
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex] != 0) {
			if ((m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 865) || (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 866)) {
				m_pClientList[iClientH]->m_cMagicMastery[94] = 0;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_RESUR_OFF, 0, 0, 0, 0);
			}
		}
		// Centuu : Fixed las armas Blood by KaoZureS
		else if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 491 || m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 492) { // Axe
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMBLOOD, 0, 0, 0, 0);
		}
	}
	if (cEquipPos == DEF_EQUIPPOS_TWOHAND) {
		// Centuu : Fixed las armas Blood by KaoZureS
		if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sIDnum == 490) { // Sword
			SendNotifyMsg(0, iClientH, DEF_NOTIFY_ITEMBLOOD, 0, 0, 0, 0);
		}
	}
	// 戮脝脌脤脜脹脌脟 脟脴脕娄脌搂脛隆驴隆 赂脗掳脭 Appr潞炉录枚赂娄 脕露脕陇脟脩麓脵.
	switch (cEquipPos) {
	case DEF_EQUIPPOS_RHAND:
		// 驴脌赂楼录脮驴隆 脌氓脗酶脟脧麓脗 戮脝脌脤脜脹. 鹿芦卤芒路霉脌脤麓脵. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0x0FFFFFFF; // 鹿芦卤芒 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;

		// V1.3 鹿芦卤芒录脫碌碌 脌莽掳猫禄锚 <- 赂脟录脮脌脤麓脧卤帽 0
		iTemp = m_pClientList[iClientH]->m_iStatus;
		iTemp = iTemp & 0xFFFFFFF0;
		m_pClientList[iClientH]->m_iStatus = iTemp;
		break;

	case DEF_EQUIPPOS_LHAND:
		// 驴脼录脮驴隆 脌氓脗酶脟脧麓脗 戮脝脌脤脜脹. 鹿忙脝脨路霉脌脤麓脵. 
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xFFF0;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xF0FFFFFF; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_TWOHAND:
		// 戮莽录脮驴隆 脌氓脗酶脟脩麓脵脕枚赂赂 禄莽陆脟 驴脌赂楼录脮驴隆赂赂 碌茅掳铆 脌脰麓脵. 驴脼录脮脌潞 潞帽戮卯脌脰麓脗 禄贸脜脗.
		sTemp = m_pClientList[iClientH]->m_sAppr2;
		sTemp = sTemp & 0xF00F;	// 鹿芦卤芒 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr2 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0x0FFFFFFF; // 鹿芦卤芒 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BODY:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 掳脩驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		// 掳脩驴脢 脠庐脌氓 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xFF7F;
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFF0FFFFF; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_BACK:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0xF0FF;	// 赂脕脜盲 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFF0FFFF; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_ARMS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFFF0;	// 掳脩驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFF0FFF; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	//case DEF_EQUIPPOS_BOOTS:
	case DEF_EQUIPPOS_LEGGINGS:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xF0FF;	// 掳脩驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFF0FF; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	//case DEF_EQUIPPOS_LEGGINGS:
	case DEF_EQUIPPOS_BOOTS:
		sTemp = m_pClientList[iClientH]->m_sAppr4;
		sTemp = sTemp & 0x0FFF;	// 掳脩驴脢 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr4 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFF0F; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_HEAD:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0xFF0F;	// 脜玫卤赂 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFFFFFF0; // 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;

	case DEF_EQUIPPOS_RELEASEALL:
		sTemp = m_pClientList[iClientH]->m_sAppr3;
		sTemp = sTemp & 0x0FFF;	// 赂脕脜盲 脝炉录潞脛隆 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_sAppr3 = sTemp;

		iTemp = m_pClientList[iClientH]->m_iApprColor; // v1.4 脛脙路炉掳陋 录录脝脙 
		iTemp = iTemp & 0xFFF0FFFF; // 赂脕脜盲 禄枚 潞帽脝庐赂娄 脜卢赂庐戮卯.
		m_pClientList[iClientH]->m_iApprColor = iTemp;
		break;
	}

	//v1.432 脝炉录枚 麓脡路脗脌脤 潞脦驴漏碌脠 戮脝脌脤脜脹脌脤露贸赂茅 脟脙路隆卤脳 录鲁脕陇 
	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY) {
		// 掳酶掳脻 脝炉录枚 麓脡路脗 潞帽脝庐 脜卢赂庐戮卯
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFF3;
	}

	if (m_pClientList[iClientH]->m_pItemList[sItemIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY) {
		// 鹿忙戮卯卤赂 脝炉录枚 麓脡路脗 潞帽脝庐 脜卢赂庐戮卯
		m_pClientList[iClientH]->m_sAppr4 = m_pClientList[iClientH]->m_sAppr4 & 0xFFFC;
	}

	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;
	m_pClientList[iClientH]->m_sItemEquipmentStatus[cEquipPos] = -1;

	// 驴脺脟眉脌脤 鹿脵虏茂掳脥脌禄 戮脣赂掳麓脵.
	if (bNotice == true)
		SendEventToNearClient_TypeA(iClientH, DEF_OWNERTYPE_PLAYER, MSGID_EVENT_MOTION, DEF_OBJECTNULLACTION, 0, 0, 0);

	// 脌脤 戮脝脌脤脜脹脌禄 脟脴脕娄脟脧麓脗 掳脥驴隆 碌没赂拢麓脗 脝炉录潞脛隆脌脟 潞炉脠颅赂娄 掳猫禄锚脟脩麓脵. 
	CalcTotalItemEffect(iClientH, sItemIndex, true);
}

bool CGame::bPlayerItemToBank(int iClientH, short sItemIndex)
{
	int i, iIndex;

	if (m_pClientList[iClientH] == 0) return false;

	for (i = 0; i < DEF_MAXBANKITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemInBankList[i] == 0) {
			iIndex = i;
			goto NEXT_STEP_PLTB;
		}
	return false;

NEXT_STEP_PLTB:;

	ReleaseItemHandler(iClientH, sItemIndex, true);

	m_pClientList[iClientH]->m_pItemInBankList[iIndex] = m_pClientList[iClientH]->m_pItemList[sItemIndex];
	m_pClientList[iClientH]->m_pItemList[sItemIndex] = 0;
	m_pClientList[iClientH]->m_bIsItemEquipped[sItemIndex] = false;

	for (i = 1; i < DEF_MAXITEMS; i++)
		if ((m_pClientList[iClientH]->m_pItemList[i - 1] == 0) && (m_pClientList[iClientH]->m_pItemList[i] != 0)) {
			m_pClientList[iClientH]->m_pItemList[i - 1] = m_pClientList[iClientH]->m_pItemList[i];
			m_pClientList[iClientH]->m_bIsItemEquipped[i - 1] = m_pClientList[iClientH]->m_bIsItemEquipped[i];
			m_pClientList[iClientH]->m_pItemList[i] = 0;
			m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;
		}

	return true;
}

bool CGame::bBankItemToPlayer(int iClientH, short sItemIndex)
{
	int i, iIndex;

	if (m_pClientList[iClientH] == 0) return false;

	if (m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] == 0) return false;

	for (i = 0; i < DEF_MAXITEMS; i++)
		if (m_pClientList[iClientH]->m_pItemList[i] == 0) {
			iIndex = i;
			goto NEXT_STEP_PLTB;
		}
	return false;

NEXT_STEP_PLTB:;

	m_pClientList[iClientH]->m_pItemList[iIndex] = m_pClientList[iClientH]->m_pItemInBankList[sItemIndex];

	m_pClientList[iClientH]->m_pItemInBankList[sItemIndex] = 0;

	return true;
}

void CGame::RequestRetrieveItemHandler(int iClientH, char* pData)
{
	char* cp, cBankItemIndex, cMsg[100];
	int i, j, iRet, iItemWeight;
	DWORD* dwp;
	WORD* wp;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	cBankItemIndex = *cp;

	if (m_pClientList[iClientH]->m_bIsInsideWarehouse == false) return;

	if ((cBankItemIndex < 0) || (cBankItemIndex >= DEF_MAXBANKITEMS)) return;
	if (m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] == 0) {
		ZeroMemory(cMsg, sizeof(cMsg));

		dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
		*dwp = MSGID_RESPONSE_RETRIEVEITEM;
		wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
		*wp = DEF_MSGTYPE_REJECT;

		iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
	}
	else {
		iItemWeight = iGetItemWeight(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex], m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

		if ((iItemWeight + m_pClientList[iClientH]->m_iCurWeightLoad) > _iCalcMaxLoad(iClientH)) {
			ZeroMemory(cMsg, sizeof(cMsg));

			dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
			*dwp = MSGID_NOTIFY;
			wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
			*wp = DEF_NOTIFY_CANNOTCARRYMOREITEM;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 6);
			switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				DeleteClient(iClientH, true, true);
				break;
			}
			return;
		}

		if ((m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_CONSUME) ||
			(m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType == DEF_ITEMTYPE_ARROW)) {
			for (i = 0; i < DEF_MAXITEMS; i++)
				if ((m_pClientList[iClientH]->m_pItemList[i] != 0) &&
					(m_pClientList[iClientH]->m_pItemList[i]->m_cItemType == m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cItemType) &&
					(memcmp(m_pClientList[iClientH]->m_pItemList[i]->m_cName, m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_cName, 20) == 0)) {
					SetItemCount(iClientH, i, m_pClientList[iClientH]->m_pItemList[i]->m_dwCount + m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex]->m_dwCount);

					delete m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
					m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = 0;

					for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++) {
						if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != 0) && (m_pClientList[iClientH]->m_pItemInBankList[j] == 0)) {
							m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

							m_pClientList[iClientH]->m_pItemInBankList[j + 1] = 0;
						}
					}

					ZeroMemory(cMsg, sizeof(cMsg));

					dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
					*dwp = MSGID_RESPONSE_RETRIEVEITEM;
					wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
					*wp = DEF_MSGTYPE_CONFIRM;

					cp = (char*)(cMsg + DEF_INDEX2_MSGTYPE + 2);
					*cp = cBankItemIndex;
					cp++;
					*cp = i;
					cp++;

					iCalcTotalWeight(iClientH);
					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						DeleteClient(iClientH, true, true);
						break;
					}
					return;
				}

			goto RRIH_NOQUANTITY;
		}
		else {
		RRIH_NOQUANTITY:;
			for (i = 0; i < DEF_MAXITEMS; i++)
				if (m_pClientList[iClientH]->m_pItemList[i] == 0) {

					m_pClientList[iClientH]->m_pItemList[i] = m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex];
					m_pClientList[iClientH]->m_ItemPosList[i].x = 40;
					m_pClientList[iClientH]->m_ItemPosList[i].y = 30;

					m_pClientList[iClientH]->m_bIsItemEquipped[i] = false;

					m_pClientList[iClientH]->m_pItemInBankList[cBankItemIndex] = 0;

					for (j = 0; j <= DEF_MAXBANKITEMS - 2; j++) {
						if ((m_pClientList[iClientH]->m_pItemInBankList[j + 1] != 0) && (m_pClientList[iClientH]->m_pItemInBankList[j] == 0)) {
							m_pClientList[iClientH]->m_pItemInBankList[j] = m_pClientList[iClientH]->m_pItemInBankList[j + 1];

							m_pClientList[iClientH]->m_pItemInBankList[j + 1] = 0;
						}
					}

					ZeroMemory(cMsg, sizeof(cMsg));

					dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
					*dwp = MSGID_RESPONSE_RETRIEVEITEM;
					wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
					*wp = DEF_MSGTYPE_CONFIRM;

					cp = (char*)(cMsg + DEF_INDEX2_MSGTYPE + 2);
					*cp = cBankItemIndex;
					cp++;
					*cp = i;
					cp++;

					iCalcTotalWeight(iClientH);

					m_pClientList[iClientH]->m_cArrowIndex = _iGetArrowItemIndex(iClientH);

					iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
					switch (iRet) {
					case DEF_XSOCKEVENT_QUENEFULL:
					case DEF_XSOCKEVENT_SOCKETERROR:
					case DEF_XSOCKEVENT_CRITICALERROR:
					case DEF_XSOCKEVENT_SOCKETCLOSED:
						DeleteClient(iClientH, true, true);
						break;
					}
					return;
				}
			ZeroMemory(cMsg, sizeof(cMsg));

			dwp = (DWORD*)(cMsg + DEF_INDEX4_MSGID);
			*dwp = MSGID_RESPONSE_RETRIEVEITEM;
			wp = (WORD*)(cMsg + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(cMsg, 8);
		}
	}

	switch (iRet) {
	case DEF_XSOCKEVENT_QUENEFULL:
	case DEF_XSOCKEVENT_SOCKETERROR:
	case DEF_XSOCKEVENT_CRITICALERROR:
	case DEF_XSOCKEVENT_SOCKETCLOSED:
		DeleteClient(iClientH, true, true);
		break;
	}
}

// 05/29/2004 - Hypnotoad - Limits some items from not dropping
void CGame::_PenaltyItemDrop(int iClientH, int iTotal, bool bIsSAattacked)
{
	int i, j, iRemainItem;
	char cItemIndexList[DEF_MAXITEMS], cItemIndex;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;

	// kazin
	if (m_pClientList[iClientH]->IsInMap("team") || m_pClientList[iClientH]->IsInMap("fightzone1"))
		return;

	// SNOOPY: Lucky effect will prevent drops,  even of a ZEM.
	if ((m_pClientList[iClientH]->m_bIsLuckyEffect > 0) && ((iDice(1, 100) <= m_pClientList[iClientH]->m_bIsLuckyEffect)))
	{
		return;
	}

	if ((m_pClientList[iClientH]->m_iAlterItemDropIndex != -1) && (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex] != 0)) {
		// Testcode
		if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP) {
			if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0) {
				m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;
				SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, m_pClientList[iClientH]->m_iAlterItemDropIndex, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan, 0, 0);
			}
			DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);

			m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
		}
		else {
			// v2.04 testcode

			for (i = 0; i < DEF_MAXITEMS; i++)
				if ((m_pClientList[iClientH]->m_pItemList[i] != 0) && (m_pClientList[iClientH]->m_pItemList[i]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ALTERITEMDROP)) {
					m_pClientList[iClientH]->m_iAlterItemDropIndex = i;
					if (m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan > 0) {
						m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan--;
						SendNotifyMsg(0, iClientH, DEF_NOTIFY_CURLIFESPAN, m_pClientList[iClientH]->m_iAlterItemDropIndex, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_wCurLifeSpan, 0, 0);
					}
					DropItemHandler(iClientH, m_pClientList[iClientH]->m_iAlterItemDropIndex, -1, m_pClientList[iClientH]->m_pItemList[m_pClientList[iClientH]->m_iAlterItemDropIndex]->m_cName);
					m_pClientList[iClientH]->m_iAlterItemDropIndex = -1;
					break;
				}

			goto PID_DROP;
		}
		return;
	}

PID_DROP:;

	for (i = 1; i <= iTotal; i++) {
		iRemainItem = 0;
		ZeroMemory(cItemIndexList, sizeof(cItemIndexList));

		for (j = 0; j < DEF_MAXITEMS; j++)
		{
			if (m_pClientList[iClientH]->m_pItemList[j] != 0)
			{
				// Don't drop personal items
				if ((m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectType != 0)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue1 == m_pClientList[iClientH]->m_sCharIDnum1)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue2 == m_pClientList[iClientH]->m_sCharIDnum2)
					&& (m_pClientList[iClientH]->m_pItemList[j]->m_sTouchEffectValue3 == m_pClientList[iClientH]->m_sCharIDnum3))
				{
					continue;
				}

				// Don't drop equiped activable items.
				if (((m_pClientList[iClientH]->m_pItemList[j]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_SPECABLTY)
					|| (m_pClientList[iClientH]->m_pItemList[j]->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY))
					&& (bIsSAattacked == false))
				{
					continue;
				}
				cItemIndexList[iRemainItem] = j;
				iRemainItem++;
			}
		}

		if (iRemainItem == 0) return;
		cItemIndex = cItemIndexList[iDice(1, iRemainItem) - 1];

		DropItemHandler(iClientH, cItemIndex, -1, m_pClientList[iClientH]->m_pItemList[cItemIndex]->m_cName);
	}
}

void CGame::GetRewardMoneyHandler(int iClientH)
{
	int iEraseReq, iWeightLeft, iRewardGoldLeft;

	char cItemName[21];
	class CItem* pItem;

	if (m_pClientList[iClientH] == 0) return;
	if (m_pClientList[iClientH]->m_bIsInitComplete == false) return;


	iWeightLeft = _iCalcMaxLoad(iClientH) - iCalcTotalWeight(iClientH);

	if (iWeightLeft <= 0) return;
	iWeightLeft = iWeightLeft / 2;
	if (iWeightLeft <= 0) return;

	pItem = new class CItem;
	ZeroMemory(cItemName, sizeof(cItemName));
	wsprintf(cItemName, "Gold");
	_bInitItemAttr(pItem, cItemName);

	if ((iWeightLeft / iGetItemWeight(pItem, 1)) >= m_pClientList[iClientH]->m_iRewardGold) {
		pItem->m_dwCount = m_pClientList[iClientH]->m_iRewardGold;
		iRewardGoldLeft = 0;
	}
	else {
		pItem->m_dwCount = (iWeightLeft / iGetItemWeight(pItem, 1));
		iRewardGoldLeft = m_pClientList[iClientH]->m_iRewardGold - (iWeightLeft / iGetItemWeight(pItem, 1));
	}

	if (_bAddClientItemList(iClientH, pItem, &iEraseReq) == true) {
		m_pClientList[iClientH]->m_iRewardGold = iRewardGoldLeft;

		SendItemNotifyMsg(iClientH, DEF_NOTIFY_ITEMOBTAINED, pItem, 0);

		SendNotifyMsg(0, iClientH, DEF_NOTIFY_REWARDGOLD, 0, 0, 0, 0);

		if (iEraseReq == 1) delete pItem;
	}
	else {
		// Centuu : DEF_NOTIFY_CANNOTCARRYMOREITEM
		SendNotifyMsg(0, iClientH, DEF_NOTIFY_CANNOTCARRYMOREITEM, 0, 0, 0, 0);
	}
}


int CGame::_iCalcMaxLoad(int iClientH)
{
	if (m_pClientList[iClientH] == 0) return 0;

	return (((m_pClientList[iClientH]->m_iStr) * 500) + ((m_pClientList[iClientH]->m_iLevel) * 500));
}

bool CGame::bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex)
{
	if (m_pClientList[iClientH] == 0) return false;
	if (m_pClientList[iClientH]->m_pItemList[iItemIndex] == 0) return false;

	switch (m_pClientList[iClientH]->m_pItemList[iItemIndex]->m_sIDnum) {
	case 4:  // Dagger +1
	case 9:  // Short Sword +1
	case 13: // Main Gauge +1
	case 16: // Gradius +1
	case 18: // Long Sword +1
	case 19: // Long Sword +2
	case 21: // Excaliber +1
	case 24: // Sabre +1
	case 26: // Scimitar +1
	case 27: // Scimitar +2
	case 29: // Falchoin +1
	case 30: // Falchion +2
	case 32: // Esterk +1
	case 33: // Esterk +2
	case 35: // Rapier +1
	case 36: // Rapier +2
	case 39: // Broad Sword +1
	case 40: // Broad Sword +2
	case 43: // Bastad Sword +1
	case 44: // Bastad Sword +2
	case 47: // Claymore +1
	case 48: // Claymore +2
	case 51: // Great Sword +1
	case 52: // Great Sword +2
	case 55: // Flameberge +1
	case 56: // Flameberge +2
	case 60: // Light Axe +1
	case 61: // Light Axe +2
	case 63: // Tomahoc +1
	case 64: // Tomohoc +2
	case 66: // Sexon Axe +1
	case 67: // Sexon Axe +2
	case 69: // Double Axe +1
	case 70: // Double Axe +2
	case 72: // War Axe +1
	case 73: // War Axe +2

	case 580: // Battle Axe +1
	case 581: // Battle Axe +2
	case 582: // Sabre +2
		return true;

	}
	return false;
}

void CGame::ArmorLifeDecrement(int iAttackerH, int iTargetH, char cOwnerType, int iValue)
{
	int iTemp;

	if (m_pClientList[iAttackerH] == 0) return;
	if (m_pClientList[iTargetH] == 0) return;

	if (iAttackerH == iTargetH) return;

	if (m_pClientList[iAttackerH]->m_cSide == m_pClientList[iTargetH]->m_cSide) return;

	switch (iDice(1, 13)) {
	case 1:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 2:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 3:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 4: 
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 5:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 6:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 7:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 8:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 9:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 10:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 11:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}

		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {

			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 12:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;

	case 13:
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {
			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		iTemp = m_pClientList[iTargetH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BOOTS];
		if ((iTemp != -1) && (m_pClientList[iTargetH]->m_pItemList[iTemp] != 0)) {


			if ((m_pClientList[iTargetH]->m_cSide != 0) && (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan > 0)) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan -= iValue;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_CURLIFESPAN, iTemp, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan, 0, 0);
				if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 2000) {
					ReleaseItemHandler(iTargetH, iTemp, true);
					SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMRELEASED, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				}
			}
			if (m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan <= 0) {
				m_pClientList[iTargetH]->m_pItemList[iTemp]->m_wCurLifeSpan = 0;
				SendNotifyMsg(0, iTargetH, DEF_NOTIFY_ITEMLIFESPANEND, m_pClientList[iTargetH]->m_pItemList[iTemp]->m_cEquipPos, iTemp, 0, 0);
				ReleaseItemHandler(iTargetH, iTemp, true);
			}
		}
		break;
	}
}

int CGame::_cCheckHeroItemEquipped(int iClientH)
{
	int iBonus = 0;
	short sHeroLeggings, sHeroHauberk, sHeroArmor, sHeroHelm;

	if (m_pClientList[iClientH] == 0) return 0;

	sHeroHelm = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_HEAD];
	sHeroArmor = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_BODY];
	sHeroHauberk = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_ARMS];
	sHeroLeggings = m_pClientList[iClientH]->m_sItemEquipmentStatus[DEF_EQUIPPOS_LEGGINGS];

	if ((sHeroHelm < 0) || (sHeroLeggings < 0) || (sHeroArmor < 0) || (sHeroHauberk < 0)) return 0;

	if (m_pClientList[iClientH]->m_pItemList[sHeroHelm] == 0) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroLeggings] == 0) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroArmor] == 0) return 0;
	if (m_pClientList[iClientH]->m_pItemList[sHeroHauberk] == 0) return 0;

	if (m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 403 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 407 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 404 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 408 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 405 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 409 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 406 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 410 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 392 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 393 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 394 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 395)
		iBonus += 2;
		
	if (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 411 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 415 ||
		m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 412 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 416 ||
		m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 413 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 417 ||
		m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 414 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 418)
		iBonus += 2;
	
	if (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 419 || m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 421 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 420 || m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 422)
		iBonus += 2;
	
	if (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 423 || m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 424 ||
		m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 425 || m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 426)
		iBonus += 2;

	if (m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 1061 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 1062 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 1063 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 1064 ||
		m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 147 || m_pClientList[iClientH]->m_pItemList[sHeroHelm]->m_sIDnum == 148)
		iBonus += 4;

	if (m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 1065 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 1066 ||
		m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 1067 || m_pClientList[iClientH]->m_pItemList[sHeroArmor]->m_sIDnum == 1068)
		iBonus += 4;

	if (m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 1069 || m_pClientList[iClientH]->m_pItemList[sHeroHauberk]->m_sIDnum == 1070)
		iBonus += 4;

	if (m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 1071 || m_pClientList[iClientH]->m_pItemList[sHeroLeggings]->m_sIDnum == 1072)
		iBonus += 4;

	if (iBonus > 15) iBonus = 15;

	return iBonus;
}

void CGame::_bDecodeDupItemIDFileContents(char* pData, DWORD dwMsgSize)
{
	char* pContents, * token, cTxt[120];
	char seps[] = "= \t\n";
	char cReadModeA = 0;
	char cReadModeB = 0;
	int  iIndex = 0;
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
					// 胶懦 锅龋 
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}

					if (m_pDupItemIDList[atoi(token)] != 0) {
						// 捞固 且寸等 锅龋啊 乐促. 俊矾捞促.
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Duplicate magic number.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[atoi(token)] = new class CItem;
					iIndex = atoi(token);

					cReadModeB = 2;
					break;

				case 2:
					// m_sTouchEffectType
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectType = atoi(token);
					cReadModeB = 3;
					break;

				case 3:
					// m_sTouchEffectValue1
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue1 = atoi(token);
					cReadModeB = 4;
					break;

				case 4:
					// m_sTouchEffectValue2
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue2 = atoi(token);
					cReadModeB = 5;
					break;

				case 5:
					// m_sTouchEffectValue3
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_sTouchEffectValue3 = atoi(token);
					cReadModeB = 6;
					break;

				case 6:
					// m_wPrice
					if (_bGetIsStringIsNumber(token) == false) {
						PutLogList("(!!!) ERROR! DupItemID configuration file error - Wrong Data format.");
						delete[] pContents;
						delete pStrTok;
						return;
					}
					m_pDupItemIDList[iIndex]->m_wPrice = (WORD)atoi(token);
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
			if (memcmp(token, "DupItemID", 9) == 0) {
				cReadModeA = 1;
				cReadModeB = 1;
			}

		}
		token = pStrTok->pGet();
	}

	delete pStrTok;
	delete[] pContents;

	if ((cReadModeA != 0) || (cReadModeB != 0)) {
		PutLogList("(!!!) ERROR! DupItemID configuration file contents error!");
		return;
	}

	wsprintf(cTxt, "(!) DupItemID(Total:%d) configuration - success!", iIndex);
	PutLogList(cTxt);
}

bool CGame::_bCheckDupItemID(CItem* pItem)
{
	int i;

	for (i = 0; i < DEF_MAXDUPITEMID; i++)
		if (m_pDupItemIDList[i] != 0) {
			if ((pItem->m_sTouchEffectType == m_pDupItemIDList[i]->m_sTouchEffectType) &&
				(pItem->m_sTouchEffectValue1 == m_pDupItemIDList[i]->m_sTouchEffectValue1) &&
				(pItem->m_sTouchEffectValue2 == m_pDupItemIDList[i]->m_sTouchEffectValue2) &&
				(pItem->m_sTouchEffectValue3 == m_pDupItemIDList[i]->m_sTouchEffectValue3)) {
				// 掳隆掳脻 脕陇潞赂赂娄 掳禄陆脜脟脧掳铆 赂庐脜脧.
				pItem->m_wPrice = m_pDupItemIDList[i]->m_wPrice;
				return true;
			}
		}

	return false;
}

void CGame::_AdjustRareItemValue(CItem* pItem)
{
	DWORD dwSWEType, dwSWEValue;
	double dV1, dV2, dV3;

	if ((pItem->m_dwAttribute & 0x00F00000) != 0) {
		dwSWEType = (pItem->m_dwAttribute & 0x00F00000) >> 20;
		dwSWEValue = (pItem->m_dwAttribute & 0x000F0000) >> 16;
		//if (iDice(1, 10000) >= m_iRareDropRate) { // Centuu : Agregado para controlar el drop rare.
			switch (dwSWEType) {
			case 0:
				break;

			case ITEMSTAT_AGILE:
				pItem->m_cSpeed--;
				if (pItem->m_cSpeed < 0) pItem->m_cSpeed = 0;
				break;

			case ITEMSTAT_LIGHT:
				dV2 = (double)pItem->m_wWeight;
				dV3 = (double)(dwSWEValue * 4);
				dV1 = (dV3 / 100.0f) * dV2;
				pItem->m_wWeight -= (int)dV1;

				if (pItem->m_wWeight < 1) pItem->m_wWeight = 1;
				break;

			case ITEMSTAT_STRONG:
			case ITEMSTAT_ANCIENT:
				dV2 = (double)pItem->m_wMaxLifeSpan;
				dV3 = (double)(dwSWEValue * 7);
				dV1 = (dV3 / 100.0f) * dV2;
				pItem->m_wMaxLifeSpan += (int)dV1;
				break;
			}
		//}
	}
}

// Item Logging
bool CGame::_bItemLog(int iAction, int iGiveH, int iRecvH, class CItem* pItem, bool bForceItemLog)
{
	char  cTxt[1024], cTemp1[120], cTemp2[120];
	int iItemCount;
	if (pItem == 0) return false;

	// !!脕脰脌脟 脟脩卤鹿驴隆 脌没驴毛脟脪露搂  New Item 脌脤 禄媒卤忙露搂麓脗  iGive掳隆 鲁脦脌脧录枚 脌脰麓脵.
	if (m_pClientList[iGiveH]->m_cCharName == 0) return false;

	if (iAction == DEF_ITEMLOG_DUPITEMID) {
		// 潞鹿禄莽碌脠 戮脝脌脤脜脹 脌煤脌氓 驴盲脙禄脌脤麓脵. 
		if (m_pClientList[iGiveH] == 0) return false;
		if (m_pClientList[iGiveH]->m_cCharName == 0) return false;
		wsprintf(G_cTxt, "(!) Delete-DupItem(%s %d %d %d %d) Owner(%s)", pItem->m_cName, pItem->m_dwCount, pItem->m_sTouchEffectValue1,
			pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3,
			m_pClientList[iGiveH]->m_cCharName);
		bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, 0, G_cTxt);
		return true;
	}

	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTemp1, sizeof(cTemp1));
	ZeroMemory(cTemp2, sizeof(cTemp2));

	switch (iAction) {

	case DEF_ITEMLOG_EXCHANGE:
		if (m_pClientList[iRecvH]->m_cCharName == 0) return false;
		wsprintf(cTxt, "(%s) PC(%s)\tExchange\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
		break;

	case DEF_ITEMLOG_GIVE:
		if (m_pClientList[iRecvH]->m_cCharName == 0) return false;
		wsprintf(cTxt, "(%s) PC(%s)\tGive\t%s(%d %d %d %d %x)\t%s(%d %d)\tPC(%s)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY, m_pClientList[iRecvH]->m_cCharName);
		break;

	case DEF_ITEMLOG_DROP:
		wsprintf(cTxt, "(%s) PC(%s)\tDrop\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_GET:
		wsprintf(cTxt, "(%s) PC(%s)\tGet\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_MAKE:
		wsprintf(cTxt, "(%s) PC(%s)\tMake\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_DEPLETE:
		wsprintf(cTxt, "(%s) PC(%s)\tDeplete\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_BUY:
		iItemCount = iRecvH;
		wsprintf(cTxt, "(%s) PC(%s)\tBuy\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, iItemCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_SELL:
		wsprintf(cTxt, "(%s) PC(%s)\tSell\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_RETRIEVE:
		wsprintf(cTxt, "(%s) PC(%s)\tRetrieve\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_DEPOSIT:
		wsprintf(cTxt, "(%s) PC(%s)\tDeposit\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_UPGRADEFAIL:
		wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Fail\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
			pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;

	case DEF_ITEMLOG_UPGRADESUCCESS:
		wsprintf(cTxt, "(%s) PC(%s)\tUpgrade Success\t%s(%d %d %d %d %x)\t%s(%d %d)", m_pClientList[iGiveH]->m_cIPaddress, m_pClientList[iGiveH]->m_cCharName, pItem->m_cName,
			pItem->m_dwCount, pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3, pItem->m_dwAttribute,
			m_pClientList[iGiveH]->m_cMapName, m_pClientList[iGiveH]->m_sX, m_pClientList[iGiveH]->m_sY);
		break;
	default:
		return false;
	}
	bSendMsgToLS(MSGID_GAMEITEMLOG, iGiveH, 0, cTxt);
	return true;
}

bool CGame::_bItemLog(int iAction, int iClientH, char* cName, class CItem* pItem)
{
	if (pItem == 0) return false;
	if (_bCheckGoodItem(pItem) == false) return false;
	if (iAction != DEF_ITEMLOG_NEWGENDROP)
	{
		if (m_pClientList[iClientH] == 0) return false;
	}
	char  cTxt[200], cTemp1[120];
	//  路脦卤脳 鲁虏卤盲麓脵. 
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(cTemp1, sizeof(cTemp1));
	if (m_pClientList[iClientH] != 0) m_pClientList[iClientH]->m_pXSock->iGetPeerAddress(cTemp1);

	switch (iAction) {

	case DEF_ITEMLOG_NEWGENDROP:
		if (pItem == 0) return false;
		wsprintf(cTxt, "NPC(%s)\tDrop\t%s(%d %d %d %d)", cName, pItem->m_cName, pItem->m_dwCount,
			pItem->m_sTouchEffectValue1, pItem->m_sTouchEffectValue2, pItem->m_sTouchEffectValue3);
		break;
	case DEF_ITEMLOG_SKILLLEARN:
	case DEF_ITEMLOG_MAGICLEARN:
		if (cName == 0) return false;
		if (m_pClientList[iClientH] == 0) return false;
		wsprintf(cTxt, "PC(%s)\tLearn\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;
	case DEF_ITEMLOG_SUMMONMONSTER:
		if (cName == 0) return false;
		if (m_pClientList[iClientH] == 0) return false;
		wsprintf(cTxt, "PC(%s)\tSummon\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;
	case DEF_ITEMLOG_POISONED:
		if (m_pClientList[iClientH] == 0) return false;
		wsprintf(cTxt, "PC(%s)\tBe Poisoned\t \t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;

	case DEF_ITEMLOG_REPAIR:
		if (cName == 0) return false;
		if (m_pClientList[iClientH] == 0) return false;
		wsprintf(cTxt, "PC(%s)\tRepair\t(%s)\t%s(%d %d)\t \tIP(%s)", m_pClientList[iClientH]->m_cCharName, cName,
			m_pClientList[iClientH]->m_cMapName, m_pClientList[iClientH]->m_sX, m_pClientList[iClientH]->m_sY, cTemp1);
		break;

	default:
		return false;
	}
	bSendMsgToLS(MSGID_GAMEITEMLOG, iClientH, 0, cTxt);
	return true;
}

bool CGame::_bCheckGoodItem(class CItem* pItem)
{
	if (pItem == 0) return false;

	/*if (pItem->m_sIDnum == 90)
	{
		if (pItem->m_dwCount > 10000) return true;  //Gold驴隆 脟脩脟脴 10000驴酶 脌脤禄贸赂赂 路脦卤脳驴隆 鲁虏卤盲麓脵.
		else return false;
	}*/
	switch (pItem->m_sIDnum) {
		// Gold 脙脽掳隆 
	case 259:
	case 290:
	case 291:
	case 292:
	case 300:
	case 305:
	case 308:
	case 311:
	case 334:
	case 335:
	case 336:
	case 338:
	case 380:
	case 381:
	case 382:
	case 391:
	case 400:
	case 401:
	case 490:
	case 491:
	case 492:
	case 508:
	case 581:
	case 610:
	case 611:
	case 612:
	case 613:
	case 614:
	case 616:  // 碌楼赂贸-陆陆路鹿脌脤戮卯
	case 618:  // 麓脵脜漏驴陇脟脕-潞赂驴矛

	case 620:
	case 621:
	case 622:
	case 623:

	case 630:
	case 631:

	case 632:
	case 633:
	case 634:
	case 635:
	case 636:
	case 637:
	case 638:
	case 639:
	case 640:
	case 641:

	case 642:
	case 643:

	case 644:
	case 645:
	case 646:
	case 647:

	case 650:
	case 654:
	case 655:
	case 656:
	case 657:

	case 700: 	// v2.03 禄贸戮卯 戮脝脌脤脜脹 
	case 701:
	case 702:
	case 703:
	case 704:
	case 705:
	case 706:
	case 707:
	case 708:
	case 709:
	case 710:
	case 711:
	case 712:
	case 713:
	case 714:
	case 715:

	case 720:
	case 721:
	case 722:
	case 723:

	case 724:
	case 725:
	case 726:
	case 727:
	case 728:
	case 729:
	case 730:
	case 731:
	case 732:
	case 733:

	case 734:
	case 735:

	case 736:  // 禄玫路脦驴卯 禄贸戮卯脌脟 脌脷脌脤戮冒脝庐 录脪碌氓
	case 737:  // 禄玫路脦驴卯 脠忙卤芒禄莽脌脟 脌脷脌脤戮冒脝庐 录脪碌氓
	case 738:  // 禄玫路脦驴卯 脠忙赂露鹿媒禄莽脌脟赂脜脕梅驴酶碌氓

		return true;  //脝炉潞掳脟脩 戮脝脌脤脜脹脌脤卤芒 露芦陆脙 卤芒路脧...

	default:
		// v2.17 2002-7-31 脕娄脌脹 戮脝脌脤脜脹碌碌 路脦卤脳驴隆 鲁虏掳脭 脟脩麓脵.
		if ((pItem->m_dwAttribute & 0xF0F0F001) == 0) return false;  //脝炉潞掳脟脩 戮脝脌脤脜脹碌脦 戮脝麓脧卤赂 脝炉录潞脛隆碌碌 戮酶麓脵赂茅 潞掳路莽..
		else if (pItem->m_sIDnum > 30) return true;  //脝炉潞掳脟脩 戮脝脌脤脜脹脌潞 戮脝麓脧脕枚赂赂 脝炉录潞脛隆掳隆 脌脰掳铆 麓脺掳脣路霉掳隆 戮脝麓脧露贸赂茅 脕脕脌潞 戮脝脜脹..
		else return false;  //脝炉潞掳脟脩 戮脝脌脤脜脹碌脦 戮脝麓脧卤赂 脝炉录潞脛隆麓脗 脌脰脕枚赂赂 麓脺掳脣路霉露贸赂茅 潞掳路莽...
	}
}

void CGame::RequestSellItemListHandler(int iClientH, char* pData)
{
	int i, * ip, iAmount;
	char* cp, cIndex;
	struct {
		char cIndex;
		int  iAmount;
	} stTemp[12];

	if (m_pClientList[iClientH] == 0) return;

	cp = (char*)(pData + 6);
	for (i = 0; i < 12; i++) {
		stTemp[i].cIndex = *cp;
		cp++;

		ip = (int*)cp;
		stTemp[i].iAmount = *ip;
		cp += 4;
	}

	// 鲁禄驴毛脌禄 麓脵 脌脨戮煤麓脵. 录酶录颅麓毛路脦 脝脠戮脝脛隆驴卯麓脵.
	for (i = 0; i < 12; i++) {
		cIndex = stTemp[i].cIndex;
		iAmount = stTemp[i].iAmount;

		if ((cIndex == -1) || (cIndex < 0) || (cIndex >= DEF_MAXITEMS)) return;
		if (m_pClientList[iClientH]->m_pItemList[cIndex] == 0) return;

		// 驴隆 脟脴麓莽脟脧麓脗 戮脝脌脤脜脹脌禄 脝脟麓脵.
		ReqSellItemConfirmHandler(iClientH, cIndex, iAmount, 0);
		// 脌脤 路莽脝戮脌禄 录枚脟脿脟脩 麓脵脌陆 脜卢露贸脌脤戮冒脝庐掳隆 禄猫脕娄碌脟戮煤脌禄 录枚 脌脰脌赂麓脧 脕脰脌脟!

	}
}

int CGame::iGetItemWeight(CItem* pItem, int iCount)
{
	int iWeight;

	// 戮脝脌脤脜脹脌脟 录枚路庐驴隆 碌没赂楼 鹿芦掳脭赂娄 掳猫禄锚脟脩麓脵. Gold脌脦 掳忙驴矛 鹿芦掳脭赂娄 20潞脨脌脟 1路脦 潞炉掳忙 
	iWeight = pItem->m_wWeight;
	if (iCount < 0) iCount = 1;
	iWeight = iWeight * iCount;
	if (pItem->m_sIDnum == 90 || pItem->m_sIDnum == 77 || pItem->m_sIDnum == 1002) iWeight = 1; //iWeight = iWeight / 20; // Centuu : el oro no tiene peso :D
	if (iWeight < 0) iWeight = 1;

	return iWeight;
}

bool CGame::bCopyItemContents(CItem* pCopy, CItem* pOriginal)
{
	if (pOriginal == 0) return false;
	if (pCopy == 0) return false;

	pCopy->m_sIDnum = pOriginal->m_sIDnum;					// 戮脝脌脤脜脹脌脟 掳铆脌炉 鹿酶脠拢 
	pCopy->m_cItemType = pOriginal->m_cItemType;
	pCopy->m_cEquipPos = pOriginal->m_cEquipPos;
	pCopy->m_sItemEffectType = pOriginal->m_sItemEffectType;
	pCopy->m_sItemEffectValue1 = pOriginal->m_sItemEffectValue1;
	pCopy->m_sItemEffectValue2 = pOriginal->m_sItemEffectValue2;
	pCopy->m_sItemEffectValue3 = pOriginal->m_sItemEffectValue3;
	pCopy->m_sItemEffectValue4 = pOriginal->m_sItemEffectValue4;
	pCopy->m_sItemEffectValue5 = pOriginal->m_sItemEffectValue5;
	pCopy->m_sItemEffectValue6 = pOriginal->m_sItemEffectValue6;
	pCopy->m_wMaxLifeSpan = pOriginal->m_wMaxLifeSpan;
	pCopy->m_sSpecialEffect = pOriginal->m_sSpecialEffect;

	//v1.432 赂铆脕脽路眉 掳隆掳篓 禄莽驴毛 戮脠脟脩麓脵. 麓毛陆脜 脝炉录枚 麓脡路脗 录枚脛隆掳隆 碌茅戮卯掳拢麓脵.
	pCopy->m_sSpecialEffectValue1 = pOriginal->m_sSpecialEffectValue1;
	pCopy->m_sSpecialEffectValue2 = pOriginal->m_sSpecialEffectValue2;

	pCopy->m_sSprite = pOriginal->m_sSprite;
	pCopy->m_sSpriteFrame = pOriginal->m_sSpriteFrame;

	pCopy->m_cApprValue = pOriginal->m_cApprValue;
	pCopy->m_cSpeed = pOriginal->m_cSpeed;

	pCopy->m_wPrice = pOriginal->m_wPrice;
	pCopy->m_wWeight = pOriginal->m_wWeight;
	pCopy->m_sLevelLimit = pOriginal->m_sLevelLimit;
	pCopy->m_cGenderLimit = pOriginal->m_cGenderLimit;

	pCopy->m_sRelatedSkill = pOriginal->m_sRelatedSkill;

	pCopy->m_cCategory = pOriginal->m_cCategory;
	pCopy->m_bIsForSale = pOriginal->m_bIsForSale;
	// 

	pCopy->m_dwCount = pOriginal->m_dwCount;
	pCopy->m_sTouchEffectType = pOriginal->m_sTouchEffectType;
	pCopy->m_sTouchEffectValue1 = pOriginal->m_sTouchEffectValue1;
	pCopy->m_sTouchEffectValue2 = pOriginal->m_sTouchEffectValue2;
	pCopy->m_sTouchEffectValue3 = pOriginal->m_sTouchEffectValue3;
	pCopy->m_cItemColor = pOriginal->m_cItemColor;
	pCopy->m_sItemSpecEffectValue1 = pOriginal->m_sItemSpecEffectValue1;
	pCopy->m_sItemSpecEffectValue2 = pOriginal->m_sItemSpecEffectValue2;
	pCopy->m_sItemSpecEffectValue3 = pOriginal->m_sItemSpecEffectValue3;
	pCopy->m_wCurLifeSpan = pOriginal->m_wCurLifeSpan;
	pCopy->m_dwAttribute = pOriginal->m_dwAttribute;
	//Magn0S:: New Variables
	pCopy->m_wContribPrice = pOriginal->m_wContribPrice;
	pCopy->m_wEkPrice = pOriginal->m_wEkPrice;
	pCopy->m_wCoinPrice = pOriginal->m_wCoinPrice;

	pCopy->bEkSale = pOriginal->bEkSale;
	pCopy->bContrbSale = pOriginal->bContrbSale;
	pCopy->bCoinSale = pOriginal->bCoinSale;

	pCopy->m_sNewEffect1 = pOriginal->m_sNewEffect1;
	pCopy->m_sNewEffect2 = pOriginal->m_sNewEffect2;
	pCopy->m_sNewEffect3 = pOriginal->m_sNewEffect3;
	pCopy->m_sNewEffect4 = pOriginal->m_sNewEffect4;

	pCopy->m_iClass = pOriginal->m_iClass;

	pCopy->m_iReqStat = pOriginal->m_iReqStat;
	pCopy->m_iQuantStat = pOriginal->m_iQuantStat;

	return true;
}