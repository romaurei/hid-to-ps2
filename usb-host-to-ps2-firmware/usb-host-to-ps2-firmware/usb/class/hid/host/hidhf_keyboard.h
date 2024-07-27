/*
 * hidhf_keyboard.h
 *
 * Created: 9/06/2024 4:32:17 PM
 *  Author: RoMaNo
 */ 


#ifndef HIDHF_KEYBOARD_H_
#define HIDHF_KEYBOARD_H_

#include "usbhf.h"
#include "usb_protocol_hid.h"

#ifndef CONF_HIDHF_KEYBOARD_RPT_SIZE
#define CONF_HIDHF_KEYBOARD_RPT_SIZE 8
#endif

#ifndef HIDHF_IN_PIPE_IDX
#define HIDHF_IN_PIPE_IDX 0
#endif

struct hidhf_keyboard;

/** Cast a pointer to USB HID Host Function Driver */
#define HIDHF_KEYBOARD_PTR(p) ((struct hidhf_keyboard *)(p))

/** \brief Keyboard generic callback */
typedef void (*hidhf_keyboard_cb_t)(struct hidhf_keyboard *func, uint8_t state);

/** USB HID Host Keyboard Driver support */
struct hidhf_keyboard {
	/** General data for USB Host function driver. */
	struct usbhf_driver func;
	/** Interface for the function */
	int8_t iface;
	/** Function is installed */
	volatile uint8_t is_enabled : 1;
	/** It's function for a boot device */
	uint8_t is_bootdevice : 1;
	/** Reserved bits */
	uint8_t reserved_bits : 6;
	/** Report buffer size */
	uint8_t report_size;
	/** Last button status */
	uint8_t btn_state;
	/** Pipes - IN */
	struct usb_h_pipe *pipe[1];
	/** Callback invoked when button state changes */
	hidhf_keyboard_cb_t btn_cb;
	/** Input Report buffer */
	uint8_t report[CONF_HIDHF_KEYBOARD_RPT_SIZE];
};

void hidf_keyboard_send_leds(hid_kbd_output_report_t leds);

#endif /* HIDHF_KEYBOARD_H_ */