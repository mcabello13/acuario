/*-----------------------------------------------------------------------------
 * Copyright (c) 2013 - 2019 Arm Limited (or its affiliates). All
 * rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1.Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   2.Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   3.Neither the name of Arm nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * Name:    Board_LED.h
 * Purpose: LED interface header file
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

#ifndef __BOARD_LED_H
#define __BOARD_LED_H

#include <stdint.h>

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Control all LEDs with the bit vector \em val
  \param[in]   val  each bit represents the status of one LED.
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of available LEDs on evaluation hardware
  \return      Number of available LEDs
*/

extern int32_t  LED_Initialize_stm   (void); //SE HA A�ADIDO A CADA FUNCION "_stm" PARA EVITAR EL ERROR DE DUPLICIDAD CON EL BOARD.
extern int32_t  LED_Uninitialize_stm (void);
extern int32_t  LED_On_stm           (uint32_t num);
extern int32_t  LED_Off_stm          (uint32_t num);
extern int32_t  LED_SetOut_stm       (uint32_t val);
extern uint32_t LED_GetCount_stm     (void);

#endif /* __BOARD_LED_H */
