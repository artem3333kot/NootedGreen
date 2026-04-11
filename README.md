# NootedGreen

Lilu plugin for Intel Raptor Lake-P (RPL-P) iGPU acceleration on macOS Sonoma via TGL driver spoofing.

## What it does

Patches Apple's Tiger Lake (Gen12) graphics drivers to work with Raptor Lake-P (Gen12.5) silicon. Handles MMIO addressing, ForceWake, GPU topology, display controller init, and GT workarounds.

## Status

**Work in progress.** Display output works. GPU acceleration (stamp 3) is under active debugging.

## Requirements

- [Lilu](https://github.com/acidanthera/Lilu) 1.7.2+
- macOS Sonoma 14.x
- Intel Raptor Lake-P iGPU (device ID `0x9A49`)
- AAPL,ig-platform-id: `0300C89B`

## Boot args

```
-v keepsyms=1 debug=0x100 -liludbg -NGreenDebug -disablegfxfirmware -nbdyldoff ngreen-dmc=skip -allow3d
```

| Arg | Purpose |
|---|---|
| `-NGreenDebug` | Enable NootedGreen debug logging |
| `-disablegfxfirmware` | Disable GuC/HuC firmware loading |
| `-nbdyldoff` | Disable DYLD patches |
| `ngreen-dmc=skip` | Skip DMC firmware |
| `-allow3d` | Force 3D acceleration |

## Building

Open `NootedBlue.xcodeproj`, select the **NootedGreen** scheme, and build with Xcode.

## License

[Thou Shalt Not Profit License 1.0](LICENSE)
