#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

// Function Prototypes.
void writeFile(const char* filePath);

void readFile(const char* filePath);

const char* filePath = "C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\Mini_File_Editor\\testfile.txt";

void _tmain(void)
{
	// Call write function.
	writeFile(filePath);

}

void writeFile(const char* filePath)
{
    HANDLE hfile = CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
 
}

void readFile(const char* filePath)
{

}
