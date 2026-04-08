# ViX Transition Architecture for a Unix-like OS

## 1) Conceptual architecture diagram

```text
+----------------------------------------------------------------------------------+
|                           ViX Contract-Oriented System                           |
+----------------------------------------------------------------------------------+
| User Runtimes                                                                    |
| - Language runtimes (C, Rust, managed VMs)                                      |
| - App frameworks (portable, non-ABI, replaceable)                               |
+----------------------------------------------------------------------------------+
| Versioned Subsystems (stable semantic contracts)                                 |
| - OpenPM  : windowing, input focus, composition, desktop/session semantics       |
| - OpenGPI : drawing surfaces, compositing primitives, display pipelines          |
| - OpenIPC : channels, capabilities, routing, identity, QoS                       |
| - OpenEXE : executable formats, process model, lifecycle policy                  |
| - OpenSCH : scheduling classes, quotas, latency/throughput contracts             |
| - OpenTIME: monotonic/realtime domains, timers, determinism rules                |
| - OpenIMG : canonical image/pixel ABI (`vix_image.h`)                            |
+----------------------------------------------------------------------------------+
| System ABI Layer (frozen C ABI + binary object contracts + feature negotiation)  |
| - Handle model, object lifetimes, rights model                                   |
| - Explicit versioning, capability descriptors, structured errors                 |
| - Compatibility stubs and translation shims                                      |
+----------------------------------------------------------------------------------+
| Kernel Core (implementation, not product identity)                               |
| - Memory management, isolation, interrupts, low-level scheduling                 |
| - IPC transport primitives, VM objects, device mediation                         |
| - Policy hooks consumed by subsystem managers                                    |
+----------------------------------------------------------------------------------+
| Hardware, firmware, and drivers (replaceable backends)                           |
+----------------------------------------------------------------------------------+
```

The key reinterpretation is that **the OS is the contract surface** (System ABI + formal subsystems), while kernel and libraries are replaceable implementations.

---

## 1.1) OpenGPI + OpenPM specialization (OS/2-inspired, ViX-native)

To make the transition concrete, OpenGPI and OpenPM are defined as **normative system subsystems** (not toolkit behavior), borrowing architectural intent from OS/2 while remaining ViX contract-first.

### OpenGPI (OS/2 GPI-like graphics contract)

OpenGPI is the canonical 2D/compose/render contract for the OS:

- **Device-context contract**: stable drawing contexts with explicit target surfaces and rights.
- **Path/text/raster primitives**: deterministic semantic set (fill, stroke, blit, glyph run, clip, transform).
- **Surface model**: shared memory surface handles + synchronization fences.
- **Color and format contract**: standardized pixel formats tied to OpenIMG (`vix_image.h`), including color space metadata.
- **Backend independence**: software rasterizers, GPU pipelines, or third-party engines are implementation modules only.

### GACK render contract (Graphics Acceleration Contract Kernel)

Inside OpenGPI, the render backend boundary is formalized as the **GACK contract**:

- Defines command submission packets, resource residency/mapping, and fence semantics.
- Declares capability classes (CPU, 2D accel, GPU queue, presentation assist) via feature negotiation.
- Provides deterministic fallback rules when acceleration features are missing.
- Prevents any renderer-specific ABI leak (e.g., Cairo, Skia, vendor APIs).

GACK is not exposed as a third-party library ABI; it is a versioned internal subsystem contract under System ABI governance.

### OpenPM (OS/2 PM server-like session and window manager contract)

OpenPM is a privileged subsystem server that owns presentation semantics:

- **Session and desktop objects**: login/session/workspace lifecycle as explicit state machines.
- **Window object model**: creation, parenting, z-order, visibility, and damage propagation contracts.
- **Input routing and focus policy**: keyboard/pointer delivery, focus transfer, and grab/capture rules.
- **Composition policy hooks**: ties into OpenGPI for present scheduling and frame policy.
- **Security boundaries**: strict contracts for input injection, capture/screenshot, accessibility, and automation channels.

Toolkits (GTK/Qt/custom) are clients of OpenPM/OpenGPI and cannot redefine system-level behavior.

---

## 2) Unix concepts → ViX concepts

