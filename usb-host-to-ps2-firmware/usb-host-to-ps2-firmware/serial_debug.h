#ifdef SERIAL_DEBUG_H
#define SERIAL_DEBUG_H

#include <stdint.h>

void serial_debug_init(void);
void serial_debug_send_buffer(const uint8_t * buffer, const uint16_t len);
void serial_debug_send_string(const char * buffer);

#endif /* SERIAL_DEBUG_H */