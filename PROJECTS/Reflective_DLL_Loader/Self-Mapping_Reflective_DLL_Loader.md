
---

## 🧩 Stage 2 — Implementing a Self-Mapping Reflective DLL Loader

Excellent. You're stepping into the **core of modern stealthy malware loading**. This is where you'll build a Reflective Loader that mimics the OS loader and loads a DLL manually **from memory** — without using `LoadLibrary`.

Here’s the **overall goal**:

> 🔁 Your loader reads a raw DLL binary (or its memory representation), parses it, allocates memory for it, fixes relocations and imports, and then calls its `DllMain()` manually — all *within the same process*.

---

## 🧱 Step-by-Step Algorithm (No code yet)

### **1. Read Your DLL File into Memory**

* Open and read the DLL file into a memory buffer (`BYTE* dll_bytes`).
* You'll simulate what an injector would send over the wire.

---


### 🎯 Objective:

You want to read your compiled DLL binary (e.g., `mydll.dll`) into a `BYTE*` buffer in memory, simulating the memory-based loading that injection would typically do.

---

### 🧠 Algorithm:

1. **Open the DLL file** in binary mode using `fopen()` or `CreateFile()`.
2. **Get its size**:

   * Seek to the end of the file.
   * Store the size using `ftell()` or `GetFileSize()`.
3. **Allocate a buffer** with `malloc()` to hold the entire contents.
4. **Read the entire file** into the buffer with `fread()` (or `ReadFile()`).
5. **Close the file**.

---

### 🧪 Validations:

* Ensure the buffer is non-null.
* Ensure the size is non-zero.
* Ensure the file read completes successfully.

---

### 📦 Output:

* `BYTE* dll_buffer`: Contains the raw bytes of the DLL.
* `DWORD dll_size`: The size of the DLL in bytes.

You’ll pass this buffer into your reflective loader for parsing and mapping in later steps.

---

### 🧠 Pro Tip:

In a real-world reflective injection, this buffer could come from:

* A socket
* An encrypted blob
* A memory section
* An embedded resource

But for now, local file I/O will do.

---

> **Step 2: Parsing the PE headers from the buffer.**


---

### **2. Parse PE Headers**

* Cast the buffer to a `IMAGE_DOS_HEADER`.
* Locate the `IMAGE_NT_HEADERS` via `e_lfanew`.
* Extract:

  * `OptionalHeader.SizeOfImage`
  * `OptionalHeader.SizeOfHeaders`
  * Section headers

✅ This tells you how big of a memory block to allocate and how to arrange the sections.

---

### **3. Allocate Memory for the DLL**

* Use `VirtualAlloc()` with `MEM_COMMIT | MEM_RESERVE`, size = `SizeOfImage`.
* This is the memory block where you’ll reconstruct the DLL.
* Call this memory block `target_image`.

---

### **4. Copy Headers and Sections**

* Copy `SizeOfHeaders` bytes from `dll_bytes` to `target_image`.
* For each section in `IMAGE_SECTION_HEADER[]`:

  * Copy raw data from `dll_bytes + section.PointerToRawData`
  * To `target_image + section.VirtualAddress`

✅ Now your DLL is roughly mapped into memory.

---

### **5. Fix Relocations (Base Relocation Table)**

* If `OptionalHeader.ImageBase` ≠ `target_image` base address:

  * You need to fix absolute addresses using the `.reloc` section.
  * Parse the `IMAGE_BASE_RELOCATION` table:

    * For each block:

      * For each entry (of 2 bytes): apply delta fix (`new_base - preferred_base`) to the addresses

🔧 This is crucial for pointers hardcoded to the original preferred image base.

---

### **6. Resolve Imports (Import Address Table)**

* Parse `OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]`.
* For each DLL:

  * Load the module using `LoadLibraryA()`.
  * For each function:

    * Use `GetProcAddress()` to resolve function pointer.
    * Write it into the IAT at the mapped address.

🧠 This part is how the DLL links itself at runtime — just like the Windows loader.

---

### **7. Call DllMain() Manually**

* Get the `entry_point = target_image + OptionalHeader.AddressOfEntryPoint`
* Cast to `DllMain`-type function pointer:

  ```c
  BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved);
  ```
* Call it like:

  ```c
  DllMain((HMODULE)target_image, DLL_PROCESS_ATTACH, NULL);
  ```

🎯 If this returns `TRUE`, your DLL has been reflectively loaded!

---

### 📁 Optional Step: Make It Loader-Friendly

* You can now make your loader accept:

  * A DLL from a file (`fopen`)
  * A DLL from a byte array (e.g., embedded, encrypted blob)
* This will be **exactly what malware or payloads do** during reflective injection.

---

## ✅ You’ve Completed Stage 2 When:

* You load your own DLL manually from memory.
* It runs successfully and executes its `DllMain()` code (like a message box or file dropper).
* You **don’t use `LoadLibrary()`** to load the DLL — only to resolve dependencies.

---
