#pragma once

#include <stdint.h>

#define OPENPM_ABI_MAJOR 1
#define OPENPM_ABI_MINOR 0

/*
 * OpenPM is the canonical windowing subsystem for ViX, inspired by PM on OS/2.
 * It defines the system-level contracts for sessions, desktops, windows, focus,
 * and input routing. Rendering/presentation is delegated to OpenGPI over the
 * GACK graphics frontend.
 *
 * Wayland compatibility is supported as an application-facing compatibility
 * profile only. Wayland semantics are translated at the app boundary and do not
 * redefine OpenPM object semantics or security policy.
 */

typedef enum {
  OPENPM_STATUS_OK = 0,
  OPENPM_STATUS_INVALID_ARGUMENT = 1,
  OPENPM_STATUS_UNSUPPORTED_VERSION = 2,
  OPENPM_STATUS_PERMISSION_DENIED = 3,
  OPENPM_STATUS_NOT_FOUND = 4,
} openpm_status_t;

typedef enum {
  OPENPM_CAP_SESSION_MANAGER = 1,
  OPENPM_CAP_WINDOW_MANAGER = 2,
  OPENPM_CAP_INPUT_ROUTER = 3,
  OPENPM_CAP_COMPOSITOR = 4,
  OPENPM_CAP_WAYLAND_APP_COMPAT = 5,
} openpm_capability_t;

typedef struct {
  uint16_t abi_major;
  uint16_t abi_minor;
  uint32_t capability_count;
  const uint32_t *capabilities;
} openpm_contract_descriptor_t;

typedef struct {
  uint16_t requested_major;
  uint16_t requested_minor;
  uint16_t negotiated_major;
  uint16_t negotiated_minor;
  uint8_t wayland_app_compat_enabled;
} openpm_session_info_t;

openpm_status_t openpm_negotiate_contract(openpm_session_info_t *session);
openpm_status_t openpm_get_contract_descriptor(openpm_contract_descriptor_t *out_desc);

