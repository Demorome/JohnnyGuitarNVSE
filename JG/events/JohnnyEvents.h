#pragma once
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnDyingEventHandler, SetOnDyingEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnStartQuestEventHandler, SetOnStartQuestEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnStopQuestEventHandler, SetOnStopQuestEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnySeenDataEventHandler, SetSeenDataEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnLimbGoneEventHandler, SetOnLimbGoneEventHandler, , 0, 5, kParams_Event_OneForm_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnChallengeCompleteEventHandler, SetOnChallengeCompleteEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnCrosshairEventHandler, SetOnCrosshairEventHandler, , 0, 5, kParams_Event_OneForm_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnFailQuestEventHandler, SetOnFailQuestEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnCompleteQuestEventHandler, SetOnCompleteQuestEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnSettingsUpdateEventHandler, SetOnSettingsUpdateEventHandler, , 0, 3, kParams_Event);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnAddPerkEventHandler, SetOnAddPerkEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnRemovePerkEventHandler, SetOnRemovePerkEventHandler, , 0, 4, kParams_Event_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(SetJohnnyOnRenderUpdateEventHandler, SetOnRenderUpdateEventHandler, , 0, 3, kParams_Event);

EventInformation* OnDyingHandler;
EventInformation* OnStartQuestHandler;
EventInformation* OnStopQuestHandler;
EventInformation* OnFailQuestHandler;
EventInformation* OnCompleteQuestHandler;
EventInformation* OnSeenDataUpdateHandler;
EventInformation* OnLimbGoneHandler;
EventInformation* OnChallengeCompleteHandler;
EventInformation* OnCrosshairHandler;
EventInformation* OnSettingsUpdateHandler;
EventInformation* OnAddPerkHandler;
EventInformation* OnRemovePerkHandler;
EventInformation* OnRenderGameModeUpdateHandler;
EventInformation* OnRenderRenderedMenuUpdateHandler;

void __fastcall handleRemovePerkEvent(Actor* actor, int EDX, BGSPerk* perk, bool isTeammatePerk)
{
	if (!actor->GetPerkRank(perk, isTeammatePerk))
		return;
	for (auto const& callback : OnRemovePerkHandler->event_callbacks)
	{
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, perk)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, actor, 0, &EventResultPtr, OnRemovePerkHandler->num_max_args, perk);
		}
	}
	actor->RemovePerk(perk, isTeammatePerk);
}

void __fastcall handleAddPerkEvent(Actor* actor, int EDX, BGSPerk* perk, UInt8 newRank, bool isTeammatePerk)
{
	for (auto const& callback : OnAddPerkHandler->event_callbacks)
	{
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, perk)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, actor, 0, &EventResultPtr, OnAddPerkHandler->num_max_args, perk, newRank - 1, newRank);
		}
	}
	actor->SetPerkRank(perk, newRank, isTeammatePerk);
}

void __stdcall handleDyingEvent(Actor* thisObj) {
	if (thisObj->IsActor() && thisObj->lifeState == 1 && (*thisObj->GetTheName() || thisObj == g_thePlayer)) {
		for (auto const& callback : OnDyingHandler->event_callbacks) {
			if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, thisObj)) // 0 is filter one, and we only use an argument so we don't need to check further filters
			{
				FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnDyingHandler->num_max_args, thisObj);
			}
		}
	}
}
UInt32 __fastcall handleCrosshairEvent(TESObjectREFR* crosshairRef) {
	if (crosshairRef) {
		for (auto const& callback : OnCrosshairHandler->event_callbacks) {
			auto filter = reinterpret_cast<GenericEventFilters*>(callback.eventFilter);
			if ((filter->IsInFilter(0, crosshairRef) || filter->IsBaseInFilter(0, crosshairRef)) && filter->IsInFilter(1, crosshairRef->baseForm->typeID))
			{
				FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnCrosshairHandler->num_max_args, crosshairRef);
			}
		}
	}
	return ThisStdCall<UInt32>(0x579280, crosshairRef);
}
bool __fastcall HandleLimbGoneEvent(ExtraDismemberedLimbs* xData, Actor* actor, byte dummy, int limb, byte isExplode) {
	for (auto const& callback : OnLimbGoneHandler->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsInFilter(0, actor) &&
			reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsInFilter(1, limb))
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnLimbGoneHandler->num_max_args, actor, limb);
		}
	}
	return ThisStdCall_B(0x430410, xData, actor, limb, isExplode);
}
void __fastcall handleQuestStartStop(TESQuest* Quest, bool IsStarted) {
	EventInformation* thisEvent = IsStarted ? OnStartQuestHandler : OnStopQuestHandler;
	for (auto const& callback : thisEvent->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, Quest)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, thisEvent->num_max_args, Quest);
		}
	}
}

void __cdecl handleQuestComplete(TESQuest* Quest) {
	for (auto const& callback : OnCompleteQuestHandler->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, Quest)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnCompleteQuestHandler->num_max_args, Quest);
		}
	}
	CdeclCall(0x77A480, Quest);
}

