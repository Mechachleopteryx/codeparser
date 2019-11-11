

path = FileNameJoin[{DirectoryName[$CurrentTestSource], "ASTTestUtils"}]
PrependTo[$Path, path]

Needs["ASTTestUtils`"]



Needs["AST`"]

(*

Parse File

*)

sample = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "sample.wl"}]

cst = ConcreteParseFile[sample]

Test[
	cst
	,
	FileNode[File, {
		LeafNode[Token`Newline, "\n", <|Source -> {{2, 0}, {2, 0}}|>],
		InfixNode[Plus, {
			LeafNode[Integer, "1", <|Source -> {{2, 1}, {2, 1}}|>],
			LeafNode[Token`Plus, "+", <|Source -> {{2, 2}, {2, 2}}|>],
    		LeafNode[Integer, "1", <|Source -> {{2, 3}, {2, 3}}|>] }, <|Source -> {{2, 1}, {2, 3}}|>],
    	LeafNode[Token`Newline, "\n", <|Source -> {{3, 0}, {3, 0}}|>] }, <|Source -> {{2, 0}, {3, 0}}|>]
	,
	TestID->"File-20181230-J0G3I8"
]



Test[
	ToInputFormString[cst]
	,
	"\n\n 1 + 1 \n\n"
	,
	TestID->"File-20181230-T2D2W6"
]








(*
shebangwarning = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "shebangwarning.wl"}]

cst = ConcreteParseFile[shebangwarning]

TestMatch[
	cst
	,
	FileNode[File, {
		LeafNode[Slot, "#something", <|Source -> {{1, 1}, {1, 10}}|>],
		LeafNode[Token`Newline, "\n", <|Source -> {{2, 0}, {2, 0}}|>] },
		KeyValuePattern[{
			Source -> {{1, 1}, {2, 0}},
			SyntaxIssues -> {SyntaxIssue["Shebang", "# on first line looks like #! shebang", "Remark", <|Source -> {{1, 1}, {1, 1}}|>]} }] ]
	,
	TestID->"File-20181230-M7H7Q7"
]
*)


carriagereturn = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn.wl"}]

cst = ConcreteParseFile[carriagereturn]

TestMatch[
	cst
	,
	FileNode[File, {LeafNode[Token`Newline, "\r", <|Source -> {{2, 0}, {2, 0}}|>],
					LeafNode[Token`Newline, "\r", <|Source -> {{3, 0}, {3, 0}}|>],
					LeafNode[Symbol, "A", <|Source -> {{3, 1}, {3, 1}}|>]},
										<| SyntaxIssues->{FormatIssue["UnexpectedCarriageReturn", _, _, <|Source -> {{2, 0}, {2, 0}}|>],
											FormatIssue["UnexpectedCarriageReturn", _, _, <|Source -> {{3, 0}, {3, 0}}|>]}, Source -> {{2, 0}, {3, 1}}|>]
	,
	TestID->"File-20190422-C6U5B6"
]

carriagereturn2 = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn2.wl"}]

cst = ConcreteParseFile[carriagereturn2]

TestMatch[
	cst
	,
	FileNode[File, {LeafNode[String, "\"\r\n123\"", <|Source->{{1,1},{2,4}}|>]}, <|Source->{{1,1},{2,4}}|>]
	,
	TestID->"File-20190606-O8I6M9"
]










(*
warnings
*)

package = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "package.wl"}]

ast = ParseFile[package]

TestMatch[
	ast
	,
	FileNode[File, {
		CallNode[LeafNode[Symbol, "BeginPackage", <|Source -> {{2, 1}, {2, 12}}|>], {
			LeafNode[String, "\"Foo.m`\"", <|Source -> {{2, 14}, {2, 21}}|>]}, <|Source -> {{2, 1}, {2, 22}}|>], 
		CallNode[LeafNode[Symbol, "EndPackage", <|Source -> {{4, 1}, {4, 10}}|>], {}, <|Source -> {{4, 1}, {4, 12}}|>]}, <|Source -> {{2, 0}, {6, 0}}, AbstractSyntaxIssues -> {SyntaxIssue["Package", "Package directive does not have correct syntax.", "Error", _]}|>]
	,
	TestID->"File-20190601-E8O7Y2"
]






