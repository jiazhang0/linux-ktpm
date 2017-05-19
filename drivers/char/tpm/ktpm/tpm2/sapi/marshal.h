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

#ifndef MARSHAL_H
#define MARSHAL_H

#include <stdlib.h>
#include <sapi/tpm20.h>

#define TSS2_TYPES_RC_LAYER TSS2_ERROR_LEVEL(14)
#define TSS2_TYPES_RC_BAD_REFERENCE \
    ((TSS2_RC)(TSS2_TYPES_RC_LAYER | TSS2_BASE_RC_BAD_REFERENCE))
#define TSS2_TYPES_RC_INSUFFICIENT_BUFFER \
    ((TSS2_RC)(TSS2_TYPES_RC_LAYER | TSS2_BASE_RC_INSUFFICIENT_BUFFER))

#ifdef __cplusplus
extern "C" {
#endif

TSS2_RC
u8_Marshal (
    u8           src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
u8_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    u8           *dest
    );

TSS2_RC
s8_Marshal (
    s8            src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
s8_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    s8           *dest
    );

TSS2_RC
s16_Marshal (
    s16           src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
s16_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    s16          *dest
    );

TSS2_RC
s32_Marshal (
    s32           src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
s32_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    s32          *dest
    );

TSS2_RC
s64_Marshal (
    s64           src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
s64_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    s64          *dest
    );

TSS2_RC
u8_Marshal (
    u8           src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
u8_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    u8          *dest
    );

TSS2_RC
u16_Marshal (
    u16          src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
u16_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    u16         *dest
    );

TSS2_RC
u32_Marshal (
    u32          src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
u32_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    u32         *dest
    );

TSS2_RC
u64_Marshal (
    u64          src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
u64_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    u64         *dest
    );

TSS2_RC
TPM_CC_Marshal (
    TPM_CC          src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
TPM_CC_Unmarshal (
    u8 const   buffer [],
    size_t          buffer_size,
    size_t         *offset,
    TPM_CC         *dest
    );

TSS2_RC
TPM_ST_Marshal (
    TPM_ST          src,
    u8         buffer [],
    size_t          buffer_size,
    size_t         *offset
    );

TSS2_RC
TPM_ST_Unmarshal (
    u8 const   buffer[],
    size_t          buffer_size,
    size_t         *offset,
    TPM_ST         *dest
    );

#ifdef __cplusplus
}
#endif

#endif /* MARSHAL_H */
