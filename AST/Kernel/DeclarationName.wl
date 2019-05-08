BeginPackage["AST`DeclarationName`"]

Begin["`Private`"]

Needs["AST`"]

(*

given an LHS node, determine its declared name

DeclarationName will try to work with concrete syntax and abstract syntax
*)

DeclarationName[SymbolNode[Symbol, s_, _]] := s

DeclarationName[CallNode[node_, _, _]] := DeclarationName[node]

DeclarationName[BinaryNode[Condition, {node_, _}, _]] := DeclarationName[node]

DeclarationName[CallNode[SymbolNode[Symbol, "Condition", _], {node_, _}, _]] := DeclarationName[node]

(*
handle something like:
(pg_PointerGraph)["toGraph", opts___] := ToGraph[pg, opts]
*)
DeclarationName[GroupNode[GroupParen, {node_}, _]] := DeclarationName[node]

DeclarationName[PatternBlankNode[_, {node_, _}, _]] := DeclarationName[node]

DeclarationName[CallNode[SymbolNode[Symbol, "Pattern", _], {node_, _}, _]] := DeclarationName[node]

DeclarationName[BinaryNode[Pattern, {node_, _}, _]] := DeclarationName[node]

DeclarationName[BinaryNode[PatternTest, {node_, _}, _]] := DeclarationName[node]

DeclarationName[BinaryNode[BinaryAt, {node_, _}, _]] := DeclarationName[node]


(*
handle something like:
_CharacterEncodingData[_] := versionpanic[];
*)
DeclarationName[BlankNode[Blank, {node_}, _]] := DeclarationName[node]


DeclarationName[BinaryNode[BinarySlashSlash, {_, node_}, _]] := DeclarationName[node]



DeclarationName[BinaryNode[op_, _, _]] := op

DeclarationName[InfixNode[op_, _, _]] := op





(*
handle something like:
#LeftHandSide := ShortForm[#RightHandSide]
*)
(*
DeclarationName[args:SlotNode[_, _, _]] := Failure["Undefined", <|"Function"->DeclarationName, "Arguments"->HoldForm[{args}]|>]
*)



(*
DeclarationName[_StringNode] := String

DeclarationName[_SlotNode] := Slot

DeclarationName[GroupNode[List, _, _]] := List

DeclarationName[InfixNode[NonCommutativeMultiply, _, _]] := NonCommutativeMultiply

DeclarationName[InfixNode[Dot, _, _]] := Dot

DeclarationName[InfixNode[Alternatives, _, _]] := Alternatives

DeclarationName[BinaryNode[Rule, _, _]] := Rule

DeclarationName[PartNode[node_, _, _]] := DeclarationName[node]
*)

DeclarationName[args___] := Failure["InternalUnhandled", <|"Function"->DeclarationName, "Arguments"->HoldForm[{args}]|>]


End[]

EndPackage[]
