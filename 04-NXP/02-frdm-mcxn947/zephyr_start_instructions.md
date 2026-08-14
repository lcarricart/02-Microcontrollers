# Zephyr + LinkServer Setup

The following commands are required Power Shell session. The Zephyr virtual environment needs to be activated and two paths need to be added (Zephyr base and LinkServer).

1. `cd C:\Users\luchi\zephyrproject`
2. `.\.venv\Scripts\Activate.ps1`
3. `$env:ZEPHYR_BASE = "C:\Users\luchi\zephyrproject\zephyr"`
4. `$env:Path += ";C:\nxp\LinkServer_25.6.131"`
5. `cd "C:\Users\luchi\source\repos\lcarricart\02-Microcontrollers\04-NXP\02-frdm-mcxn947\03_zephyr_can_loopback"`

## Useful Commands
- `west build -b frdm_mcxn947/mcxn947/cpu0`
- (Open debug session to attach VSC GUI) `west debugserver -d build -r linkserver`
