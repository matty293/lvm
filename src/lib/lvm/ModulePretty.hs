{-*-----------------------------------------------------------------------
  The Core Assembler.

  Copyright 2001, Daan Leijen. All rights reserved. This file
  is distributed under the terms of the GHC license. For more
  information, see the file "license.txt", which is included in
  the distribution.
-----------------------------------------------------------------------*-}

-- $Id$

module ModulePretty ( modulePretty ) where

import PPrint
import Byte     ( stringFromBytes )
import Id       ( Id, stringFromId )
import IdMap    ( listFromMap )
import Module

----------------------------------------------------------------
--
----------------------------------------------------------------
modulePretty :: (v -> Doc) -> Module v -> Doc
modulePretty ppValue mod
  = ppModule ppValue mod

ppModule ::  (v -> Doc) -> Module v -> Doc
ppModule ppValue (Module moduleName major minor decls)
  =  text "module" <+> ppId moduleName <+> text "where"
 <$> vcat (map (\decl -> ppDecl ppValue decl <> line) decls)
 <$> empty

ppDecl ppValue decl
  = nest 2 (ppDeclHeader decl <> (  
    case decl of
      DeclValue{} -> line <> text "=" <+> ppValue (valueValue decl)
      DeclCon{}   -> line <> text "=" <+> text "[tag=" <> pretty (conTag decl) 
                                      <>  text ", arity=" <> pretty (declArity decl) <> text "]"
      other       -> empty
   ))

ppDeclHeader decl
  = ppDeclKind decl <> ppId (declName decl) <+> text ":" <+> ppAccess (declAccess decl) <+> ppCustoms (declCustoms decl)

ppDeclKind decl
  = case decl of
      DeclValue{}   -> empty
      DeclCon{}     -> text "con "
      DeclExtern{}  -> text "extern "
      DeclCustom{}  -> text "custom "
      DeclImport{}  -> text "import "
      DeclAbstract{}-> text "abstract "
      other         -> text "<unknown declaration>"

ppCustoms customs
  = list (map ppCustom customs)

ppCustom custom
  = case custom of
      CustomInt i     -> pretty i
      CustomName id   -> ppId id
      CustomBytes bs  -> dquotes (string (stringFromBytes bs))
      CustomDecl (Link id kind) -> text "link" <+> ppId id
      other           -> error "ModulePretty.ppCustom: unknown custom kind"

ppId :: Id -> Doc
ppId id
  = text (stringFromId id)

ppAccess acc
  = case acc of
      Defined public 
        -> ppPublic public
      Imported public modid impid impkind major minor
        -> ppPublic public <+> text "import" <+> ppId modid <> char '.' <> ppId impid

ppPublic public
  = if public then text "public" else text "private"

