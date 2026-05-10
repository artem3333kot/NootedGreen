//  Copyright © 2026 Stezza @ inc. Licensed under the Thou Shalt Not Profit License version 1.0.
//  See LICENSE for details.
//
//  AppleIntelParams.hpp — typed mirror of Apple's framebuffer-driver parameter
//  structures. Reconstructed from Ghidra analysis of AppleIntelTGLGraphicsFramebuffer
//  (Display 12 layout) and confirmed against the call sites in setupDSCEngineParams,
//  hwSetMode, hwSetupMemory, paramsSurfCompare, setupPipeScaler, setupPipeWatermarks.
//
//  These structs replace raw `pending[N]` uint32_t array indexing in our hooks. The
//  layout is what Apple's TGL framebuffer driver writes to / reads from — using the
//  named fields lets us address the origin of wrong values (per jalavoui's advice:
//  "fix the origin, don't hack memory writes after-the-fact"). Each offset is verified
//  against IDA disassembly hex offsets — do not guess.
//
//  299 call sites use CRTCParams across the kext (SetupParams, SetupTimings,
//  SetupDPSSTTimings, SetupMSTDPTimings, SetupDPTimings, setupPipeScaler×2,
//  setupPipeWatermarks, setupDSCEngineParams×3, paramsSurfCompare×2, hwSetMode,
//  hwSetupMemory, hwCRTCToIODetailedTiming, hwGetCRTC, etc).

#ifndef AppleIntelParams_hpp
#define AppleIntelParams_hpp

#include <stdint.h>

