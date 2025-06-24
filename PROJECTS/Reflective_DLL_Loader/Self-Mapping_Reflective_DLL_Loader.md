
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

### 🔥 **Let’s get deep into the internals of your DLL. Welcome to PE Parsing: Stage 2** 🔥
This stage is about exploring and interpreting the Portable Executable (PE) file headers — just like the Windows loader would when preparing to load a DLL.

---

### 🧠 **Goal of Stage 2: Understand and Parse PE Headers**

We’re going to write a `parser(BYTE* buffer)` function that does the following:

---

## ✅ Step-by-Step Overview:

### **1. Parse the DOS Header**

The beginning of every PE file is a legacy **MS-DOS header** (`IMAGE_DOS_HEADER`).

```c
PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER) buffer;
```

* **Check**: The first two bytes should be `"MZ"` (0x5A4D). This is the signature.
* **Field of interest**: `dos->e_lfanew` – gives you the offset to the real PE header.

---

### **2. Parse the NT Headers**

The **NT headers** are located at `buffer + dos->e_lfanew`.

```c
PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(buffer + dos->e_lfanew);
```

* **Check**: `nt->Signature` must be `IMAGE_NT_SIGNATURE` (value `0x00004550`, i.e. "PE\0\0").
* Contains:

  * `IMAGE_FILE_HEADER` – info about machine type, sections, etc.
  * `IMAGE_OPTIONAL_HEADER` – crucial details about memory layout, entry point, etc.

---

### **3. Print Key Header Info (for learning and debugging)**

Print values such as:

* Number of sections
* Size of image
* Address of entry point
* Image base
* Section alignment
* DLL characteristics

✅ This reinforces understanding and ensures our pointer math is correct.

---

### **4. (Optional for now)**: Walk Through Section Headers

You can access section headers right after `IMAGE_NT_HEADERS`.

```c
PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
```

Each section will tell you:

* Its name (`.text`, `.data`, etc.)
* Where it’s located in memory
* Where it is in the file
* How big it is

---

## 🧱 Your Parser Skeleton:


---

## 🛠 Headers You'll Need to Include

```c
#include <windows.h>
#include <winnt.h>   // For IMAGE_DOS_HEADER, IMAGE_NT_HEADERS, etc.
```

---

### ✅ Your Task Now:

1. Add `parser(buffer);` call in your `main()` after reading the DLL.
2. Implement the `parser()` function as described above.
3. Run and observe the output of PE metadata from your DLL file.


---

### **3. Allocate Memory for the DLL**

* Use `VirtualAlloc()` with `MEM_COMMIT | MEM_RESERVE`, size = `SizeOfImage`.
* This is the memory block where you’ll reconstruct the DLL.
* Call this memory block `target_image`.


---

### 🔨 Stage 3 — Memory Mapping (Manual Load)

**Mapping the PE image into memory manually**, which is the soul of reflective DLL loading.
### 🧠 What You’re Emulating:

When `LoadLibrary()` is called by Windows, it:

1. Allocates memory (using `VirtualAlloc`)
2. Copies over:

   * The **headers**
   * All **sections** into their proper locations
3. Fixes relocations
4. Resolves imports
5. Calls `DllMain`

You’re now going to **manually do step 1 and 2**: allocate memory and copy headers/sections.

---

### 🧩 Step-by-Step Breakdown

#### ✅ 1. Parse the headers again

You already have this in your `parser()`:

```c
PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER) buffer;
PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(buffer + dos->e_lfanew);
```

#### ✅ 2. Allocate memory using `VirtualAlloc`

You’ll allocate memory using `nt->OptionalHeader.SizeOfImage`, which is the full size required by the image in memory.

```c
BYTE* remote_image = VirtualAlloc(NULL, nt->OptionalHeader.SizeOfImage,
                                   MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
```

✅ Check if `remote_image` is `NULL`. If it is, `VirtualAlloc` failed.

---

#### ✅ 3. Copy the PE headers

```c
memcpy(remote_image, buffer, nt->OptionalHeader.SizeOfHeaders);
```

---

#### ✅ 4. Copy each section to its `VirtualAddress`

Loop over the section headers and copy each one from the file buffer to its destination in `remote_image`:

```c
PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);

for (int i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
    BYTE* dest = remote_image + section->VirtualAddress;
    BYTE* src  = buffer + section->PointerToRawData;

    memcpy(dest, src, section->SizeOfRawData);
}
```

---

### 🧠 Quick Analogy:

You're **loading the game map into RAM**, byte for byte. No imports resolved, no relocations yet — you're just making it exist in memory exactly as the PE loader would.

---

### ✅ Outcome of This Step:

After this stage, you should now have:

* A **faithful in-memory replica of the DLL**, at the right base address, with all its sections and headers in the correct locations.

We’ll test this before continuing to Stage 4: Relocations.

---

## 🎯 Your Mission

1. Add a function like `manual_map()` that:

   * Accepts `buffer` as input
   * Allocates memory
   * Copies headers and sections
   * Returns the `remote_image` pointer

2. In `_tmain()`, call `manual_map(buffer);`

___

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
