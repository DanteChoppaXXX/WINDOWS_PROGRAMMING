# **Project 1: Simple Process Spawner**
###  **A foundational system programming project that introduces you to key Windows APIs like `CreateProcess`.**

---



### 🧾 Goal:

Create a C program that:

* Accepts a path to an `.exe` file as input.
* Launches that `.exe` as a child process using Windows API.
* Optionally controls how the window looks (hidden, maximized, minimized).
* Waits for the process to complete (or not).

---

## 🧩 Core Concepts You’ll Use:

| Concept               | Purpose                                     |
| --------------------- | ------------------------------------------- |
| `STARTUPINFO`         | Controls how the new process window behaves |
| `PROCESS_INFORMATION` | Gets the new process's PID, handle, etc.    |
| `CreateProcess`       | Main API to spawn a new process             |
| `WaitForSingleObject` | Optional: Wait until process exits          |
| `CloseHandle`         | Clean up resources                          |

---

## 🧪 Inputs:

* `argv[1]`: Full path to the `.exe` file
* (Optional) `argv[2]`: Window behavior (e.g., `"hidden"`, `"minimized"`)

---

## 🧰 Algorithm Walkthrough

1. **Initialize `STARTUPINFO` and `PROCESS_INFORMATION` structs**

   * Zero them out
   * Set `cb` field in `STARTUPINFO` to its size

2. **Set Window Behavior (Optional)**

   * If user passed `"hidden"` or `"minimized"`, adjust the `STARTUPINFO.dwFlags` and `wShowWindow` accordingly

3. **Call `CreateProcess()`**

   * Use the `.exe` path as `lpApplicationName`
   * Pass `NULL` for environment and current directory (initially)
   * Handle success or failure

4. **Wait (Optional)**

   * You can `WaitForSingleObject` on `pi.hProcess` if you want to block until it's done

5. **Close Handles**

   * Close `hProcess` and `hThread` after use to avoid resource leaks

6. **Print Feedback**

   * Print the child process’s PID or status

---

## 🧠 Example Behaviors

### ➤ Input:

```
spawner.exe "C:\\Windows\\System32\\notepad.exe"
```

### ➤ Output:

```
[+] Launched Notepad (PID: 4321)
```

---

## ➕ Extras (Optional):

If you want to go further:

* Add an option to launch multiple processes in a loop.
* Add a timer to kill the process after some seconds.
* Try launching `.bat` or `.cmd` scripts.
* Add a flag to detach from the process (non-blocking).

---