| Unix-like concept | Keep / transform / remove | ViX reinterpretation |
|---|---|---|
| Syscall-by-convention API | Transform | Define a frozen System ABI with explicit object contracts, stable structs, and semantic versioning. |
| POSIX as default behavioral law | Transform | POSIX becomes an optional compatibility profile implemented atop ViX contracts. |
| `fork/exec` implicit process semantics | Transform | OpenEXE process/session contracts with explicit inheritance and policy descriptors. |
| Signals as ambient async control | Deprecate | Replace with typed event channels and capability-bound control endpoints in OpenIPC/OpenEXE. |
| `/proc` text interfaces for control | Transform | Structured introspection/control objects with binary-stable schemas (optionally projected to text). |
| `/dev/fb0` and ad-hoc graphics IOCTLs | Remove as primitive | OpenGPI formal graphics objects and negotiated capabilities; framebuffer exposed only via compatibility shim. |
| X11/Wayland as userland stack convention | Transform | OpenPM/OpenGPI become first-class OS subsystems with normative ABI and security model. |
| Toolkit-defined GUI behavior (GTK/Qt assumptions) | Remove as primitive | Toolkits consume OpenPM/OpenGPI; they no longer define system GUI semantics. |
| Library-specific image handling (PNG/JPEG/bitmap paths) | Transform | OpenIMG canonical in-memory format (`vix_image.h`) with format adapters at edges. |
| Scheduler knobs as implementation detail | Transform | OpenSCH exposes contractual scheduling classes, quotas, and admission rules. |
| Unix permissions + DAC only | Transform | Capability-centric rights model + policy layer; legacy UID/GID retained only in compatibility domain. |
| IPC split across pipes/sockets/shared memory with weak typing | Transform | OpenIPC typed channels, object passing, identity, QoS, and policy hooks as one subsystem. |
| Time APIs with mixed semantics | Transform | OpenTIME with explicit time domains (monotonic, realtime, synchronized), guarantees, and drift policy. |
| ABI leakage from libc/toolchain details | Remove | Freeze ABI at OS boundary; libc/compiler/runtime can evolve independently. |
| Third-party library ABI exposure (e.g., Cairo ABI bleed) | Remove | External engines are plugins/backends under subsystem contracts, never public system ABI. |

---

## 3) ABI contracts to freeze

Freeze these as normative, versioned contracts (e.g., `vix_abi_v1` namespace with forward-negotiation to v2+):

1. **Core object ABI**
   - Handle encoding, ownership transfer, duplication, revocation.
   - Object discovery and feature negotiation descriptors.

2. **Error and status ABI**
   - Stable, enumerable status space (no backend-specific errno leakage).
   - Structured error payload extension mechanism.

3. **Memory and buffer ABI**
   - Shared buffer objects, mapping rights, cache/coherency hints.
   - Zero-copy transfer rules for IPC/graphics/images.

4. **OpenIPC ABI**
   - Typed message envelopes, capability transfer, endpoint identity.
   - QoS classes, flow control, backpressure semantics.

5. **OpenEXE ABI**
   - Executable object format contract, launch descriptor, inheritance map.
   - Process/job/session lifecycle state machine.

6. **OpenSCH ABI**
   - Scheduling class definitions, quota contracts, priority bands.
   - Reservation/admission APIs and deterministic behavior guarantees.

7. **OpenTIME ABI**
   - Clock domain IDs, timestamp formats, timer and deadline semantics.
   - Monotonicity, leap/sync behavior, virtualization behavior.

8. **OpenGPI ABI**
   - Surface/context objects, composition primitives, sync fences.
   - Color space metadata, frame pacing, presentation contracts.

9. **OpenPM ABI**
   - Window/session objects, input focus model, desktop policy hooks.
   - Security boundaries for capture, injection, accessibility.

10. **OpenIMG ABI (`vix_image.h`)**
    - Canonical pixel format enum, layout/stride rules, metadata blocks.
    - Conversion and validation API contract; decoder plugins out-of-ABI.

11. **Policy ABI**
    - Execution policy descriptors, IPC policy descriptors, scheduling policy profiles.
    - Signed policy bundle format and enforcement API.

12. **Observability ABI**
    - Structured tracing/events/metrics schema contracts.
    - Stable correlation IDs across kernel and subsystem boundaries.

---

## 4) Unix assumptions to remove or deprecate

1. **“Everything is a file” as universal control model**  
   Keep file semantics for storage; remove it as the mandatory control plane for graphics, IPC, execution policy, and scheduling.

