/**
  ******************************************************************************
  * @file    usbd_midi_if.h
  * @author  Sam Kent
  * @brief   Header for usbd_midi_if.c file. Updated by Moreto.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MIDI_IF_H
#define __USBD_MIDI_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
	MIDI_NOTE_ON = 0x09,
	MIDI_NOTE_OFF = 0x08
} MIDI_NoteOnOff_typedef;

/* This struct takes only 4 bytes: */
typedef struct {
	uint8_t CIN : 4;	// Code Index Number (4 LSB bits of byte 0).
	uint8_t cable : 4;	// Cable Number (4 MSB bits of byte 0)
	uint8_t channel : 4;	// Channel number (4 LSB bits of byte 1).
	MIDI_NoteOnOff_typedef type : 4; // Message type (4 MSB bits of byte 1). This is the same as CIN
	uint8_t note : 7;			// byte 2
	uint8_t : 0; // Force a new byte as program value is only 7 bits.
	uint8_t velocity : 7;		// byte 3
} MIDI_NoteStruct_typedef;

/* With the union type we can have the struct and the byte array in the same
 * variable (no need to declare temporary arrays when sending or receiving data). */
typedef union {
	MIDI_NoteStruct_typedef fields;
	uint8_t array[4];
} MIDI_NoteUnion_typedef;

/* This struct takes only 4 bytes: */
typedef struct {
	uint8_t CIN : 4;	// Code Index Number (4 LSB bits of byte 0).
	uint8_t cable : 4;	// Cable Number (4 MSB bits of byte 0)
	uint8_t channel : 4;	// Channel number (4 LSB bits of byte 1).
	uint8_t type : 4;	// Message type number, same as CIN. (4 MSB bits of byte 1).
	uint8_t program : 7;
	uint8_t : 0; // Force a new byte as program value is only 7 bits.
	uint8_t unused : 8; // To complete the 4 bytes message (zero padding).
} MIDI_ProgChangeStruct_typedef;

typedef union {
	MIDI_ProgChangeStruct_typedef fields;
	uint8_t array[4];
} MIDI_ProgChangeUnion_typedef;

/* This struct takes only 4 bytes: */
typedef struct {
	uint8_t CIN : 4;	// Code Index Number (4 LSB bits of byte 0).
	uint8_t cable : 4;	// Cable Number (4 MSB bits of byte 0)
	uint8_t channel : 4;	// Channel number (4 LSB bits of byte 1).
	uint8_t type : 4;	// Message type number, same as CIN. (4 MSB bits of byte 1).
	uint8_t controller : 7;
	uint8_t : 0;
	uint8_t value : 7;
} MIDI_CtrChangeStruct_typedef;

typedef union {
	MIDI_CtrChangeStruct_typedef fields;
	uint8_t array[4];
} MIDI_CtrChangeUnion_typedef;

/* Exported constants --------------------------------------------------------*/

extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_MIDI_ItfTypeDef  USBD_MIDI_fops;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint8_t MIDI_SendNote(MIDI_NoteUnion_typedef *note);
uint8_t MIDI_SendProgramChange(MIDI_ProgChangeUnion_typedef *pc);
uint8_t MIDI_SendControlChange(MIDI_CtrChangeUnion_typedef *cc);
// Callbacks:
void MIDI_ReceivedNoteCallback(MIDI_NoteUnion_typedef receivedNote);
void MIDI_ReceivedProgramChangeCallback(MIDI_ProgChangeUnion_typedef receivedProgramChange);
void MIDI_ReceivedControlChangeCallback(MIDI_CtrChangeUnion_typedef receivedControlChange);
#ifdef __cplusplus
}
#endif

#endif /* __USBD_MIDI_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
