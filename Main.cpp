#include "main.h"

void CL_CreateMove(float frametime, usercmd_s *cmd, int active)
{

	g_Client.CL_CreateMove(frametime, cmd, active);

	if (cmd->buttons&IN_JUMP)
	{
		cmd->buttons &= ~IN_JUMP;

		if (pmove->flags & FL_ONGROUND || pmove->waterlevel >= 2)
		{
			cmd->buttons |= IN_JUMP;
		}
	}


}
bool HookTable(DWORD dwTablePtrPtr)
{
	DWORD dwIndexFunction = 0;

	DWORD dwPtrPtrTable = dwTablePtrPtr;
	DWORD dwPtrOldTable = *(PDWORD)dwPtrPtrTable;

	for (dwIndexFunction = 0; ((PDWORD)*(PDWORD)dwTablePtrPtr)[dwIndexFunction]; dwIndexFunction++)
		if (IsBadCodePtr((FARPROC)((PDWORD)*(PDWORD)dwTablePtrPtr)[dwIndexFunction]))
			break;

	DWORD dwSizeTable = sizeof(DWORD) * dwIndexFunction;

	if (dwIndexFunction && dwSizeTable)
	{
		DWORD dwPtrNewTable = (DWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSizeTable);
		memcpy((PVOID)dwPtrNewTable, (PVOID)*(PDWORD)dwTablePtrPtr, dwSizeTable);

		*(PDWORD)dwTablePtrPtr = dwPtrNewTable;

		return true;
	}

	return false;
}
//		g_pIPanel = (vgui::IPanel*)(CaptureInterface(vgui2_factory, VGUI_PANEL_INTERFACE_VERSION));
void* g_pIPanel;
void SetupHook()
{
	SetupConsole();
	_puts("hi");
	while ( !c_Offset.GetRendererInfo() )
		Sleep( 100 );

	g_pClient = (cl_clientfunc_t*)c_Offset.ClientFuncs();
	if (!g_pClient) _puts("Oh noes! client!");
	g_pEngine = (cl_enginefunc_t*)c_Offset.EngineFuncs();
	if (!g_pEngine) _puts("Oh noes! EngineFuncs!");

	g_pStudio = (engine_studio_api_t*)c_Offset.StudioFuncs();
	if (!g_pStudio) _puts("Oh noes! g_pStudio!");

	pmove = (playermove_t*)c_Offset.FindPlayerMove();
	if (!pmove) _puts("Oh noes! pmove!");

Hook:
	memcpy( &g_Client , g_pClient , sizeof( cl_clientfunc_t ) );
	memcpy( &g_Engine , g_pEngine , sizeof( cl_enginefunc_t ) );
	memcpy( &g_Studio , g_pStudio , sizeof( engine_studio_api_t ) );

	if ( !g_Client.V_CalcRefdef || !g_Engine.V_CalcShake || !g_Studio.StudioSetupSkin )
		goto Hook;

	g_Engine.Con_Printf("Base loaded\n");

	g_pClient->CL_CreateMove = CL_CreateMove;
	_puts("Hooked CL_CreateMove");
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpReserved )
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		DisableThreadLibraryCalls( hinstDLL );

		CreateThread( 0 , 0 , (LPTHREAD_START_ROUTINE)SetupHook , 0 , 0 , 0 );
	}

	return TRUE;
}
