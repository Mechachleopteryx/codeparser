
#include "Parselet.h"

//
// Atom parselets
//

std::shared_ptr<Node> SymbolParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto SymbolSpan = TheSourceManager->getTokenSpan();
    
    
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    //
    // if we are here, then we know that Sym could bind to _
    //
    
    if (Tok == TOKEN_UNDER) {
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
        
        auto underParselet = dynamic_cast<UnderParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        return underParselet->parseContextSensitive(Sym, Ctxt);
        
    } else if (Tok == TOKEN_UNDERUNDER) {
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
        
        auto underParselet = dynamic_cast<UnderUnderParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        return underParselet->parseContextSensitive(Sym, Ctxt);
        
    } else if (Tok == TOKEN_UNDERUNDERUNDER) {
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
        
        auto underParselet = dynamic_cast<UnderUnderUnderParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        return underParselet->parseContextSensitive(Sym, Ctxt);
        
    } else if (Tok == TOKEN_UNDERDOT) {
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
        
        auto underParselet = dynamic_cast<UnderDotParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        return underParselet->parseContextSensitive(Sym, Ctxt);
    }
    
    //
    // when parsing a in a:b  then ColonFlag is false
    // when parsing b in a:b  then ColonFlag is true
    //
    // It is necessary to go to colonParselet->parse here (even though it seems non-contextSensitive)
    // because in e.g., a_*b:f[]  the b is the last node in the Times expression and needs to bind with :f[]
    // Parsing a_*b completely, and then parsing :f[] would be wrong.
    //
    if (!Ctxt.ColonFlag) {

        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {

            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));

            return colonParselet->parse(Sym, Ctxt);
        }

        return Sym;
    }
    
    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Sym = std::make_shared<SymbolNode>(Str, SymbolSpan, Issues, Comments);
    
    return Sym;
}

//
// parsing x in _x
//
// we know it can only be a symbol
//
std::shared_ptr<Node> SymbolParselet::parseContextSensitive(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<SymbolNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> IntegerParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<IntegerNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> RealParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<RealNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> StringParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();

    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<StringNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> HashParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<SlotNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> HashHashParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<SlotSequenceNode>(Str, Span, Issues, Comments);
}

std::shared_ptr<Node> PercentParselet::parse(ParserContext CtxtIn) {
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<OutNode>(Str, Span, Issues, Comments);
}




//
// Base Operators parselets
//

std::shared_ptr<Node> PrefixOperatorParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    auto Str = TheParser->getTokenString();
    
    auto TokIn = TheParser->currentToken();
    
    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);

    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    

    auto operand = TheParser->parse(Ctxt);
    
    if (Span.end.Line != operand->getSourceSpan().start.Line) {

        auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{Span.start, operand->getSourceSpan().end}));
    
        TheParser->addIssue(Issue);
    }
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<PrefixNode>(PrefixOperatorToSymbol(TokIn), Span, operand, Issues, Comments);
}

std::shared_ptr<Node> BinaryOperatorParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    auto TokIn = TheParser->currentToken();
    
    // Clear String
    auto Str = TheParser->getTokenString();
    
    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);

    auto recalculatedPrecedence = static_cast<precedence_t>(prec - (isRight() ? 1 : 0));
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = recalculatedPrecedence;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    auto Right = TheParser->parse(Ctxt);
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<BinaryNode>(BinaryOperatorToSymbol(TokIn), Left, Right, Issues, Comments);
}

std::shared_ptr<Node> InfixOperatorParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    // clear String
    auto Str = TheParser->getTokenString();

    auto TokIn = TheParser->currentToken();
    
    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);

    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    
    std::vector<std::shared_ptr<Node>> Args;
    
    Args.push_back(Left);
    
    auto breadth = 1;
    while (true) {
        
        //
        // Check isAbort() inside loops
        //
        if (TheParser->isAbort()) {
            
            return nullptr;
        }
        
        if (breadth == MAX_EXPRESSION_BREADTH) {

            auto Span = TheSourceManager->getTokenSpan();
        
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_MAXEXPRESSIONBREADTH, std::string("Max expression breadth reached. Consider breaking up into smaller expressions."), SYNTAXISSUESEVERITY_FORMATTING, Span);
        
            TheParser->addIssue(Issue);
        }

        auto Tok = TheParser->currentToken();
        
        if (Tok == TokIn) {
            
            // clear String
            auto Str = TheParser->getTokenString();
            
            TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
            
            auto operand = TheParser->parse(Ctxt);
            
            Args.push_back(operand);
            
        } else {
            break;
        }

        breadth++;
    } // while
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<InfixNode>(InfixOperatorToSymbol(TokIn), Args, Issues, Comments);
}

