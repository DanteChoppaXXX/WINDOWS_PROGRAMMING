/*
  This program enumerate all currently running processes on a Windows system, showing: 
  * Process ID (PID)
  * Executable name (e.g. explorer.exe)
  * Parent PID
*/

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

int _tmain()
{
  // Define and create a snapshot of running processes.
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  // Check if the handle is valid.
  if (snapshot == INVALID_HANDLE_VALUE)
  {
    fprintf(stderr, "CreateToolhelp32Snapshot Failed!\n");
    exit(EXIT_FAILURE);  
  }

  // Initialize PROCESSENTRY32 Structure to receive information about each process from the snap.
  PROCESSENTRY32 process_info;
  process_info.dwSize = sizeof(PROCESSENTRY32);

  // Use Process32First to Start iteration.
  BOOL first_process = Process32First(snapshot, &process_info);
  // Check for failure.
  if (!first_process)
  {
    fprintf(stderr, "Process32First Failed!\n");
    CloseHandle(snapshot);
    exit(EXIT_FAILURE);  
  }

  DWORD count = 0;
  printf("%03lu => Process ID: %-6lu\tExecutable Name: %-30s\t Parent PID: %-6lu\n", count, process_info.th32ProcessID, process_info.szExeFile, process_info.th32ParentProcessID);
  // Loop with Process32Next to iterate over all processes in the snapshot.
  while (Process32Next(snapshot, &process_info))
  {
    count ++;
    printf("%03lu => Process ID: %-6lu\tExecutable Name: %-30s\t Parent PID: %-6lu\n", count, process_info.th32ProcessID, process_info.szExeFile, process_info.th32ParentProcessID);
  }

  CloseHandle(snapshot);

  return EXIT_SUCCESS;
}
