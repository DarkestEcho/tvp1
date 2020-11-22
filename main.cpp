#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>
#include <tchar.h>
#include <thread>
#include <atomic>
#include <sstream>

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
    cout << setw(24) << left << name << "|  " << value << endl;
}

/// <summary>
/// Formatted printing mem info
/// </summary>
void PrintM(const string& name, const unsigned long& old_value, const unsigned long& new_value){
    cout << setw(28) << left << name << "|  "
         << setw(12) << left << old_value << "|  "
         << setw(12) << left << new_value << "|  "
         << ((new_value > old_value)? new_value - old_value : old_value - new_value) << endl;
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
    cout << setw(28) << left <<  "Description" << "|  "
         << setw(12) << left << "Previous" << "|  "
         << setw(12) << left << "Current" << "|  "
         << setw(12) << left << "Difference" << endl;
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

void RunApplication(const string& path){
    ShellExecute(nullptr, "open", path.c_str(),"-L -S",nullptr,SW_SHOW); //запускаем

}


int main() {
    statex_old.dwLength = sizeof(statex_old);
    string command_line;
    cout << "for help enter 'help'"<<endl;
    while (true){
        cout << ">>";
        getline(cin, command_line);
        string command;

        stringstream ss(command_line);

        ss >> command;
        if(command == "sysinfo"){
            PrintSysInfo();
        }
        else if(command == "monitor"){
            StartMonitoring();
        }
        else if(command == "region_r"){
            size_t size;
            ss >> size;
            void* res_addr = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
            void* com_addr = VirtualAlloc(res_addr, size, MEM_COMMIT, PAGE_READWRITE);
            cout << "Reserved: " <<com_addr << endl;
        }
        else if(command == "region_r_ip"){
            void* addr;
            ss >> addr;
            size_t size;
            ss >> size;

            void* res_addr = VirtualAlloc(addr, size, MEM_RESERVE, PAGE_READWRITE);
            void* com_addr = VirtualAlloc(res_addr, size, MEM_COMMIT, PAGE_READWRITE);
            cout << "Reserved: " <<com_addr << endl;
        }
        else if(command == "region_c_ip"){
            void* addr;
            ss >> addr;
            size_t size;
            ss >> size;

            void* res_addr = VirtualAlloc(addr, size, MEM_RESERVE, PAGE_READWRITE);
            void* com_addr = VirtualAlloc(res_addr, size, MEM_COMMIT, PAGE_READWRITE);
            cout << "Reserved and committed: " <<com_addr << endl;
        }
        else if(command == "region_d_ip"){
            void* addr;
            ss >> addr;
            size_t size;
            ss >> size;

            bool res = VirtualFree(addr, size, MEM_DECOMMIT);
            cout << ((res == 1)? "decommitted: " : "didn't decommit: ") << addr << endl;
        }
        else if(command == "release_region_ip"){
            void* addr;
            ss >> addr;

            bool res = VirtualFree(addr, 0, MEM_RELEASE);
            cout << ((res == 1)? "released: " : "didn't release: ") << addr << endl;
        }
        else if(command == "run"){
            string path;
            ss >> path;
            RunApplication(path);
        }
        else if(command == "help"){
            cout << "List of commands (command - description):" << endl;
            cout << setw(29) << left << "sysinfo" << "- print system information" << endl;
            cout << setw(29) << left << "monitor" << "- start monitoring memory's status" << endl;
            cout << setw(29) << left << "region_r 'size'" << "- reserve region with an unspecified address" << endl;
            cout << setw(29) << left << "region_r_ip 'address' 'size'" << "- reserve region with an specified address" << endl;
            cout << setw(29) << left << "region_c_ip 'address' 'size'" << "- commit region with an specified address" << endl;
            cout << setw(29) << left << "region_d_ip 'address' 'size'" << "- decommit region with an specified address" << endl;
            cout << setw(29) << left << "release_region_ip 'address'" << "- release region with an specified address" << endl;
        }
        else if(command == "echo"){
            string word;
            ss >> word;
            cout << word << endl;
        }
        else if(command == "exit"){
            break;
        }
        else{
            cout << "Invalid command" << endl;
        }
    }

    return 0;
}
