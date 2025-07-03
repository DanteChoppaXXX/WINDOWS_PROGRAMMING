#include <fileapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <winnt.h>

// Function Prototypes.
void writeFile(const char* filePath, const char* text);

void readFile(const char* filePath);

//const char* filePath = "C:\\Users\\kiriko\\Documents\\GIT\\WINDOWS_PROGRAMMING\\PROJECTS\\Mini_File_Editor\\testfile.txt";

void usage(char* argv)
{
    // Extract the program name from path.
    char *filename = strrchr(argv, '\\');
    
    printf("Usage: %s \"path\\to\\file\" <option> \nOptions: -r <to read the file's content>\n\t -w \"text\" <to write \"text\" to file>\"\n", (filename+1));
}

int _tmain(int argc, char* argv[])
{
    // Check for correct arguments.
    if (argc < 2 || argc > 4)
    {
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[2], "-r") == 0)
    {
        // Call read function.
        readFile(argv[1]);
    }
    else if (strcmp(argv[2], "-w") == 0 && (argv[3] != NULL))
    {
        char* text = argv[3];
        text[strlen(text)] = '\n';

        // Call the write function.
        writeFile(argv[1], text);
    }


 	return 0;

}

void writeFile(const char* filePath, const char* text)
{
  HANDLE hfile = CreateFile(filePath, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
	
	//char buffer[] = "I Use Neovim By The Way!";
	DWORD bytes_written = 0;

	BOOL written = WriteFile(hfile, text, strlen(text), &bytes_written, NULL);

	if (!written) 
	{
		fprintf(stderr, "WriteFile Failed!\n");
		exit(1);	
	}

	printf("Wrote %lu bytes to file.\n", bytes_written);
	 
	CloseHandle(hfile);
}

void readFile(const char* filePath)
{
	// Open file for reading
	HANDLE hfile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
	
	BYTE* buffer = malloc(dll_size); 
	if (buffer == NULL) 
	{
		fprintf(stderr, "Memory Allocation Failed!\n");
		exit(1);
	
	}
	DWORD bytes_read = 0;

	BOOL read = ReadFile(hfile, buffer, dll_size, &bytes_read, NULL);
	if (!read) 
	{
		fprintf(stderr, "ReadFile Failed!\n");
		exit(1);	
	}

	printf("Read %lu bytes from file.\n", bytes_read);

	printf("Characters Read:\n==================\n");
	
  BYTE* pBuffer = buffer;

	// Display the bytes read.
	for (DWORD i = 0; i < bytes_read; i++) 
	{
		printf("%c", *pBuffer);
		pBuffer++;
	}
	printf("\n");
	 
  printf("%s\n", buffer);

  MessageBox(NULL, ((const char *) buffer), "ATTENTION!!!", MB_OK);

	free(buffer);
	CloseHandle(hfile);
}
