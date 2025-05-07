#include <windows.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <limits>
using namespace std;

struct MemoryAddress {
    uintptr_t address;
    int originalValue;
};

vector<MemoryAddress> foundAddresses;
HANDLE processHandle;

void searchValue(int targetValue) {
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t address = 0;
    foundAddresses.clear();

    cout << "Searching for value: " << targetValue << " in memory..." << endl;

    while (VirtualQueryEx(processHandle, (LPCVOID)address, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_READWRITE)) {
            vector<BYTE> buffer(mbi.RegionSize);
            SIZE_T bytesRead;

            if (ReadProcessMemory(processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                for (size_t i = 0; i < bytesRead - sizeof(int); i++) {
                    int* value = reinterpret_cast<int*>(&buffer[i]);
                    if (*value == targetValue) {
                        uintptr_t foundAddress = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + i;
                        foundAddresses.push_back({foundAddress, *value});
                        cout << "Found address: 0x" << hex << foundAddress << " | Value: " << dec << *value << endl;
                    }
                }
            }
        }
        address += mbi.RegionSize;
    }

    if (foundAddresses.empty()) {
        cout << "No addresses found containing the specified value." << endl;
    }
}

void changeValue() {
    if (foundAddresses.empty()) {
        cout << "No addresses found! Perform a search first." << endl;
        return;
    }

    int newValue;
    cout << "Enter new value to replace: ";
    cin >> newValue;

    for (size_t i = 0; i < foundAddresses.size(); i++) {
        MemoryAddress& entry = foundAddresses[i];

        if (WriteProcessMemory(processHandle, (LPVOID)entry.address, &newValue, sizeof(newValue), NULL)) {
            cout << "Modified address: 0x" << hex << entry.address << " | New Value: " << dec << newValue << endl;
            entry.originalValue = newValue;
        } else {
            cout << "Failed to modify address: 0x" << hex << entry.address << endl;
        }
    }
}

void rescanAndModifyValues() {
    if (foundAddresses.empty()) {
        cout << "No addresses found! Perform a search first." << endl;
        return;
    }

    int checkValue, newValue;
    cout << "Enter value to check for changes: ";
    cin >> checkValue;

    cout << "Enter new value to update matched addresses: ";
    cin >> newValue;

    cout << "Scanning and updating addresses with value: " << checkValue << " ..." << endl;

    bool found = false;
    for (size_t i = 0; i < foundAddresses.size(); i++) {
        MemoryAddress& entry = foundAddresses[i];

        int currentValue;
        SIZE_T bytesRead;
        if (ReadProcessMemory(processHandle, (LPCVOID)entry.address, &currentValue, sizeof(int), &bytesRead)) {
            if (currentValue == checkValue) {
                if (WriteProcessMemory(processHandle, (LPVOID)entry.address, &newValue, sizeof(newValue), NULL)) {
                    cout << "Updated Address: 0x" << hex << entry.address
                         << " | New Value: " << dec << newValue << endl;
                    entry.originalValue = newValue;
                    found = true;
                } else {
                    cout << "Failed to modify address: 0x" << hex << entry.address << endl;
                }
            }
        }
    }

    if (!found) {
        cout << "No addresses found with the specified value." << endl;
    }
}

int main() {
    DWORD processID;

    while (true) {
        try {
            cout << "Enter target process ID: ";
            cin >> processID;

            if (cin.fail()) {
                throw runtime_error("Invalid input! Please enter a valid numeric process ID.");
            }

            processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, processID);
            if (!processHandle) {
                throw runtime_error("Failed to open process! Ensure the PID is correct and run as Administrator.");
            }

            break; // Exit loop if successful
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    int choice;
    while (true) {
        cout << "\n==== Memory Editor ====" << endl;
        cout << "1 - Search Value" << endl;
        cout << "2 - Change Value" << endl;
        cout << "3 - Rescan and Modify Values" << endl;
        cout << "4 - Exit" << endl;
        cout << "Enter Option: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int targetValue;
                cout << "Enter value to search: ";
                cin >> targetValue;
                searchValue(targetValue);
                break;
            }
            case 2:
                changeValue();
                break;
            case 3:
                rescanAndModifyValues();
                break;
            case 4:
                cout << "Exiting program..." << endl;
                CloseHandle(processHandle);
                return 0;
            default:
                cout << "Invalid choice, try again!" << endl;
        }
    }

    CloseHandle(processHandle);
    return 0;
}