std::shared_ptr<Node> PostfixOperatorParselet::parse(std::shared_ptr<Node> Operand, ParserContext CtxtIn) {
    
    // Clear String
    auto Str = TheParser->getTokenString();
    
    auto TokIn = TheParser->currentToken();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    

    if (Operand->getSourceSpan().end.Line != Span.start.Line) {

        auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{Operand->getSourceSpan().start, Span.end}));
    
        TheParser->addIssue(Issue);
    }
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<PostfixNode>(PostfixOperatorToSymbol(TokIn), Span, Operand, Issues, Comments);
}




//
// Group parselets
//

std::shared_ptr<Node> GroupParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto OpenerSpan = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    Ctxt.GroupDepth++;
    Ctxt.OperatorDepth++;

    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    Token Tok;

    std::vector<std::shared_ptr<Node>> Args;
    
    auto Closer = GroupOpenerToCloser(Opener);
    
    SourceSpan CloserSpan;
    
    auto breadth = 0;
    while (true) {
        
        //
        // Check isAbort() inside loops
        //
        if (TheParser->isAbort()) {
            
            return nullptr;
        }
        
        if (breadth == MAX_EXPRESSION_BREADTH) {

            auto Span = TheSourceManager->getTokenSpan();
        
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_MAXEXPRESSIONBREADTH, std::string("Max expression breadth reached. Consider breaking up into smaller expressions."), SYNTAXISSUESEVERITY_FORMATTING, Span);
        
            TheParser->addIssue(Issue);
        }

        Tok = TheParser->currentToken();
        
        if (Tok == TOKEN_EOF) {
            
            //
            // Handle something like { a EOF
            //
            
            auto EOFSpan = TheSourceManager->getTokenSpan();

            auto Issues = TheParser->getIssues();
            auto Comments = TheParser->getComments();
            
            auto group = std::make_shared<GroupNode>(GroupOpenerToMissingCloserSymbol(Opener), OpenerSpan, EOFSpan, Args, Issues, Comments);
            
            return group;
            
        } else if (Tok == Closer) {
            
            // Clear String
            TheParser->getTokenString();
            
            CloserSpan = TheSourceManager->getTokenSpan();
            
            auto Ctxt2 = Ctxt;
            Ctxt2.GroupDepth--;
            Ctxt2.OperatorDepth--;
            
            TheParser->nextToken(Ctxt2, PRESERVE_TOPLEVEL_NEWLINES);
    
            break;
            
        } else if (Tok == TOKEN_COMMA) {
            
            //
            // Reporting of commas, e.g., {1,,2} is done later
            //
            
            auto Str = TheParser->getTokenString();
            
            auto Issues = TheParser->getIssues();
            TheParser->getComments();
            
            auto Span = TheSourceManager->getTokenSpan();
            
            TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);

            auto CommaNode = std::make_shared<TokenNode>(Tok, Str, Span, Issues);
            
            Args.push_back(CommaNode);
            
        } else {
            
            //
            // Handle the expression
            //

            auto Ctxt2 = Ctxt;
            Ctxt2.Precedence = PRECEDENCE_LOWEST;
            Ctxt2.ColonFlag = false;
            Ctxt2.LinearSyntaxFlag = false;
            auto operand = TheParser->parse(Ctxt2);
            
            Args.push_back(operand);
        }
        
        breadth++;

    } // while
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    auto group = std::make_shared<GroupNode>(GroupOpenerToSymbol(Opener), OpenerSpan, CloserSpan, Args, Issues, Comments);
    
    return group;
}


//
// Call parselets
//

CallParselet::CallParselet(Token Opener) : Opener(Opener), groupParselet(std::make_shared<GroupParselet>(Opener)) {}

std::shared_ptr<Node> CallParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    auto TokIn = TheParser->currentToken();

    // Clear String
    TheParser->getTokenString();

    //
    // if we used PRECEDENCE_CALL here, then e.g., a[]?b should technically parse as   a <call> []?b
    //
    auto prec = PRECEDENCE_HIGHEST;
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    
    auto Right = TheParser->parse(Ctxt);
    
    if (!isContiguous(Left->getSourceSpan(), Right->getSourceSpan())) {

        auto Issue = SyntaxIssue(SYNTAXISSUETAG_NOTCONTIGUOUS, "Head and brackets are not contiguous", SYNTAXISSUESEVERITY_FORMATTING,(SourceSpan{Left->getSourceSpan().end, Right->getSourceSpan().start}));
    
        TheParser->addIssue(Issue);
    }
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    if (auto GroupExpr = std::dynamic_pointer_cast<GroupNode>(Right)) {
            
        return std::make_shared<CallNode>(Left, GroupExpr, Issues, Comments);
    }
        
    return std::make_shared<SyntaxErrorNode>(TokIn, std::vector<std::shared_ptr<Node>> { Right }, Issues, Comments);
}



