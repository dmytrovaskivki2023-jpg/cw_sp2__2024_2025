#define _CRT_SECURE_NO_WARNINGS
/************************************************************
* N.Kozak // Lviv'2024-2025 // wrapWithInOutPipe            *
*                         file: wrapper.cpp                 *
*                                                  (draft!) *
*************************************************************/
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define DEFAULT_INPUT_FILENAME "../test_programs/file3.exe" // TODO: move!

void ErrorExit(const char* msg) {
    fprintf(stderr, "%s. Error code: %lu\n", msg, GetLastError());
    ExitProcess(1);
}

int main() {
    char *param1 = "input_filename", *param2 = "output_filename", *param3 = "-d";
    int arg1 = 1, arg2 = 1, arg3 = 1;
    char cmdArgs[256];

    printf("wrap: \"%s\"\r\n", DEFAULT_INPUT_FILENAME);
    printf("args: \"%s\" \"%s\" \"%s\"\r\n", param1, param2, param3);
    printf("commandline: \"%s\" \"%s\" \"%s\" \"%s\"\r\n", DEFAULT_INPUT_FILENAME, param1, param2, param3);
    printf("input: %d; %d; %d;\r\n", arg1, arg2, arg3);
    printf("output: computed result by formula\r\n");
    printf("\r\nPress Enter to run: ");
    (void)getchar();

    sprintf(cmdArgs, "%s %s %s %s", DEFAULT_INPUT_FILENAME, param1, param2, param3);
    //sprintf(cmdArgs, "%s", DEFAULT_INPUT_FILENAME);

    HANDLE hChildStdOutRead, hChildStdOutWrite;
    HANDLE hChildStdInRead, hChildStdInWrite;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hChildStdOutRead, &hChildStdOutWrite, &sa, 0))
        ErrorExit("Failed to create stdout pipe");
    if (!SetHandleInformation(hChildStdOutRead, HANDLE_FLAG_INHERIT, 0))
        ErrorExit("Failed to set handle information for stdout");

    if (!CreatePipe(&hChildStdInRead, &hChildStdInWrite, &sa, 0))
        ErrorExit("Failed to create stdin pipe");
    if (!SetHandleInformation(hChildStdInWrite, HANDLE_FLAG_INHERIT, 0))
        ErrorExit("Failed to set handle information for stdin");

    HPCON hPC;
    COORD consoleSize = { 80, 25 };
    if (CreatePseudoConsole(consoleSize, hChildStdInRead, hChildStdOutWrite, 0, &hPC) != S_OK) {
        ErrorExit("Failed to create pseudo console");
    }

    STARTUPINFOEX siEx;
    ZeroMemory(&siEx, sizeof(STARTUPINFOEX));
    siEx.StartupInfo.cb = sizeof(STARTUPINFOEX);

    SIZE_T attrListSize;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);
    siEx.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)malloc(attrListSize);
    if (!InitializeProcThreadAttributeList(siEx.lpAttributeList, 1, 0, &attrListSize)) {
        ErrorExit("Failed to initialize proc thread attribute list");
    }

    if (!UpdateProcThreadAttribute(siEx.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, hPC, sizeof(HPCON), NULL, NULL)) {
        ErrorExit("Failed to update proc thread attribute for pseudo console");
    }

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessA(
        NULL,          // Application name
        cmdArgs,       // Command line arguments
        NULL,          // Process handle not inheritable
        NULL,          // Thread handle not inheritable
        TRUE,          // Inherit handles
        EXTENDED_STARTUPINFO_PRESENT, // Use extended startup info
        NULL,          // Use parent's environment block
        NULL,          // Use parent's starting directory 
        &siEx.StartupInfo, // Pointer to STARTUPINFOEX structure
        &pi))          // Pointer to PROCESS_INFORMATION structure
    {
        printf("\r\nMaybe file \"%s\" does not exist.\r\n", DEFAULT_INPUT_FILENAME);
        ErrorExit("Failed to create process");
    }

    CloseHandle(hChildStdOutWrite);
    CloseHandle(hChildStdInRead);

    char inputBuffer[128];
    sprintf(inputBuffer, "%d\r\n%d\r\n%d\r\n", arg1, arg2, arg3);
    DWORD bytesWritten;
    if (!WriteFile(hChildStdInWrite, inputBuffer, strlen(inputBuffer), &bytesWritten, NULL)) {
        ErrorExit("Failed to write to child stdin");
    }
    Sleep(500);
    CloseHandle(hChildStdInWrite);

    char buffer[128];
    DWORD bytesRead;
    if (ReadFile(hChildStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        printf("Output: %s", buffer);
        //MessageBoxA(NULL, buffer, "", 0);
    }
    else {
        ErrorExit("Failed to read from child stdout");
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        printf("\nChild process exited with code %lu\n", exitCode);
    }
    else {
        ErrorExit("Failed to get exit code");
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hChildStdOutRead);
    DeleteProcThreadAttributeList(siEx.lpAttributeList);
    free(siEx.lpAttributeList);
    ClosePseudoConsole(hPC);

    return 0;
}