/*
 * hidhf_controller.c
 *
 * Created: 9/06/2024 4:34:52 PM
 *  Author: Rodri
 */ 
#include "hidhf_controller.h"

int32_t hidhf_controller_init(struct usbhc_driver *core, struct hidhf_keyboard *kb_func, struct hidhf_mouse *m_func)
{
	int32_t res;
	res = hidhf_mouse_init(core, m_func);
	
	if (res ==ERR_NONE) {
		res = hidhf_keyboard_init(core, kb_func);	
	}
	
	return res;
}
