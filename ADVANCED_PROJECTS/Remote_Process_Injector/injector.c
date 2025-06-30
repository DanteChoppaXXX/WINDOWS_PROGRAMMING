#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

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
    

    // Start the chilu process.
    BOOL creationResult;

    creationResult = CreateProcess(argv[1], NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startup_info, &process_info);
    
    if (!creationResult)
    {
        printf("CreateProcess Failed! Error: (%lu).\n", GetLastError());
        return;
    }

    printf("[+] Launched Notepad (PID: %lu)[SUCCESS]\n", process_info.dwProcessId);

    
    
    

    /* PROCESS INJECTION TIME */
    
    const char* dllPath = "C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\Remote_Process_Injector\\MyDLL.dll";
    SIZE_T path_len = strlen(dllPath) + 1;
    
    // Open target process.
    HANDLE target_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_info.dwProcessId);
    
    if (!target_process)
    {
        printf("OpenProcess Failed! Error: (%lu).\n", GetLastError());
    }
    
    printf("Process PID: %lu Opened Successfully!\n", process_info.dwProcessId);

    
    
    // Allocate memory in target.
    
    LPVOID remote_memory = VirtualAllocEx(
        target_process,
        NULL,
        path_len,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);    
        
        if (!remote_memory)
        {
            printf("VirtualAllocEx Failed! Error: (%lu).\n", GetLastError());
            CloseHandle(target_process);
        return;
    }
    
    printf("Allocated Memory at %p in target process.\n", remote_memory);

    
    // Write payload.
    SIZE_T bytes_written;
    
    if (!WriteProcessMemory(target_process, remote_memory, dllPath, path_len, &bytes_written))
    {
        printf("WriteProcessMemory Failed! Error: (%lu).\n", GetLastError());
        CloseHandle(target_process);
        return;
        
    }
    // printf("Bytes Written: (%lu).\n", bytes_written);
    // printf("%d Characters Written: (%s).\n",strlen(dllPath), dllPath);
    // printf("Characters Written: (%c).\n", remote_memory);
    
    
    printf("Still Running...?\n");
    Sleep(3000);
    
    // Execute remotely.
    printf("Creating Remote Thread...\n");
    LPDWORD remote_thread_Id;
    HANDLE remote_thread = CreateRemoteThread(target_process, NULL, 0, remote_memory, NULL, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, remote_thread_Id);
    
    if (!remote_thread)
    {
        printf("CreateRemoteThread Failed! Error: (%lu).\n", GetLastError());
        CloseHandle(target_process);
        return;
        
    }
    

    Sleep(1000);
    printf("Remote Thread_Id: (%lu).\n", remote_thread_Id);
    
    Sleep(1000);
    
    printf("Injecting DLL...\n");
    
    Sleep(3000);

    if (!LoadLibraryA("MyDLL.dll"))
    {
        printf("LoadLibraryA Failed! Error: (%lu).\n", GetLastError());
        CloseHandle(target_process);
        return;
        
    }
    else
    {
        ResumeThread(process_info.hThread);       
    }
    
    
    // ResumeThread(remote_thread);
    
    WaitForSingleObject(process_info.hProcess, 2000);
    

    
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    CloseHandle(target_process);
    
}