2. **Text streams as privileged control interface**  
   Text remains a projection/debug surface, not normative machine interface.

3. **Implicit inheritance (`fork`) as default composition primitive**  
   Replace with explicit launch contracts and inheritance manifests.

4. **Ambient authority model (UID-driven broad access)**  
   Shift to least-privilege capabilities and explicit delegation tokens.

5. **GUI defined outside the OS contract**  
   Promote GUI/session/input/composition into OpenPM/OpenGPI.

6. **ABI defined by “what current libc exposes”**  
   Freeze ABI documents and conformance tests independent of libc implementation.

7. **Backend-specific behavior as de facto standard**  
   Require feature negotiation and declared fallback behavior.

8. **Scheduler behavior treated as best effort only**  
   Introduce contractual service classes and policy-governed admission.

9. **Time semantics assumed but unspecified**  
   Require explicit clock domain selection and deterministic timeout rules.

10. **Security policy scattered across ad-hoc daemons/config**  
    Unify into signed, versioned policy artifacts evaluated by subsystem policy engines.

---

## 5) Staged migration strategy (incremental, no flag-day rewrite)

### Stage 0 — Contract inventory and freeze perimeter
- Inventory existing syscall/ioctl/libc surfaces and classify: retain, shim, deprecate, replace.
- Publish `vix_abi_v1` baseline spec with object, error, and version negotiation rules.
- Stand up ABI conformance tests and binary compatibility CI gates.

### Stage 1 — Compatibility shell around legacy Unix behavior
- Implement a **Unix compatibility runtime** atop early ViX core contracts.
- Keep existing binaries running where practical, but route through translation layers.
- Mark non-deterministic/unsafe legacy features as compatibility-only.

### Stage 2 — Subsystem extraction and formalization
- Carve out OpenIPC, OpenTIME, OpenSCH contracts first (security/policy critical).
- Introduce OpenGPI and OpenPM service managers with strict ABI boundaries.
- Move GUI ownership from X11/toolkit conventions to OpenPM/OpenGPI contracts.

### Stage 3 — Image and graphics normalization
- Land `vix_image.h` and OpenIMG conversion/validation contract.
- Require subsystem-internal canonical image surfaces; decode/encode libraries become adapters.
- Decouple Cairo or any raster engine into replaceable backend modules.

### Stage 4 — Policy-first execution model
- Deploy OpenEXE launch descriptors and explicit inheritance model.
- Introduce signed execution/scheduling/IPC policy bundles and enforcement hooks.
- Deprecate direct reliance on implicit POSIX process/signal semantics.

### Stage 5 — Deprecation and hardening
- Publish deprecation timelines for legacy ioctl/text control paths.
- Enforce subsystem contract version pinning and capability checks.
- Lock long-term support policy around ABI major versions and formal conformance suites.

### Stage 6 — Long-horizon stewardship
- Govern changes through ABI review board + public contract RFCs.
- Open-source architecture artifacts (specs, state machines, tests), not just implementation code.
- Treat implementation diversity as healthy as long as conformance remains exact.

---

## 6) Boundary definitions

### Kernel
- Provides primitive isolation, transport, scheduling mechanics, hardware mediation.
- Does **not** define user-visible semantics by accident.

### System ABI
- Single canonical contract surface for binary compatibility.
- Owns handles, object semantics, status codes, and version negotiation.

### Subsystems
- Domain contracts (OpenPM/OpenGPI/OpenIPC/OpenTIME/OpenSCH/OpenEXE/OpenIMG).
- Each subsystem has normative state machines, policy hooks, and compatibility matrix.

### User runtimes
- libc/language VMs/toolkits/engines.
- Freely replaceable; can optimize ergonomics but cannot redefine system semantics.

---

## 7) Practical policy posture

- **Determinism first** for time, scheduling, and IPC under declared policy profiles.
- **Capability security by construction** instead of post-hoc permission patching.
- **ABI longevity as product promise** with strict semantic version governance.
- **Open architecture by default**: publish contracts, invariants, test vectors, and conformance tooling.

This transition preserves useful Unix assets (process isolation, filesystem strengths, mature tooling) while replacing historical ambiguity with durable contracts, enabling a ViX-style OS defined by semantics rather than inherited behavior.
