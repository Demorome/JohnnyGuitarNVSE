#pragma once
// Functions affecting gameplay
DEFINE_COMMAND_PLUGIN(ToggleLevelUpMenu, , 0, 1, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(TogglePipBoy, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(Jump, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(StopVATSCam, , 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(SetCameraShake, , 0, 2, kParams_TwoFloats);
DEFINE_COMMAND_PLUGIN(ApplyWeaponPoison, , 1, 1, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(SetVelEx, , 1, 3, kParams_ThreeFloats);
DEFINE_COMMAND_PLUGIN(StopSoundAlt, , 0, 2, kParams_TwoForms);
DEFINE_COMMAND_PLUGIN(DisableMuzzleFlashLights, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(UnsetAV, , 1, 1, kParams_OneActorValue);
DEFINE_COMMAND_PLUGIN(UnforceAV, , 1, 1, kParams_OneActorValue);
DEFINE_COMMAND_PLUGIN(ToggleNthPipboyLight, , 0, 2, kParams_TwoInts);
DEFINE_COMMAND_PLUGIN(GetRunSpeed, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(DisableMenuArrowKeys, , 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(EnableMenuArrowKeys, , 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(HighlightBodyPartAlt, , 1, 1, kParams_OneOptionalFloat);
DEFINE_COMMAND_PLUGIN(DeactivateAllHighlightsAlt, , 1, 1, kParams_OneOptionalFloat);
DEFINE_COMMAND_PLUGIN(GetNearestCompassHostileDirection, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetNearestCompassHostile, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(SetDisablePlayerControlsHUDVisibilityFlags, SetDPCHUDFlags, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(IsCompassHostile, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(ToggleCombatMusic, , 0, 1, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(IsCombatMusicEnabled, , 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(IsHostilesNearby, , 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(ModNthTempEffectTimeLeft, , 1, 2, kParams_OneInt_OneFloat);
DEFINE_COMMAND_PLUGIN(GetCalculatedSpread, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(SendStealingAlarm, , 1, 1, kParams_OneRef);
DEFINE_COMMAND_PLUGIN(GetCompassHostiles, , 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(ToggleDisableSaves, , 0, 1, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(SendTrespassAlarmAlt, , 1, 0, NULL);
DEFINE_COMMAND_PLUGIN(IsCrimeOrEnemy, , 1, 0, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetLocationSpecificLoadScreensOnly, , , 0, NULL);
DEFINE_COMMAND_PLUGIN(GetLocationName, , 1, 0, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(HasScriptPackage, , , 1, NULL);
DEFINE_COMMAND_PLUGIN(GetPlayingEffectShaders, , 1, 0, NULL);
void(__cdecl* HandleActorValueChange)(ActorValueOwner* avOwner, int avCode, float oldVal, float newVal, ActorValueOwner* avOwner2) =
(void(__cdecl*)(ActorValueOwner*, int, float, float, ActorValueOwner*))0x66EE50;
bool(*Cmd_HighLightBodyPart)(COMMAND_ARGS) = (bool (*)(COMMAND_ARGS)) 0x5BB570;
bool(*Cmd_DeactivateAllHighlights)(COMMAND_ARGS) = (bool (*)(COMMAND_ARGS)) 0x5BB6C0;
void(__cdecl* HUDMainMenu_UpdateVisibilityState)(signed int) = (void(__cdecl*)(signed int))(0x771700);
#define NUM_ARGS *((UInt8*)scriptData + *opcodeOffsetPtr)

bool Cmd_GetPlayingEffectShaders_Execute(COMMAND_ARGS) {
	*result = 0;
	ListNode<BSTempEffect>* iter = g_processManager->tempEffects.Head();
	MagicShaderHitEffect* effect;
	NVSEArrayVar* effArr = g_arrInterface->CreateArray(NULL, 0, scriptObj);

	do
	{
		effect = (MagicShaderHitEffect*)iter->data;
		if (effect && IS_TYPE(effect, MagicShaderHitEffect) && effect->target && effect->target->refID == thisObj->refID) {
			g_arrInterface->AppendElement(effArr, NVSEArrayElement(effect->effectShader));
		}
	} while (iter = iter->next);

	if (g_arrInterface->GetArraySize(effArr)) g_arrInterface->AssignCommandResult(effArr, result);
	return true;
}

bool HasScriptPackage(TESObjectREFR* ref) {
	Actor* actor = DYNAMIC_CAST(ref, TESObjectREFR, Actor);
	if (actor && actor->baseProcess) {
		TESPackage* package = actor->baseProcess->GetStablePackage();
		if (package) {
			ExtraPackage* xPackage = (ExtraPackage*)actor->extraDataList.GetByType(kExtraData_Package);
			if (xPackage && xPackage->package && (xPackage->package->packageFlags & 0x800 != 0)) return true;
		}
	}
	return false;
}

bool Cmd_HasScriptPackage_Eval(COMMAND_ARGS_EVAL) {
	*result = HasScriptPackage(thisObj);
	return true;
}

bool Cmd_HasScriptPackage_Execute(COMMAND_ARGS) {
	*result = HasScriptPackage(thisObj);
	return true;
}


TESWorldSpace* GetWorldspace(TESObjectREFR* ref) {
	TESObjectCELL* cell = ref->parentCell;
	if (!cell) cell = ref->childCell.GetPersistentCell();
	if (cell && (cell->cellFlags & 1) == 0) return cell->worldSpace;
	return nullptr;
}

bool Cmd_GetLocationName_Execute(COMMAND_ARGS) {
	*result = 0;
	const char* locationName = nullptr;
	if (thisObj->parentCell && (thisObj->parentCell->cellFlags & 1) != 0) {
		locationName = thisObj->parentCell->fullName.name.CStr();
	}
	else {
		TESWorldSpace* wspc = GetWorldspace(thisObj);
		if (wspc) {
			String* stringBuf = (String*)GameHeapAlloc(sizeof(String));;
			NiPoint3* pos = thisObj->GetPos();
			wspc->Unk_4E(stringBuf, *pos);
			locationName = stringBuf->CStr();
			GameHeapFree(stringBuf);
		}
	}
	if (locationName) g_strInterface->Assign(PASS_COMMAND_ARGS, locationName);
	return true;
}

bool Cmd_GetLocationSpecificLoadScreensOnly_Execute(COMMAND_ARGS) {
	*result = (float)(*(UInt8*)0x11CABB8);
	return true;
}

bool Cmd_GetLocationSpecificLoadScreensOnly_Eval(COMMAND_ARGS_EVAL) {
	*result = (float)(*(UInt8*)0x11CABB8);
	return true;
}

bool IsCombatTarget(Actor* source, Actor* toSearch) {
	if (source->isInCombat && source->combatTargets) {
		Actor** actorsArr = source->combatTargets->data;
		UInt32 count = source->combatTargets->size;
		if (!actorsArr) return false;
		for (; count; count--, actorsArr++)
		{
			if (*actorsArr == toSearch) return true;
		}
	}
	return false;
}

bool IsHostileCompassTarget(Actor* toSearch) {
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile && target->target == toSearch) return true;
	}
	return false;
}
bool Cmd_IsCrimeOrEnemy_Execute(COMMAND_ARGS) {
	*result = 0;
	Actor* actor = (Actor*)thisObj;
	if (ThisStdCall_B(0x579690, thisObj) && (!thisObj->IsActor() || !actor->isTeammate) ||
		thisObj->IsActor() && (IsCombatTarget(actor, g_thePlayer) || IsHostileCompassTarget(actor))) {
		*result = 1;
	}
	if (IsConsoleMode()) Console_Print("IsCrimeOrEnemy >> %.f", *result);
	return true;
}

bool Cmd_SendTrespassAlarmAlt_Execute(COMMAND_ARGS) {
	*result = 0;
	ExtraOwnership* xOwn = ThisStdCall<ExtraOwnership*>(0x567790, thisObj); // TESObjectREFR::ResolveOwnership
	if (xOwn) {
		ThisStdCall(0x8C0EC0, g_thePlayer, thisObj, xOwn, 0xFFFFFFFF); //TESObjectREFR::HandleMinorCrime 
		*result = 1;
	}
	return true;
}
bool Cmd_GetCompassHostiles_Execute(COMMAND_ARGS) {
	*result = 0;
	UInt32 skipInvisible = 0;
	ExtractArgsEx(EXTRACT_ARGS_EX, &skipInvisible);
	NVSEArrayVar* hostileArr = g_arrInterface->CreateArray(NULL, 0, scriptObj);
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile)
		{
			if (skipInvisible > 0 && (target->target->avOwner.Fn_02(kAVCode_Invisibility) > 0 || target->target->avOwner.Fn_02(kAVCode_Chameleon) > 0)) {
				continue;
			}
			g_arrInterface->AppendElement(hostileArr, NVSEArrayElement(target->target));
		}
	}
	if (g_arrInterface->GetArraySize(hostileArr)) g_arrInterface->AssignCommandResult(hostileArr, result);
	return true;
}
bool Cmd_SendStealingAlarm_Execute(COMMAND_ARGS)
{
	TESObjectREFR* container;
	*result = 0;
	if (thisObj->IsActor() && ExtractArgsEx(EXTRACT_ARGS_EX, &container))
	{
		ExtraOwnership* xOwn = ThisStdCall<ExtraOwnership*>(0x567790, container); // TESObjectREFR::ResolveOwnership
		ThisStdCall(0x8BFA40, thisObj, container, NULL, NULL, 1, xOwn); // Actor::HandleStealing, 
		*result = 1;
	}
	return true;
}

bool Cmd_GetCalculatedSpread_Execute(COMMAND_ARGS) {
	*result = 0;
	Actor* actor = static_cast<Actor*>(thisObj);
	ContChangesEntry* weapInfo = actor->baseProcess->GetWeaponInfo();
	if (weapInfo && weapInfo->type) {
		bool hasDecreaseSpreadEffect = ThisStdCall<bool>(0x4BDA70, weapInfo, 3);
		double minSpread = ThisStdCall<double>(0x524B80, weapInfo->type, hasDecreaseSpreadEffect);
		double weapSpread = ThisStdCall<float>(0x524BE0, weapInfo->type, hasDecreaseSpreadEffect);
		double spread = ThisStdCall<double>(0x8B0DD0, PlayerCharacter::GetSingleton(), 1);

		float totalSpread = (weapSpread * spread + minSpread) * 0.01745329238474369;

		TESAmmo* eqAmmo = ThisStdCall<TESAmmo*>(0x525980, weapInfo->type, static_cast<MobileObject*>(actor));
		totalSpread = CdeclCall<float>(0x59A030, 3, (eqAmmo ? &eqAmmo->effectList : nullptr), totalSpread);

		double spreadPenalty = ThisStdCall<double>(0x8B0DD0, PlayerCharacter::GetSingleton(), 2);

		Setting* fNPCMaxGunWobbleAngle;
		GameSettingCollection::GetSingleton()->GetGameSetting("fNPCMaxGunWobbleAngle", &fNPCMaxGunWobbleAngle);

		totalSpread += spreadPenalty * fNPCMaxGunWobbleAngle->data.f * 0.01745329238474369;
		
		float noIdea = ThisStdCall<HighProcess*>(0x8D8520, actor)->angle1D0;
		totalSpread = totalSpread + noIdea;

		bool hasSplitBeamEffect = ThisStdCall<bool>(0x4BDA70, weapInfo, 0xC);
		if (hasSplitBeamEffect) {
			totalSpread *= ThisStdCall<float>(0x4BCF60, weapInfo->type, 0xC, 1);
		}
		*result = totalSpread;
	}
	if (IsConsoleMode()) Console_Print("GetCalculatedSpread >> %f", *result);
	return true;
}


bool __fastcall ValidTempEffect(EffectItem* effectItem)
{
	if (!effectItem || (effectItem->duration <= 0) || !effectItem->setting) return false;
	UInt8 archtype = effectItem->setting->archtype;
	return !archtype || ((archtype == 1) && (effectItem->setting->effectFlags & 0x2000)) || ((archtype > 10) && (archtype < 14)) || (archtype == 24) || (archtype > 33);
}

bool Cmd_ModNthTempEffectTimeLeft_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 index;
	float modTimeLeft;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &index, &modTimeLeft) || !thisObj->IsActor()) return true;
	ActiveEffectList* effList = ((Actor*)thisObj)->magicTarget.GetEffectList();
	if (!effList) return true;
	ListNode<ActiveEffect>* iter = effList->Head();
	ActiveEffect* activeEff;
	do
	{
		activeEff = iter->data;
		if (!activeEff || !activeEff->bApplied || !ValidTempEffect(activeEff->effectItem) || !activeEff->magicItem ||
			!DYNAMIC_CAST(activeEff->magicItem, MagicItem, TESForm)) continue;
		if (!index--)
		{
			activeEff->timeElapsed += -modTimeLeft;
			if (activeEff->timeElapsed > activeEff->duration) activeEff->Remove(true);
			*result = 1;
			break;
		}
	} while (iter = iter->next);
	return true;
}
bool Cmd_IsHostilesNearby_Execute(COMMAND_ARGS) {
	*result = 0;
	TESObjectCELL* actorCell = g_thePlayer->parentCell;
	if (actorCell)
		*result = ThisStdCall_B(0x9764A0, g_processManager, actorCell->IsInterior());
	return true;
}
bool Cmd_ToggleCombatMusic_Execute(COMMAND_ARGS) {
	UInt32 toggle = 1;
	ExtractArgsEx(EXTRACT_ARGS_EX, &toggle);
	bCombatMusicDisabled = (toggle == 0);
	return true;
}
bool Cmd_IsCombatMusicEnabled_Execute(COMMAND_ARGS) {
	*result = (bCombatMusicDisabled == 0);
	if (IsConsoleMode()) Console_Print("IsCombatMusicEnabled >> %.f", *result);
	return true;
}
bool Cmd_IsCompassHostile_Execute(COMMAND_ARGS) {
	*result = 0;
	Actor* toCheck = (Actor*)thisObj;
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile && target->target == toCheck)
		{
			*result = 1;
			break;
		}
	}
	if (IsConsoleMode()) Console_Print("IsCompassHostile >> %.f", *result);
	return true;
}
void RestoreDisabledPlayerControlsHUDFlags()
{
	SafeWrite32(0x771A53, HUDMainMenu::kXpMeter | HUDMainMenu::kSubtitles | HUDMainMenu::kMessages | HUDMainMenu::kQuestReminder | HUDMainMenu::kRadiationMeter);
}

bool Cmd_SetDisablePlayerControlsHUDVisibilityFlags_Execute(COMMAND_ARGS)
{
	UInt32 flags;
	if (NUM_ARGS && ExtractArgsEx(EXTRACT_ARGS_EX, &flags))
	{
		SafeWrite32(0x771A53, flags);
		HUDMainMenu_UpdateVisibilityState(HUDMainMenu::kHUDState_RECALCULATE);
	}
	else
	{
		RestoreDisabledPlayerControlsHUDFlags();
	}

	return true;
}
bool Cmd_GetNearestCompassHostile_Execute(COMMAND_ARGS)
{
	*result = -1;

	NiPoint3* playerPos = g_thePlayer->GetPos();

	float fSneakMaxDistance = *(float*)(0x11CD7D8 + 4);
	float fSneakExteriorDistanceMult = *(float*)(0x11CDCBC + 4);
	bool isInterior = g_thePlayer->GetParentCell()->IsInterior();
	float interiorDistanceSquared = fSneakMaxDistance * fSneakMaxDistance;
	float exteriorDistanceSquared = (fSneakMaxDistance * fSneakExteriorDistanceMult) * (fSneakMaxDistance * fSneakExteriorDistanceMult);
	float maxDist = isInterior ? interiorDistanceSquared : exteriorDistanceSquared;
	Actor* closestHostile = nullptr;
	UInt32 skipInvisible = 0;
	ExtractArgsEx(EXTRACT_ARGS_EX, &skipInvisible);
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile)
		{
			if (skipInvisible > 0 && (target->target->avOwner.Fn_02(kAVCode_Invisibility) > 0 || target->target->avOwner.Fn_02(kAVCode_Chameleon) > 0)) {
				continue;
			}
			auto distToPlayer = target->target->GetPos()->CalculateDistSquared(playerPos);
			if (distToPlayer < maxDist)
			{
				maxDist = distToPlayer;
				closestHostile = target->target;
			}
		}
	}

	if (closestHostile)	*(UInt32*)result = closestHostile->refID;

	return true;
}
bool Cmd_GetNearestCompassHostileDirection_Execute(COMMAND_ARGS)
{
	*result = -1;

	NiPoint3* playerPos = g_thePlayer->GetPos();

	float fSneakMaxDistance = *(float*)(0x11CD7D8 + 4);
	float fSneakExteriorDistanceMult = *(float*)(0x11CDCBC + 4);
	bool isInterior = g_thePlayer->GetParentCell()->IsInterior();
	float maxDist = isInterior ? powf(fSneakMaxDistance, 2) : powf((fSneakMaxDistance * fSneakExteriorDistanceMult), 2);
	Actor* closestHostile = nullptr;
	UInt32 skipInvisible = 0;
	ExtractArgsEx(EXTRACT_ARGS_EX, &skipInvisible);
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile)
		{
			if (skipInvisible > 0 && (target->target->avOwner.Fn_02(kAVCode_Invisibility) > 0 || target->target->avOwner.Fn_02(kAVCode_Chameleon) > 0)) {
				continue;
			}
			auto distToPlayer = target->target->GetPos()->CalculateDistSquared(playerPos);
			if (distToPlayer < maxDist)
			{
				maxDist = distToPlayer;
				closestHostile = target->target;
			}
		}
	}

	if (closestHostile)
	{
		auto playerRotation = g_thePlayer->AdjustRot(0);
		double headingAngle = GetAngleBetweenPoints(closestHostile->GetPos(), playerPos, playerRotation);

		// shift the coordinates from -180:180 to 0:360 and offset them (360 / 8 quadrants / 2) degrees
		int angle = headingAngle + 180 + 22.5;

		// convert the angle to which quadrant the NPC is in
		angle /= 45;
		*result = (angle + 4) % 8; // make 0 in front, 1 front right, 2 right, 3 back right ...
	}

	return true;
}
bool Cmd_HighlightBodyPartAlt_Execute(COMMAND_ARGS)
{
	return Cmd_HighLightBodyPart(PASS_COMMAND_ARGS);
}

bool Cmd_DeactivateAllHighlightsAlt_Execute(COMMAND_ARGS)
{
	return Cmd_DeactivateAllHighlights(PASS_COMMAND_ARGS);
}

bool Cmd_DisableMenuArrowKeys_Execute(COMMAND_ARGS) {
	if (!bArrowKeysDisabled) {
		bArrowKeysDisabled = true;
	}
	return true;
}
bool Cmd_EnableMenuArrowKeys_Execute(COMMAND_ARGS) {
	if (bArrowKeysDisabled) {
		bArrowKeysDisabled = false;
	}
	return true;
}
bool Cmd_GetRunSpeed_Execute(COMMAND_ARGS) {
	*result = 0;
	Actor* actor = (Actor*)thisObj;
	*result = ThisStdCall_F(0x884EB0, actor);
	if (IsConsoleMode()) Console_Print("GetRunSpeed >> %.2f", *result);
	return true;
}
bool Cmd_ToggleNthPipboyLight_Execute(COMMAND_ARGS)
{
	UInt32 index, isVisible;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &index, &isVisible) && index < 3)
	{
		FOPipboyManager* pipboyManager = g_interfaceManager->pipboyManager;
		if (pipboyManager->byte028)
		{
			if (isVisible)
			{
				pipboyManager->pipboyLightGlow[index]->m_flags &= ~1;
			}
			else
			{
				pipboyManager->pipboyLightGlow[index]->m_flags |= 1;
			}
			*result = 1;
		}
	}
	return true;
}
bool Cmd_UnsetAV_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 avCode;
	if (thisObj->IsActor() && ExtractArgsEx(EXTRACT_ARGS_EX, &avCode))
	{
		Actor* actor = (Actor*)thisObj;
		ActorValueOwner* avOwner = &actor->avOwner;
		float oldVal = avOwner->GetActorValue(avCode);

		tList<void>* actorPermSetAVList = &actor->list0E0;
		void* avEntry = ThisStdCall<void*>(0x937760, actorPermSetAVList, avCode);
		ThisStdCall(0x937400, actorPermSetAVList, avEntry);
		thisObj->MarkAsModified(0x400000);

		if (!actor->IsPlayerRef())
		{
			BaseProcess* base = actor->baseProcess;
			if (base)
			{
				base->Unk_EC(avCode);
			}
		}

		// call handle change with new value
		float newVal = avOwner->GetActorValue(avCode);
		HandleActorValueChange(avOwner, avCode, oldVal, newVal, NULL);
		*result = 1;
	}
	return true;
}

bool Cmd_UnforceAV_Execute(COMMAND_ARGS)
{
	UInt32 avCode;
	*result = 0;
	if (thisObj->IsActor() && ExtractArgsEx(EXTRACT_ARGS_EX, &avCode))
	{
		Actor* actor = (Actor*)thisObj;
		ActorValueOwner* avOwner = &actor->avOwner;
		float oldVal = avOwner->GetActorValue(avCode);

		tList<void>* actorPermForceAVList = &actor->list0D0;
		void* avEntry = ThisStdCall<void*>(0x937760, actorPermForceAVList, avCode);
		ThisStdCall(0x937400, actorPermForceAVList, avEntry);
		thisObj->MarkAsModified(0x800000);

		if (!actor->IsPlayerRef())
		{
			BaseProcess* base = actor->baseProcess;
			if (base)
			{
				base->Unk_EC(avCode);
			}
		}

		// call handle change with new value
		float newVal = avOwner->GetActorValue(avCode);
		HandleActorValueChange(avOwner, avCode, oldVal, newVal, NULL);
		*result = 1;
	}
	return true;
}

bool Cmd_StopSoundAlt_Execute(COMMAND_ARGS) {
	TESSound* soundForm;
	TESObjectREFR* source;
	BSFadeNode* fadeNode;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &soundForm, &source))
	{
		if (soundForm->soundFile.path.m_dataLen)
		{
			const char* soundPath = soundForm->soundFile.path.m_data;
			BSGameSound* gameSound;
			for (auto sndIter = g_audioManager->playingSounds.Begin(); !sndIter.End(); ++sndIter)
			{
				gameSound = sndIter.Get();
				if (gameSound && StrBeginsCI(gameSound->filePath + 0xB, soundPath)) {
					fadeNode = (BSFadeNode*)g_audioManager->soundPlayingObjects.Lookup(gameSound->mapKey);
					if (fadeNode && fadeNode->GetFadeNode() && fadeNode->linkedObj && fadeNode->linkedObj == source)
					{
						gameSound->stateFlags &= 0xFFFFFF0F;
						gameSound->stateFlags |= 0x10;
						*result = 1;
						break;
					}
				}
			}
		}
	}
	return true;
}

bool Cmd_SetVelEx_Execute(COMMAND_ARGS) {
	NiPoint3 Point;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &(Point.x), &(Point.y), &(Point.z))) {
		((void(__cdecl*)(NiNode*, NiPoint3*, int))(0x62B8D0))(thisObj->GetNiNode(), &Point, 1);
		*result = 1;
	}
	return true;
}

