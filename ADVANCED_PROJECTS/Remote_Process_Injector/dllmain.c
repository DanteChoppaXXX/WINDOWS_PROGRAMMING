#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule,
                    DWORD ul_reason_for_call,
                    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL,"DLL Injected Successfully!", "DLL", MB_OK);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
   }
   return TRUE;
}