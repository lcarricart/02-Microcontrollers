# Zephyr Setup on Windows for NXP FRDM-RW612

This guide explains how to set up a Zephyr development environment on Windows using PowerShell and VS Code, then build and flash the `hello_world` sample for the NXP FRDM-RW612 board.

## Current Tool Versions

The following tools are already visible from PowerShell:

```text
Python 3.12.10
CMake 4.3.2
Ninja 1.13.2
Git 2.47.0
```

That means the basic Windows toolchain is available and the Zephyr setup can continue.

## Goal

The goal is to:

```text
1. Create a Zephyr workspace
2. Create a Python virtual environment
3. Install west
4. Download Zephyr
5. Install Zephyr dependencies
6. Install the Zephyr SDK
7. Build Hello World for frdm_rw612
8. Flash the board
9. View serial output in VS Code
```

## Important Concepts

### VS Code vs. Zephyr Tools

VS Code is the editor.

Zephyr itself is controlled mainly through command-line tools:

```text
west      Zephyr workspace, build, and flash tool
CMake     build-configuration system
Ninja     low-level build executor
Python    used by Zephyr scripts
SDK       compiler and target tools for embedded builds
```

### PowerShell vs. Serial Monitor

There are two different places where output appears:

```text
PowerShell / VS Code Terminal
→ shows build and flash logs

VS Code Serial Monitor
→ shows application output from the board
```

For example, commands like these run in PowerShell:

```powershell
west build
west flash
```

But output from Zephyr code such as `printk()`, `printf()`, or `LOG_INF()` appears in the serial monitor, not directly inside the normal PowerShell terminal.

---

## Step 1 — Create the Zephyr Workspace

Start from your user folder:

```powershell
PS C:\Users\user>
```

Run:

```powershell
mkdir zephyrproject
cd zephyrproject
```

If the folder already exists, just enter it:

```powershell
cd zephyrproject
```

You should end up here:

```text
PS C:\Users\user\zephyrproject>
```

---

## Step 2 — Create the Python Virtual Environment

Create the virtual environment:

```powershell
py -3.12 -m venv .venv
```

Activate it:

```powershell
.\.venv\Scripts\Activate.ps1
```

Your prompt should change to something like:

```text
(.venv) PS C:\Users\user\zephyrproject>
```

The `(.venv)` part is important. It means the isolated Python environment for Zephyr is active.

### If PowerShell Blocks Activation

Run this once:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Then activate again:

```powershell
.\.venv\Scripts\Activate.ps1
```

---

## Step 3 — Install West

Install `west`:

```powershell
pip install west
```

Check that it is available:

```powershell
west --version
```

You should see a West version number.

`west` is Zephyr's command-line workspace, build, and flash tool.

---

## Step 4 — Initialize the Zephyr Workspace

Make sure you are still here:

```text
(.venv) PS C:\Users\user\zephyrproject>
```

Then initialize the current folder as a Zephyr workspace:

```powershell
west init .
```

Download Zephyr and the required modules:

```powershell
west update
```

This step downloads many repositories, so it may take a while.

After it finishes, the folder should look roughly like this:

```text
zephyrproject/
├── .venv/
├── .west/
├── zephyr/
├── modules/
├── bootloader/
└── tools/
```

---

## Step 5 — Export Zephyr to CMake

Run:

```powershell
west zephyr-export
```

This allows CMake to find Zephyr correctly.

---

## Step 6 — Install Zephyr Python Dependencies

Run:

```powershell
pip install -r zephyr\scripts\requirements.txt
```

This installs the Python packages used by Zephyr scripts.

---

## Step 7 — Install the Zephyr SDK

Run:

```powershell
west sdk install
```

The SDK (Software Development Kit) contains the compiler and target tools needed to build firmware for embedded boards.

---

## Step 8 — Build Hello World for FRDM-RW612

For the NXP FRDM-RW612, the Zephyr board target is:

```text
frdm_rw612
```

Build the first Zephyr application:

```powershell
west build -p always -b frdm_rw612 zephyr\samples\hello_world
```

Meaning:

```text
west build                     build a Zephyr app
-p always                      force a clean/pristine build
-b frdm_rw612                  target board = FRDM-RW612
zephyr\samples\hello_world     application path
```

If the build succeeds, a `build` folder is created.

---

## Step 9 — Flash the Board

Connect the board by USB, then run:

```powershell
west flash
```

If flashing fails because a debug tool is missing, the likely missing piece is a flashing/debug runner such as J-Link or LinkServer.

---

## Step 10 — Open Serial Output in VS Code