(*

Tokenize File

*)

sample = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "sample.wl"}]

Test[
	TokenizeFile[sample]
	,
	{LeafNode[Token`Newline, "\n", <|Source -> {{2, 0}, {2, 0}}|>], 
 LeafNode[Integer, "1", <|Source -> {{2, 1}, {2, 1}}|>], 
 LeafNode[Token`Plus, "+", <|Source -> {{2, 2}, {2, 2}}|>], 
 LeafNode[Integer, "1", <|Source -> {{2, 3}, {2, 3}}|>], 
 LeafNode[Token`Newline, "\n", <|Source -> {{3, 0}, {3, 0}}|>]}
	,
	TestID->"File-20181230-Q3C4N0"
]









(*

test large number of comments

*)

comments = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "comments.wl"}]

cst = ConcreteParseFile[comments]

TestMatch[
	cst
	,
	FileNode[File, _, _]
	,
	TestID->"File-20190601-G6E3K9"
]







strange = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "strange.wl"}]

cst = ConcreteParseFile[strange]

TestMatch[
	cst
	,
	FileNode[File, {
		BinaryNode[Set, {
			LeafNode[Symbol, "\.01x", <|Source -> {{1, 1}, {1, 2}}|>],
			LeafNode[Token`WhiteSpace, " ", <|Source -> {{1, 3}, {1, 3}}|>],
			LeafNode[Token`Equal, "=", <|Source -> {{1, 4}, {1, 4}}|>],
			LeafNode[Token`WhiteSpace, " ", <|Source -> {{1, 5}, {1, 5}}|>],
			LeafNode[Integer, "1", <|Source -> {{1, 6}, {1, 6}}|>] }, <|Source -> {{1, 1}, {1, 6}}|>] }, <|SyntaxIssues -> {
				(* from CharacterDecoder, strange character in general *)
				SyntaxIssue["UnexpectedCharacter", "Unexpected character: ``\\.01``.", "Warning", _],
				(* from Tokenizer, strange letterlike *)
				SyntaxIssue["UnexpectedCharacter", "Unexpected character: ``\\.01``.", "Warning", _]}, Source -> {{1, 1}, {1, 6}}|>]
	,
	TestID->"File-20190602-N5D1B8"
]



strange = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "string1.wl"}]

cst = ConcreteParseFile[strange]

TestMatch[
	cst
	,
	FileNode[File, {
		LeafNode[Token`Newline, "\n", <|Source -> {{2, 0}, {2, 0}}|>],
		LeafNode[String, "\"data\\\\\n\"", <|Source -> {{2, 1}, {3, 1}}|>],
		LeafNode[Token`Newline, "\n", <|Source -> {{4, 0}, {4, 0}}|>],
		LeafNode[Token`Newline, "\n", <|Source -> {{5, 0}, {5, 0}}|>],
		LeafNode[Symbol, "x", <|Source -> {{5, 1}, {5, 1}}|>],
		LeafNode[Token`Newline, "\n", <|Source -> {{6, 0}, {6, 0}}|>]}, <|Source -> {{2, 0}, {6, 0}}|>]
	,
	TestID->"File-20190804-K7V2D8"
]





(*

Yes, it is unsatisfactory to have 1\\\n be treated as an Integer, with no LineContinuation token coming after

*)

continuation = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "continuation.wl"}]

cst = ConcreteParseFile[continuation]

TestMatch[
	cst
	,
	FileNode[File, {
		GroupNode[List, {
			LeafNode[Token`OpenCurly, "{", <|Source->{{1, 1}, {1, 1}}|>],
			LeafNode[Token`Newline, "\n", <|Source->{{2, 0}, {2, 0}}|>],
			LeafNode[Token`WhiteSpace, "\t", <|Source->{{2, 1}, {2, 1}}|>],
			LeafNode[Integer, "1", <|Source->{{2, 2}, {2, 2}}|>],
			LeafNode[Token`CloseCurly, "}", <|Source->{{3, 1}, {3, 1}}|>]}, <|Source -> {{1, 1}, {3, 1}}|>]}, <|Source -> {{1, 1}, {3, 1}}|>]
	,
	TestID->"File-20191025-I3T9F3"
]