bool Cmd_ApplyWeaponPoison_Execute(COMMAND_ARGS) {
	AlchemyItem* poison;
	TESObjectWEAP* weapon = nullptr;
	ExtraDataList* xData;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &poison) && IS_TYPE(poison, AlchemyItem) && poison->IsPoison()) {
		if (!thisObj->IsActor()) {
			InventoryRef* invRef = InventoryRefGetForID(thisObj->refID);
			if (!invRef) return true;
			weapon = (TESObjectWEAP*)(invRef->data.type);
			xData = invRef->data.xData;
		}
		else {
			ContChangesEntry* wpnInfo = ((Actor*)thisObj)->baseProcess->GetWeaponInfo();
			if (wpnInfo && wpnInfo->extendData)
			{
				weapon = ((TESObjectWEAP*)wpnInfo->type);
				xData = wpnInfo->extendData->GetFirstItem();
			}
		}
		if (weapon) {
			UInt32 weaponSkill = weapon->weaponSkill;
			if (weaponSkill != kAVCode_Unarmed && weaponSkill != kAVCode_MeleeWeapons) return true;
			if (xData)
			{
				ExtraPoison* xPoison = GetExtraType((*xData), Poison);
				if (!xPoison) {
					ThisStdCall(0x419D10, xData, poison); // ExtraDataList::UpdateExtraPoison
					*result = 1;
				}
			}
		}
	}
	return true;
}

