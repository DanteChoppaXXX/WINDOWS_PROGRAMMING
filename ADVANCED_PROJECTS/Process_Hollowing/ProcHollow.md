Excellent. Let’s now craft a **clear, structured algorithm** for **Process Hollowing** — no code, just the mental blueprint. This will make it easier to implement and understand each part with purpose.

---

## 🧠 🔹 Process Hollowing — Full Algorithm Outline (Windows x86/x64)

> 🎯 *Goal:* Start a legit process (like `notepad.exe`), suspend it, hollow out its memory, inject a malicious image or shellcode, set the entry point, then resume the thread.

---

### 🪛 **Phase 1: Create Suspended Legit Process**

1. **Choose a legitimate host process** (e.g., `notepad.exe`, `svchost.exe`).
2. **Create the process in suspended mode**, using flags like `CREATE_SUSPENDED`.
3. **Obtain the process handle** and **main thread handle** from the `PROCESS_INFORMATION` structure.

---

### 🧬 **Phase 2: Retrieve Remote Process Information**

4. Use a **syscall or API (e.g., `NtQueryInformationProcess`)** to obtain the **PEB address** of the remote process.
5. Read the **Image Base Address** from the PEB using `ReadProcessMemory`.

---

### ❌ **Phase 3: Hollow Out the Original Image**

6. **Unmap the original executable image** in the remote process using a syscall like `NtUnmapViewOfSection`.

   * This clears out the original binary (e.g., `notepad.exe`) from memory.

---

### 🧠 **Phase 4: Prepare the Replacement Payload**

7. Load the **malicious image** or shellcode into memory (locally, in your injector process).
8. Parse the image’s **PE headers** to get:

   * Image Base
   * Size of image
   * Entry point
   * Section headers (text, data, etc.)

---

### 📦 **Phase 5: Inject the Payload into the Remote Process**

9. **Allocate memory** inside the remote process using `VirtualAllocEx`, using the original image base address if possible.
10. **Write the PE headers** of your malicious image into the allocated memory.
11. For each section in the PE:

    * **Write its contents** into the remote process using `WriteProcessMemory`.

---

### 🧭 **Phase 6: Set Thread Context to New Entry Point**

12. Retrieve the **context of the suspended thread** using `GetThreadContext`.
13. Modify the **instruction pointer** (EIP/RIP) to point to the **Entry Point of your injected payload**.
14. Set the modified thread context using `SetThreadContext`.

---

### ▶️ **Phase 7: Resume Execution**

15. Resume the suspended thread using `ResumeThread`.
16. The process now runs your payload while appearing to be the legitimate host.

---

### 🔒 **(Optional) Evasion Techniques**

17. Modify **PE header** to hide evidence (e.g., zero out headers).
18. Set **Parent PID spoofing** for stealth using `STARTUPINFOEX`.
19. Use **sleeping or injection delay** to avoid sandbox detection.
20. Clean up handles or erase mapped file paths.

---

## ✅ Final Outcome

* Process name: `notepad.exe` (or other legitimate binary)
* Behavior: Runs your custom shellcode or PE
* Appearance: Looks completely legitimate in Task Manager or Process Hacker
* Use cases:

  * Malware loaders
  * C2 implants
  * AV evasion
  * Red teaming stealth implants

---

## 🧠 TL;DR Mental Map:

```
Create legit suspended process
→ Hollow it out
→ Inject your code
→ Set entry point
→ Resume
```

---

Let me know if you'd now like:

* 🛠 The **actual implementation in C (step-by-step)**
* 🧪 A walkthrough of building a **dummy payload**
* 🔍 How to detect hollowing using forensic tools
* ⚔️ How to chain this with DLL injection or token stealing for privilege escalation

What’s your move, operator?
