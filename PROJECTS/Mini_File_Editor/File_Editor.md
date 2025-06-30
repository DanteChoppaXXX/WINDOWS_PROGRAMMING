# 🔨 **STAGE 1 – PROJECT 1: Mini File Writer/Reader**

**Goal:** Learn to open a file, write to it, close it, then read it back using only the Windows API.

---

### ✅ **Phase 1: File Writing (Write Mode)**

#### 🔹 Algorithm:

1. **Set target file path** — choose a path for the file (e.g., `C:\\Users\\You\\Documents\\testfile.txt`).
2. **Call `CreateFile`** with `GENERIC_WRITE` access.

   * Set `CREATE_ALWAYS` to overwrite or create the file.
3. **Check if the handle is valid** (not `INVALID_HANDLE_VALUE`).
4. **Prepare a buffer** — some data string you want to write (e.g., "Hello from WinAPI").
5. **Call `WriteFile`** with the buffer, buffer length, and capture bytes written.
6. **Confirm bytes were written**, optionally print it.
7. **Close the file handle** using `CloseHandle`.

---

### ✅ **Phase 2: File Reading (Read Mode)**

#### 🔹 Algorithm:

1. **Call `CreateFile` again**, but this time with `GENERIC_READ` and `OPEN_EXISTING`.
2. **Validate the handle again**.
3. **Use `GetFileSizeEx`** to determine the size of the file.
4. **Allocate a buffer** using `malloc()` to hold the file data.
5. **Call `ReadFile`** with the handle and buffer.
6. **Check bytes read** and display the result as a string or byte array.
7. **Free memory** and close handle when done.

---

### 🧠 Bonus Challenges (Optional):

* Handle Unicode file paths (`LPCWSTR`)
* Append mode instead of overwrite
* Write binary data (e.g., raw bytes instead of strings)

---

### 🎯 Outcome:

By the end of this project, you will:

* Have created and read a file *entirely using WinAPI*
* Understand `HANDLE`, file access modes, and I/O buffers
* Be confident with `CreateFile`, `WriteFile`, `ReadFile`, and `CloseHandle`

---

