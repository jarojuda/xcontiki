/*
 * Copyright (c) 2015, SICS Swedish ICT.
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
 *         Header file for the ringbufindex library
 * \author
 *         Simon Duquennoy <simonduq@sics.se>
 */

#ifndef XCONTIKI_OS_LIB_RINGBUFINDEX_H
#define XCONTIKI_OS_LIB_RINGBUFINDEX_H

struct __PACKED xcontiki_os_lib_RingbufIndex__ringbufindex {
    uint8_t mask;
    /* These must be 8-bit quantities to avoid race conditions. */
    uint8_t put_ptr, get_ptr;
};

typedef struct xcontiki_os_lib_RingbufIndex__ringbufindex xcontiki_os_lib_RingbufIndex__ringbufindex_t;

/**
 * \brief Initialize a ring buffer. The size must be a power of two
 * \param r Pointer to ringbufindex
 * \param size Size of ring buffer
 */
void xcontiki_os_lib_RingbufIndex__init(xcontiki_os_lib_RingbufIndex__ringbufindex_t  *r, uint8_t size);

/**
 * \brief Put one element to the ring buffer
 * \param r Pointer to ringbufindex
 * \retval 0 Failure; the ring buffer is full
 * \retval 1 Success; an element is added
 */
bool xcontiki_os_lib_RingbufIndex__put(xcontiki_os_lib_RingbufIndex__ringbufindex_t  *r);

/**
 * \brief Check if there is space to put an element.
 * \param r Pointer to ringbufindex
 * \retval >= 0 The index where the next element is to be added.
 * \retval -1 Failure; the ring buffer is full
 */
int xcontiki_os_lib_RingbufIndex__peek_put(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

/**
 * \brief Remove the first element and return its index
 * \param r Pointer to ringbufindex
 * \retval >= 0 The index of the first element
 * \retval -1 No element in the ring buffer
 */
int xcontiki_os_lib_RingbufIndex__get(xcontiki_os_lib_RingbufIndex__ringbufindex_t  *r);

/**
 * \brief Return the index of the first element which will be removed if calling
 *        xcontiki_os_lib_RingbufIndex__get.
 * \param r Pointer to ringbufindex
 * \retval >= 0 The index of the first element
 * \retval -1 No element in the ring buffer
 */
int xcontiki_os_lib_RingbufIndex__peek_get(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

/**
 * \brief Return the ring buffer size
 * \param r Pointer to ringbufindex
 * \return The size of the ring buffer
 */
uint8_t xcontiki_os_lib_RingbufIndex__size(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

/**
 * \brief Return the number of elements currently in the ring buffer.
 * \param r Pointer to ringbufindex
 * \return The number of elements in the ring buffer
 */
uint8_t xcontiki_os_lib_RingbufIndex__elements(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

/**
 * \brief Is the ring buffer full?
 * \retval 0 Not full
 * \retval 1 Full
 */
bool xcontiki_os_lib_RingbufIndex__full(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

/**
 * \brief Is the ring buffer empty?
 * \retval 0 Not empty
 * \retval 1 Empty
 */
bool xcontiki_os_lib_RingbufIndex__empty(const xcontiki_os_lib_RingbufIndex__ringbufindex_t *r);

#define XCONTIKI_OS_LIB_RINGBUFFERINDEX_NEW_STATIC(rbi)\
\
static xcontiki_os_lib_RingbufIndex__ringbufindex_t _##rbi;\
\
static void rbi##init(uint8_t size_power_of_two) {\
xcontiki_os_lib_RingbufIndex__init(&_##rbi, size_power_of_two);\
}\
\
static bool rbi##put(void) {\
    return xcontiki_os_lib_RingbufIndex__put(&_##rbi);\
}\
\
static int rbi##peek_put(void) {\
    return xcontiki_os_lib_RingbufIndex__peek_put(&_##rbi);\
}\
\
static int rbi##get(void) {\
    return xcontiki_os_lib_RingbufIndex__get(&_##rbi);\
}\
\
static int rbi##peek_get(void) {\
    return xcontiki_os_lib_RingbufIndex__peek_get(&_##rbi);\
}\
\
static uint8_t rbi##size(void) {\
    return xcontiki_os_lib_RingbufIndex__size(&_##rbi);\
}\
\
static uint8_t rbi##elements(void) {\
    return xcontiki_os_lib_RingbufIndex__elements(&_##rbi);\
}\
\
static bool rbi##full(void) {\
    return xcontiki_os_lib_RingbufIndex__full(&_##rbi);\
}\
\
static bool rbi##empty(void) {\
    return xcontiki_os_lib_RingbufIndex__empty(&_##rbi);\
}\
\
static const struct {\
    void (* init)(uint8_t);\
    bool (* put)(void);\
    int (* peek_put)(void);\
    int (* get)(void);\
    int (* peek_get)(void);\
    uint8_t(* size)(void);\
    uint8_t(* elements)(void);\
    bool (* full)(void);\
    bool (* empty)(void);\
\
} rbi = {rbi##init, rbi##put, rbi##peek_put, rbi##get, rbi##peek_get, rbi##size, rbi##elements, rbi##full, rbi##empty};

#endif /* XCONTIKI_OS_LIB_RINGBUFINDEX_H */
