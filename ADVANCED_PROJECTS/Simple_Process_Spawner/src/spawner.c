#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <string.h>

void usage(char* argv)
{
    // Extract the program name from path.
    char *filename = strrchr(argv, '\\');
    
    printf("Usage: %s <\"path\\to\\executable\"> <window behaviour>\nWindow Behaviour (Optional): {\"hidden\", \"minimized\"}\n", (filename+1));
}

void _tmain(int argc, char* argv[])
{
    // Check for correct arguments.
    if (argc < 2 || argc > 3)
    {
        usage(argv[0]);
        return;
    }

    // Initialize STARTUPINFO and PROCESS_INFORMATION structs.
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&process_info, sizeof(process_info));
    
    startup_info.cb = sizeof(startup_info);
    // startup_info.lpReserved = NULL;
    // startup_info.dwFlags |= STARTF_USESHOWWINDOW;
    // startup_info.wShowWindow = SW_NORMAL;
    

    // Start the child process.
    BOOL creationResult;

    creationResult = CreateProcess(argv[1], NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startup_info, &process_info);
    
    if (!creationResult)
    {
        printf("CreateProcess Failed! (%ld).\n", GetLastError());
        return;
    }

    char message[64];
    snprintf(message, sizeof(message), "[+] Launched Notepad (PID: %ld)[SUCCESS]\n", process_info.dwProcessId); 
    MessageBox(NULL, message, "SUCCESS", MB_OK);
    
    WaitForSingleObject(process_info.hProcess, 100000);
    
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    
}