namespace AppleIntel {

// CRTCParams — per-pipe transcoder + DSC + plane configuration block built by
// AppleIntelBaseController::SetupParams and consumed by hwSetMode. Layout
// matches what Apple's binary code writes at fixed offsets — fields with `_padN`
// names are gaps where Apple uses the offset for state we have not yet identified.
//
// Verified offsets (Ghidra Structure Editor + IDA disasm cross-check):
//   0x00 TRANS_CLK_SEL       — transcoder clock source select
//   0x04 TRANS_DDI_FUNC_CTL  — DDI port enable / mode / lane count (V97P clears bit16)
//   0x08 TRANS_DDI_FUNC_CTL2 — secondary DDI function control
//   0x0C TRANS_MSA_MISC      — MSA misc (sync, colorimetry, depth)
//   0x10 TRANS_HTOTAL        — horizontal total / active
//   0x14 TRANS_HBLANK        — horizontal blanking
//   0x18 TRANS_HSYNC         — horizontal sync
//   0x1C TRANS_VTOTAL        — vertical total / active
//   0x20 TRANS_VBLANK        — vertical blanking
//   0x24 TRANS_VSYNC         — vertical sync
//   0x28 PIPE_SRCSZ          — pipe source size (width-1 in low half, height-1 in high half)
//   0x2C TRANS_CONF          — transcoder enable / depth / interlace (V97C aligns to HW)
//   0x88-0xB4 PPS_0..PPS_16  — DSC PPS (12 uint32_t slots; sparse — Apple writes PPS_0/1/2/3/4/5/6/7/8/9/10/16)
//   0xE8 DSC_ENGINE_SEL      — DSC engine select bits (0x40000000/0x60000000/0x70000000)
//   0xEC DSC_JOINER_CTL      — DSC joiner control (0x80008000 / 0x80000000)
//
// CRTCParams is at least 0xF0 bytes; full size is unknown but Apple does not
// reference offsets beyond DSC fields in the paths we have traced.
struct CRTCParams {
    uint32_t TRANS_CLK_SEL;       // +0x00
    uint32_t TRANS_DDI_FUNC_CTL;  // +0x04
    uint32_t TRANS_DDI_FUNC_CTL2; // +0x08
    uint32_t TRANS_MSA_MISC;      // +0x0C
    uint32_t TRANS_HTOTAL;        // +0x10
    uint32_t TRANS_HBLANK;        // +0x14
    uint32_t TRANS_HSYNC;         // +0x18
    uint32_t TRANS_VTOTAL;        // +0x1C
    uint32_t TRANS_VBLANK;        // +0x20
    uint32_t TRANS_VSYNC;         // +0x24
    uint32_t PIPE_SRCSZ;          // +0x28
    uint32_t TRANS_CONF;          // +0x2C
    uint32_t _pad_30[(0x88 - 0x30) / 4]; // +0x30..+0x87 unknown
    uint32_t PPS_0;               // +0x88
    uint32_t PPS_1;               // +0x8C
    uint32_t PPS_2;               // +0x90
    uint32_t PPS_3;               // +0x94
    uint32_t PPS_4;               // +0x98
    uint32_t PPS_5;               // +0x9C
    uint32_t PPS_6;               // +0xA0
    uint32_t PPS_7;               // +0xA4
    uint32_t PPS_8;               // +0xA8
    uint32_t PPS_9;               // +0xAC
    uint32_t PPS_10;              // +0xB0
    uint32_t PPS_16;              // +0xB4
    uint32_t _pad_B8[(0xE8 - 0xB8) / 4]; // +0xB8..+0xE7 unknown
    uint32_t DSC_ENGINE_SEL;      // +0xE8
    uint32_t DSC_JOINER_CTL;      // +0xEC
};
static_assert(__builtin_offsetof(CRTCParams, TRANS_DDI_FUNC_CTL) == 0x04, "CRTCParams.TRANS_DDI_FUNC_CTL offset");
static_assert(__builtin_offsetof(CRTCParams, PIPE_SRCSZ)         == 0x28, "CRTCParams.PIPE_SRCSZ offset");
static_assert(__builtin_offsetof(CRTCParams, TRANS_CONF)         == 0x2C, "CRTCParams.TRANS_CONF offset");
static_assert(__builtin_offsetof(CRTCParams, PPS_0)              == 0x88, "CRTCParams.PPS_0 offset");
static_assert(__builtin_offsetof(CRTCParams, PPS_16)             == 0xB4, "CRTCParams.PPS_16 offset");
static_assert(__builtin_offsetof(CRTCParams, DSC_ENGINE_SEL)     == 0xE8, "CRTCParams.DSC_ENGINE_SEL offset");
static_assert(__builtin_offsetof(CRTCParams, DSC_JOINER_CTL)     == 0xEC, "CRTCParams.DSC_JOINER_CTL offset");

// SCALERPARAMS — per-scaler config block, used by setupPipeScaler and
// AppleIntelScaler17syncScalerUpdate. Possibly incomplete (Ghidra dump shows
// 3 confirmed fields; more may exist after +0x0C).
//
// Verified offsets:
//   0x00 PS_CTRL    — scaler enable + filter mode
//   0x04 PS_WIN_POS — scaler window position (top-left x:16, y:16)
//   0x08 PS_WIN_SZ  — scaler window size (width:16, height:16)
struct SCALERPARAMS {
    uint32_t PS_CTRL;    // +0x00
    uint32_t PS_WIN_POS; // +0x04
    uint32_t PS_WIN_SZ;  // +0x08
};
static_assert(__builtin_offsetof(SCALERPARAMS, PS_CTRL)    == 0x00, "SCALERPARAMS.PS_CTRL offset");
static_assert(__builtin_offsetof(SCALERPARAMS, PS_WIN_POS) == 0x04, "SCALERPARAMS.PS_WIN_POS offset");
static_assert(__builtin_offsetof(SCALERPARAMS, PS_WIN_SZ)  == 0x08, "SCALERPARAMS.PS_WIN_SZ offset");

// PLANEPARAMS — per-plane config block consumed by paramsSurfCompare and
// hwSetMode. Friend removed this from the Ghidra export so we have to rebuild
// from disasm. Confirmed entry points (paramsSurfCompare signature):
//   ulong paramsSurfCompare(CRTCParams *old, CRTCParams *new, PLANEPARAMS *old, PLANEPARAMS *new)
//
// Fields below are placeholders — fill in as we identify offsets from disasm.
// For now this is a forward declaration so we can write hook signatures that
// accept the right pointer types instead of `void *`.
struct PLANEPARAMS;

} // namespace AppleIntel

#endif // AppleIntelParams_hpp