//
// Special parselets
//

//
// prefix
//
std::shared_ptr<Node> UnderParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Span.start, Right->getSourceSpan().end};
    }

    //
    // For something like _:""  when parsing _
    // ColonFlag == false
    // the : here is Optional, and so we want to go parse with ColonParselet's parseContextSensitive method
    //
    // For something like a:_:""  when parsing _
    // ColonFlag == true
    // make sure to not parse the second : here
    // We are already inside ColonParselet from the first :, and so ColonParselet will also handle the second :
    //
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Blank = std::make_shared<BlankNode>(Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {
            
            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));
            
            return colonParselet->parseContextSensitive(Blank, Ctxt);
        }
        
        return Blank;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Blank = std::make_shared<BlankNode>(Right, Span, Issues, Comments);
    
    return Blank;
}

//
// infix
//
std::shared_ptr<Node> UnderParselet::parseContextSensitive(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto TokenSpan = TheSourceManager->getTokenSpan();
    
    auto Span = SourceSpan{Left->getSourceSpan().start, TokenSpan.end};
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Left->getSourceSpan().start, Right->getSourceSpan().end};
    }

    //
    // For something like a:b_c:d when parsing _
    // ColonFlag == true
    //
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Pat = std::make_shared<PatternBlankNode>(Left, Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {

            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));

            return colonParselet->parseContextSensitive(Pat, Ctxt);
        }
        
        return Pat;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Pat = std::make_shared<PatternBlankNode>(Left, Right, Span, Issues, Comments);
    
    return Pat;
}

//
// prefix
//
std::shared_ptr<Node> UnderUnderParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Span.start, Right->getSourceSpan().end};
    }
    
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Blank = std::make_shared<BlankSequenceNode>(Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {
            
            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));
            
            return colonParselet->parseContextSensitive(Blank, Ctxt);
        }
        
        return Blank;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Blank = std::make_shared<BlankSequenceNode>(Right, Span, Issues, Comments);
    
    return Blank;
}

//
// infix
//
std::shared_ptr<Node> UnderUnderParselet::parseContextSensitive(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto TokenSpan = TheSourceManager->getTokenSpan();
    
    auto Span = SourceSpan{Left->getSourceSpan().start, TokenSpan.end};
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Left->getSourceSpan().start, Right->getSourceSpan().end};
    }
    
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Pat = std::make_shared<PatternBlankSequenceNode>(Left, Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {
            
            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));
            
            return colonParselet->parseContextSensitive(Pat, Ctxt);
        }
        
        return Pat;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Pat = std::make_shared<PatternBlankSequenceNode>(Left, Right, Span, Issues, Comments);
    
    return Pat;
}

//
// prefix
//
std::shared_ptr<Node> UnderUnderUnderParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Span.start, Right->getSourceSpan().end};
    }
    
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Blank = std::make_shared<BlankNullSequenceNode>(Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {
            
            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));
            
            return colonParselet->parseContextSensitive(Blank, Ctxt);
        }
        
        return Blank;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Blank = std::make_shared<BlankNullSequenceNode>(Right, Span, Issues, Comments);
    
    return Blank;
}

//
// infix
//
std::shared_ptr<Node> UnderUnderUnderParselet::parseContextSensitive(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto TokenSpan = TheSourceManager->getTokenSpan();
    
    auto Span = SourceSpan{Left->getSourceSpan().start, TokenSpan.end};
    
    auto Ctxt = CtxtIn;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    std::shared_ptr<Node> Right = nullptr;
    if (Tok == TOKEN_SYMBOL) {
        
        auto symbolParselet = dynamic_cast<SymbolParselet*>(TheParser->findContextSensitiveParselet(Tok));
        
        Right = symbolParselet->parseContextSensitive(Ctxt);
        
        Span = SourceSpan{Left->getSourceSpan().start, Right->getSourceSpan().end};
    }
    
    if (!Ctxt.ColonFlag) {
        
        Tok = TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        
        auto Comments = TheParser->getComments();
        
        auto Pat = std::make_shared<PatternBlankNullSequenceNode>(Left, Right, Span, Issues, Comments);
        
        if (Tok == TOKEN_COLON) {
            
            auto colonParselet = dynamic_cast<ColonParselet*>(TheParser->findContextSensitiveParselet(Tok));
            
            return colonParselet->parseContextSensitive(Pat, Ctxt);
        }
        
        return Pat;
    }

    TheParser->tryNextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    auto Pat = std::make_shared<PatternBlankNullSequenceNode>(Left, Right, Span, Issues, Comments);
    
    return Pat;
}

