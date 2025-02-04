//**********************************************************************;
// Copyright (c) 2015, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//**********************************************************************;

// This file contains extra TPM2B structures
#ifndef _TPMB_H
#define _TPMB_H

#ifndef TSS2_API_VERSION_1_1_1_1
#error Version mismatch among TSS2 header files. \
       Do not include this file, #include <sapi/tpm20.h> instead.
#endif  /* TSS2_API_VERSION_1_1_1_1 */

typedef struct {
    u16  size;
    u8    buffer[1];
} TPM2B;

// This macro helps avoid having to type in the structure in order to create a new TPM2B type that
// is used in a function.

#define TPM2B_TYPE(name, bytes)			    \
    typedef union {				    \
	struct  {					    \
	    u16  size;				    \
	    u8    buffer[(bytes)];			    \
	} t;						    \
	TPM2B   b;					    \
    } TPM2B_##name

#define TPM2B_TYPE1(name, bytes, bufferName)			    \
    typedef union {				    \
	struct  {					    \
	    u16  size;				    \
	    u8    bufferName[(bytes)];			    \
	} t;						    \
	TPM2B   b;					    \
    } TPM2B_##name

#define TPM2B_TYPE2(name, type, bufferName )			    \
    typedef union {				    \
	struct  {					    \
	    u16  size;				    \
	    type bufferName;			    \
	} t;						    \
	TPM2B   b;					    \
    } TPM2B_##name


// Macro to instance and initialize a TPM2B value

#define TPM2B_INIT(TYPE, name)					\
    TPM2B_##TYPE    name = {sizeof(name.t.buffer), {0}}

#define TPM2B_u8_VALUE(bytes) TPM2B_TYPE(bytes##_u8_VALUE, bytes)

#endif
