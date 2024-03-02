#include <atmel_start.h>
#include <hal_delay.h>
#include <hpl_dma.h>

// Buffer length to transfer/receive
#define BUFFER_LEN 8

// DMA channel numbers for receive and transmit
#define USART_RX_DMA_CH 0
#define USART_TX_DMA_CH 1

static uint16_t string[BUFFER_LEN];

// callbacks
static void dma_transfer_done_rx(struct _dma_resource *const resource)
{
	_dma_enable_transaction(USART_TX_DMA_CH, false);
}

static void dma_error_rx(struct _dma_resource *const resource)
{
	// write error handling code here
}

static void dma_transfer_done_tx(struct _dma_resource *const resource)
{
	_dma_enable_transaction(USART_RX_DMA_CH, false);
}

static void dma_error_tx(struct _dma_resource *const resource)
{
	// write error handling code here
}

// register callbacks
void Register_dma_rx_callback(void)
{
	struct _dma_resource *resource_rx;
	_dma_get_channel_resource(&resource_rx, USART_RX_DMA_CH);
	resource_rx->dma_cb.transfer_done = dma_transfer_done_rx;
	resource_rx->dma_cb.error         = dma_error_rx;
}

void Register_dma_tx_callback(void)
{
	struct _dma_resource *resource_tx;
	_dma_get_channel_resource(&resource_tx, USART_TX_DMA_CH);
	resource_tx->dma_cb.transfer_done = dma_transfer_done_tx;
	resource_tx->dma_cb.error         = dma_error_tx;
}

// USART RX channel configuration
void Configure_Channel_rx()
{
	_dma_set_source_address(USART_RX_DMA_CH, (uint32_t) & (((Sercom *)(USART_0.device.hw))->USART.DATA.reg));
	_dma_set_destination_address(USART_RX_DMA_CH, (uint32_t *)string);
	_dma_set_data_amount(USART_RX_DMA_CH, (uint32_t)BUFFER_LEN);

	// callback
	Register_dma_rx_callback();

	// Enable DMA transfer complete interrupt
	_dma_set_irq_state(USART_RX_DMA_CH, DMA_TRANSFER_COMPLETE_CB, true);
}

// USART TX channel configuration
void Configure_Channel_tx()
{
	_dma_set_source_address(USART_TX_DMA_CH, (uint32_t *)string);
	_dma_set_destination_address(USART_TX_DMA_CH, (uint32_t) & (((Sercom *)(USART_0.device.hw))->USART.DATA.reg));
	_dma_set_data_amount(USART_TX_DMA_CH, (uint32_t)BUFFER_LEN);

	// callback
	Register_dma_tx_callback();

	// Enable DMA transfer complete interrupt
	_dma_set_irq_state(USART_TX_DMA_CH, DMA_TRANSFER_COMPLETE_CB, true);
}

static void _mouse_move_cb(struct hidhf_mouse *mouse, int8_t x, int8_t y, int8_t scroll)
{
	/* LED toggles if any movements detected */
	gpio_toggle_pin_level(LED0);
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	// enable USART module
	usart_sync_enable(&USART_0);

	// Configure DMA channels
	Configure_Channel_rx();
	Configure_Channel_tx();
	
	_dma_enable_transaction(USART_RX_DMA_CH, false);

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