//
// prefix
//
std::shared_ptr<Node> UnderDotParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<OptionalDefaultNode>(Span, Issues, Comments);
}

//
// postfix
//
std::shared_ptr<Node> UnderDotParselet::parseContextSensitive(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    auto TokenSpan = TheSourceManager->getTokenSpan();
    
    auto Span = SourceSpan{Left->getSourceSpan().start, TokenSpan.end};
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    auto Issues = TheParser->getIssues();
    
    auto Comments = TheParser->getComments();
    
    return std::make_shared<OptionalDefaultPatternNode>(Left, Span, Issues, Comments);
}


std::shared_ptr<Node> SemiParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    // // Clear String
    auto Str = TheParser->getTokenString();

    auto lastSpan = TheSourceManager->getTokenSpan();

    auto operand = Left;

    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);

    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;

    TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    Token Tok;

    std::vector<std::shared_ptr<Node>> Args;
    
    Args.push_back(Left);
    
    auto eatTheNextSemi = false;
    auto lastWasSemi = true;

    auto breadth = 1;
    while (true) {
        
        //
        // Check isAbort() inside loops
        //
        if (TheParser->isAbort()) {
            
            return nullptr;
        }
        
        if (breadth == MAX_EXPRESSION_BREADTH) {

            auto Span = TheSourceManager->getTokenSpan();
        
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_MAXEXPRESSIONBREADTH, std::string("Max expression breadth reached. Consider breaking up into smaller expressions."), SYNTAXISSUESEVERITY_FORMATTING, Span);
        
            TheParser->addIssue(Issue);
        }
        
        Tok = TheParser->currentToken();

        if (Tok == TOKEN_NEWLINE) {
            
            // Clear String
            TheParser->getTokenString();
            
            if (lastWasSemi) {

                auto Empty = std::make_shared<InternalNullNode>(lastSpan);

                Args.push_back(Empty);
            }

            break;

        } else if (Tok == TOKEN_SEMI) {
            
            //
            // something like a; ; parses as CompoundExpression[a, Null, Null]
            //
            
            if (!eatTheNextSemi) {

                auto Empty = std::make_shared<InternalNullNode>(lastSpan);
                
                Args.push_back(Empty);
            }
            
            // Clear String
            Str = TheParser->getTokenString();
            
            eatTheNextSemi = false;
            lastWasSemi = true;
            lastSpan = TheSourceManager->getTokenSpan();

            if (operand->getSourceSpan().end.Line != lastSpan.start.Line) {
                
                auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{operand->getSourceSpan().end, lastSpan.start}));
            
                TheParser->addIssue(Issue);
            }

            TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
            
        } else if (!TheParser->isPossibleBeginningOfExpression(Tok)) {
            
            //
            // A InfixNode[CompoundExpression, ...] may or may not end with a ;
            // Need to test the next token to decide
            //
            
            if (lastWasSemi) {
                
                auto Empty = std::make_shared<InternalNullNode>(lastSpan);
                
                Args.push_back(Empty);
            }
            
            break;
            
        } else {
            
            //
            // Parse the expression
            //

            operand = TheParser->parse(Ctxt);

            Args.push_back(operand);
            
            eatTheNextSemi = true;
            lastWasSemi = false;
        }

        breadth++;

    } // while
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<InfixNode>(SYMBOL_COMPOUNDEXPRESSION, Args, Issues, Comments);
}

