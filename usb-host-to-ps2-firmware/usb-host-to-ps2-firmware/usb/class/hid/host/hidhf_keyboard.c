/*
 * hidf_keyboard.c
 *
 * Created: 9/06/2024 4:31:48 PM
 *  Author: RoMaNo
 */ 

#include "hidhf_keyboard.h"
#include "serial_debug.h"

/** \brief Callback invoked when interrupt IN transfer is done */
static void hidhf_keyboard_in_end(struct usb_h_pipe *pipe)
{
	struct hidhf_keyboard *keyboard = (struct hidhf_keyboard *)pipe->owner;
	uint8_t *           rpt   = pipe->x.bii.data;
	
	serial_debug_send_buffer(rpt, 8);
	
	//if (!keyboard->is_enabled) {
		//return;
	//}
	
		
	//if (pipe->x.bii.status == USB_H_OK && pipe->x.bii.count >= 4) {
		///* Decode mouse report */
//
	//}
	/* Restart interrupt IN anyway */
	usb_h_bulk_int_iso_xfer(pipe, keyboard->report, keyboard->report_size, false);
}

static void hidhf_keyboard_uninstall(struct hidhf_keyboard *func)
{
	func->is_enabled = false;
	if (func->pipe[HIDHF_IN_PIPE_IDX]) {
		usb_h_pipe_abort(func->pipe[HIDHF_IN_PIPE_IDX]);
		usb_h_pipe_free(func->pipe[HIDHF_IN_PIPE_IDX]);
	}
}

/** \brief Try to install the keyboard driver
 *  \retval ERR_NONE installed
 *  \retval ERR_NOT_FOUND not installed
 *  \retval ERR_NO_CHANGE already taken
 */
static inline int32_t hidhf_keyboard_install(struct hidhf_keyboard *func, struct usbh_descriptors *desc)
{
	struct usbhd_driver *  dev  = usbhf_get_dev(func);
	struct usbhc_driver *  core = usbhf_get_core(func);
	struct usb_h_desc *    hcd  = core->hcd;
	struct usb_iface_desc *piface;
	struct usb_ep_desc *   pep = NULL;
	struct usb_h_pipe *    pipe;
	uint8_t *              pd;

	if (func->is_enabled) {
		/* Driver already in use */
		return ERR_NO_CHANGE;
	}
	/* find very first interface */
	pd = usb_find_desc(desc->sod, desc->eod, USB_DT_INTERFACE);
	if (!pd) {
		/* No interface found */
		return ERR_NOT_FOUND;
	}
	/* Try to install HID interface */
	piface = (struct usb_iface_desc *)pd;
	if (piface->bInterfaceClass != HID_CLASS || piface->bInterfaceProtocol != HID_PROTOCOL_KEYBOARD
	    || piface->bNumEndpoints == 0) {
		return ERR_NOT_FOUND;
	}
	/* Find endpoints */
	while (1) {
		pd = usb_desc_next(pd);
		pd = usb_find_ep_desc(pd, desc->eod);
		if (NULL == pd) {
			break;
		}
		pep = (struct usb_ep_desc *)pd;
		if (pep->bEndpointAddress & USB_EP_DIR_OUT) {
			/* Skip OUT for STD mouse only */
			continue;
		}
		/* Break to allocate EP IN */
		break;
	}
	pipe = usb_h_pipe_allocate(hcd,
	                           dev->dev_addr,
	                           pep->bEndpointAddress,
	                           pep->wMaxPacketSize,
	                           pep->bmAttributes,
	                           pep->bInterval,
	                           dev->speed,
	                           true);
	if (pipe == NULL) {
		hidhf_keyboard_uninstall(func);
		return ERR_NO_RESOURCE;
	}
	pipe->owner                   = (void *)func;
	func->pipe[HIDHF_IN_PIPE_IDX] = pipe;

	/* Update descriptors pointers */
	desc->sod = usb_find_iface_after((uint8_t *)piface, desc->eod, piface->bInterfaceNumber);

	/* Update status */
	func->iface         = piface->bInterfaceNumber;
	func->is_enabled    = true;
	func->is_bootdevice = (piface->bInterfaceSubClass == HID_SUB_CLASS_BOOT);
	func->report_size   = func->pipe[HIDHF_IN_PIPE_IDX]->max_pkt_size;
	if (func->report_size > CONF_HIDHF_KEYBOARD_RPT_SIZE) {
		func->report_size = CONF_HIDHF_KEYBOARD_RPT_SIZE;
	}

	func->btn_state = 0;
	usb_h_pipe_register_callback(pipe, hidhf_keyboard_in_end);
	usb_h_bulk_int_iso_xfer(pipe, func->report, func->report_size, false);
	return ERR_NONE;
}

/** \brief Callback invoked on install/uninstall the function driver
 *  \param func  Pointer to the function driver instance
 *  \param ctrl  Control operation code
 *  \param param Parameter for install/uninstall
 */
static int32_t hidhf_keyboard_ctrl(struct usbhf_driver *func, enum usbhf_control ctrl, void *param)
{
	switch (ctrl) {
	case USBHF_INSTALL:
		return hidhf_keyboard_install(HIDHF_KEYBOARD_PTR(func), (struct usbh_descriptors *)param);
	case USBHF_UNINSTALL:
		hidhf_keyboard_uninstall(HIDHF_KEYBOARD_PTR(func));
		return ERR_NONE;
	default:
		return ERR_INVALID_ARG;
	}
}

int32_t hidhf_keyboard_init(struct usbhc_driver *core, struct hidhf_keyboard *func)
{
	int32_t rc = 0;

	if (func->is_enabled) {
		return ERR_DENIED;
	}
	rc = usbhc_register_funcd(core, USBHF_PTR(func));
	if (rc) {
		return rc;
	}
	func->iface         = -1;
	func->is_enabled    = false;
	func->is_bootdevice = false;
	func->pipe[0]       = NULL;

	func->func.ctrl = hidhf_keyboard_ctrl;

	func->btn_cb  = NULL;

	return ERR_NONE;
}