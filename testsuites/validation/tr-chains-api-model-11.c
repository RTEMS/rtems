/******************************************************************************
 * FV2-201
 *
 * Copyright (C) 2019-2021 Trinity College Dublin (www.tcd.ie)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of the copyright holders nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


#include <rtems.h>
#include <rtems/test.h>
#include <rtems/chain.h>
#include "tr-chains-api-model.h"

//  ===============================================

// @@@ 0 NAME Chain_AutoGen
// @@@ 0 DEF MAX_SIZE 8
#define MAX_SIZE 8
// @@@ 0 DCLARRAY Node memory MAX_SIZE
static item memory[MAX_SIZE];
// @@@ 0 DECL unsigned nptr NULL
static item * nptr = NULL;
// @@@ 0 DECL Control chain
static rtems_chain_control chain;

//  ===== TEST CODE SEGMENT 0 =====

static void TestSegment0( Context* ctx ) {
  const char rtems_test_name[] = "Model_Chain_API";
  
  T_log(T_NORMAL,"@@@ 0 INIT");
  rtems_chain_initialize_empty( &chain );
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 0" );
  
  T_log(T_NORMAL,"@@@ 0 PTR nptr 0");
  T_eq_ptr( nptr, NULL );
  T_log(T_NORMAL,"@@@ 0 CALL append 22 3");
  memory[3].val = 22;
  rtems_chain_append_unprotected( &chain, (rtems_chain_node*)&memory[3] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 SCALAR _ 22");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 22 0" );
  
  T_log(T_NORMAL,"@@@ 0 CALL append 23 4");
  memory[4].val = 23;
  rtems_chain_append_unprotected( &chain, (rtems_chain_node*)&memory[4] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 SCALAR _ 22");
  T_log(T_NORMAL,"@@@ 0 SCALAR _ 23");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 22 23 0" );
  
  T_log(T_NORMAL,"@@@ 0 CALL getNonNull 3");
  nptr = get_item( &chain );
  T_eq_ptr( nptr, &memory[3] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 SCALAR _ 23");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 23 0" );
  
  T_log(T_NORMAL,"@@@ 0 PTR nptr 3");
  T_eq_ptr( nptr, &memory[3] );
  T_log(T_NORMAL,"@@@ 0 STRUCT nptr");
  T_log(T_NORMAL,"@@@ 0 SCALAR itm 22");
  T_eq_int( nptr->val, 22 );
  T_log(T_NORMAL,"@@@ 0 END nptr");
  T_log(T_NORMAL,"@@@ 0 CALL getNonNull 4");
  nptr = get_item( &chain );
  T_eq_ptr( nptr, &memory[4] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 0" );
  
  T_log(T_NORMAL,"@@@ 0 PTR nptr 4");
  T_eq_ptr( nptr, &memory[4] );
  T_log(T_NORMAL,"@@@ 0 STRUCT nptr");
  T_log(T_NORMAL,"@@@ 0 SCALAR itm 23");
  T_eq_int( nptr->val, 23 );
  T_log(T_NORMAL,"@@@ 0 END nptr");
  T_log(T_NORMAL,"@@@ 0 CALL append 21 6");
  memory[6].val = 21;
  rtems_chain_append_unprotected( &chain, (rtems_chain_node*)&memory[6] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 SCALAR _ 21");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 21 0" );
  
  T_log(T_NORMAL,"@@@ 0 CALL getNonNull 6");
  nptr = get_item( &chain );
  T_eq_ptr( nptr, &memory[6] );
  
  T_log(T_NORMAL,"@@@ 0 SEQ chain");
  T_log(T_NORMAL,"@@@ 0 END chain");
  show_chain( &chain, ctx->buffer );
  T_eq_str( ctx->buffer, " 0" );
  
  T_log(T_NORMAL,"@@@ 0 PTR nptr 6");
  T_eq_ptr( nptr, &memory[6] );
  T_log(T_NORMAL,"@@@ 0 STRUCT nptr");
  T_log(T_NORMAL,"@@@ 0 SCALAR itm 21");
  T_eq_int( nptr->val, 21 );
  T_log(T_NORMAL,"@@@ 0 END nptr");
}

//  ===============================================


/* post-amble empty for now */
void RtemsModelChainsAPI_Run11(
)
{
  Context ctx;

  memset( &ctx, 0, sizeof( ctx ) );

  T_set_verbosity( T_NORMAL );

  TestSegment0( &ctx );
}

T_TEST_CASE( RtemsModelChainAPI11 )
{
  RtemsModelChainsAPI_Run11( );
}
