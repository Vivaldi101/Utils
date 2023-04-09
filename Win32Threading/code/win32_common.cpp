extern volatile byte* globalWin32VirtualMemoryBase;

function INT 
PageFaultExceptionFilter(_EXCEPTION_POINTERS* exceptionPointers)
{
	const PEXCEPTION_RECORD per = exceptionPointers->ExceptionRecord;

    // If the exception is not a page fault, exit.

	if (!globalWin32VirtualMemoryBase 
	|| per->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

	const byte* faultedAddress = Cast(per->ExceptionInformation[1], byte*);

	// Make sure we faulted higher than before.

	CheckMemory(faultedAddress > globalWin32VirtualMemoryBase);

	// Otherwise, commit the pages.

	const size_t allocationSize = faultedAddress - globalWin32VirtualMemoryBase;

	if (!VirtualAlloc(Cast(globalWin32VirtualMemoryBase, void*), allocationSize, MEM_COMMIT, PAGE_READWRITE))
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // Continue execution where the page fault occurred.

    return EXCEPTION_CONTINUE_EXECUTION;
}

function DWORD
GetWin32PageSize()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	return sysInfo.dwPageSize;
}

function DWORD
GetWin32CoreCount()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	return sysInfo.dwNumberOfProcessors;
}
