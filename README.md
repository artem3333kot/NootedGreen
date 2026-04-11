# NootedGreen

Lilu plugin for Intel Raptor Lake-P (RPL-P) iGPU acceleration on macOS Sonoma via TGL driver spoofing.

## What it does

Patches Apple's Tiger Lake (Gen12) graphics drivers to work with Raptor Lake-P (Gen12.5) silicon. Handles MMIO addressing, ForceWake, GPU topology, display controller init, and GT workarounds.

## Status

**Work in progress.** Framebuffer controller starts, combo PHY calibration patched, accelerator ring initialises. Display pipeline under active development.

## Requirements

- [Lilu](https://github.com/acidanthera/Lilu) 1.7.2+
- macOS Sonoma 14.x
- Intel Raptor Lake-P iGPU
- SMBIOS: `MacBookPro16,2`
- Cpuid1Data spoof to Ice Lake (`0x000706E5`)
- No iGPU DeviceProperties needed — NootedGreen auto-injects `AAPL,ig-platform-id`
- Discrete GPU disabled via `disable-gpu` on its PCI path

## Boot args

```
-v keepsyms=1 debug=0x100 IGLogLevel=8 -wegdbg -NGreenDebug -liludbg liludump=60 -nbdyldoff ngreen-dmc=skip -allow3d -disablegfxfirmware
```

| Arg | Purpose |
|---|---|
| `-NGreenDebug` | Enable NootedGreen debug logging |
| `-disablegfxfirmware` | Disable GuC/HuC firmware loading |
| `-nbdyldoff` | Disable DYLD patches |
| `ngreen-dmc=skip` | Skip DMC firmware |
| `-allow3d` | Force 3D acceleration |
| `-nbwegcoex` | Enable WEG coexistence mode (run alongside WhateverGreen, skips overlapping routes) |
| `IGLogLevel=8` | Maximum Intel GPU driver logging |
| `-wegdbg` | Enable WhateverGreen debug logging |
| `-liludbg` | Enable Lilu debug logging |
| `liludump=60` | Dump Lilu logs after 60 seconds |

## Compatibility

The original NootedBlue support is fully preserved:

- **Haswell** (10.12+)
- **Broadwell** (10.14+)
- **Braswell** (10.14+)

NootedGreen adds:

- **Raptor Lake-P** (macOS Sonoma 14.x) — work in progress

## Building

Open `NootedGreen.xcodeproj` and select the **NootedGreen** scheme to build the RPL-P plugin, or one of the original NootedBlue schemes for legacy hardware. Build with Xcode.

## Authors

- **Stefano Giammorino** ([@sgiammori](https://github.com/sgiammori)) — reverse engineering, driver development, hardware testing
- **Claude Sonnet** (Anthropic) — AI pair-programming, code generation, debug analysis
- **Claude Opus 4.6** via GitHub Copilot — AI pair-programming, code generation, debug analysis

## License

[Thou Shalt Not Profit License 1.0](LICENSE)
