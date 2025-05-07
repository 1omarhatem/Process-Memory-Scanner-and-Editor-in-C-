# 🧠 Process Memory Scanner and Editor in C++

A C++ application that scans and edits values in the memory of another running process (by PID), using the Windows API. This tool is useful for game memory editing, debugging, and learning how process memory works in practice.

---

## 📌 Project Idea

This project implements a memory editor that:

- Accepts a **Process ID (PID)**.
- Scans the process memory for a specific integer value.
- Displays all memory addresses where the value is found.
- Allows the user to modify the value(s) in real-time.

---

## 📊 Features

- Search for integer values in another process's memory.
- Modify memory addresses with new values.
- Rescan and update values selectively.
- Handles memory access permissions and errors gracefully.

---

## 🧩 Algorithms Used

### 🔍 Linear Memory Scan (Search)
- Iterate through committed memory regions using `VirtualQueryEx`.
- Read each region with `ReadProcessMemory`.
- Scan each 4-byte segment in a brute-force linear search for the target integer.

### ✏️ Modify Found Values
- For each matched address, use `WriteProcessMemory` to update the value.

### 🔁 Rescan & Modify
- Read saved addresses and compare with a new target.
- If matched, overwrite with a new value.

---

## ⏱️ Time Complexity

| Operation               | Complexity     |
|------------------------|----------------|
| Open process           | O(1)           |
| Query regions          | O(n)           |
| Read memory blocks     | O(m) per block |
| Search in memory       | O(n × m)       |
| Modify values          | O(k)           |

Where:
- `n` = number of memory regions
- `m` = average size of each region
- `k` = number of found addresses

---

## ⚙️ Windows APIs Used

- `OpenProcess`
- `VirtualQueryEx`
- `ReadProcessMemory`
- `WriteProcessMemory`
- `CloseHandle`

---

## 🛡️ Security Notes

- Run the program as **Administrator**.
- Cannot access system or protected processes.
- May trigger antivirus software as a memory editor (false positive).

---

## 👨‍💻 Sample Code Behavior

```plaintext
1. Launch target app (e.g., game with Health = 100).
2. Run this tool and input the target's PID.
3. Search for value "100".
4. Tool prints all memory addresses with that value.
5. User inputs new value "999".
6. Tool updates the addresses instantly.
7. Switch back to the game — health becomes 999!
