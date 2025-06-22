#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

// Function Prototypes.
byte* read_DLL_2_Mem(void);
void parser(byte* buffer);

void _tmain(void)
{
    // Call read_DLL_2_Mem() function.
    // BYTE* buffer = malloc(sizeof(BYTE *));
    // buffer = read_DLL_2_Mem();
    
    BYTE* buffer = read_DLL_2_Mem();
    for (DWORD i = 0; i < 4096; i++)
    {
        printf("%c", *buffer);
        buffer++;
    }

    Sleep(1000);
    printf("\n========\nThe End!\n========\n");

    free(buffer);
    
}

byte* read_DLL_2_Mem(void)
{
    /* Read Your DLL File Into Memory */
    
    
    // Open the DLL file in binary mode using CreateFile().
    // LPCWSTR filename = "C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\Remote_Process_Injector\\MyDLL.dll";
    
    HANDLE hfile = CreateFile("C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\Remote_Process_Injector\\MyDLL.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hfile)
    {
        printf("CreateFile Failed! Error: (%lu).\n", GetLastError());
        exit(1);
    }
    
    // Get size of file.
    PLARGE_INTEGER filesize = malloc(sizeof(PLARGE_INTEGER));

    if (GetFileSizeEx(hfile, filesize) == 0)
    {
        printf("GetFileSizeEx Failed! Error: (%lu).\n", GetLastError());
        exit(1);
    }
    
    DWORD dll_size = filesize->LowPart;
    printf("Filesize => %lu\n", dll_size);
    
    // Allocate a memory buffer with malloc() to hold the entire contents.
    BYTE* dll_bytes = malloc(dll_size);
    if (dll_bytes == NULL)
    {
        printf("Memory Allocation Failed! Error: (%lu).\n", GetLastError());
        CloseHandle(hfile);
        free(filesize);
        exit(1);
    }
    
    printf("Memory Allocated at => %p\n", dll_bytes);

    // Read the entire file into the buffer with ReadFile().
    DWORD bytes_read = 0;

    if(!ReadFile(hfile, dll_bytes, dll_size, &bytes_read, NULL)) 
    {
        printf("ReadFile Failed! Error: (%lu).\n", GetLastError());
        CloseHandle(hfile);
        free(dll_bytes);
        free(filesize);
        exit(1);
    }

    printf("Read %lu bytes of data into memory region => %p\n", bytes_read, dll_bytes);

    
    printf("\nRaw Bytes Data\n===============\n");
    Sleep(3000);
    
    for (DWORD i = 0; i < bytes_read; i++)
    {
        printf("%c", *dll_bytes);
        dll_bytes++;
    }

    Sleep(1000);
    printf("\n========\nThe End!\n========\n");
    
    dll_bytes -= bytes_read;
    
    

    free(filesize);
    CloseHandle(hfile);
    
    return dll_bytes;
}