The `hello_world` sample prints to the board's serial console.

Install the **Serial Monitor** extension in VS Code if it is not installed yet.

Recommended extension:

```text
Serial Monitor
Publisher: Microsoft
Extension ID: ms-vscode.vscode-serial-monitor
```

Open the Serial Monitor panel and configure it like this:

```text
Port: COMx
Baud rate: 115200
Data bits: 8
Parity: none
Stop bits: 1
Flow control: none
```

This is usually written as:

```text
115200 8N1
```

Meaning:

```text
115200 baud
8 data bits
No parity
1 stop bit
```

After opening the serial monitor, press the board's reset button.

Expected output:

```text
Hello World! frdm_rw612
```

The exact Zephyr boot banner may differ depending on the Zephyr version.

---

## Recommended Command Sequence

Copy and run this block step by step:

```powershell
cd $HOME
mkdir zephyrproject
cd zephyrproject
py -3.12 -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install west
west --version
```

If `west --version` works, continue:

```powershell
west init .
west update
west zephyr-export
pip install -r zephyr\scripts\requirements.txt
west sdk install
west build -p always -b frdm_rw612 zephyr\samples\hello_world
```

Then flash the board:

```powershell
west flash
```

Then open the VS Code Serial Monitor at:

```text
115200 8N1
```

Reset the board and check the serial output.

---

## Building Another Zephyr Sample

When switching to another sample, avoid blindly reusing an old `build` folder.

A safe pattern is:

```powershell
west build -p always -b frdm_rw612 -d build-hello zephyr\samples\hello_world
west flash -d build-hello
```

For another sample:

```powershell
west build -p always -b frdm_rw612 -d build-blinky zephyr\samples\basic\blinky
west flash -d build-blinky
```

The `-d` option chooses the build directory.

Recommended naming pattern:

```text
build-hello
build-blinky
build-mqtt
```

This helps avoid mixing generated files from different samples.

---

## Git Ignore Recommendation

Do not commit Zephyr build folders to your repository.

Add this to `.gitignore`:

```gitignore
**/build/
**/build-*/
```

Useful mental model:

```text
app/              source code, should be committed
build/           generated output, should not be committed
build-hello/     generated output for Hello World
build-blinky/    generated output for Blinky
```

---

## Troubleshooting

### `west` Is Not Recognized

Make sure the virtual environment is active:

```powershell
.\.venv\Scripts\Activate.ps1
```

Then check again:

```powershell
west --version
```

### PowerShell Blocks Virtual Environment Activation

Run:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Then activate again:

```powershell
.\.venv\Scripts\Activate.ps1
```

### No Output Appears in Serial Monitor

Check these points:

```text
1. The correct COM port is selected.
2. Baud rate is set to 115200.
3. Serial Monitor was opened before resetting the board.
4. No other program is using the COM port.
5. You are looking at Serial Monitor, not the normal VS Code terminal.
```

A useful method to identify the correct COM port:

```text
1. Open Device Manager.
2. Go to Ports (COM & LPT).
3. Unplug the board.
4. Check which COM port disappears.
5. Plug the board back in.
6. Use the COM port that reappears.
```

### Build Folder Confusion

The `build` folder is generated output.

You usually do not need to delete it manually.

For a clean rebuild, use:

```powershell
west build -p always -b frdm_rw612 zephyr\samples\hello_world
```

or choose a separate build directory:

```powershell
west build -p always -b frdm_rw612 -d build-hello zephyr\samples\hello_world
```

---

## Final Result

At the end of this setup, the Zephyr environment is ready on Windows, and the FRDM-RW612 can build and run the first Zephyr application:

```text
Hello World! frdm_rw612
```

## HOW-TO Save Project in a Different Directory Than Zephyr Workspace?

Assume:

**Zephyr workspace:**

```text
C:\Users\user\Github-Repository\not-repos\zephyrproject
```

**Your repo:**

```text
C:\Users\user\Github-Repository\my-rw612-project
```

Go to the Zephyr workspace:

```powershell
cd C:\Users\user\Github-Repository\not-repos\zephyrproject
.\.venv\Scripts\Activate.ps1
```

Then build your external project by passing its path:

```powershell
west build -p always -b frdm_rw612 -d C:\Users\user\Github-Repository\my-rw612-project\build C:\Users\user\Github-Repository\my-rw612-project
```

Then use the same build directory for flashing:

```powershell
west flash -d C:\Users\user\Github-Repository\my-rw612-project\build
```

That path tells Zephyr:

```text
Build this application, but use the Zephyr workspace I am currently inside.
```
