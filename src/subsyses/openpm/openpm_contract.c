#include "openpm_contract.h"

static const uint32_t g_openpm_caps[] = {
    OPENPM_CAP_SESSION_MANAGER,
    OPENPM_CAP_WINDOW_MANAGER,
    OPENPM_CAP_INPUT_ROUTER,
    OPENPM_CAP_COMPOSITOR,
    OPENPM_CAP_WAYLAND_APP_COMPAT,
};

openpm_status_t openpm_negotiate_contract(openpm_session_info_t *session) {
  if (!session) {
    return OPENPM_STATUS_INVALID_ARGUMENT;
  }

  if (session->requested_major != OPENPM_ABI_MAJOR) {
    return OPENPM_STATUS_UNSUPPORTED_VERSION;
  }

  session->negotiated_major = OPENPM_ABI_MAJOR;
  session->negotiated_minor = OPENPM_ABI_MINOR;
  session->wayland_app_compat_enabled = 1;
  return OPENPM_STATUS_OK;
}

openpm_status_t openpm_get_contract_descriptor(openpm_contract_descriptor_t *out_desc) {
  if (!out_desc) {
    return OPENPM_STATUS_INVALID_ARGUMENT;
  }

  out_desc->abi_major = OPENPM_ABI_MAJOR;
  out_desc->abi_minor = OPENPM_ABI_MINOR;
  out_desc->capability_count = (uint32_t)(sizeof(g_openpm_caps) / sizeof(g_openpm_caps[0]));
  out_desc->capabilities = g_openpm_caps;
  return OPENPM_STATUS_OK;
}
