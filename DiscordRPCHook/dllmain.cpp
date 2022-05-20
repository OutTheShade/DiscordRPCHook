#include "pch.h"

void (*UpdatePresence)(DiscordRichPresence* presence);

auto MyPresence = DiscordRichPresence();

inline void UpdatePresenceDetour(DiscordRichPresence* presence)
{
	/*
	if (presence != &MyPresence)
	{
		printf("State: %s\n", presence->state);
		printf("Details: %s\n", presence->details);
		printf("Join Secret: %s\n", presence->joinSecret);
		printf("Party Id: %s\n", presence->partyId);
		printf("Party Size: %d\n", presence->partySize);
		printf("Party Max: %d\n", presence->partyMax);
		printf("Small Image Key: %s\n", presence->smallImageKey);
		printf("Small Image Text: %s\n", presence->smallImageText);
		printf("Large Image Key: %s\n", presence->largeImageKey);
		printf("Large Image Text: %s\n", presence->largeImageText);
	}
	*/

	UpdatePresence(&MyPresence);
}

void WINAPI Main()
{
	auto DiscordRPCAddy =
		GetProcAddress(
			GetModuleHandle(L"discord-rpc.dll"),
			"Discord_UpdatePresence");

	if (!DiscordRPCAddy) return;

	UpdatePresence = decltype(UpdatePresence)(DiscordRPCAddy);

	MyPresence.details = "DiscordRPCHook";
	MyPresence.state = "DiscordRPCHook";
	MyPresence.smallImageText = "DiscordRPCHook";
	MyPresence.largeImageText = "DiscordRPCHook";
	MyPresence.partySize = 1337;
	MyPresence.partyMax = 1337;
	MyPresence.smallImageKey = "battlepass"; //different for each game

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)UpdatePresence, UpdatePresenceDetour);

	if (DetourTransactionCommit() != NO_ERROR) return;

	UpdatePresence(&MyPresence);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