//
// prefix
// ;;a
//
std::shared_ptr<Node> SemiSemiParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    auto Str = TheParser->getTokenString();
    
    auto PrefixSpan = TheSourceManager->getTokenSpan();
    
    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);
    auto recalculatedPrecedence = static_cast<precedence_t>(prec - (isRight() ? 1 : 0));
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = recalculatedPrecedence;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    if (TheParser->isPossibleBeginningOfExpression(Tok)) {
        
        auto operand = TheParser->parse(Ctxt);
        

        if (PrefixSpan.end.Line != operand->getSourceSpan().start.Line) {
                
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{PrefixSpan.start, operand->getSourceSpan().end}));
        
            TheParser->addIssue(Issue);
        }

        auto Issues = TheParser->getIssues();
        auto Comments = TheParser->getComments();
        
        if (auto BinOp = std::dynamic_pointer_cast<BinaryNode>(operand)) {
            
            if (BinOp->getOperator() == SYMBOL_SPAN) {
                
                auto SpanOpSource = BinOp->getSourceSpan();
                auto SpanOpLeft = BinOp->getLeft();
                auto SpanOpRight = BinOp->getRight();
                
                if (auto SpanOpRightEmpty = std::dynamic_pointer_cast<InternalAllNode>(SpanOpRight)) {
                    
                    //
                    // This is ;;;; or ;;a;; and is not a single Span expression
                    //
                    
                    std::shared_ptr<Node> NewLeft;
                    if (std::dynamic_pointer_cast<InternalOneNode>(SpanOpLeft)) {
                        NewLeft = std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), std::make_shared<InternalAllNode>(PrefixSpan), std::vector<SyntaxIssue>(), std::vector<Comment>());
                    } else {
                        NewLeft = std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), SpanOpLeft, std::vector<SyntaxIssue>(), std::vector<Comment>());
                    }
                    
                    auto NewRight = std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(SpanOpSource), std::make_shared<InternalAllNode>(SpanOpSource), std::vector<SyntaxIssue>(), std::vector<Comment>());
                    
                    return std::make_shared<InfixNode>(SYMBOL_IMPLICITTIMES, std::vector<std::shared_ptr<Node>>{NewLeft, NewRight}, Issues, Comments);
                }
                
                //
                // We know it is ternary Span
                //
                
                if (std::dynamic_pointer_cast<InternalOneNode>(SpanOpLeft)) {
                    return std::make_shared<TernaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), std::make_shared<InternalAllNode>(PrefixSpan), SpanOpRight, Issues, Comments);
                } else {
                    return std::make_shared<TernaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), SpanOpLeft, SpanOpRight, Issues, Comments);
                }
            }
        }
        
        return std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), operand, Issues, Comments);
    }
        
    return std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(PrefixSpan), std::make_shared<InternalAllNode>(PrefixSpan), std::vector<SyntaxIssue>(), std::vector<Comment>());
}

//
// infix
// a;;b
//
std::shared_ptr<Node> SemiSemiParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    // Clear String
    auto Span1Str = TheParser->getTokenString();
    
    auto Span1Span = TheSourceManager->getTokenSpan();
    
    if (Left->getSourceSpan().end.Line != Span1Span.start.Line) {

        auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Span1Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{Left->getSourceSpan().start, Span1Span.end}));
    
        TheParser->addIssue(Issue);
    }

    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);
    auto recalculatedPrecedence = static_cast<precedence_t>(prec - (isRight() ? 1 : 0));
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = recalculatedPrecedence;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
    
    if (TheParser->isPossibleBeginningOfExpression(Tok)) {
        
        auto Right = TheParser->parse(Ctxt);
        
        if (Span1Span.end.Line != Right->getSourceSpan().start.Line) {
                
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, Span1Str + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{Span1Span.start, Right->getSourceSpan().end}));
        
            TheParser->addIssue(Issue);
        }

        auto Issues = TheParser->getIssues();
        auto Comments = TheParser->getComments();
        
        if (auto BinLeft = std::dynamic_pointer_cast<BinaryNode>(Left)) {
            
            if (BinLeft->getOperator() == SYMBOL_SPAN) {
                
                auto LeftSpanLeft = BinLeft->getLeft();
                auto LeftSpanRight = BinLeft->getRight();
                
                return std::make_shared<TernaryNode>(SYMBOL_SPAN, LeftSpanLeft, LeftSpanRight, Right, Issues, Comments);
            }
        }
        
        if (auto BinRight = std::dynamic_pointer_cast<BinaryNode>(Right)) {
            
            if (BinRight->getOperator() == SYMBOL_SPAN) {
                
                auto RightSpanSource = BinRight->getSourceSpan();
                auto RightSpanLeft = BinRight->getLeft();
                auto RightSpanRight = BinRight->getRight();
                
                if (auto RightSpanRightEmpty = std::dynamic_pointer_cast<InternalAllNode>(RightSpanRight)) {
                    
                    //
                    // This is a;;;; or a;;b;; and is not a single Span expression
                    //
                    
                    std::shared_ptr<Node> NewLeft;
                    if (std::dynamic_pointer_cast<InternalOneNode>(RightSpanLeft)) {
                        NewLeft = std::make_shared<BinaryNode>(SYMBOL_SPAN, Left, std::make_shared<InternalAllNode>(RightSpanSource), std::vector<SyntaxIssue>(), std::vector<Comment>());
                    } else {
                        NewLeft = std::make_shared<BinaryNode>(SYMBOL_SPAN, Left, RightSpanLeft, std::vector<SyntaxIssue>(), std::vector<Comment>());
                    }
                    
                    auto NewRight = std::make_shared<BinaryNode>(SYMBOL_SPAN, std::make_shared<InternalOneNode>(RightSpanSource), std::make_shared<InternalAllNode>(RightSpanSource), std::vector<SyntaxIssue>(), std::vector<Comment>());
                    
                    return std::make_shared<InfixNode>(SYMBOL_IMPLICITTIMES, std::vector<std::shared_ptr<Node>>{NewLeft, NewRight}, Issues, Comments);
                }
                
                
                //
                // We know it is ternary Span
                //
                
                if (std::dynamic_pointer_cast<InternalOneNode>(RightSpanLeft)) {
                    return std::make_shared<TernaryNode>(SYMBOL_SPAN, Left, std::make_shared<InternalAllNode>(RightSpanSource), RightSpanRight, Issues, Comments);
                } else {
                    return std::make_shared<TernaryNode>(SYMBOL_SPAN, Left, RightSpanLeft, RightSpanRight, Issues, Comments);
                }
            }
        }
        
        return std::make_shared<BinaryNode>(SYMBOL_SPAN, Left, Right, Issues, Comments);
    }
    
    //
    // Not possible beginning of expression
    //
    
    return std::make_shared<BinaryNode>(SYMBOL_SPAN, Left, std::make_shared<InternalAllNode>(Span1Span), std::vector<SyntaxIssue>(), std::vector<Comment>());
}




