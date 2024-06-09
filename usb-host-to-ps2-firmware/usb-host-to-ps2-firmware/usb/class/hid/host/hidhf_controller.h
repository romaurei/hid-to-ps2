/*
 * hidhf_controller.h
 *
 * Created: 9/06/2024 4:35:03 PM
 *  Author: Rodri
 */ 


#ifndef HIDHF_CONTROLLER_H_
#define HIDHF_CONTROLLER_H_

#include "hidhf_mouse.h"
#include "hidhf_keyboard.h"

int32_t hidhf_controller_init(struct usbhc_driver *core, struct hidhf_keyboard *kb_func, struct hidhf_mouse *m_func);



#endif /* HIDHF_CONTROLLER_H_ */