crash2 = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash2.txt"}]

cst = ConcreteParseFile[crash2]

TestMatch[
	cst
	,
	FileNode[File, {
		StartOfLineNode[Information, {
			LeafNode[Token`Question, "?", <|Source -> {{1, 1}, {1, 1}}|>],
			LeafNode[String, "123456\"", <|Source -> {{1, 2}, {2, 4}}|>]}, <|Source -> {{1, 1}, {2, 4}}|>]}, <|Source -> {{1, 1}, {2, 4}}|>]
	,
	TestID->"File-20191103-T1K0D2"
]











(*

parseTest

*)


Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn2.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-S9D2H1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn3.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-M7F7D1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn4.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190607-H0M3H1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "carriagereturn.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-N0A3V6"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "comments.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-C8M7N2"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-K3J8I5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0001.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-P8X5C7"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0002.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-T7B2I1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0003.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-G9V4A7"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-characternameoperations.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-K6L3G0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-characternamestrings.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-K3I5E7"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-characternames.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-E9U5Q2"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-contexts.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-B4F3R6"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-integers.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-I5H3E5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-nestedsymbolicarithmetic.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-I4D8M0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-random.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-K8J6I8"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-reals.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-L1U3P6"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-specialchararacters.txt"}], 1]
	,
	ok
	,
	TestID->"File-20190606-K8S9K0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-symbolicarithmetic2.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-G1A4I3"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-symbolicarithmetic3.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-U5V2M1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-symbolicarithmetic4.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-T5M9O3"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-symbolicarithmetic.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-U7S8O5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "linearsyntax.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-W9Q9W8"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "package.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-G3M7M0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "sample.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-A0H2X0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "script.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190610-D7P4F8"
]

(*
Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "shebangwarning.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-F1E7Z2"
]
*)

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "strange.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-G6Q1C7"
]


Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "span1.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190626-L4G9A9"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "string1.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190804-T9F2J6"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "comment.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190904-Q7T9Y5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "continuation.wl"}], 1]
	,
	Null
	,
	TestID->"File-20191025-Z3P6E3"
]

(*
ToExpression["a >>\n   b", InputForm, Hold] works,

but ToExpression["Hold[a >>\n   b]"] does not

Due to the parsing of >>

But really we are just testing for a crash here, so just ignore the output
*)
Test[
	Catch[
		parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash2.txt"}], 1]
		,
		"Uncaught"
	];
	ignored
	,
	ignored
	,
	TestID->"File-20191102-Z8M8J2"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash3.txt"}], 1]
	,
	Null
	,
	{Syntax::sntoct1}
	,
	TestID->"File-20191102-W1Z2A1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash4.txt"}], 1]
	,
	Null
	,
	TestID->"File-20191103-I6Y6P4"
]

(*
ToExpression["1*\\\n"] returns 1, but should fail

The new parser does the correct thing and gives an error

bug 382006

really, we are just testing if this crashed, so just ignore the output
*)
Test[
	Catch[
		parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash5.txt"}], 1]
		,
		"Uncaught"
	];
	ignored
	,
	ignored
	,
	TestID->"File-20191103-V9V1F9"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash6.txt"}], 1]
	,
	ok
	,
	TestID->"File-20191103-B9T6B0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash7.txt"}], 1]
	,
	ok
	,
	TestID->"File-20191103-A0V5S0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash8.txt"}], 1]
	,
	ok
	,
	TestID->"File-20191103-D1L3U1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash9.txt"}], 1]
	,
	Null
	,
	TestID->"File-20191103-F1U3Y6"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash10.txt"}], 1]
	,
	ok
	,
	TestID->"File-20191103-H2O6Z5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "small", "crash11.txt"}], 1]
	,
	ok
	,
	TestID->"File-20191103-U8W2U9"
]