std::shared_ptr<Node> TildeParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    auto FirstTildeStr = TheParser->getTokenString();
    
    auto FirstTildeSpan = TheSourceManager->getTokenSpan();

    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);

    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    auto Middle = TheParser->parse(Ctxt);

    auto Tok = TheParser->currentToken();
    
    auto SecondTildeStr = TheParser->getTokenString();
    
    auto SecondTildeSpan = TheSourceManager->getTokenSpan();


    if (FirstTildeSpan.end.Line != Middle->getSourceSpan().start.Line) {
                
        auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, FirstTildeStr + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{FirstTildeSpan.start, Middle->getSourceSpan().end}));
    
        TheParser->addIssue(Issue);
    }

    if (Middle->getSourceSpan().end.Line != SecondTildeSpan.start.Line) {
        
        auto Issue = SyntaxIssue(SYNTAXISSUETAG_DIFFERENTLINE, SecondTildeStr + " is not on same line as operand", SYNTAXISSUESEVERITY_REMARK, (SourceSpan{Middle->getSourceSpan().start, SecondTildeSpan.end}));
        
        TheParser->addIssue(Issue);
    }
    
    
    if (Tok != TOKEN_TILDE) {
        
        TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);

        auto Issues = TheParser->getIssues();
        auto Comments = TheParser->getComments();
        
        return std::make_shared<SyntaxErrorNode>(TOKEN_ERROR_EXPECTEDTILDE,
                                                 std::vector<std::shared_ptr<Node>> { Left, std::make_shared<TokenNode>(TOKEN_TILDE, FirstTildeStr, FirstTildeSpan, std::vector<SyntaxIssue>()),
                Middle, std::make_shared<TokenNode>(Tok, SecondTildeStr, SecondTildeSpan, std::vector<SyntaxIssue>()) }, Issues, Comments);
    }
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);

    auto Right = TheParser->parse(Ctxt);

    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<TernaryNode>(SYMBOL_TERNARYTILDE, Left, Middle, Right, Issues, Comments);
}



//
// symbol:object
//
// when parsing a in a:b  then ColonFlag is false
// when parsing b in a:b  then ColonFlag is true
//
std::shared_ptr<Node> ColonParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    assert(!CtxtIn.ColonFlag);
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto prec = PRECEDENCE_FAKE_PATTERNCOLON;
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    Ctxt.ColonFlag = true;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    auto Right = TheParser->parse(Ctxt);
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    if (!std::dynamic_pointer_cast<SymbolNode>(Left)) {
        
        return std::make_shared<SyntaxErrorNode>(TOKEN_ERROR_EXPECTEDSYMBOL, std::vector<std::shared_ptr<Node>> { Left, std::make_shared<TokenNode>(TOKEN_COLON, Str, Span, std::vector<SyntaxIssue>()), Right }, Issues, Comments);
    }

    auto Pat = std::make_shared<BinaryNode>(SYMBOL_PATTERN, Left, Right, Issues, Comments);
    
    auto Tok = TheParser->currentToken();

    if (Tok == TOKEN_COLON) {

        Ctxt.ColonFlag = false;
        return parseContextSensitive(Pat, Ctxt);
    }

    return Pat;
}

