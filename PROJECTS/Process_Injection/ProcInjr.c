#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

void usage(char* argv)
{
    // Extract the program name from path.
    char *filename = strrchr(argv, '\\');
    
    printf("Usage: %s \"path\\to\\executable\"\n", (filename+1));
}

int _tmain(int argc, char* argv[])
{
    // Check for correct arguments.
    if (argc < 2 || argc > 3)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize STARTUPINFO and PROCESS_INFORMATION structs.
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&process_info, sizeof(process_info));
    
    startup_info.cb = sizeof(startup_info);

    // Start the child process.
    BOOL creationResult;

    creationResult = CreateProcess(argv[1], NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startup_info, &process_info);
    
    if (!creationResult)
    {     
        fprintf(stderr, "CreateProcess Failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("[+] Launched Notepad (PID: %lu)[SUCCESS]\n", process_info.dwProcessId);

    /* PROCESS INJECTION TIME */
    
    const char* dllPath = "C:\\Users\\kiriko\\Desktop\\MyDLL.dll";
    SIZE_T path_len = strlen(dllPath) + 1;
    
    // Open target process.
    HANDLE target_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_info.dwProcessId);
    
    if (target_process == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "OpenProcess Failed!\n");
        exit(EXIT_FAILURE);
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
        fprintf(stderr, "VirtualAllocEx Failed!\n");          
        CloseHandle(target_process);
        exit(EXIT_FAILURE);
    }
    
    printf("Allocated Memory at %p in target process.\n", remote_memory);

    // Write payload.
    SIZE_T bytes_written;
    
    if (!WriteProcessMemory(target_process, remote_memory, dllPath, path_len, &bytes_written))
    {
        fprintf(stderr, "WriteProcessMemory Failed!\n");
        CloseHandle(target_process);
        exit(EXIT_FAILURE);
    }
    
    printf("Still Running...?\n");
    Sleep(3000);
    
    // Execute remotely.
    printf("Creating Remote Thread...\n");
    DWORD remote_thread_Id;

    // Get address of LoadLibraryA
    LPTHREAD_START_ROUTINE load_library = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    if (!load_library)
    {
        fprintf(stderr, "Failed to get address of LoadLibraryA\n");
        CloseHandle(target_process);
        exit(EXIT_FAILURE);
    }
    
    // Create the remote thread to load the DLL.
    printf("Injecting DLL...\n");
    
    Sleep(500);

    ResumeThread(process_info.hThread);       

    HANDLE remote_thread = CreateRemoteThread(target_process, NULL, 0, load_library, remote_memory, 0, &remote_thread_Id);
    
    if (remote_thread == INVALID_HANDLE_VALUE)
    { 
        fprintf(stderr, "CreateRemoteThread Failed!\n");
        CloseHandle(target_process);
        exit(EXIT_FAILURE);
    }
    
    Sleep(1000);
    printf("Remote Thread_Id: (%lu).\n", remote_thread_Id);
    
        
    Sleep(1000);
    
    
    WaitForSingleObject(process_info.hProcess, INFINITE);

    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    CloseHandle(target_process);
    
    return EXIT_SUCCESS;
}
