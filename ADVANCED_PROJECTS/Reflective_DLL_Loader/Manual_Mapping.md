
---

# **⚙️ Stage 2 – Manually Mapping the DLL into Memory**
🔥 You’ve officially crossed into the heart of Reflective DLL Injection.

This is where you begin playing the role of the Windows loader:

> “Instead of calling `LoadLibrary`, you’ll allocate memory yourself and manually relocate and resolve the DLL’s parts.”

---

### 🧭 What You're Going to Do in Stage 2

You will write code (eventually inside your Reflective Loader) to:

1. **Read your DLL into memory** (from disk or from an embedded resource).
2. **Parse the PE headers** to find:

   * Section headers
   * Image base
   * Entry point
3. **Allocate memory** inside the *target process* (usually via `VirtualAllocEx`).
4. **Copy sections** into their correct virtual memory addresses.
5. **Perform relocations** if the allocated memory is not at the preferred ImageBase.
6. **Resolve imports** manually using `LoadLibraryA` and `GetProcAddress`.
7. **Call the DLL's entry point (`DllMain`)**, passing `DLL_PROCESS_ATTACH`.

---

### 🛠️ The Core Components of Manual Mapping

Here’s the sequence broken into digestible chunks (remember, **no code yet**, just the algorithm):

---

### 🔹 1. **Parse PE Headers**

* Start with the **DOS header**.

  * Verify `MZ` magic number.
  * Use `e_lfanew` to locate the **NT headers**.
* From the **Optional Header**, get:

  * `ImageBase`
  * `SizeOfImage`
  * `AddressOfEntryPoint`
  * `DataDirectory` (e.g., import table, reloc table)
* From the **Section Headers**, get:

  * Virtual size
  * Raw size
  * Raw offset
  * Characteristics (e.g., executable, readable)

---

### 🔹 2. **Allocate Memory in Target Process**

* Use `VirtualAllocEx` (or `VirtualAlloc` if doing self-injection for now).
* Allocate `SizeOfImage` worth of memory.
* Set the right protection flags per section (RWX if unsure for now).

---

### 🔹 3. **Copy Headers and Sections**

* Copy the DOS + NT headers to the start of the allocated memory.
* For each section:

  * Copy `RawData` from file to `VirtualAddress` inside the allocated memory.

---

### 🔹 4. **Fix Relocations (if needed)**

* If the `ImageBase` in the file ≠ actual base address in memory:

  * Go to `.reloc` section.
  * Parse the relocation entries.
  * For each, fix the delta:
    `new_base - old_image_base`
  * Patch addresses accordingly.

---

### 🔹 5. **Resolve Imports**

* Go to the Import Directory in the Optional Header’s DataDirectory.
* For each DLL listed:

  * Call `LoadLibraryA` to load it.
* For each imported function:

  * Use `GetProcAddress`.
  * Patch the Import Address Table (IAT) with the function pointer.

---

### 🔹 6. **Call the Entry Point (DllMain)**

* Cast the base address + EntryPoint RVA as a function pointer.
* Call it like this:
  `DllMain(baseAddress, DLL_PROCESS_ATTACH, NULL)`

---

### 🧪 Test Strategy

Since this is complex:

* You can first **build a prototype** that does this inside your own process (not target injection yet).
* Test each stage (allocation, copy, relocation, import resolution) **in isolation**.
* Once it works, you’ll be ready to embed the Reflective Loader into a DLL and inject it with `CreateRemoteThread`.

---

### 🗂️ Coming Next: Stage 3

After this mapping code is working:

* You’ll move it **inside a DLL**.
* You’ll write a small stub (`DllMain`) that calls your `ReflectiveLoader`.
* That loader will read itself from memory and remap itself in the remote process.

This is the magic of **Reflective DLLs**.

---
