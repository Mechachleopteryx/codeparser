BeginPackage["CodeParser`Scoping`"]

ScopingData


scopingDataObject

Begin["`Private`"]

Needs["CodeParser`"]
Needs["CodeParser`Utils`"]


ScopingData[astIn_] :=
Module[{ast, definitions},

  ast = astIn;

  Block[{$LexicalScope, $Data, $ExcludePatternNames, $ConditionPatternNames, $Definitions},
  
    (*
    $LexicalScope is an assoc of name -> scope
    *)
    $LexicalScope = <||>;

    (*
    $Data is an assoc of name -> {scopingDataObject[]}
    *)
    $Data = <||>;

    (*
    $ExcludePatternNames is a list of names
    *)
    $ExcludePatternNames = {};
    
    (*
    $ConditionPatternNames is a list of names
    *)
    $ConditionPatternNames = {};


    definitions = Flatten[Cases[ast, _[_, _, KeyValuePattern["Definitions" -> defs_]] :> defs, Infinity]];

    (*
    $Definitions is an assoc of name -> symbols
    *)
    $Definitions = GroupBy[definitions, #[[2]]&];

    $LexicalScope = <|(# -> {"Defined"})& /@ Keys[$Definitions]|>;


    walk[ast];

    Flatten[Values[$Data]]
  ]
]


walk[ContainerNode[_, children_, _]] :=
  Flatten[walk /@ children]

freePatterns[ContainerNode[_, children_, _]] := 
  Flatten[freePatterns /@ children]


walk[PackageNode[_, children_, _]] :=
  Flatten[walk /@ children]

freePatterns[PackageNode[_, children_, _]] :=
  Flatten[freePatterns /@ children]


walk[ContextNode[_, children_, _]] :=
  Flatten[walk /@ children]

freePatterns[ContextNode[_, children_, _]] :=
  Flatten[freePatterns /@ children]


walk[NewContextPathNode[_, children_, _]] :=
  Flatten[walk /@ children]

freePatterns[NewContextPathNode[_, children_, _]] :=
  Flatten[freePatterns /@ children]



walk[CallNode[LeafNode[Symbol, "Module", _], {CallNode[LeafNode[Symbol, "List", _], vars_, _], body_}, _]] :=
Module[{variableSymbolsAndRHSOccurring, variableSymbols, rhsOccurring, variableNames, newScope, bodyOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    variableSymbolsAndRHSOccurring = Replace[vars, {
      sym:LeafNode[Symbol, _, _] :> {{sym}, {}},
      CallNode[LeafNode[Symbol, "Set" | "SetDelayed", _], {lhs:LeafNode[Symbol, _, _], rhs_}, _] :> {{lhs}, walk[rhs]},
      _ :> {{}, {}}
    }, 1];

    variableSymbols = Flatten[variableSymbolsAndRHSOccurring[[All, 1]]];
    rhsOccurring = Flatten[variableSymbolsAndRHSOccurring[[All, 2]]];

    variableNames = #[[2]]& /@ variableSymbols;

    newScope = <| (# -> {"Module"})& /@ variableNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, variableSymbols];

    rhsOccurring ~Join~ Complement[bodyOccurring, variableNames]
  ]
]


optPat = CallNode[LeafNode[Symbol, "Rule" | "RuleDelayed", _], {_, _}, _]

(*
DynamicModule can have options

The options have the same scope as the body, they understand the local variables
*)
walk[CallNode[LeafNode[Symbol, "DynamicModule", _], {CallNode[LeafNode[Symbol, "List", _], vars_, _], body_, optSeq:optPat...}, _]] :=
Module[{variableSymbolsAndRHSOccurring, variableSymbols, rhsOccurring, variableNames, newScope, bodyOccurring, optOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    variableSymbolsAndRHSOccurring = Replace[vars, {
      sym:LeafNode[Symbol, _, _] :> {{sym}, {}},
      CallNode[LeafNode[Symbol, "Set" | "SetDelayed", _], {lhs:LeafNode[Symbol, _, _], rhs_}, _] :> {{lhs}, walk[rhs]},
      _ :> {{}, {}}
    }, 1];

    variableSymbols = Flatten[variableSymbolsAndRHSOccurring[[All, 1]]];
    rhsOccurring = Flatten[variableSymbolsAndRHSOccurring[[All, 2]]];

    variableNames = #[[2]]& /@ variableSymbols;

    newScope = <| (# -> {"DynamicModule"})& /@ variableNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    optOccurring = Flatten[walk /@ {optSeq}];

    Scan[add[#, bodyOccurring]&, variableSymbols];

    rhsOccurring ~Join~ Complement[bodyOccurring, variableNames] ~Join~ optOccurring
  ]
]

walk[CallNode[LeafNode[Symbol, tag: "Block" | "Internal`InheritedBlock", _], {CallNode[LeafNode[Symbol, "List", _], vars_, _], body_}, _]] :=
Module[{variableSymbolsAndRHSOccurring, variableSymbols, rhsOccurring, variableNames, newScope, bodyOccurring, usedHeuristics},

  (*

  Now we will use heuristics to pare down the list of unused variables in Block

  if you have Block[{x = 1}, b]  then it is probably on purpose
  i.e., setting x to a value shows intention

  Blocking fully-qualified symbol is probably on purpose

  after removing fully-qualified symbols, now scan for lowercase symbols and only let those through

  on the assumption that lowercase symbols will be treated as "local" variables
  *)
  usedHeuristics = <||>;

  Internal`InheritedBlock[{$LexicalScope},

    variableSymbolsAndRHSOccurring = Replace[vars, {
      sym:LeafNode[Symbol, _, _] :> (If[fullyQualifiedSymbolQ[sym] || uppercaseOrDollarSymbolQ[sym], usedHeuristics[sym] = True];{{sym}, {}}),
      CallNode[LeafNode[Symbol, "Set" | "SetDelayed", _], {lhs:LeafNode[Symbol, _, _], rhs_}, _] :> (usedHeuristics[lhs] = True;{{lhs}, walk[rhs]}),
      _ :> {{}, {}}
    }, 1];

    variableSymbols = Flatten[variableSymbolsAndRHSOccurring[[All, 1]]];
    rhsOccurring = Flatten[variableSymbolsAndRHSOccurring[[All, 2]]];

    variableNames = #[[2]]& /@ variableSymbols;

    newScope = <| (# -> {tag})& /@ variableNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring || Lookup[usedHeuristics, #, False]]&, variableSymbols];

    rhsOccurring ~Join~ Complement[bodyOccurring, variableNames]
  ]
]

(*
if there is a ` anywhere in the symbol, then assume it is fully-qualified
*)
fullyQualifiedSymbolQ[LeafNode[Symbol, s_, _]] :=
  StringContainsQ[s, "`"]

uppercaseOrDollarSymbolQ[LeafNode[Symbol, s_, _]] :=
  StringMatchQ[s, RegularExpression["[A-Z\\$].*"]]


(*
With base case
*)
walk[CallNode[LeafNode[Symbol, "With", _], {CallNode[LeafNode[Symbol, "List", _], vars_, _], body_}, _]] :=
Module[{paramSymbolsAndRHSOccurring, paramSymbols, rhsOccurring, paramNames, newScope, bodyOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndRHSOccurring = Replace[vars, {
      CallNode[LeafNode[Symbol, "Set" | "SetDelayed", _], {lhs:LeafNode[Symbol, _, _], rhs_}, _] :> {{lhs}, walk[rhs]},
      _ :> {{}, {}}
    }, 1];

    paramSymbols = Flatten[paramSymbolsAndRHSOccurring[[All, 1]]];
    rhsOccurring = Flatten[paramSymbolsAndRHSOccurring[[All, 2]]];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"With"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    rhsOccurring ~Join~ Complement[bodyOccurring, paramNames]
  ]
]

walk[CallNode[LeafNode[Symbol, "With", _], {
  vars:CallNode[LeafNode[Symbol, "List", _], _, _],
  varsRestSeq:PatternSequence[
    CallNode[LeafNode[Symbol, "List", _], _, _],
    CallNode[LeafNode[Symbol, "List", _], _, _]...],
  body_}, data_]] :=
Module[{newBody, paramSymbolsAndRHSOccurring, paramSymbols, rhsOccurring, paramNames, newScope, bodyOccurring},

  newBody = CallNode[LeafNode[Symbol, "With", <||>], {varsRestSeq} ~Join~ {body}, data];

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndRHSOccurring = Replace[vars[[2]], {
      CallNode[LeafNode[Symbol, "Set" | "SetDelayed", _], {lhs:LeafNode[Symbol, _, _], rhs_}, _] :> {{lhs}, walk[rhs]},
      _ :> {{}, {}}
    }, 1];

    paramSymbols = Flatten[paramSymbolsAndRHSOccurring[[All, 1]]];
    rhsOccurring = Flatten[paramSymbolsAndRHSOccurring[[All, 2]]];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"With"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[newBody];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    rhsOccurring ~Join~ Complement[bodyOccurring, paramNames]
  ]
]

walk[CallNode[LeafNode[Symbol, tag : "SetDelayed" | "RuleDelayed", _], {lhs_, rhs_}, _]] :=
Module[{patterns, lhsOccurring, patternSymbols, patternAssoc, patternNames, newScope, rhsOccurring, conditionOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    patterns = freePatterns[lhs];

    Internal`InheritedBlock[{$ExcludePatternNames},

      $ExcludePatternNames = $ExcludePatternNames ~Join~ (#[[2, 1, 2]]& /@ patterns);

      lhsOccurring = walk[lhs];

      conditionOccurring = walkCondition[lhs];
    ];

    patternSymbols = Replace[patterns, {
      pat:CallNode[LeafNode[Symbol, "Pattern", _], {sym:LeafNode[Symbol, _, _], _}, _] :> sym
    }, 1];

    patternAssoc = GroupBy[patternSymbols, #[[2]]&];

    patternNames = Keys[patternAssoc];

    newScope = <| (# -> {tag})& /@ patternNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    rhsOccurring = walk[rhs];

    KeyValueMap[
      Function[{name, syms},
        Which[
          Length[syms] > 1,
            (*
            non-linear pattern, so mark all as used
            *)
            Scan[add[#, True]&, syms]
          ,
          !MemberQ[$ExcludePatternNames, name],
            add[syms[[1]], rhsOccurring ~Join~ conditionOccurring]
        ]
      ]
      ,
      patternAssoc
    ];

    lhsOccurring ~Join~ Complement[rhsOccurring, patternNames]
  ]
]

freePatterns[CallNode[LeafNode[Symbol, "SetDelayed" | "RuleDelayed", _], {lhs_, rhs_}, _]] := 
  freePatterns[rhs]


walk[CallNode[LeafNode[Symbol, "TagSetDelayed", _], {tag:LeafNode[Symbol, _, _], lhs_, rhs_}, _]] :=
Module[{tagOccurring, patterns, lhsOccurring, patternSymbols, patternAssoc, patternNames, newScope, rhsOccurring, conditionOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    tagOccurring = walk[tag];

    patterns = freePatterns[lhs];

    Internal`InheritedBlock[{$ExcludePatternNames},

      $ExcludePatternNames = $ExcludePatternNames ~Join~ (#[[2, 1, 2]]& /@ patterns);

      lhsOccurring = walk[lhs];

      conditionOccurring = walkCondition[lhs];
    ];

    patternSymbols = Replace[patterns, {
      pat:CallNode[LeafNode[Symbol, "Pattern", _], {sym:LeafNode[Symbol, _, _], _}, _] :> sym
    }, 1];

    patternAssoc = GroupBy[patternSymbols, #[[2]]&];

    patternNames = Keys[patternAssoc];

    newScope = <| (# -> {"TagSetDelayed"})& /@ patternNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    rhsOccurring = walk[rhs];

    KeyValueMap[
      Function[{name, syms},
        Which[
          Length[syms] > 1,
            (*
            non-linear pattern, so mark all as used
            *)
            Scan[add[#, True]&, syms]
          ,
          !MemberQ[$ExcludePatternNames, name],
            add[syms[[1]], rhsOccurring ~Join~ conditionOccurring]
        ]
      ]
      ,
      patternAssoc
    ];

    tagOccurring ~Join~ lhsOccurring ~Join~ Complement[rhsOccurring, patternNames]
  ]
]

freePatterns[CallNode[LeafNode[Symbol, "TagSetDelayed", _], {LeafNode[Symbol, _, _], lhs_, rhs_}, _]] := 
  freePatterns[rhs]


walk[CallNode[LeafNode[Symbol, "UpSetDelayed", _], {CallNode[_, children_, _], rhs_}, _]] :=
Module[{patterns, childrenOccurring, patternSymbols, patternAssoc, patternNames, newScope, rhsOccurring, conditionOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    patterns = Flatten[freePatterns /@ children];

    Internal`InheritedBlock[{$ExcludePatternNames},

      $ExcludePatternNames = $ExcludePatternNames ~Join~ (#[[2, 1, 2]]& /@ patterns);

      childrenOccurring = Flatten[walk /@ children];

      conditionOccurring = Flatten[walkCondition /@ children];
    ];

    patternSymbols = Replace[patterns, {
      pat:CallNode[LeafNode[Symbol, "Pattern", _], {sym:LeafNode[Symbol, _, _], _}, _] :> sym
    }, 1];

    patternAssoc = GroupBy[patternSymbols, #[[2]]&];

    patternNames = Keys[patternAssoc];

    newScope = <| (# -> {"UpSetDelayed"})& /@ patternNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    rhsOccurring = walk[rhs];

    KeyValueMap[
      Function[{name, syms},
        Which[
          Length[syms] > 1,
            (*
            non-linear pattern, so mark all as used
            *)
            Scan[add[#, True]&, syms]
          ,
          !MemberQ[$ExcludePatternNames, name],
            add[syms[[1]], rhsOccurring ~Join~ conditionOccurring]
        ]
      ]
      ,
      patternAssoc
    ];

    childrenOccurring ~Join~ Complement[rhsOccurring, patternNames]
  ]
]

freePatterns[CallNode[LeafNode[Symbol, "UpSetDelayed", _], {CallNode[_, _, _], rhs_}, _]] := 
  freePatterns[rhs]


walk[CallNode[LeafNode[Symbol, "Function", _], {body_}, _]] :=
Module[{newScope, bodyOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    newScope = <| $slotName -> {"Function"} |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    add[$slotName, bodyOccurring];

    Complement[bodyOccurring, {$slotName}]
  ]
]

walk[CallNode[LeafNode[Symbol, "Function", _], {LeafNode[Symbol, "Null", _], body_, attrs:(PatternSequence[] | _)}, _]] :=
Module[{newScope, bodyOccurring, attrsOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    attrsOccurring = Flatten[walk /@ {attrs}];

    newScope = <| $slotName -> {"Function"} |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    add[$slotName, bodyOccurring];

    Complement[bodyOccurring, {$slotName}] ~Join~ attrsOccurring
  ]
]

walk[CallNode[LeafNode[Symbol, "Function", _], {paramSymbol:LeafNode[Symbol, _, _], body_, attrs:(PatternSequence[] | _)}, _]] :=
Module[{paramName, newScope, bodyOccurring, attrsOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    attrsOccurring = Flatten[walk /@ {attrs}];

    paramName = paramSymbol[[2]];

    newScope = <| (paramName -> {"Function"}) |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    add[paramSymbol, bodyOccurring];

    Complement[bodyOccurring, {paramName}] ~Join~ attrsOccurring
  ]
]

walk[CallNode[LeafNode[Symbol, "Function", _], {CallNode[LeafNode[Symbol, "List", _], params:{LeafNode[Symbol, _, _]...}, _], body_, attrs:(PatternSequence[] | _)}, _]] :=
Module[{paramSymbols, paramNames, newScope, bodyOccurring, attrsOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    attrsOccurring = Flatten[walk /@ {attrs}];

    paramSymbols = Replace[params, {
      sym:LeafNode[Symbol, _, _] :> sym
    }, 1];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"Function"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    Complement[bodyOccurring, paramNames] ~Join~ attrsOccurring
  ]
]


compiledFunctionTypePat = CallNode[LeafNode[Symbol, "Typed", _], {LeafNode[Symbol, _, _], _}, _]

walk[CallNode[LeafNode[Symbol, "Function", _], {CallNode[LeafNode[Symbol, "List", _], params:{compiledFunctionTypePat, compiledFunctionTypePat...}, _], body_, attrs:(PatternSequence[] | _)}, _]] :=
Module[{paramSymbolsAndTypeOccurring, paramSymbols, typeOccurring, paramNames, newScope, bodyOccurring, attrsOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndTypeOccurring = Replace[params, {
      CallNode[LeafNode[Symbol, "Typed", _], {sym:LeafNode[Symbol, _, _], type_}, _] :> {{sym}, walk[type]}
    }, 1];

    paramSymbols = Flatten[paramSymbolsAndTypeOccurring[[All, 1]]];
    typeOccurring = Flatten[paramSymbolsAndTypeOccurring[[All, 2]]];

    attrsOccurring = Flatten[walk /@ {attrs}];
    
    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"Function"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    typeOccurring ~Join~ Complement[bodyOccurring, paramNames] ~Join~ attrsOccurring
  ]
]


iterPat = CallNode[LeafNode[Symbol, "List", _], {LeafNode[Symbol, _, _], _, _ | PatternSequence[], _ | PatternSequence[]} | {_}, _]

(*
Do | Table base case
*)
walk[CallNode[head:LeafNode[Symbol, tag : "Do" | "Table", _], {body_, iter:iterPat}, _]] :=
Module[{paramSymbolsAndIterOccurring, paramSymbols, iterOccurring, paramNames, newScope, bodyOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndIterOccurring =
      Replace[iter[[2]], {
        {sym:LeafNode[Symbol, _, _], max_} :> {{sym}, walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_} :> {{sym}, walk[min] ~Join~ walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_, d_} :> {{sym}, walk[min] ~Join~ walk[max] ~Join~ walk[d]},
        {max_} :> {{}, walk[max]},
        _ :> {{}, {}}
      }, {0}];

    paramSymbols = paramSymbolsAndIterOccurring[[1]];
    iterOccurring = paramSymbolsAndIterOccurring[[2]];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {tag})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    iterOccurring ~Join~ Complement[bodyOccurring, paramNames]
  ]
]

walk[CallNode[head:LeafNode[Symbol, tag : "Do" | "Table", _], {body_, iter:iterPat, iterRestSeq:PatternSequence[iterPat, iterPat...]}, data_]] :=
Module[{newBody, paramSymbolsAndIterOccurring, paramSymbols, iterOccurring, paramNames, newScope, bodyOccurring},

  newBody = CallNode[LeafNode[Symbol, tag, <||>], {body} ~Join~ {iterRestSeq}, data];

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndIterOccurring =
      Replace[iter[[2]], {
        {sym:LeafNode[Symbol, _, _], max_} :> {{sym}, walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_} :> {{sym}, walk[min] ~Join~ walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_, d_} :> {{sym}, walk[min] ~Join~ walk[max] ~Join~ walk[d]},
        {max_} :> {{}, walk[max]},
        _ :> {{}, {}}
      }, {0}];

    paramSymbols = paramSymbolsAndIterOccurring[[1]];
    iterOccurring = paramSymbolsAndIterOccurring[[2]];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {tag})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[newBody];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    iterOccurring ~Join~ Complement[bodyOccurring, paramNames]
  ]
]

(*
Sum | ParallelTable base case

Sum and ParallelTable can have options
*)
walk[CallNode[head:LeafNode[Symbol, tag : "Sum" | "ParallelTable", _], {body_, iter:iterPat, optSeq:optPat...}, _]] :=
Module[{paramSymbolsAndIterOccurring, paramSymbols, iterOccurring, paramNames, newScope, bodyOccurring, optOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndIterOccurring =
      Replace[iter[[2]], {
        {sym:LeafNode[Symbol, _, _], max_} :> {{sym}, walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_} :> {{sym}, walk[min] ~Join~ walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_, d_} :> {{sym}, walk[min] ~Join~ walk[max] ~Join~ walk[d]},
        {max_} :> {{}, walk[max]},
        _ :> {{}, {}}
      }, {0}];

    paramSymbols = paramSymbolsAndIterOccurring[[1]];
    iterOccurring = paramSymbolsAndIterOccurring[[2]];

    optOccurring = Flatten[walk /@ {optSeq}];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {tag})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    iterOccurring ~Join~ Complement[bodyOccurring, paramNames] ~Join~ optOccurring
  ]
]

walk[CallNode[head:LeafNode[Symbol, tag : "Sum" | "ParallelTable", _], {body_, iter:iterPat, iterRestSeq:PatternSequence[iterPat, iterPat...], optSeq:optPat...}, data_]] :=
Module[{newBody, paramSymbolsAndIterOccurring, paramSymbols, iterOccurring, paramNames, newScope, bodyOccurring, optOccurring},

  newBody = CallNode[LeafNode[Symbol, head[[2]], <||>], {body} ~Join~ {iterRestSeq}, data];

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndIterOccurring =
      Replace[iter[[2]], {
        {sym:LeafNode[Symbol, _, _], max_} :> {{sym}, walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_} :> {{sym}, walk[min] ~Join~ walk[max]},
        {sym:LeafNode[Symbol, _, _], min_, max_, d_} :> {{sym}, walk[min] ~Join~ walk[max] ~Join~ walk[d]},
        {max_} :> {{}, walk[max]},
        _ :> {{}, {}}
      }, {0}];

    paramSymbols = paramSymbolsAndIterOccurring[[1]];
    iterOccurring = paramSymbolsAndIterOccurring[[2]];

    optOccurring = Flatten[walk /@ {optSeq}];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {tag})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[newBody];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    iterOccurring ~Join~ Complement[bodyOccurring, paramNames] ~Join~ optOccurring
  ]
]


rangePat = CallNode[LeafNode[Symbol, "List", _], {LeafNode[Symbol, _, _], _, _}, _]

walk[CallNode[head:LeafNode[Symbol, "Play", _], {body_, range:rangePat}, _]] :=
Module[{paramSymbolsAndRangeOccurring, paramSymbols, rangeOccurring, paramNames, newScope, bodyOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndRangeOccurring =
      Replace[range[[2]], {
        {sym:LeafNode[Symbol, _, _], min_, max_} :> {{sym}, walk[min] ~Join~ walk[max]}
      }, {0}];

    paramSymbols = paramSymbolsAndRangeOccurring[[1]];
    rangeOccurring = paramSymbolsAndRangeOccurring[[2]];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"Play"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    rangeOccurring ~Join~ Complement[bodyOccurring, paramNames]
  ]
]


compileTypePat = CallNode[LeafNode[Symbol, "List", _], {LeafNode[Symbol, _, _], _, PatternSequence[] | _}, _]

walk[CallNode[LeafNode[Symbol, "Compile", _], {CallNode[LeafNode[Symbol, "List", _], types:{compileTypePat, compileTypePat...}, _], body_, optionsAndPatterns___}, _]] :=
Module[{paramSymbolsAndTypeOccurring, paramSymbols, typeOccurring, paramNames, newScope, bodyOccurring, optionsAndPatternsOccurring},

  Internal`InheritedBlock[{$LexicalScope},

    paramSymbolsAndTypeOccurring =
      Function[{type},
        Replace[type[[2]], {
          {sym:LeafNode[Symbol, _, _], pattern_} :> {{sym}, walk[pattern]},
          {sym:LeafNode[Symbol, _, _], pattern_, rank_} :> {{sym}, walk[pattern] ~Join~ walk[rank]},
          _ :> {{}, {}}
        }, {0}]
      ] /@ types;

    paramSymbols = Flatten[paramSymbolsAndTypeOccurring[[All, 1]]];
    typeOccurring = Flatten[paramSymbolsAndTypeOccurring[[All, 2]]];

    optionsAndPatternsOccurring = Flatten[walk /@ {optionsAndPatterns}];

    paramNames = #[[2]]& /@ paramSymbols;

    newScope = <| (# -> {"Compile"})& /@ paramNames |>;

    $LexicalScope = Merge[{$LexicalScope, newScope}, Flatten];

    bodyOccurring = walk[body];

    Scan[add[#, bodyOccurring]&, paramSymbols];

    typeOccurring ~Join~ Complement[bodyOccurring, paramNames] ~Join~ optionsAndPatternsOccurring
  ]
]



freePatterns[CallNode[LeafNode[Symbol, "Condition", _], {lhs_, rhs_}, _]] := 
  freePatterns[lhs]

walkCondition[CallNode[head:LeafNode[Symbol, "Condition", _], {lhs_, rhs_}, _]] :=
Module[{lhsOccurring, rhsOccurring},

  lhsOccurring = walkCondition[lhs];

  Internal`InheritedBlock[{$ConditionPatternNames},

    $ConditionPatternNames = $ConditionPatternNames ~Join~ $ExcludePatternNames;

    rhsOccurring = walkCondition[rhs];
  ];

  lhsOccurring ~Join~ rhsOccurring
]


walk[CallNode[LeafNode[Symbol, "Pattern", _], {lhs:LeafNode[Symbol, name_, _], rhs_}, _]] :=
Module[{decls},

  Internal`InheritedBlock[{$LexicalScope},

    decls = Lookup[$LexicalScope, name, {}];

    If[MatchQ[decls, {___, "SetDelayed" | "RuleDelayed" | "TagSetDelayed" | "UpSetDelayed"}],
      (*
      If the pattern name is already bound to another pattern, then treat this as an error

      e.g.:
      f[x_] := g[x_]

      treat the 2nd x as an error
      *)
      decls = decls ~Join~ {"Error"};
      $LexicalScope[name] = decls
    ];

    If[!MemberQ[$ExcludePatternNames, name],
      walk[lhs] ~Join~ walk[rhs]
      ,
      walk[rhs]
    ]
  ]
]

freePatterns[pat:CallNode[LeafNode[Symbol, "Pattern", _], {LeafNode[Symbol, _, _], rhs_}, _]] := 
  Flatten[{pat} ~Join~ freePatterns[rhs]]


walk[CallNode[LeafNode[Symbol, "Slot" | "SlotSequence", _], _, data_]] :=
Catch[
Module[{decls, entry},

  decls = Lookup[$LexicalScope, $slotName, {}];

  If[!empty[decls],

    (*
    Source may have been abstracted away
    *)
    If[KeyExistsQ[data, Source],

      entry = Lookup[$Data, $slotName, {}];

      AppendTo[entry,
        scopingDataObject[
          data[[Key[Source]]],
          decls,
          modifiersSet[decls, True]
        ]
      ];

      $Data[$slotName] = entry;
    ];

    Throw[{$slotName}]
  ];


  (*
  naked Slot
  *)
  (*
  Source may have been abstracted away
  *)
  If[KeyExistsQ[data, Source],

    entry = Lookup[$Data, $slotName, {}];

    AppendTo[entry,
      scopingDataObject[
        data[[Key[Source]]],
        decls,
        {"error"}
      ]
    ];

    $Data[$slotName] = entry;
  ];

  {$slotName}
]]


walk[CallNode[head_, children_, _]] :=
  Flatten[Join[walk[head], walk /@ children]]

freePatterns[CallNode[head_, children_, _]] := 
  Flatten[freePatterns[head] ~Join~ (freePatterns /@ children)]

walkCondition[CallNode[head_, children_, _]] :=
  Flatten[Join[walkCondition[head], walkCondition /@ children]]



walk[sym:LeafNode[Symbol, name_, data_]] :=
Catch[
Module[{decls, entry, defs},

  decls = Lookup[$LexicalScope, name, {}];

  defs = Lookup[$Definitions, name, {}];

  (*
  a definition
  *)
  If[MemberQ[defs, sym],

    (*
    Source may have been abstracted away
    *)
    If[KeyExistsQ[data, Source],

      entry = Lookup[$Data, name, {}];

      AppendTo[entry,
        scopingDataObject[
          data[[Key[Source]]],
          decls,
          {"definition"}
        ]
      ];

      $Data[name] = entry;
    ];

    Throw[{name}]
  ];

  If[!empty[decls],

    (*
    Source may have been abstracted away
    *)
    If[KeyExistsQ[data, Source],

      entry = Lookup[$Data, name, {}];

      AppendTo[entry,
        scopingDataObject[
          data[[Key[Source]]],
          decls,
          modifiersSet[decls, True]
        ]
      ];

      $Data[name] = entry;
    ];

    Throw[{name}]
  ];

  {}
]]

walkCondition[sym:LeafNode[Symbol, name_, data_]] :=
Module[{decls, entry},
  
  decls = Lookup[$LexicalScope, name, {}];

  decls = decls ~Join~ {"ThingThatUnderstandsCondition"};

  If[MemberQ[$ConditionPatternNames, name],

    entry = Lookup[$Data, name, {}];

    (*
    Remove any previous data

    We may have more knowledge now about the symbol that is in the Condition

    We may now know that it is shadowed

    So remove previous entries for the same symbol
    *)
    entry = DeleteCases[entry, scopingDataObject[data[[Key[Source]]], _, _]];

    AppendTo[entry,
      scopingDataObject[
        data[[Key[Source]]],
        decls,
        modifiersSet[decls, True]
      ]
    ];

    $Data[name] = entry;

    {name}
    ,
    {}
  ]
]

walkCondition[LeafNode[_, _, _]] :=
  {}


add[sym:LeafNode[Symbol, name_, data_], True] :=
Module[{decls, entry},

  decls = Lookup[$LexicalScope, name, {}];

  entry = Lookup[$Data, name, {}];

  AppendTo[entry,
    scopingDataObject[
      data[[Key[Source]]],
      decls,
      modifiersSet[decls, True]
    ]
  ];

  $Data[name] = entry;
]

add[sym:LeafNode[Symbol, name_, data_], occurringScopedNames_] :=
Module[{decls, entry},

  decls = Lookup[$LexicalScope, name, {}];

  entry = Lookup[$Data, name, {}];

  AppendTo[entry,
    scopingDataObject[
      data[[Key[Source]]],
      decls,
      modifiersSet[decls, MemberQ[occurringScopedNames, name]]
    ]
  ];

  $Data[name] = entry;
]


walk[LeafNode[_, _, _]] :=
  {}

freePatterns[LeafNode[_, _, _]] := 
  {}


walk[ErrorNode[_, _, _]] :=
  {}

freePatterns[ErrorNode[_, _, _]] := 
  {}


walk[AbstractSyntaxErrorNode[_, _, _]] :=
  {}

freePatterns[AbstractSyntaxErrorNode[_, _, _]] := 
  {}


walk[UnterminatedCallNode[_, _, _]] :=
  {}

freePatterns[UnterminatedCallNode[_, _, _]] := 
  {}


walk[UnterminatedGroupNode[_, _, _]] :=
  {}

freePatterns[UnterminatedGroupNode[_, _, _]] := 
  {}


walk[BoxNode[_, children_, _]] :=
  Flatten[walk /@ children]

freePatterns[BoxNode[_, body_, _]] := 
  Flatten[freePatterns /@ body]

walkCondition[BoxNode[_, children_, _]] :=
  Flatten[walkCondition /@ children]




(*
modifiersSet[decls_, used_]
*)
modifiersSet[{___, "Error"}, _] :=
  {"error"}

(*
Handle the common case of entering:

foo[a_] :=
Module[{a},
  xxx
]

The pattern and the Module variable have the same name

FIXME: I should probably do more to handle more of these errors: errors of Module variables shadowing patterns

*)
modifiersSet[{___, "SetDelayed" | "UpsetDelayed" | "RuleDelayed" | "TagSetDelayed" | "UpSetDelayed", "Module" | "Block"}, _] :=
  {"error"}


modifiersSet[{_}, False] :=
  {"unused"}
modifiersSet[{_}, True] :=
  {}
modifiersSet[_, False] :=
  {"unused", "shadowed"}
modifiersSet[_, True] :=
  {"shadowed"}



End[]

EndPackage[]
