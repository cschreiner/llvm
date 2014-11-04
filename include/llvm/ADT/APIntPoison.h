//===-- llvm/ADT/APIntPoison.h - Arbitrary Precision Integer Poison issues --*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a namespace of functions that detect when an
///   APInt has become poisoned. 
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_APINTPOISON_H
#define LLVM_ADT_APINTPOISON_H

#include "llvm/ADT/APInt.h"
#include <cassert>
#include <climits>
#include <cstring>
#include <string>

namespace llvm {

//===----------------------------------------------------------------------===//
//                              APIntPoison Namespace
namespace APIntPoison {

// TODO: add const designations here as appropriate.

// TODO: add overloads here as appropriate, for example, one operand
// may be an integer (signed or not).

void poisonIfNeeded_uadd( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_sadd( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
// looks like we need separate checks for ++ vs a full add.
void poisonIfNeeded_usub( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_ssub( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_umul( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_smul( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_udiv( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_sdiv( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );

void poisonIfNeeded_ushl( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_sshl( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_ulshr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_slshr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_uashr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );
void poisonIfNeeded_sashr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );

void poisonIfNeeded_getelementptr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool noWrap );

} // End of APIntPoison namespace

} // End of llvm namespace

#endif
