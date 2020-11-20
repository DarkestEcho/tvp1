#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>
#include <tchar.h>
#include <thread>
#include <atomic>

#define DIV 1024

using namespace std;

string operator *(const string& str, const int& n){
    string new_str = str;
    for (int i = 1; i < n; ++i) {
        new_str += str;
    }
    return new_str;
}
/// <summary>
/// Formatted printing sys info with unsigned value
/// </summary>
void PrintU(const string& name, const unsigned& value){
    string s_value = to_string(value);
    size_t n = 27 - name.length() + s_value.length();
    cout << name << setw(n) << "|  " + s_value << endl;
}
/// <summary>
/// Formatted printing mem info
/// </summary>
void PrintM(const string& name, const unsigned long& old_value, const unsigned long& new_value){
    string s_old_value = to_string(old_value);
    unsigned long difference = (new_value > old_value)? new_value - old_value : old_value - new_value;
    size_t n1 = 31 - name.length();
    size_t n2 = 15 - s_old_value.length();
    size_t n3 = 15 - to_string(new_value).length();
    cout << name << setw(n1) << "|  "
         << s_old_value << setw(n2)<< "|  "
         << new_value << setw(n3) << "|  "
         << difference << endl;
}

/// <summary>
/// This function print available system info
/// </summary>
void PrintSysInfo(){
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    cout << "\nSystem information:" << endl;
    string line = "-";
    cout << line * 32 << endl;
    PrintU("OEM ID", system_info.dwOemId);
    PrintU("Number of processors", system_info.dwNumberOfProcessors);
    PrintU("Processor type", system_info.dwProcessorType);
    PrintU("Processor level", system_info.wProcessorLevel);
    PrintU("Processor revision", system_info.wProcessorRevision);
    PrintU("Processor Architecture", system_info.wProcessorArchitecture);
    PrintU("Reserved", system_info.wReserved);
    PrintU("Page size", system_info.dwPageSize);
    PrintU("Page size", system_info.dwPageSize);
    PrintU("Active processor mask", system_info.dwActiveProcessorMask);
    PrintU("Allocation granularity", system_info.dwAllocationGranularity);
    cout << line * 32 << endl;
    cout << "Minimum application address: " <<  system_info.lpMinimumApplicationAddress << endl;
    cout << "Maximum application address: " <<  system_info.lpMaximumApplicationAddress << '\n'<< endl;

}

MEMORYSTATUSEX statex_old;
/// <summary>
/// Print memory status
/// </summary>
void PrintMemoryStatus(){
    cout << "\nMemory monitor" << endl;
    string line = "-";
    cout << line * 71 << endl;
    cout << "Description" << setw(20) <<"|  "
         << "Previous" << setw(7) << "|  "
         << "Current" << setw(8) << "|  "
         << "Difference" << endl;
    cout << line * 71 << endl;
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);

    GlobalMemoryStatusEx(&statex);
    PrintM("Memory in use (%)", statex_old.dwMemoryLoad, statex.dwMemoryLoad);
    PrintM("Total physical memory (KB)", statex_old.ullTotalPhys/DIV , statex.ullTotalPhys/DIV);
    PrintM("Free physical memory (KB)", statex_old.ullAvailPhys/DIV , statex.ullAvailPhys/DIV);
    PrintM("Total paging file (KB)", statex_old.ullTotalPageFile/DIV , statex.ullTotalPageFile/DIV);
    PrintM("Free paging file (KB)", statex_old.ullAvailPageFile/DIV, statex.ullAvailPageFile/DIV);
    PrintM("Total virtual memory (KB)", statex_old.ullTotalVirtual/DIV, statex.ullTotalVirtual/DIV);
    PrintM("Free virtual memory (KB)", statex_old.ullAvailVirtual/DIV, statex.ullAvailVirtual/DIV);
    cout << line * 71 << endl;
    cout << "to exit enter: e\n" << endl;
    statex_old = statex;
}
atomic<bool> is_need_exit {false};

/// <summary>
/// Start monitoring memory status
/// </summary>
void StartMonitoring(){

    thread mem_print([]{while(!is_need_exit.load(std::memory_order_relaxed)){
        system ("cls");
        PrintMemoryStatus();
        Sleep(3000);
    }});

    while(!is_need_exit.load(std::memory_order_relaxed)) {
            char a;//GetAsyncKeyState(VK_BACK);
            cin >> a;
            if(a=='e'){
                is_need_exit.store(true, std::memory_order_relaxed);
                break;
            }
        }

    mem_print.join();
}




int main() {
    statex_old.dwLength = sizeof(statex_old);

    while (true){
        string command;
        cin >> command;
        if(command == "sysinfo"){
            PrintSysInfo();
            cout << "hi" << endl;
        }
        else if(command == "monitor"){
            StartMonitoring();
        }
        else if(command == "region"){
            void* res_addr = VirtualAlloc(NULL, 100, MEM_RESERVE, PAGE_READWRITE);
            void* com_addr = VirtualAlloc(res_addr, 100, MEM_COMMIT, PAGE_READWRITE);
            cout << "Reserved and committed: " <<com_addr << endl;
        }
        else if(command == "region_ip"){
            void* addr;
            cin >> addr;
            void* res_addr = VirtualAlloc(addr, 100, MEM_RESERVE, PAGE_READWRITE);
            void* com_addr = VirtualAlloc(res_addr, 100, MEM_COMMIT, PAGE_READWRITE);
            cout << "Reserved and committed: " <<com_addr << endl;
        }
        else if(command == "exit"){
            break;
        }
        else{
            cout << "Invalid command" << endl;
        }
    }
/*
    void* s = reinterpret_cast<void *>(0x1e1001);
    void* a = VirtualAlloc(NULL, 100, MEM_RESERVE, PAGE_READWRITE);
    cout << a << endl;
    void* b = VirtualAlloc(a, 100, MEM_COMMIT, PAGE_READWRITE);
    cout << b << endl;*/


    //StartMonitoring();
//    char Arr[4096];
//    HANDLE hCon = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
//    int x = 0;
//    while (true)
//    {
//        memset(Arr, ' ', sizeof(Arr));
//        memcpy(Arr, "Hello", 5);
//        for (int i = 9; i >= 0; --i)
//        {
//            if (x - i >= 0)
//                Arr[x - i] = '@';
//        }
//        x = (x + 1) % sizeof(Arr);
//        DWORD w;
//        WriteConsoleOutputCharacter(hCon, Arr, 4096, COORD{ 0, 0 }, &w);
//        Sleep(10);
//    }

    return 0;
}
