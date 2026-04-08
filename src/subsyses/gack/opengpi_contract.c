#include "opengpi_contract.h"

static const uint32_t g_default_caps[] = {
    GACK_CAP_CPU_RASTER,
    GACK_CAP_2D_ACCEL,
    GACK_CAP_PRESENTATION_ASSIST,
};

opengpi_status_t opengpi_negotiate_contract(opengpi_session_info_t *session) {
  if (!session) {
    return OPENGPI_STATUS_INVALID_ARGUMENT;
  }

  if (session->requested_major != OPENGPI_ABI_MAJOR) {
    return OPENGPI_STATUS_UNSUPPORTED_VERSION;
  }

  session->negotiated_major = OPENGPI_ABI_MAJOR;
  session->negotiated_minor = OPENGPI_ABI_MINOR;
  session->acceleration_enabled = 1;

  return OPENGPI_STATUS_OK;
}

opengpi_status_t opengpi_get_gack_descriptor(gack_contract_descriptor_t *out_desc,
                                             size_t capability_slots) {
  size_t i;
  size_t cap_count = sizeof(g_default_caps) / sizeof(g_default_caps[0]);

  if (!out_desc) {
    return OPENGPI_STATUS_INVALID_ARGUMENT;
  }

  if (capability_slots < cap_count) {
    return OPENGPI_STATUS_BUFFER_TOO_SMALL;
  }

  out_desc->abi_major = GACK_ABI_MAJOR;
  out_desc->abi_minor = GACK_ABI_MINOR;
  out_desc->capability_count = (uint32_t)cap_count;

  for (i = 0; i < cap_count; i++) {
    out_desc->capabilities[i] = g_default_caps[i];
  }

  return OPENGPI_STATUS_OK;
}

opengpi_status_t opengpi_validate_gack_packet(const gack_command_packet_t *packet) {
  if (!packet) {
    return OPENGPI_STATUS_INVALID_ARGUMENT;
  }

  if (packet->payload_size > 0 && !packet->payload) {
    return OPENGPI_STATUS_INVALID_ARGUMENT;
  }

  switch (packet->command_type) {
  case GACK_CMD_FILL_RECT:
  case GACK_CMD_STROKE_PATH:
  case GACK_CMD_BLIT:
  case GACK_CMD_GLYPH_RUN:
  case GACK_CMD_PRESENT:
    return OPENGPI_STATUS_OK;
  default:
    return OPENGPI_STATUS_NOT_FOUND;
  }
}

int opengpi_has_capability(const gack_contract_descriptor_t *desc,
                           uint32_t capability) {
  uint32_t i;

  if (!desc) {
    return 0;
  }

  for (i = 0; i < desc->capability_count && i < OPENGPI_MAX_CAPS; i++) {
    if (desc->capabilities[i] == capability) {
      return 1;
    }
  }

  return 0;
}
