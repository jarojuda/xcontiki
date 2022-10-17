/*
 * Copyright (c) 2020, Jaroslaw Juda <mail at JaroslawJuda.site>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Based on the Contiki http://contiki-os.org
 * and the Contiki-NG  http://www.contiki-ng.org/
 * projects. 
 */

/* 
 * File:   xcontiki_os.h
 * 
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#ifndef XCONTIKI_OS_H
#define	XCONTIKI_OS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xcontiki/os/sys/xcontiki_os_sys.h"
#include "xcontiki/os/lib/xcontiki_os_lib.h"
    
 /** \def CC_ACCESS_NOW(x)
 * This macro ensures that the access to a non-volatile variable can
 * not be reordered or optimized by the compiler.
 * See also https://lwn.net/Articles/508991/ - In Linux the macro is
 * called ACCESS_ONCE
 * The type must be passed, because the typeof-operator is a gcc
 * extension
 */

#define XCONTIKI_OS__ACCESS_NOW(type, variable) (*(volatile type *)&(variable))

    
    

#ifdef	__cplusplus
}
#endif

#endif	/* XCONTIKI_OS_H */

