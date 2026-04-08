#pragma once

#include <stddef.h>
#include <stdint.h>

#define OPENGPI_ABI_MAJOR 1
#define OPENGPI_ABI_MINOR 0

#define GACK_ABI_MAJOR 1
#define GACK_ABI_MINOR 0

#define OPENGPI_MAX_CAPS 64

typedef enum {
  OPENGPI_STATUS_OK = 0,
  OPENGPI_STATUS_INVALID_ARGUMENT = 1,
  OPENGPI_STATUS_UNSUPPORTED_VERSION = 2,
  OPENGPI_STATUS_BUFFER_TOO_SMALL = 3,
  OPENGPI_STATUS_NOT_FOUND = 4,
} opengpi_status_t;

typedef enum {
  GACK_CAP_CPU_RASTER = 1,
  GACK_CAP_2D_ACCEL = 2,
  GACK_CAP_GPU_QUEUE = 3,
  GACK_CAP_PRESENTATION_ASSIST = 4,
} gack_capability_t;

typedef enum {
  GACK_CMD_FILL_RECT = 1,
  GACK_CMD_STROKE_PATH = 2,
  GACK_CMD_BLIT = 3,
  GACK_CMD_GLYPH_RUN = 4,
  GACK_CMD_PRESENT = 5,
} gack_command_type_t;

typedef struct {
  uint16_t abi_major;
  uint16_t abi_minor;
  uint32_t capability_count;
  uint32_t capabilities[OPENGPI_MAX_CAPS];
} gack_contract_descriptor_t;

typedef struct {
  uint32_t command_type;
  uint32_t sequence_id;
  uint64_t resource_handle;
  uint64_t fence_in;
  uint64_t fence_out;
  uint32_t payload_size;
  const void *payload;
} gack_command_packet_t;

typedef struct {
  uint16_t requested_major;
  uint16_t requested_minor;
  uint16_t negotiated_major;
  uint16_t negotiated_minor;
  uint8_t acceleration_enabled;
} opengpi_session_info_t;

opengpi_status_t opengpi_negotiate_contract(opengpi_session_info_t *session);
opengpi_status_t opengpi_get_gack_descriptor(gack_contract_descriptor_t *out_desc,
                                             size_t capability_slots);
opengpi_status_t opengpi_validate_gack_packet(const gack_command_packet_t *packet);
int              opengpi_has_capability(const gack_contract_descriptor_t *desc,
                                        uint32_t capability);

