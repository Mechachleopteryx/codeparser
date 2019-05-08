

path = FileNameJoin[{DirectoryName[$CurrentTestSource], "ASTTestUtils"}]
PrependTo[$Path, path]

Needs["ASTTestUtils`"]



Needs["AST`"]

(*

Parse File

*)

sample = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "sample.wl"}]

cst = ConcreteParseFile[sample]

Test[
	cst
	,
	FileNode[File, {
		InfixNode[Plus, {
			IntegerNode[Integer, "1", <|Source -> {{2, 1}, {2, 1}}|>],
			TokenNode[Token`Plus, "+", <|Source -> {{2, 2}, {2, 2}}|>],
    		IntegerNode[Integer, "1", <|Source -> {{2, 3}, {2, 3}}|>] }, <|Source -> {{2, 1}, {2, 3}}|>] }, <|SyntaxIssues -> {}, Source -> {{2, 1}, {2, 3}}|>]
	,
	TestID->"File-20181230-J0G3I8"
]



Test[
	ToInputFormString[cst]
	,
	" 1 + 1 "
	,
	TestID->"File-20181230-T2D2W6"
]









shebangwarning = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "shebangwarning.wl"}]

cst = ConcreteParseFile[shebangwarning]

TestMatch[
	cst
	,
	FileNode[File, {SlotNode[Slot, "#something", <|Source -> {{1, 1}, {1, 10}}|>]},
		KeyValuePattern[{
			Source -> {{1, 1}, {1, 10}},
			SyntaxIssues -> {SyntaxIssue["Shebang", "# on first line looks like #! shebang", "Remark", <|Source -> {{1, 1}, {1, 1}}|>]} }] ]
	,
	TestID->"File-20181230-M7H7Q7"
]


carriagereturn = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn.wl"}]

cst = ConcreteParseFile[carriagereturn]

TestMatch[
	cst
	,
	FileNode[File, {SymbolNode[Symbol, "A", <|Source -> {{3, 1}, {3, 1}}|>]},
										<| SyntaxIssues->{SyntaxIssue["StrayCarriageReturn", _, "Remark", <|Source -> {{2, 0}, {2, 0}}|>],
											SyntaxIssue["StrayCarriageReturn", _, "Remark", <|Source -> {{3, 0}, {3, 0}}|>]}, Source -> {{3, 1}, {3, 1}}|>]
	,
	TestID->"File-20190422-C6U5B6"
]

carriagereturn2 = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn2.wl"}]

cst = ConcreteParseFile[carriagereturn2]

TestMatch[
	cst
	,
	FileNode[File, {StringNode[String, "\"\r\n123\"", <|Source->{{1,1},{2,4}}|>]}, <|SyntaxIssues->{}, Source->{{1,1},{2,4}}|>]
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
	FileNode[File, {CallNode[SymbolNode[Symbol, "BeginPackage", <|Source -> {{2, 1}, {2, 12}}|>], {
		StringNode[String, "\"Foo.m`\"", <|Source -> {{2, 14}, {2, 21}}|>]}, <|Source -> {{2, 1}, {2, 22}}|>], 
		CallNode[SymbolNode[Symbol, "EndPackage", <|Source -> {{4, 1}, {4, 10}}|>], {}, <|Source -> {{4, 1}, {4, 12}}|>]}, 
		<|Source -> {{2, 1}, {4, 12}}, AbstractSyntaxIssues -> {
			SyntaxIssue["Package", "Package directive does not have correct syntax.", "Error", <|Source -> {{2, 1}, {2, 22}}|>]}|>]
	,
	TestID->"File-20190601-E8O7Y2"
]
















(*

Tokenize File

*)

sample = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "sample.wl"}]

Test[
	TokenizeFile[sample]
	,
	{TokenNode[Token`Newline, "\n", <|Source -> {{2, 0}, {2, 0}}|>], 
 TokenNode[Token`Integer, "1", <|Source -> {{2, 1}, {2, 1}}|>], 
 TokenNode[Token`Plus, "+", <|Source -> {{2, 2}, {2, 2}}|>], 
 TokenNode[Token`Integer, "1", <|Source -> {{2, 3}, {2, 3}}|>], 
 TokenNode[Token`Newline, "\n", <|Source -> {{3, 0}, {3, 0}}|>]}
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







strange = FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "strange.wl"}]

cst = ConcreteParseFile[strange]

TestMatch[
	cst
	,
	FileNode[File, {BinaryNode[Set, {SymbolNode[Symbol, "\.01x", <|Source -> {{1, 1}, {1, 2}}|>],
		TokenNode[Token`Equal, "=", <|Source -> {{1, 4}, {1, 4}}|>], IntegerNode[Integer, "1",
			<|Source -> {{1, 6}, {1, 6}}|>]}, <|Source -> {{1, 1}, {1, 6}}|>]}, <|SyntaxIssues -> {
			SyntaxIssue["StrangeCharacter", "Strange character in symbol: \\.01", "Warning", <|Source -> {{1, 1}, {1, 1}}|>]},
			Source -> {{1, 1}, {1, 6}}|>]
	,
	TestID->"File-20190602-N5D1B8"
]







(*

parseTest

*)


Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn2.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-S9D2H1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn3.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-M7F7D1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn4.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190607-H0M3H1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "carriagereturn.wl"}], 1]
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
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "crash.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-K3J8I5"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0001.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-P8X5C7"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0002.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-T7B2I1"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-0003.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-G9V4A7"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-characternameoperations.txt"}], 1]
	,
	Null
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
	Null
	,
	TestID->"File-20190606-E9U5Q2"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-comments.txt"}], 1]
	,
	Null
	,
	TestID->"File-20190606-L5D5P4"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "inputs-contexts.txt"}], 1]
	,
	Null
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
	Null
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
	Null
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
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "sample.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-A0H2X0"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "shebangwarning.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-F1E7Z2"
]

Test[
	parseTest[FileNameJoin[{DirectoryName[$CurrentTestSource], "files", "strange.wl"}], 1]
	,
	Null
	,
	TestID->"File-20190606-G6Q1C7"
]