void __cdecl handleQuestFail(TESQuest* Quest) {
	for (auto const& callback : OnFailQuestHandler->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, Quest)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnFailQuestHandler->num_max_args, Quest);
		}
	}
	CdeclCall(0x77A480, Quest);
}

void __cdecl handleSettingsUpdate() {
	CdeclCall(0x7D6D70);
	for (auto const& callback : OnSettingsUpdateHandler->event_callbacks) {
		FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnSettingsUpdateHandler->num_max_args);
	}
}
ExtraDataList* __fastcall HandleSeenDataUpdateEvent(TESObjectCELL* cell) {
	for (auto const& callback : OnSeenDataUpdateHandler->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, cell)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnSeenDataUpdateHandler->num_max_args, cell);
		}
	}
	return &cell->extraDataList;
}
UInt32 __fastcall HandleChallengeCompleteEvent(TESChallenge* challenge) {
	for (auto const& callback : OnChallengeCompleteHandler->event_callbacks) {
		if (reinterpret_cast<GenericEventFilters*>(callback.eventFilter)->IsBaseInFilter(0, challenge)) // 0 is filter one, and we only use an argument so we don't need to check further filters
		{
			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnChallengeCompleteHandler->num_max_args, challenge);
		}
	}
	return challenge->data.type;
}


UInt32 __fastcall handlerRenderGameEvent(void* ECX, void* edx, int arg1, int arg2, int arg3) {
	for (auto const& callback : OnRenderGameModeUpdateHandler->event_callbacks) {

			FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnRenderGameModeUpdateHandler->num_max_args);

	}
	return ThisStdCall<UInt32>(0x08706B0, ECX, arg1, arg2, arg3);
}

UInt32 __fastcall handlerRenderMenuEvent(void* ECX, void* edx, int arg1, int arg2, int arg3) {
	for (auto const& callback : OnRenderRenderedMenuUpdateHandler->event_callbacks) {

		FunctionCallScript(callback.ScriptForEvent, NULL, 0, &EventResultPtr, OnRenderRenderedMenuUpdateHandler->num_max_args);

	}
	return ThisStdCall<UInt32>(0x08706B0, ECX, arg1, arg2, arg3);
}
__declspec(naked) void OnCrosshairEventAsm() {
	static const UInt32 retnAddr = 0x775A69;
	__asm {
		mov ecx, [ebp + 0x8]
		call handleCrosshairEvent
		movzx ecx, [ebp + 0x10]
		test ecx, ecx
		jmp retnAddr
	}
}
__declspec (naked) void OnDyingEventAsm()
{
	static const UInt32 checkProtect = 0xEC408C;
	__asm
	{
		push dword ptr[ebp - 0x18]
		call handleDyingEvent
		mov ecx, dword ptr ss : [ebp - 0xC]
		mov dword ptr fs : [0] , ecx
		pop ecx
		pop esi
		mov ecx, dword ptr ss : [ebp - 0x14]
		xor ecx, ebp
		call checkProtect
		mov esp, ebp
		pop ebp
		mov esp, ebx
		pop ebx
		retn 8
	}
}

__declspec (naked) void OnQuestStartStopEventAsm()
{
	__asm
	{
		mov ecx, dword ptr[ebp - 4]
		mov dl, byte ptr[ebp + 8]
		call handleQuestStartStop
		mov esp, ebp
		pop ebp
		ret 4
	}
}
bool Cmd_SetJohnnyOnLimbGoneEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = nullptr;
	EventFilter_OneForm_OneInt filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form, &filter.intID) || NOT_TYPE(script, Script))) return true;
	{
		if (OnLimbGoneHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnLimbGoneHandler->RegisterEvent(script, filterArr);
			else OnLimbGoneHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnSettingsUpdateEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = nullptr;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	{
		if (OnSettingsUpdateHandler)
		{
			if (setOrRemove)
				OnSettingsUpdateHandler->RegisterEvent(script);
			else OnSettingsUpdateHandler->RemoveEvent(script);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnCrosshairEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = nullptr;
	EventFilter_OneForm_OneInt filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form, &filter.intID) || NOT_TYPE(script, Script))) return true;
	{
		if (OnCrosshairHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnCrosshairHandler->RegisterEvent(script, filterArr);
			else OnCrosshairHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnRemovePerkEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnRemovePerkHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnRemovePerkHandler->RegisterEvent(script, filterArr);
			else OnRemovePerkHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnAddPerkEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter) || NOT_TYPE(script, Script))) return true;
	{
		if (OnAddPerkHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnAddPerkHandler->RegisterEvent(script, filterArr);
			else OnAddPerkHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnChallengeCompleteEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnChallengeCompleteHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnChallengeCompleteHandler->RegisterEvent(script, filterArr);
			else OnChallengeCompleteHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnySeenDataEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnSeenDataUpdateHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnSeenDataUpdateHandler->RegisterEvent(script, filterArr);
			else OnSeenDataUpdateHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}
bool Cmd_SetJohnnyOnDyingEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnDyingHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnDyingHandler->RegisterEvent(script, filterArr);
			else OnDyingHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}

