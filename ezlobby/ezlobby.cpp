#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <WinBase.h>
#include <TlHelp32.h>

int nP = 0;

BOOL SuspendProcess(DWORD ProcessId, bool Suspend)
{
	HANDLE snHandle = NULL;
	BOOL rvBool = FALSE;
	THREADENTRY32 te32 = { 0 };
	snHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snHandle == INVALID_HANDLE_VALUE) return (FALSE);
	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(snHandle, &te32))
	{
		do
		{
			if (te32.th32OwnerProcessID == ProcessId)
			{
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
				if (Suspend == false)
				{
					ResumeThread(hThread);
				}
				else
				{
					SuspendThread(hThread);
				}
				CloseHandle(hThread);
			}
		} while (Thread32Next(snHandle, &te32));
		rvBool = TRUE;
	}
	else
	rvBool = FALSE;
	CloseHandle(snHandle);
}

PDWORD GetProcesses() {
	DWORD processes[1024], cbNeeded, cProcesses, target;
	EnumProcesses(processes, sizeof(processes), &cbNeeded);
	nP = cbNeeded / sizeof(DWORD);
	return processes;
}

BOOL isGTAPID(DWORD pid) {
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, pid);
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;
		DWORD b = NULL;
		if (EnumProcessModulesEx(hProcess, &hMod, sizeof(hMod),
			&cbNeeded, b))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}
	CloseHandle(hProcess);
	return _tcscmp(szProcessName, _T("GTA5.exe")) == 0;
}

int main(void)
{
	PDWORD processes = GetProcesses();
	DWORD gtaPID = NULL;

	for (int i = 0; i < nP; i++) {
		if (isGTAPID(processes[i])) {
			gtaPID = processes[i];
			break;
		}
	}

	if (gtaPID == NULL) {
		printf("ERROR: GTA V is not executing\n\n");
		system("pause");
		return 1;
	}
	else {
		printf("Found GTA V process\n");
		printf("Suspending for 8 seconds\n");
		SuspendProcess(gtaPID, true);
		Sleep(8000);
		SuspendProcess(gtaPID, false);
		printf("Success!\n\n");
		system("pause");
	}

}