#include <atmel_start.h>
#include <hal_delay.h>

static void _mouse_move_cb(struct hidhf_mouse *mouse, int8_t x, int8_t y, int8_t scroll)
{
	/* LED toggles if any movements detected */
	gpio_toggle_pin_level(LED0);
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* LED is turned on by polling buttons state */
	/* LED is toggled by move callback */
	hidhf_mouse_register_callback(&USB_HOST_HID_MOUSE_0_inst, HIDHF_MOUSE_MOVE_CB, (FUNC_PTR)_mouse_move_cb);
	/* Start host */
	usbhc_start(&USB_HOST_CORE_INSTANCE_inst);

	while (1) {
		/* Wait until mouse connected */
		while (!hidhf_mouse_is_enabled(&USB_HOST_HID_MOUSE_0_inst))
			;
		/* Loop until mouse removed */
		while (hidhf_mouse_is_enabled(&USB_HOST_HID_MOUSE_0_inst)) {
			/* Blink LED if no button is down */
			if (hidhf_mouse_lbtn(&USB_HOST_HID_MOUSE_0_inst) || hidhf_mouse_rbtn(&USB_HOST_HID_MOUSE_0_inst)
			    || hidhf_mouse_mbtn(&USB_HOST_HID_MOUSE_0_inst)) {
				gpio_set_pin_level(LED0, false);
				continue;
			}
			gpio_toggle_pin_level(LED0);
			delay_ms(500);
		}
		/* Turn OFF LED if mouse is removed */
		gpio_set_pin_level(LED0, true);
	}
}
