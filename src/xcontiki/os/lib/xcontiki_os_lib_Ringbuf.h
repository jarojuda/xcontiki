/*
 * Copyright (c) 2008, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Header file for the ring buffer library
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/** \addtogroup data
 * @{ */

/**
 * \defgroup ringbuf Ring buffer library
 * @{
 *
 * The ring buffer library implements ring (circular) buffer where
 * bytes can be read and written independently. A ring buffer is
 * particularly useful in device drivers where data can come in
 * through interrupts.
 *
 */

#ifndef XCONTIKI_OS_LIB_RINGBUF_H
#define XCONTIKI_OS_LIB_RINGBUF_H

/**
 * \brief      Structure that holds the state of a ring buffer.
 *
 *             This structure holds the state of a ring buffer. The
 *             actual buffer needs to be defined separately. This
 *             struct is an opaque structure with no user-visible
 *             elements.
 *
 */
struct __PACKED xcontiki_os_lib_Ringbuf__ringbuf {
    uint8_t *data;
    uint8_t mask;

    /* XXX these must be 8-bit quantities to avoid race conditions. */
    uint8_t put_ptr, get_ptr;
};

typedef  struct xcontiki_os_lib_Ringbuf__ringbuf xcontiki_os_lib_Ringbuf__ringbuf_t;
#define xcontiki_os_lib_Ringbuf__ringbuf_t xcontiki_os_lib_Ringbuf__ringbuf_t

/**
 * \brief      Initialize a ring buffer
 * \param r    A pointer to a struct ringbuf to hold the state of the ring buffer
 * \param a    A pointer to an array to hold the data in the buffer
 * \param size_power_of_two The size of the ring buffer, which must be a power of two
 *
 *             This function initiates a ring buffer. The data in the
 *             buffer is stored in an external array, to which a
 *             pointer must be supplied. The size of the ring buffer
 *             must be a power of two and cannot be larger than 128
 *             bytes.
 *
 */
void xcontiki_os_lib_Ringbuf__init(xcontiki_os_lib_Ringbuf__ringbuf_t   *r, uint8_t  *a,
        uint8_t size_power_of_two);

/**
 * \brief      Insert a byte into the ring buffer
 * \param r    A pointer to a struct ringbuf to hold the state of the ring buffer
 * \param c    The byte to be written to the buffer
 * \return     Non-zero if there data could be written, or zero if the buffer was full.
 *
 *             This function inserts a byte into the ring buffer. It
 *             is safe to call this function from an interrupt
 *             handler.
 *
 */
bool xcontiki_os_lib_Ringbuf__put(xcontiki_os_lib_Ringbuf__ringbuf_t  *r, uint8_t c);


/**
 * \brief      Get a byte from the ring buffer
 * \param r    A pointer to a struct ringbuf to hold the state of the ring buffer
 * \return     The data from the buffer, or -1 if the buffer was empty
 *
 *             This function removes a byte from the ring buffer. It
 *             is safe to call this function from an interrupt
 *             handler.
 *
 */
uint8_t xcontiki_os_lib_Ringbuf__get(xcontiki_os_lib_Ringbuf__ringbuf_t  *r);

/**
 * \brief      Get the size of a ring buffer
 * \param r    A pointer to a struct ringbuf to hold the state of the ring buffer
 * \return     The size of the buffer.
 */
uint8_t xcontiki_os_lib_Ringbuf__size(xcontiki_os_lib_Ringbuf__ringbuf_t  *r);

/**
 * \brief      Get the number of elements currently in the ring buffer
 * \param r    A pointer to a struct ringbuf to hold the state of the ring buffer
 * \return     The number of elements in the buffer.
 */
uint8_t xcontiki_os_lib_Ringbuf__elements(xcontiki_os_lib_Ringbuf__ringbuf_t  *r);

#define XCONTIKI_OS_LIB_RINGBUF__NEW_STATIC(ringbuf, size_power_of_two)\
static xcontiki_os_lib_Ringbuf__ringbuf_t _##ringbuf;\
static uint8_t ringbuf##array[size_power_of_two];\
\
static void ringbuf##init(void) {\
  xcontiki_os_lib_Ringbuf__init(&_##ringbuf, ringbuf##array, size_power_of_two);\
}\
static bool ringbuf##put(uint8_t c){\
  return xcontiki_os_lib_Ringbuf__put(&_##ringbuf, c);\
}\
static uint8_t ringbuf##get(void){\
  return xcontiki_os_lib_Ringbuf__get(&_##ringbuf);\
}\
static uint8_t ringbuf##size(void){\
  return size_power_of_two;\
}\
static uint8_t ringbuf##elements(void){\
  return xcontiki_os_lib_Ringbuf__elements(&_##ringbuf);\
}\
static const struct {\
  void (* init)(void);\
  bool (* put)(uint8_t);\
  uint8_t (* get)(void);\
  uint8_t (* size)(void);\
  uint8_t (* elements)(void);\
} ringbuf = \
{ ringbuf##init, ringbuf##put, ringbuf##get, ringbuf##size, ringbuf##elements };\


#endif /* XCONTIKI_OS_LIB_RINGBUF_H */

/** @}*/
/** @}*/