//
// pattern:optional
//
std::shared_ptr<Node> ColonParselet::parseContextSensitive(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    //
    // when parsing a in a:b  then ColonFlag is false
    // when parsing b in a:b  then ColonFlag is true
    //
    assert(!CtxtIn.ColonFlag);

    // Clear String
    auto Str = TheParser->getTokenString();

    auto prec = PRECEDENCE_FAKE_OPTIONALCOLON;
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = prec;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    auto Right = TheParser->parse(Ctxt);
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();

    return std::make_shared<BinaryNode>(SYMBOL_OPTIONAL, Left, Right, Issues, Comments);
}



std::shared_ptr<Node> SlashColonParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    // Clear String
    auto Str = TheParser->getTokenString();

    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);
    auto recalculatedPrecedence = static_cast<precedence_t>(prec - 1);
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = recalculatedPrecedence;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    auto Middle = TheParser->parse(Ctxt);

    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    if (auto BinaryMiddle = std::dynamic_pointer_cast<BinaryNode>(Middle)) {
        
        if (BinaryMiddle->getOperator() == SYMBOL_SET) {
            
            return std::make_shared<TernaryNode>(SYMBOL_TAGSET, Left, BinaryMiddle->getLeft(), BinaryMiddle->getRight(), Issues, Comments);
            
        } else if (BinaryMiddle->getOperator() == SYMBOL_SETDELAYED) {
            
            return std::make_shared<TernaryNode>(SYMBOL_TAGSETDELAYED, Left, BinaryMiddle->getLeft(), BinaryMiddle->getRight(), Issues, Comments);
            
        } else if (BinaryMiddle->getOperator() == SYMBOL_UNSET) {
            
            return std::make_shared<TernaryNode>(SYMBOL_TAGUNSET, Left, BinaryMiddle->getLeft(), BinaryMiddle->getRight(), Issues, Comments);
        }
    }

    //
    // Anything other than:
    // a /: b = c
    // a /: b := c
    // a /: b =.
    //

    return std::make_shared<SyntaxErrorNode>(TOKEN_ERROR_EXPECTEDSET, std::vector<std::shared_ptr<Node>> { Middle }, Issues, Comments);
}




std::shared_ptr<Node> LinearSyntaxOpenParenParselet::parse(ParserContext CtxtIn) {
    
    // Clear String
    TheParser->getTokenString();
    
    std::vector<std::shared_ptr<Node>> Tokens;
    
    auto OpenerSpan = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    Ctxt.GroupDepth++;
    Ctxt.OperatorDepth++;
    Ctxt.LinearSyntaxFlag = true;
    
    auto Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    SourceSpan CloserSpan;
    
    auto breadth = 0;
    while (true) {
        
        //
        // Check isAbort() inside loops
        //
        if (TheParser->isAbort()) {
            
            return nullptr;
        }
        
        if (breadth == MAX_EXPRESSION_BREADTH) {

            auto Span = TheSourceManager->getTokenSpan();
        
            auto Issue = SyntaxIssue(SYNTAXISSUETAG_MAXEXPRESSIONBREADTH, std::string("Max expression breadth reached. Consider breaking up into smaller expressions."), SYNTAXISSUESEVERITY_FORMATTING, Span);
        
            TheParser->addIssue(Issue);
        }

        if (Tok == TOKEN_EOF) {
            
            //
            // Handle something like \( a EOF
            //
            
            auto EOFSpan = TheSourceManager->getTokenSpan();
            
            auto Issues = TheParser->getIssues();
            auto Comments = TheParser->getComments();
            
            auto group = std::make_shared<GroupNode>(SYMBOL_GROUPMISSINGCLOSERLINEARSYNTAXPAREN, OpenerSpan, EOFSpan, Tokens, Issues, Comments);
            
            return group;
            
        } else if (Tok == TOKEN_LINEARSYNTAX_OPENPAREN) {
            
            auto Sub = this->parse(Ctxt);
            
            if (auto SubOpenParen = std::dynamic_pointer_cast<GroupNode>(Sub)) {
                
                if (SubOpenParen->getOperator() == SYMBOL_GROUPLINEARSYNTAXPAREN ||
                    SubOpenParen->getOperator() == SYMBOL_GROUPMISSINGCLOSERLINEARSYNTAXPAREN) {
                    
                    Tokens.push_back(SubOpenParen);
                    
                    Tok = TheParser->currentToken();
                    
                } else {
                    assert(false);
                }
                
            } else {
                assert(false);
            }
            
        } else if (Tok == TOKEN_LINEARSYNTAX_CLOSEPAREN) {
            
            // Clear String
            TheParser->getTokenString();
            
            CloserSpan = TheSourceManager->getTokenSpan();
            
            auto Ctxt2 = Ctxt;
            Ctxt2.GroupDepth--;
            Ctxt2.OperatorDepth--;
            
            TheParser->nextToken(Ctxt2, PRESERVE_TOPLEVEL_NEWLINES);
            
            break;
            
        } else {
            
            //
            // Comments are handled here
            //
            
            auto Str = TheParser->getTokenString();
            
            auto Span = TheSourceManager->getTokenSpan();
            
            Tokens.push_back(std::make_shared<TokenNode>(Tok, Str, Span, std::vector<SyntaxIssue>()));
            
            Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
        }

        breadth++;

    } // while
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<GroupNode>(SYMBOL_GROUPLINEARSYNTAXPAREN, OpenerSpan, CloserSpan, Tokens, Issues, Comments);
}

