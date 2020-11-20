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

void PrintU(const string& name, const unsigned& value, const string& units = ""){
    string s_value = to_string(value);
    size_t n = 27 - name.length() + s_value.length();
    cout << name << setw(n) << "|  " + s_value << " "<< units << endl;
}


/// <summary>
/// This function print available system info
/// </summary>
void PrintSysInfo(){
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    cout << "System information:" << endl;
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
    cout << "Maximum application address: " <<  system_info.lpMaximumApplicationAddress << endl;

}

void PrintMemoryStatus(){
    cout << "Memory monitor: " << endl;
    string line = "-";
    cout << line * 40 << endl;
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);

    GlobalMemoryStatusEx(&statex);

    PrintU("Memory in use", statex.dwMemoryLoad, "%");
    PrintU("Total physical memory", statex.ullTotalPhys/DIV, "KB");
    PrintU("Free physical memory", statex.ullAvailPhys/DIV, "KB");
    PrintU("Total paging file", statex.ullTotalPageFile/DIV, "KB");
    PrintU("Free paging file", statex.ullAvailPageFile/DIV, "KB");
    PrintU("Total virtual memory", statex.ullTotalVirtual/DIV, "KB");
    PrintU("Free virtual memory", statex.ullAvailVirtual/DIV, "KB");
    cout << line * 40 << endl;
}
atomic<bool> is_need_exit {false};

/// <summary>
/// Start monitoring memory status
/// </summary>
void StartMonitoring(){

    thread mem_print([]{while(!is_need_exit.load(std::memory_order_relaxed)){
        PrintMemoryStatus();
        Sleep(3000);
    }});

    thread stop_mem_print([]{
        while(!is_need_exit.load(std::memory_order_relaxed)) {
            short a;//GetAsyncKeyState(VK_BACK);
            cin >> a;
            if(a!=0){
                is_need_exit.store(true, std::memory_order_relaxed);
                break;
            }
        }
    });

    mem_print.join();
    stop_mem_print.join();
    //PrintMemoryStatus();
}



int main() {
    //k();

    StartMonitoring();
//    for (int i = 0; i < 1; ++i) {
//        string command;
//        cin >> command;
//        if(command == "0") {
//            thread my_thread([]{while(!is_need_exit.load(std::memory_order_relaxed)){
//                PrintMemoryStatus();
//                Sleep(3000);
//            }});
//            my_thread.join();
//            is_need_exit.store(true, std::memory_order_relaxed);
//        }
//    }
    //PrintSysInfo();
    return 0;
}
