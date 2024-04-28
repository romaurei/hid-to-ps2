#include <atmel_start.h>
#include <hpl_dma.h>
#include <string.h>
#include <stdio.h>
#include "serial_debug.h"

// Buffer length to transfer/receive
#define BUFFER_LEN		1
#define TARGET_OUT_LEN	64
#define OUT_STR_LEN		TARGET_OUT_LEN*3 + 3 //Two characters, per byte, plus space, plus line return, plus termination
#define OUT_BUFF_LEN	OUT_STR_LEN

// DMA channel numbers for receive and transmit
#define USART_RX_DMA_CH 0
#define USART_TX_DMA_CH 1

static char string[BUFFER_LEN];
static uint16_t out_buffer[OUT_BUFF_LEN];
static char str_out_buffer[OUT_STR_LEN];


// callbacks
static void dma_transfer_done_rx(struct _dma_resource *const resource)
{		
	serial_debug_send_string("Ok\r\n");
	
//	_dma_enable_transaction(USART_TX_DMA_CH, false);
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
void Configure_Channel_tx(const uint8_t * buffer, const uint8_t len)
{
	uint16_t i;
	uint8_t outlen;
	if (len < OUT_BUFF_LEN) {
		outlen = len;
	} else {
		outlen = len;
	}
	
	for (i=0; i < outlen; i++) {
		out_buffer[i] = (uint16_t)buffer[i];
	}
	
	_dma_set_source_address(USART_TX_DMA_CH, (uint32_t *)out_buffer);
	_dma_set_destination_address(USART_TX_DMA_CH, (uint32_t) & (((Sercom *)(USART_0.device.hw))->USART.DATA.reg));

	_dma_srcinc_enable(USART_TX_DMA_CH, true);
	_dma_dstinc_enable(USART_TX_DMA_CH, false);
	_dma_set_data_amount(USART_TX_DMA_CH, (uint32_t)(outlen));

	// callback
	Register_dma_tx_callback();

	// Enable DMA transfer complete interrupt
	_dma_set_irq_state(USART_TX_DMA_CH, DMA_TRANSFER_COMPLETE_CB, true);
}

void serial_debug_init(void)
{
	
	char buffer[] = "Ready.\r\n";
	
	// enable USART module
	usart_sync_enable(&USART_0);

	// Configure DMA channels
	Configure_Channel_rx();

	_dma_enable_transaction(USART_RX_DMA_CH, false);

	serial_debug_send_string(buffer);
}

void serial_debug_send_string(const char * buffer)
{
	uint16_t len = strlen(buffer);
	
	if (len > (OUT_STR_LEN - 1)) len = OUT_STR_LEN - 1;
	
	Configure_Channel_tx((uint8_t *)buffer, len);
	_dma_enable_transaction(USART_TX_DMA_CH, false);
}

void serial_debug_send_buffer(const uint8_t * buffer, const uint16_t len)
{
	uint16_t out_len;
	uint16_t i;
	char byte_to_str[4];
	char * out_ptr = str_out_buffer;
	
	if (len < TARGET_OUT_LEN) {
		out_len = len;
	} else {
		out_len = TARGET_OUT_LEN;
	}
	
	for (i = 0; i < out_len; i++) {
		sprintf(byte_to_str, "%02X ", buffer[i]);
		memcpy(out_ptr, byte_to_str, 3);
		out_ptr += 3;			
	}
	
	sprintf(out_ptr, "\r\n");
	
	serial_debug_send_string(str_out_buffer);	
}