std::shared_ptr<Node> EqualParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {

    // Clear String
    auto Str = TheParser->getTokenString();

    auto prec = getPrecedence();
    assert(prec != PRECEDENCE_UNUSED);
    auto recalculatedPrecedence = static_cast<precedence_t>(prec - (isRight() ? 1 : 0));
    
    auto Ctxt = CtxtIn;
    Ctxt.OperatorDepth++;
    Ctxt.Precedence = recalculatedPrecedence;
    
    auto EqualSpan = TheSourceManager->getTokenSpan();
    
    auto Tok = TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    if (Tok == TOKEN_DOT) {
        
        auto DotSpan = TheSourceManager->getTokenSpan();

        if (!isContiguous(EqualSpan, DotSpan)) {

            //
            // Something like a =  .
            //
            // tutorial/OperatorInputForms
            // Spaces to Avoid
            //

             auto Issue = SyntaxIssue(SYNTAXISSUETAG_NOTCONTIGUOUS, std::string("= and . are not contiguous"), SYNTAXISSUESEVERITY_FORMATTING, (SourceSpan{EqualSpan.start, DotSpan.end}));
        
             TheParser->addIssue(Issue);
         }

        // Clear String
        TheParser->getTokenString();
        
        TheParser->nextToken(Ctxt, PRESERVE_TOPLEVEL_NEWLINES);
        
        auto Issues = TheParser->getIssues();
        auto Comments = TheParser->getComments();
        
        auto Empty = std::make_shared<InternalDotNode>(DotSpan);
        
        return std::make_shared<BinaryNode>(SYMBOL_UNSET, Left, Empty, Issues, Comments);
    }
    
    auto Right = TheParser->parse(Ctxt);
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    return std::make_shared<BinaryNode>(SYMBOL_SET, Left, Right, Issues, Comments);
}







//
// Error handling and Cleanup
//

std::shared_ptr<Node> ErrorParselet::parse(ParserContext CtxtIn) {
    
    auto TokIn = TheParser->currentToken();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    auto Str = TheParser->getTokenString();
    
    auto Ctxt = CtxtIn;
    
    TheParser->nextToken(Ctxt, DISCARD_EVERYTHING);
    
    return std::make_shared<SyntaxErrorNode>(TokIn, std::vector<std::shared_ptr<Node>> { std::make_shared<TokenNode>(TokIn, Str, Span, std::vector<SyntaxIssue>()) }, Issues, Comments);
}

std::shared_ptr<Node> CleanupRestParselet::parse(std::shared_ptr<Node> Left, ParserContext CtxtIn) {
    
    auto Tok = TheParser->currentToken();
    
    auto Str = TheParser->getTokenString();
    
    auto Span = TheSourceManager->getTokenSpan();
    
    auto Ctxt = CtxtIn;
    
    std::vector<std::shared_ptr<Node>> Tokens;
    
    Tokens.push_back(Left);
    Tokens.push_back(std::make_shared<TokenNode>(Tok, Str, Span, std::vector<SyntaxIssue>()));
    
    Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
    
    //
    // do not keep track of breadth here, not a big deal
    //
    while (true) {
        
        //
        // Check isAbort() inside loops
        //
        if (TheParser->isAbort()) {
            
            return nullptr;
        }

        if (Tok == TOKEN_EOF) {
            
            break;
            
        } else {
            
            //
            // Comments are handled here
            //
            
            Str = TheParser->getTokenString();
            
            Span = TheSourceManager->getTokenSpan();
            
            Tokens.push_back(std::make_shared<TokenNode>(Tok, Str, Span, std::vector<SyntaxIssue>()));
            
            Tok = TheParser->nextToken(Ctxt, PRESERVE_EVERYTHING);
        }
    } // while

    auto Issues = TheParser->getIssues();
    auto Comments = TheParser->getComments();
    
    auto group = std::make_shared<SyntaxErrorNode>(TOKEN_ERROR_REST, Tokens, Issues, Comments);

    return group;
}