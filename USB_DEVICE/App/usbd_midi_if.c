/**
  ******************************************************************************
  * @file    usbd_midi_if.c
  * @author  Sam Kent
  * @brief   USB MIDI Access Layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi_if.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_MIDI
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_MIDI_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_FunctionPrototypes
  * @{
  */

static int8_t MIDI_Init     (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static int8_t MIDI_DeInit   (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static int8_t MIDI_Receive  (uint8_t* pbuf, uint32_t length);
static int8_t MIDI_Send     (uint8_t* pbuf, uint32_t length);

USBD_MIDI_ItfTypeDef USBD_MIDI_fops =
{
  MIDI_Init,
  MIDI_DeInit,
  MIDI_Receive,
  MIDI_Send
};

/* Callbacks weak definitions.
 * Implement your own definitions in your code if needed. */
__weak void MIDI_ReceivedNoteCallback(MIDI_NoteUnion_typedef receivedNote)
{
	UNUSED(receivedNote);
}

__weak void MIDI_ReceivedProgramChangeCallback(MIDI_ProgChangeUnion_typedef receivedProgramChange)
{
	UNUSED(receivedProgramChange);
}


__weak void MIDI_ReceivedControlChangeCallback(MIDI_CtrChangeUnion_typedef receivedControlChange)
{
	UNUSED(receivedControlChange);
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  MIDI_Init
  *         Initializes the MIDI media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /*
     Add your initialization code here
  */
  return (0);
}

/**
  * @brief  MIDI_DeInit
  *         DeInitializes the MIDI media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /*
     Add your deinitialization code here
  */
  return (0);
}

/**
  * @brief  MIDI_Send
  *
  * @param  buffer: bufferfer of data to be received
  * @param  length: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t MIDI_Send (uint8_t* buffer, uint32_t length)
{
  uint8_t ret = USBD_OK;

  USBD_MIDI_SetTxBuffer(&hUsbDeviceFS, buffer, length);

  ret = USBD_MIDI_TransmitPacket(&hUsbDeviceFS);

  return (ret);
}

/**
  * @brief  MIDI_Receive
  *
  * @param  buffer: buffer of data to be received
  * @param  length: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t MIDI_Receive (uint8_t* buffer, uint32_t length)
{
	/* According to Universal Serial Bus Device Class Definition for MIDI Devices */

	uint8_t CIN = buffer[0] & 0x0F; // Code index number (check MIDI specification)
	MIDI_NoteUnion_typedef recv_note;

	switch (CIN) {
		case 0x08: // Note OFF
			memcpy(recv_note.array, buffer,4); 	  // Copy received array to the note union array.
			MIDI_ReceivedNoteCallback(recv_note); // Call to the user Callback.
			break;
		case 0x09: // Note ON
			memcpy(recv_note.array, buffer,4);    // Copy received array to the note union array.
			MIDI_ReceivedNoteCallback(recv_note); // Call to the user Callback.
			break;
		case 0x0B: // Control Change
			MIDI_CtrChangeUnion_typedef recv_cc;
			memcpy(recv_cc.array, buffer, 4);
			MIDI_ReceivedControlChangeCallback(recv_cc);
			break;
		case 0x0C: // Program Change
			MIDI_ProgChangeUnion_typedef recv_pc;
			memcpy(recv_pc.array, buffer, 4);
			MIDI_ReceivedProgramChangeCallback(recv_pc);
			break;
		default:
			break;
	}

	return length;
}

/*
 * Send a note message through MIDI USB.
 * The note parameter should be configured in the struct pointed.
 */
uint8_t MIDI_SendNote(MIDI_NoteUnion_typedef *note){
	uint8_t ret = 0;
	//uint8_t send_buf[4];
	note->fields.CIN = note->fields.type;

	//send_buf[0] = (note->cable << 4) | note->type;
	//send_buf[1] = (note->type << 4) | note->channel;
	//send_buf[2] = note->note;
	//send_buf[3] = note->velocity;

	//ret = MIDI_Send(send_buf, 4);
	ret = MIDI_Send(note->array, 4); // It is simple as that using unions.

	return ret;
}

/*
 * Send a Program Change message through MIDI USB.
 * The note parameter should be configured in the struct pointed.
 */
uint8_t MIDI_SendProgramChange(MIDI_ProgChangeUnion_typedef *pc){
	uint8_t ret = 0;
	//uint8_t send_buf[4];
	pc->fields.CIN = 0x0C;
	pc->fields.type = 0x0C;
	pc->fields.unused = 0;

	//send_buf[0] = (pc->cable << 4) | 0x0C;
	//send_buf[1] = (0x0C << 4) | pc->channel; // 0x0C Program Change message type.
	//send_buf[2] = pc->program;
	//send_buf[3] = 0;

	//ret = MIDI_Send(send_buf, 4);
	ret = MIDI_Send(pc->array, 4);

	return ret;
}

/*
 * Send a Controle Change message through MIDI USB.
 * The note parameter should be configured in the struct pointed.
 */
uint8_t MIDI_SendControlChange(MIDI_CtrChangeUnion_typedef *cc){
	uint8_t ret = 0;
	cc->fields.CIN = 0x0B;
	cc->fields.type = 0x0B;
	//uint8_t send_buf[4];

	//send_buf[0] = (cc->cable << 4) | 0x0B;
	//send_buf[1] = (0x0B << 4) | cc->channel; // 0x0B Program Change message type.
	//send_buf[2] = cc->controller;
	//send_buf[3] = cc->value;

	//ret = MIDI_Send(send_buf, 4);
	ret = MIDI_Send(cc->array, 4);

	return ret;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

