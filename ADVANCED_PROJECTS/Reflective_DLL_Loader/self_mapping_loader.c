#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <winnt.h>   // For IMAGE_DOS_HEADER, IMAGE_NT_HEADERS, etc.


// Function Prototypes.
BYTE* read_DLL_2_Mem(void);
void parser(BYTE* buffer);
BYTE* manual_map(BYTE* buffer);

void _tmain(void)
{
    // Call read_DLL_2_Mem() function.
    BYTE* buffer = read_DLL_2_Mem();
    if (buffer == NULL)
    {
        fprintf(stderr, "Invalid Or Null Pointer!.\n");
        exit(1);
    }
    
    // Call manual_map() function.
    BYTE* target_image = manual_map(buffer);
    if (target_image == NULL)
    {
        fprintf(stderr, "Invalid Or Null Pointer!.\n");
        exit(1);
    }
    
    free(buffer);
    free(target_image);
    
}

BYTE* read_DLL_2_Mem(void)
{
    /* Read Your DLL File Into Memory */
    
    // Open the DLL file in binary mode using CreateFile().
    const char* filename = "C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\remote_Process_Injector\\MyDLL.dll";
    
    HANDLE hfile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "CreateFile Failed!\n");
        exit(1);
    }
    
    // Get size of file.
    LARGE_INTEGER filesize;
    GetFileSizeEx(hfile, &filesize);

    
    DWORD dll_size = filesize.LowPart;
    printf("Filesize => %lu\n", dll_size);
    
    // Allocate a memory buffer with malloc() to hold the entire contents.
    BYTE* dll_bytes = malloc(dll_size);
    if (dll_bytes == NULL)
    {
        fprintf(stderr, "Memory Allocation Failed! Possibly out of memory.\n");
        CloseHandle(hfile);
        exit(1);
    }
    
    printf("Memory Allocated at => %p\n", dll_bytes);

    // Read the entire file into the buffer with ReadFile().
    DWORD bytes_read = 0;

    if(!ReadFile(hfile, dll_bytes, dll_size, &bytes_read, NULL)) 
    {
        fprintf(stderr, "ReadFile Failed!.\n");
        CloseHandle(hfile);
        free(dll_bytes);
        exit(1);
    }

    printf("Read %lu bytes of data into memory region => %p\n", bytes_read, dll_bytes);

    
    printf("\nDLL Raw Bytes Loaded Into Memory\n================================\n");
    Sleep(1000);
    
    // BYTE* current = dll_bytes;

    // for (DWORD i = 0; i < bytes_read; i++)
    // {
    //     printf("%02X ", current[i]);
    // }

    // Sleep(1000);
    // printf("\n========\nThe End!\n========\n");
       
    

    CloseHandle(hfile);
    
    return dll_bytes;
}

void parser(BYTE* buffer)
{
    // 1. DOS Header
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER) buffer;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("[-] Invalid DOS Signature!\n");
        return;
    }
    printf("[+] Valid DOS Header found.\n");

    // 2. NT Headers
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(buffer + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
    {
        printf("[-] Invalid NT Signature!\n");
        return;
    }
    printf("[+] Valid NT Header found.\n");

    // 3. Print key values from Optional Header
    printf("Entry Point: 0x%X\n", nt->OptionalHeader.AddressOfEntryPoint);
    printf("Image Base: 0x%p\n", (void*)nt->OptionalHeader.ImageBase);
    printf("Number of Sections: %d\n", nt->FileHeader.NumberOfSections);
    printf("Machine Type: %d\n", nt->FileHeader.Machine);
    printf("DLL Characteristics: %d\n", nt->OptionalHeader.DllCharacteristics);
    printf("Size of Optional Header: %lu\n", nt->FileHeader.SizeOfOptionalHeader);
    printf("Size of image: 0x%X\n", nt->OptionalHeader.SizeOfImage);
    printf("Section Alignment: 0x%X\n", nt->OptionalHeader.SectionAlignment);

    // 4. Walk Through Section Headers.
    // Get to the position of the first section header; 
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
    
    //DWORD offset = offsetof(IMAGE_SECTION_HEADER, Name) + 4944; 
    printf("Buffer Pointer: %s\n", buffer);
    
    printf("Sections:\n===========\n");
    
    
    for (DWORD i = 0; i < nt->FileHeader.NumberOfSections; i++)
    {
        printf("Section VA: %lu\tSize Of RawData: %lu\tName: %s\tSection Offset: %lu\n", section->VirtualAddress, section->SizeOfRawData, section->Name, section->PointerToRawData);
        section++;

    }
    


}

BYTE* manual_map(BYTE* buffer)
{
    /* Parse The Headers */
    // 1. DOS Header
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER) buffer;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("[-] Invalid DOS Signature!\n");
        exit(1);
    }
    printf("[+] Valid DOS Header found.\n");
    
    // 2. NT Headers
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(buffer + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
    {
        printf("[-] Invalid NT Signature!\n");
        exit(1);
    }
    printf("[+] Valid NT Header found.\n");
    
    // Allocate memory using VirtualAlloc.
    BYTE* target_image = VirtualAlloc(NULL,
                        nt->OptionalHeader.SizeOfImage,
                        MEM_COMMIT | MEM_RESERVE,
                        PAGE_EXECUTE_READWRITE);
        
    if (target_image == NULL)
    {
        fprintf(stderr, "Memory Allocation Failed! Possibly out of memory.\n");
        exit(1);
    }
        
    // Copy the PE headers.
    memcpy(target_image, buffer, nt->OptionalHeader.SizeOfHeaders);
    
    printf("[+] Dos and NT Header Copied To Image Base\t\t[Done]\n\n");

    //  Copy each section to its VirtualAddress.
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
    
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++, section++)
    {
        BYTE* dest = target_image + section->VirtualAddress;
        BYTE* src  = buffer + section->PointerToRawData;
        
        printf("[+] Copying %-6s RawData at 0x%-6X to VA 0x%-6X\t", section->Name, section->PointerToRawData, section->VirtualAddress);
        memcpy(dest, src, section->SizeOfRawData);
        printf("[Done]\n");
    }
    
    return target_image;
}