bool Cmd_TogglePipBoy_Execute(COMMAND_ARGS) {
	int pipboyTab = 0;
	ExtractArgsEx(EXTRACT_ARGS_EX, &pipboyTab);
	*result = 0;
	if (pipboyTab == 0 || pipboyTab == 1002 || pipboyTab == 1003 || pipboyTab == 1023) {
		if (g_interfaceManager) {
			if (!g_interfaceManager->pipBoyMode) {
				ThisStdCall(0x70F4E0, g_interfaceManager, 0, pipboyTab);
			} else if (g_interfaceManager->pipBoyMode == 3) {
				ThisStdCall(0x70F690, g_interfaceManager, 0);
			}
			*result = 1;
		}
	}
	return true;
}

bool Cmd_ToggleLevelUpMenu_Execute(COMMAND_ARGS)
{
	UInt32 ToExtract;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &ToExtract)) isShowLevelUp = ToExtract;
	return true;
}

bool Cmd_Jump_Execute(COMMAND_ARGS) {
	if (!thisObj->IsActor()) return true;
	((MobileObject*)thisObj)->Unk_95();
	return true;
}

bool Cmd_StopVATSCam_Execute(COMMAND_ARGS)
{
	ThisStdCall(0x93E770, g_thePlayer, 2, 0);
	return true;
}

bool Cmd_SetCameraShake_Execute(COMMAND_ARGS) {
	float shakeMult, time;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &shakeMult, &time)) {
		*(float*)(0x11DFED4) = shakeMult;
		*(float*)(0x11DFED8) = time;
		*result = 1;
	}
	return true;
}

bool Cmd_DisableMuzzleFlashLights_Execute(COMMAND_ARGS) {
	int disable = -1;
	ExtractArgsEx(EXTRACT_ARGS_EX, &disable);
	if (disable >= 0 && disable <= 3) disableMuzzleLights = disable;
	*result = disableMuzzleLights;
	if (IsConsoleMode()) Console_Print("DisableMuzzleFlashLights >> %.f", *result);
	return true;
}
bool Cmd_ToggleDisableSaves_Execute(COMMAND_ARGS)
{
	int doDisable = 1;
	BYTE modIdx = scriptObj->GetModIndex();
	*result = 0;
	if (modIdx < 0xFF && ExtractArgsEx(EXTRACT_ARGS_EX, &doDisable))
	{
		if (doDisable)
		{
			SaveGameUMap.insert(modIdx);
		}
		else
		{
			SaveGameUMap.erase(modIdx);
		}
		*result = 1;
	}
	return true;
}