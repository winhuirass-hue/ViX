# ViX

ViX is an x86-family POSIX-oriented hobby operating system written mostly in C.

## Highlights

- Monolithic kernel with a readable C-first codebase
- Limine-based boot flow
- FAT32 boot partition + Ext2 root partition
- Linux-like syscall shim layer
- Networking stack integration (lwIP)
- Build support for both:
  - `x86_64` (default)
  - `i686` (32-bit build path)

## Quick start

### 1) Build dependencies/toolchain

```bash
make tools
```

### 2) Build disk image (default x86_64)

```bash
make disk
```

### 3) Run in QEMU

```bash
make qemu
```

---

## Architecture selection

The build system supports selecting the kernel architecture with `ARCH`:

- `ARCH=x86_64` (default)
- `ARCH=i686`

Examples:

```bash
# Raw kernel build
make kernel ARCH=i686

# Full disk build for i686
make disk ARCH=i686
```

Kernel make logic (`src/kernel/Makefile`) automatically switches compiler/assembler/linker flags for the selected architecture.

---

## Minimal freestanding libc shims in kernel headers

To support freestanding kernel and third-party code expectations, the kernel include tree provides mini headers:

- `ctype.h`
- `stdarg.h`
- `stdbool.h`
- `stddef.h`
- `stdint.h`
- `stdio.h`
- `stdlib.h`
- `string.h` helpers

These are intentionally lightweight and mapped to in-kernel implementations.

---

## Common targets

```bash
make kernel              # build raw kernel
make disk                # build bootable image
make disk_dirty          # build image without cleaning
make qemu                # run QEMU
make qemu_dbg            # run debug QEMU
make clean               # clean kernel build artifacts
make format              # run formatter
make format_check        # formatter check
```

---

## Project structure (high-level)

- `src/kernel/` — kernel sources
- `src/libs/` — userland/library build components
- `src/software/` — test/user programs
- `tools/` — toolchain/build helper scripts
- `docs/` — project docs

---

## Notes

- This project uses a custom cross-toolchain (`*-cavos-*`).
- If toolchain checks fail, re-run `make tools` and verify binaries in `~/opt/cross/bin/`.