bool Cmd_SetJohnnyOnStartQuestEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnStartQuestHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnStartQuestHandler->RegisterEvent(script, filterArr);
			else OnStartQuestHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}


bool Cmd_SetJohnnyOnStopQuestEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnStopQuestHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnStopQuestHandler->RegisterEvent(script, filterArr);
			else OnStopQuestHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}

bool Cmd_SetJohnnyOnCompleteQuestEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnCompleteQuestHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnCompleteQuestHandler->RegisterEvent(script, filterArr);
			else OnCompleteQuestHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}

bool Cmd_SetJohnnyOnFailQuestEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	EventFilter_OneForm filter;
	UInt32 flags = 0;
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags, &filter.form) || NOT_TYPE(script, Script))) return true;
	{
		if (OnFailQuestHandler)
		{
			auto filterArr = filter.ToFilter();
			if (setOrRemove)
				OnFailQuestHandler->RegisterEvent(script, filterArr);
			else OnFailQuestHandler->RemoveEvent(script, filterArr);
		}
		return true;
	}
}

bool Cmd_SetJohnnyOnRenderUpdateEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove = 0;
	Script* script = NULL;
	UInt32 flags = 0;
	enum EnumFlags {
		kDoNotFireInRenderMenu = 1 << 0,
		kDoNotFireInGameMode = 1 << 1,
	};
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	{
		if (!(flags & kDoNotFireInGameMode) && OnRenderGameModeUpdateHandler)
		{
			if (setOrRemove)
				OnRenderGameModeUpdateHandler->RegisterEvent(script);
			else OnRenderGameModeUpdateHandler->RemoveEvent(script);
		}

		if (!(flags & kDoNotFireInRenderMenu) && OnRenderRenderedMenuUpdateHandler)
		{
			if (setOrRemove)
				OnRenderRenderedMenuUpdateHandler->RegisterEvent(script);
			else OnRenderRenderedMenuUpdateHandler->RemoveEvent(script);
		}
		return true;
	}
}

void HandleEventHooks()
{
	OnDyingHandler = JGCreateEvent("OnDying", 1, 1);
	OnStartQuestHandler = JGCreateEvent("OnStartQuest", 1, 1);
	OnStopQuestHandler = JGCreateEvent("OnStopQuest", 1, 1);
	OnSeenDataUpdateHandler = JGCreateEvent("OnSeenDataUpdate", 1, 1);
	OnLimbGoneHandler = JGCreateEvent("OnLimbGone", 2, 2);
	OnChallengeCompleteHandler = JGCreateEvent("OnChallengeComplete", 1, 1);
	OnCrosshairHandler = JGCreateEvent("OnCrosshair", 1, 2);
	OnCompleteQuestHandler = JGCreateEvent("OnCompleteQuest", 1, 1);
	OnFailQuestHandler = JGCreateEvent("OnFailQuest", 1, 1);
	OnSettingsUpdateHandler = JGCreateEvent("OnSettingsUpdate", 0, 0);
	OnAddPerkHandler = JGCreateEvent("OnAddPerk", 3, 1);
	OnRemovePerkHandler = JGCreateEvent("OnRemovePerk", 1, 1);
	FunctionCallScript = g_scriptInterface->CallFunction;
	WriteRelCall(0x55678A, (UINT)HandleSeenDataUpdateEvent);
	WriteRelCall(0x557053, (UINT)HandleSeenDataUpdateEvent);
	WriteRelJump(0x89F4A4, (UINT)OnDyingEventAsm);
	WriteRelJump(0x60CA24, (UINT)OnQuestStartStopEventAsm);
	WriteRelCall(0x572FF1, (UINT)HandleLimbGoneEvent);
	WriteRelCall(0x5F5C78, (UINT)HandleChallengeCompleteEvent);
	WriteRelCall(0x5F6222, (UINT)HandleChallengeCompleteEvent);
	WriteRelCall(0x776010, (UINT)handleCrosshairEvent);
	WriteRelCall(0x60CB5A, (UINT)handleQuestFail);
	WriteRelCall(0x60CA78, (UINT)handleQuestComplete);
	WriteRelCall(0x7CEC93, (UINT)handleSettingsUpdate);
	WriteRelCall(0x7D11AD, (UINT)handleSettingsUpdate);
	WriteRelCall(0x5D4E5B, (UINT)handleAddPerkEvent);
	SafeWriteBuf(0x5D4E60, "\x0F\x1F\x00", 3);
	WriteRelCall(0x5D4F89, (UINT)handleRemovePerkEvent);
	SafeWriteBuf(0x5D4F8E, "\x0F\x1F\x00", 3);
	SafeWrite8(0x60CA29, 0xCC);


	//testing
	OnRenderGameModeUpdateHandler = JGCreateEvent("OnRenderGameModeUpdateHandler", 0, 0);
	WriteRelCall(0x870244, (uintptr_t)handlerRenderGameEvent);
	OnRenderRenderedMenuUpdateHandler = JGCreateEvent("OnRenderRenderedMenuUpdateHandler", 0, 0);
	WriteRelCall(0x8702A9, (uintptr_t)handlerRenderMenuEvent);

}
