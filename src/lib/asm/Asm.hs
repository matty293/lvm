{-*-----------------------------------------------------------------------
  The Core Assembler.

  Copyright 2001, Daan Leijen. All rights reserved. This file
  is distributed under the terms of the GHC license. For more
  information, see the file "license.txt", which is included in
  the distribution.
-----------------------------------------------------------------------*-}

-- $Id$

module Asm where

import Byte   ( Bytes )
import Id     ( Id )
import Module

{---------------------------------------------------------------
  Asm modules
---------------------------------------------------------------}
type AsmModule  = Module Top
type AsmValue   = DValue Top

{---------------------------------------------------------------
  low level "assembly" language
---------------------------------------------------------------}
data Top    = Top ![Id] Expr      -- arguments expression

type Atom   = Expr
data Expr   = Eval   !Id Expr Expr
            | Match  !Id ![Alt]
            | Prim   !Id ![Atom]
            -- atomic
            | LetRec ![(Id,Atom)] Expr
            | Let    !Id Atom Expr
            | Ap     !Id ![Atom]
            | Con    !Id ![Atom]
            | Lit    !Lit
            | Note   !Note !Expr

data Note   = Occur  !Occur
data Occur  = Never | Once | Many

data Lit    = LitInt   !Int
            | LitFloat !Double
            | LitBytes !Bytes

data Alt    = Alt !Pat Expr

data Pat    = PatVar !Id
            | PatCon !Id ![Id]
            | PatLit !Lit
