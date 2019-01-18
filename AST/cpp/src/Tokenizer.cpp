
#include "Tokenizer.h"

#include "CharacterDecoder.h"
#include "CodePoint.h"
#include "LongNameMap.h"

#include <iomanip>
#include <cassert>

Tokenizer::Tokenizer() : _stringifyNextToken(false), cur(TOKEN_UNKNOWN), currentCached(false), characterQueue(),
    _currentWLCharacter(0), _currentSourceLocation{0, 0}, String(), Issues() {}

void Tokenizer::init(bool skipFirstLine) {

    auto c = nextWLCharacter();

    if (skipFirstLine) {
        while (true) {
            if (c == '\n') {
                c = nextWLCharacter();
                break;
            } else if (c == WLCHARACTER_EOF) {
                c = nextWLCharacter();
                break;
            } else {
                c = nextWLCharacter();
            }
        }
    }
}

Token Tokenizer::nextToken() {
    
    assert(String.str().empty());
    
    TheSourceManager->setTokenStart();
    
    if (_stringifyNextToken) {
        
        cur = handleString();
        
        TheSourceManager->setTokenEnd();
        
        return cur;
    }
    
    auto c = currentWLCharacter();

    switch (c) {
        case 'a': case 'A': case 'b': case 'B': case 'c': case 'C': case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
        case 'g': case 'G': case 'h': case 'H': case 'i': case 'I': case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
        case 'm': case 'M': case 'n': case 'N': case 'o': case 'O': case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
        case 's': case 'S': case 't': case 'T': case 'u': case 'U': case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
        case 'y': case 'Y': case 'z': case 'Z':
            
            cur = handleSymbol();
            break;
            
        case '$': case '`':
            
            cur = handleSymbol();
            break;
            
        case '"':
            
            cur = handleString();
            break;
            
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            
            cur = handleNumber();
            break;
            
        case '\n':
            
            c = nextWLCharacter();
            
            cur = TOKEN_NEWLINE;
            break;
            
        case ' ': case '\t': case '\r':
            
            while (c == ' ' || c == '\t' || c == '\r') {

                String.put(c);
                
                c = nextWLCharacter();
            }
            
            cur = TOKEN_SPACE;
            break;
            
        case '.':
            
            cur = handleDot();
            break;
            
        case '(': case ')': case '[': case ']': case ',': case '{': case '}': case ':': case '=': case '_': case '<': case '>':
        case '-': case '|': case ';': case '!': case '#': case '&': case '/': case '@': case '+': case '~': case '?': case '*':
        case '^': case '%': case '\'':
            
            cur = handleOperator();
            break;
        
        case WLCHARACTER_LINEARSYNTAX_BANG: case WLCHARACTER_LINEARSYNTAX_PERCENT: case WLCHARACTER_LINEARSYNTAX_AMP:
        case WLCHARACTER_LINEARSYNTAX_OPENPAREN: case WLCHARACTER_LINEARSYNTAX_CLOSEPAREN: case WLCHARACTER_LINEARSYNTAX_STAR:
        case WLCHARACTER_LINEARSYNTAX_PLUS: case WLCHARACTER_LINEARSYNTAX_SLASH: case WLCHARACTER_LINEARSYNTAX_AT:
        case WLCHARACTER_LINEARSYNTAX_CARET: case WLCHARACTER_LINEARSYNTAX_UNDER: case WLCHARACTER_LINEARSYNTAX_BACKTICK:
        case WLCHARACTER_LINEARSYNTAX_SPACE:

            cur = handleLinearSyntax();
            break;
            
        case WLCHARACTER_EOF:
            
            TheSourceManager->setEOF();
            
            cur = TOKEN_EOF;
            break;
            
        default: {
            
            //
            // Everything else involving Unicode or errors
            //
            
            if (isLetterlikeCodePoint(c)) {
                
                cur = handleSymbol();
                break;
                
            } else if (isSpaceCodePoint(c)) {
                
                String << WLCharacterToString(c);
                
                c = nextWLCharacter();
                
                cur = TOKEN_SPACE;
                break;
                
            } else if (isNewlineCodePoint(c)) {
                
                String << WLCharacterToString(c);
                
                c = nextWLCharacter();
                
                cur = TOKEN_NEWLINE;
                break;
                
            } else if (isCommaCodePoint(c)) {
                
                String << WLCharacterToString(c);
                
                c = nextWLCharacter();
                
                cur = TOKEN_OPERATOR_COMMA;
                break;
                
            } else if (isOperatorCodePoint(c)) {
                
                cur = handleOperator();
                break;
                
            } else {
                
                String << WLCharacterToString(c);
                
                // Clear Issues
                auto Tmp = TheCharacterDecoder->getIssues();
                
                std::copy(Tmp.begin(), Tmp.end(), std::back_inserter(Issues));
                
                c = nextWLCharacter();

                cur = TOKEN_ERROR_UNHANDLEDCHARACTER;
                break;
            }
        }
    }
    
    TheSourceManager->setTokenEnd();
    
    return cur;
}

WLCharacter Tokenizer::nextWLCharacter(NextCharacterPolicy policy) {
    
    currentCached = false;
    
    if (!characterQueue.empty()) {
        
        auto p = characterQueue[0];
        
        auto c = p.first;
        
        // erase first
        characterQueue.erase(characterQueue.begin());
        
        return c;
    }
    
    return TheCharacterDecoder->nextWLCharacter(policy);
}

WLCharacter Tokenizer::currentWLCharacter() {

    if (currentCached) {
        
        return _currentWLCharacter;
    }

   auto c = TheCharacterDecoder->currentWLCharacter();

   return c;
}

void Tokenizer::setCurrentWLCharacter(WLCharacter current, SourceLocation Loc) {
    
    _currentWLCharacter = current;
    _currentSourceLocation = Loc;
    currentCached = true;
}

Token Tokenizer::currentToken() {
    
    assert(cur != TOKEN_UNKNOWN);
    
    return cur;
}

Token Tokenizer::handleLinearSyntax() {
    
    auto c = currentWLCharacter();

    String << WLCharacterToString(c);
    
    Token Operator;
    
    switch (c) {
        case WLCHARACTER_LINEARSYNTAX_BANG:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_BANG;
            break;
        case WLCHARACTER_LINEARSYNTAX_OPENPAREN:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_OPENPAREN;
            break;
        case WLCHARACTER_LINEARSYNTAX_STAR:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_STAR;
            break;
        case WLCHARACTER_LINEARSYNTAX_CLOSEPAREN:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_CLOSEPAREN;
            break;
        case WLCHARACTER_LINEARSYNTAX_AT:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_AT;
            break;
        case WLCHARACTER_LINEARSYNTAX_CARET:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_CARET;
            break;
        case WLCHARACTER_LINEARSYNTAX_UNDER:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_UNDER;
            break;
        case WLCHARACTER_LINEARSYNTAX_PERCENT:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_PERCENT;
            break;
        case WLCHARACTER_LINEARSYNTAX_AMP:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_AMP;
            break;
        case WLCHARACTER_LINEARSYNTAX_SLASH:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_SLASH;
            break;
        case WLCHARACTER_LINEARSYNTAX_PLUS:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_PLUS;
            break;
        case WLCHARACTER_LINEARSYNTAX_BACKTICK:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_BACKTICK;
            break;
        case WLCHARACTER_LINEARSYNTAX_SPACE:
            Operator = TOKEN_OPERATOR_LINEARSYNTAX_SPACE;
            break;
        default:
            //
            // Something like \\ or \" or \space is being used outside of a string
            //
            Operator = TOKEN_ERROR_UNHANDLEDCHARACTER;
            break;
    }
    
    c = nextWLCharacter();

    cur = Operator;
    
    return cur;
}

Token Tokenizer::handleComment() {
    // comment is already started
    
    auto c = currentWLCharacter();

    assert(c == '*');
    
    String.put('*');
    
    auto depth = 1;

    c = nextWLCharacter(INSIDE_COMMENT);
    
    if (c == WLCHARACTER_EOF) {
        return TOKEN_ERROR_UNTERMINATEDCOMMENT;
    }
    
    while (true) {
        
        if (c == '(') {
            
            String.put(c);
            
            c = nextWLCharacter(INSIDE_COMMENT);
            
            if (c == WLCHARACTER_EOF) {
                return TOKEN_ERROR_UNTERMINATEDCOMMENT;
            }
            
            if (c == '*') {
                
                String.put(c);
                
                c = nextWLCharacter(INSIDE_COMMENT);
                
                if (c == WLCHARACTER_EOF) {
                    return TOKEN_ERROR_UNTERMINATEDCOMMENT;
                }
                
                depth = depth + 1;
            }
            
        } else if (c == '*') {
            
            String.put(c);

            c = nextWLCharacter(INSIDE_COMMENT);
            
            if (c == WLCHARACTER_EOF) {
                return TOKEN_ERROR_UNTERMINATEDCOMMENT;
            }
            
            if (c == ')') {
                
                String.put(c);

                // This comment is closing
                
                depth = depth - 1;
                
                if (depth == 0) {
                    
                    // Leaving comments, make sure to grab next character
                    
                    c = nextWLCharacter();
                    
                    break;
                    
                } else {
                    
                    c = nextWLCharacter(INSIDE_COMMENT);
                    
                    if (c == WLCHARACTER_EOF) {
                        return TOKEN_ERROR_UNTERMINATEDCOMMENT;
                    }
                }
            }
            
        } else {
            
            String.put(c);
            
            // Clear Issues
            // We do not care about issues inside of comments
            TheCharacterDecoder->getIssues();
            
            c = nextWLCharacter(INSIDE_COMMENT);
            
            if (c == WLCHARACTER_EOF) {
                return TOKEN_ERROR_UNTERMINATEDCOMMENT;
            }
        }
        
    } // while

    return TOKEN_COMMENT;
}

//
// a segment is: [a-z$]([a-z]$[0-9])*
// a symbol is: (segment)?(`segment)*
//
Token Tokenizer::handleSymbol() {
    
    auto c = currentWLCharacter();

    assert(c == '`' || isAlphaOrDollar(c) || isLetterlikeCodePoint(c));
    
    if (isAlphaOrDollar(c) || isLetterlikeCodePoint(c)) {
        handleSymbolSegment();
    }
    
    c = currentWLCharacter();
    
    while (c == '`') {
        
        String.put(c);
        
        c = nextWLCharacter();
        
        if (isAlphaOrDollar(c) || isLetterlikeCodePoint(c)) {
            handleSymbolSegment();
        } else {
            
            cur = TOKEN_ERROR_UNHANDLEDCHARACTER;
            return cur;
        }
        
        c = currentWLCharacter();
        
    } // while
    
    return TOKEN_SYMBOL;
}

void Tokenizer::handleSymbolSegment() {
    
    auto c = currentWLCharacter();

    assert(isAlphaOrDollar(c) || isLetterlikeCodePoint(c));
    
    if (isAlphaOrDollar(c)) {
        
        String.put(c);
        
        c = nextWLCharacter();
        
    } else {
        
        if (isStrangeLetterlikeCodePoint(c)) {
            
            auto Span = TheSourceManager->getWLCharacterSpan();
            
            auto Issue = SyntaxIssue(TAG_STRANGECHARACTER, "Strange character in symbol: " + WLCharacterToString(c), SEVERITY_ERROR, Span);

            Issues.push_back(Issue);
        }
        
        String << WLCharacterToString(c);
        
        c = nextWLCharacter();
    }
    
    while (true) {
        
        if (isDigitOrAlphaOrDollar(c)) {
            
            String.put(c);
            
            c = nextWLCharacter();
            
        } else if (isLetterlikeCodePoint(c)) {
            
            if (isStrangeLetterlikeCodePoint(c)) {
                
                auto Span = TheSourceManager->getWLCharacterSpan();
                
                auto Issue = SyntaxIssue(TAG_STRANGECHARACTER, "Strange character in symbol: " + WLCharacterToString(c), SEVERITY_ERROR, Span);

                Issues.push_back(Issue);
            }
            
            String << WLCharacterToString(c);
            
            c = nextWLCharacter();
            
        } else {
            break;
        }
        
    } // while
}

Token Tokenizer::handleString() {
    
    auto c = currentWLCharacter();

    assert(c == '"' || _stringifyNextToken);
    
    // first eat the whitespace
    while (c == ' ' || c == '\t' || c == '\n') {
        c = nextWLCharacter(INSIDE_STRING);
    }
    
    if (_stringifyNextToken && c != '"') {
        
        //
        // magically turn into a string
        //
        
        auto empty = true;
        while (true) {
            
            if (isAlpha(c) || isDigit(c) || c == '`' || c == '$' || c == '.' || c == '_' || c == '/') {
                
                empty = false;

                String.put(c);
                
                c = nextWLCharacter(INSIDE_STRING);
                
            } else {
                
                break;
            }
            
        } // while
        
        _stringifyNextToken = false;
        
        if (empty) {

            //
            // Something like a::EOF
            //

            return TOKEN_ERROR_EMPTYSTRING;
        }

        return TOKEN_STRING;
        
    } else {
        
        String.put('"');

        while (true) {
            
            auto Tmp = TheCharacterDecoder->getIssues();
            
            std::copy(Tmp.begin(), Tmp.end(), std::back_inserter(Issues));
            
            c = nextWLCharacter(INSIDE_STRING);
            
            if (c == WLCHARACTER_EOF) {
                
                return TOKEN_ERROR_UNTERMINATEDSTRING;
                
            } else if (c == '"') {
                
                break;
                
            } else if (c == '\\') {
                
                String.put(c);
                
                auto Tmp = TheCharacterDecoder->getIssues();
                
                std::copy(Tmp.begin(), Tmp.end(), std::back_inserter(Issues));
                
                c = nextWLCharacter(INSIDE_STRING);
                
                String.put(c);
                
            } else if (c == '\r') {

                //
                // Skip \r in strings
                //
                ;

            } else {

                // too noisy
                // if (c == '\n') {

                //     auto Span = TheSourceManager->getWLCharacterSpan();

                //     auto CurrentLineNumber = Span.start.Line;

                //     auto LineWidth = TheSourceManager->getCurrentLineWidth();

                //     auto Issue = SyntaxIssue(TAG_NEWLINE, std::string("Newline in string") + "\n" + MSG_NEWLINE2, SEVERITY_REMARK,
                //         SourceSpan{SourceLocation{CurrentLineNumber-1, LineWidth+1}, Span.end});

                //     Issues.push_back(Issue);
                // }

                String << WLCharacterToString(c);
            }
            
        } // while
        
        String.put('"');
        
        c = nextWLCharacter();
        
        if (_stringifyNextToken) {
            _stringifyNextToken = false;
        }

        return TOKEN_STRING;
    }
}

//digits                  integer
//digits.digits           approximate number
//base^^digits            integer in specified base
//base^^digits.digits     approximate number in specified base
//mantissa*^n             scientific notation (mantissa*10^n)
//base^^mantissa*^n       scientific notation in specified base (mantissa*base^n)
//number`                 machine-precision approximate number
//number`s                arbitrary-precision number with precision s
//number``s               arbitrary-precision number with accuracy s
//
//
//
// base = (digits^^)?
// approximate = digits(.digits?)?|.digits
// precision = `(-?approximate)?
// accuracy = ``-?approximate
// mantissa = approximate+(precision|accuracy)?
// exponent = (*^-?digits)?
//
// numer = base+mantissa+exponent
//
Token Tokenizer::handleNumber() {
    
    auto c = currentWLCharacter();

    int base = 10;
    
    handleDigits();
    
    c = currentWLCharacter();
    
    //
    // Could be 16^^blah
    //
    if (c == '^') {
        
        c = nextWLCharacter(INSIDE_NUMBER);
        
        if (c == '^') {
            
            base = parseInteger(String.str(), 10);
            
            if (base < 2 || base > 36) {
                
                cur = TOKEN_ERROR_INVALIDBASE;
                return cur;
            }
            
            String.put('^');
            String.put(c);
            
            c = nextWLCharacter(INSIDE_NUMBER);
            
            if (isDigitOrAlpha(c)) {
                
                if (!handleDigitsOrAlpha(base)) {
                    
                    cur = TOKEN_ERROR_UNHANDLEDCHARACTER;
                    return cur;
                }
                
            } else {
                
                cur = TOKEN_ERROR_EXPECTEDDIGITORALPHA;
                return cur;
            }
            
        } else {
            
            auto Loc = TheSourceManager->getSourceLocation();
            
            characterQueue.push_back(std::make_pair(c, Loc));
            setCurrentWLCharacter('^', Loc);
            
            return TOKEN_NUMBER;
        }
    }
    
    c = currentWLCharacter();
    
    if (c == '.') {
        
        if (!handleFractionalPart(base)) {
            
            return TOKEN_NUMBER;
        }
    }
    
    c = currentWLCharacter();
    
    //
    // foo`
    // foo`bar
    // foo``bar
    //
    if (c == '`') {
        
        String.put('`');
        
        auto Loc = TheSourceManager->getSourceLocation();

        c = nextWLCharacter(INSIDE_NUMBER);
        
        bool accuracy = false;
        if (c == '`') {
            
            String.put('`');
            
            Loc = TheSourceManager->getSourceLocation();

            c = nextWLCharacter(INSIDE_NUMBER);
            
            accuracy = true;
        }
        
        if (isAlphaOrDollar(c) || isLetterlikeCodePoint(c)) {

            //
            // Something like 1.2`a
            //

            auto Loc2 = TheSourceManager->getSourceLocation();

            auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space between ` and " + WLCharacterToString(c) + " to reduce ambiguity", SEVERITY_REMARK, (SourceSpan{Loc, Loc2}));
                
            Issues.push_back(Issue);
        }

        if (accuracy || isDigit(c) || c == '-' || c == '+' || c == '.') {
            
            if (c == '-' || c == '+') {
                
                auto s = c;
                
                auto Loc2 = TheSourceManager->getSourceLocation();
                
                c = nextWLCharacter(INSIDE_NUMBER);
                
                if (isDigit(c)) {
                    
                    String.put(s);
                    
                } else if (c == '.') {
                    
                    String.put(s);
                    
                } else if (accuracy) {
                    
                    //
                    // Something like 1.2``->3
                    //
                    
                    String.put(s);
                    
                    cur = TOKEN_ERROR_EXPECTEDACCURACY;
                    return cur;
                    
                } else {

                    //
                    // Something like 1.2`->3
                    //

                    std::string msg;
                    if (s == '-') {
                        msg = "Put a space between ` and - to reduce ambiguity";
                    } else {
                        msg = "Put a space between ` and + to reduce ambiguity";
                    }
                    auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, msg, SEVERITY_REMARK, (SourceSpan{Loc, Loc2}));
                
                    Issues.push_back(Issue);
                    
                    
                    Loc = TheSourceManager->getSourceLocation();
                    
                    characterQueue.push_back(std::make_pair(c, Loc));
                    setCurrentWLCharacter(s, Loc);
                    
                    
                    return TOKEN_NUMBER;
                }
            }
            
            bool handled = false;
            
            if (isDigit(c)) {
                
                handleDigits();
                
                handled = true;
            }
            
            c = currentWLCharacter();
            
            if (c == '.') {
                
                if (handleFractionalPart(10)) {
                    
                    handled = true;
                    
                } else if (!accuracy) {
                    
                    return TOKEN_NUMBER;
                }
            }
            
            if (accuracy) {
                if (!handled) {
                    cur = TOKEN_ERROR_EXPECTEDACCURACY;
                    return cur;
                }
            }
        }
    }
    
    c = currentWLCharacter();
    
    if (c == '*') {
        
        c = nextWLCharacter(INSIDE_NUMBER);
        
        if (c == '^') {
            
            String.put('*');
            String.put(c);
            
            c = nextWLCharacter(INSIDE_NUMBER);
            
            if (c == '-' || c == '+') {
                
                String.put(c);
                
                c = nextWLCharacter(INSIDE_NUMBER);
            }
            
            if (!expectDigits()) {
                
                cur = TOKEN_ERROR_UNHANDLEDCHARACTER;
                return cur;
            }
            
            if (c == '.') {
                
                c = nextWLCharacter(INSIDE_NUMBER);
                
                cur = TOKEN_ERROR_EXPONENT;
                return cur;
            }
            
        } else {
            
            //
            // Something like 1*a
            //
            // Back out of treating * as part of the number
            //
            
            auto Loc = TheSourceManager->getSourceLocation();
            Loc = Loc - 1;
            
            characterQueue.push_back(std::make_pair(c, Loc));
            setCurrentWLCharacter('*', Loc);
            
            return TOKEN_NUMBER;
        }
    }
    
    return TOKEN_NUMBER;
}

bool Tokenizer::handleFractionalPart(int base) {
    
    auto c = currentWLCharacter();

    assert(c == '.');
    
    auto Loc = TheSourceManager->getSourceLocation();
    
    c = nextWLCharacter(INSIDE_NUMBER);
    
    if (c == '.') {
        
        //
        // Something like 0..
        //
        
        auto DigitLoc = Loc;
        DigitLoc.Col--;

        auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space before the . to reduce ambiguity", SEVERITY_REMARK, (SourceSpan{DigitLoc,Loc}));
        
        Issues.push_back(Issue);

        
        Loc = TheSourceManager->getSourceLocation();
        
        characterQueue.push_back(std::make_pair(c, Loc));
        setCurrentWLCharacter('.', Loc);
        
        
        return false; 
    }

    String.put('.');

    if (isDigitOrAlpha(c)) {
        
        if (!handleDigitsOrAlpha(base)) {
            return false;
        }
    }
    
    c = currentWLCharacter();

    if (c == '.') {

        //
        // Something like 1.2.3
        //

        auto Loc2 = TheSourceManager->getSourceLocation();
        auto Loc1 = Loc2;
        Loc1.Col--;

        auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space before the . to reduce ambiguity", SEVERITY_ERROR, (SourceSpan{Loc1,Loc2}));
        
        Issues.push_back(Issue);
    }

    return true;
}

bool Tokenizer::expectDigits() {
    
    auto c = currentWLCharacter();

    if (isDigit(c)) {
        
        handleDigits();
        
        return true;
        
    } else {
        
        return false;
    }
}

void Tokenizer::handleDigits() {
    
    auto c = currentWLCharacter();

    while (true) {
        
        if (isDigit(c)) {
            
            String.put(c);
            
            c = nextWLCharacter(INSIDE_NUMBER);
            
        } else {
            
            break;
        }
    }
}

bool Tokenizer::handleDigitsOrAlpha(int base) {
    
    auto c = currentWLCharacter();

    while (true) {
        
        if (isDigitOrAlpha(c)) {
            
            int baseDigit = toBaseDigit(c);
            
            if (baseDigit == -1 || baseDigit >= base) {
                return false;
            }
            
            String.put(c);
            
            c = nextWLCharacter(INSIDE_NUMBER);
            
        } else {
            break;
        }
    }
    
    return true;
}

void Tokenizer::handleDigitsOrAlphaOrDollar() {
    
    auto c = currentWLCharacter();

    while (true) {
        
        if (isDigitOrAlphaOrDollar(c)) {
            
            String.put(c);
            
            c = nextWLCharacter();
            
        } else {
            break;
        }
    }
}

Token Tokenizer::handleOperator() {
    
    auto c = currentWLCharacter();

    Token Operator;
    
    switch (c) {
        case ':': {
            Operator = TOKEN_OPERATOR_COLON;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case ':': {
                    Operator = TOKEN_OPERATOR_COLONCOLON;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    _stringifyNextToken = true;
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_COLONEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '>': {
                    Operator = TOKEN_OPERATOR_COLONGREATER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '(': {
            Operator = TOKEN_OPERATOR_OPENPAREN;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            if (c == '*') {
                
                return handleComment();
            }
        }
            break;
        case ')': {
            Operator = TOKEN_OPERATOR_CLOSEPAREN;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case '[': {
            
            Operator = TOKEN_OPERATOR_OPENSQUARE;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case ']': {
            Operator = TOKEN_OPERATOR_CLOSESQUARE;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case ',': {
            Operator = TOKEN_OPERATOR_COMMA;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case '{': {
            Operator = TOKEN_OPERATOR_OPENCURLY;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case '}': {
            Operator = TOKEN_OPERATOR_CLOSECURLY;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case '=': {
            Operator = TOKEN_OPERATOR_EQUAL;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '=': {
                    Operator = TOKEN_OPERATOR_EQUALEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    if (c == '=') {
                        
                        Operator = TOKEN_OPERATOR_EQUALEQUALEQUAL;
                        
                        String.put(c);
                        
                        c = nextWLCharacter();
                    }
                }
                    break;
                case '!': {
                    Operator = TOKEN_OPERATOR_UNKNOWN;
                    
                    c = nextWLCharacter();
                    
                    if (c == '=') {
                        
                        Operator = TOKEN_OPERATOR_EQUALBANGEQUAL;
                        
                        String.put('!');
                        String.put(c);
                        
                        c = nextWLCharacter();
                        
                    } else {
                        
                        auto Loc = TheSourceManager->getSourceLocation();
                        
                        characterQueue.push_back(std::make_pair(c, Loc));
                        setCurrentWLCharacter('!', Loc);
                        
                        Operator = TOKEN_OPERATOR_EQUAL;
                    }
                }
                    break;
            }
        }
            break;
        case '_': {
            Operator = TOKEN_OPERATOR_UNDER;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '_': {
                    Operator = TOKEN_OPERATOR_UNDERUNDER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    if (c == '_') {
                        
                        Operator = TOKEN_OPERATOR_UNDERUNDERUNDER;
                        
                        String.put(c);
                        
                        c = nextWLCharacter();
                    }
                }
                    break;
                case '.': {
                    Operator = TOKEN_OPERATOR_UNKNOWN;
                    
                    auto Loc = TheSourceManager->getSourceLocation();
                    
                    c = nextWLCharacter();
                    
                    if (c == '.') {
                        
                        //
                        // Could be _... which should parse as _ ...
                        //
                        
                        auto UnderLoc = Loc;
                        UnderLoc.Col--;

                        auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space between _ and . to reduce ambiguity", SEVERITY_REMARK, (SourceSpan{UnderLoc,Loc}));
                        
                        Issues.push_back(Issue);

                        
                        Loc = TheSourceManager->getSourceLocation();
                        
                        characterQueue.push_back(std::make_pair(c, Loc));
                        setCurrentWLCharacter('.', Loc);
                        
                        
                        Operator = TOKEN_OPERATOR_UNDER;
                        
                    } else if (isDigit(c)) {
                        
                        String.put('.');
                        
                        //
                        // Something like _.0
                        //
                        
                        auto UnderLoc = Loc;
                        UnderLoc.Col--;
                        
                        auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space between . and number to reduce ambiguity", SEVERITY_REMARK, (SourceSpan{UnderLoc,Loc}));
                        
                        Issues.push_back(Issue);

                        Operator = TOKEN_OPERATOR_UNDERDOT;
                        
                    } else {
                        
                        String.put('.');
                        
                        Operator = TOKEN_OPERATOR_UNDERDOT;
                    }
                }
                    break;
            }
            
            return Operator;
        }
            break;
        case '<': {
            Operator = TOKEN_OPERATOR_LESS;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '|': {
                    Operator = TOKEN_OPERATOR_LESSBAR;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '<': {
                    Operator = TOKEN_OPERATOR_LESSLESS;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    _stringifyNextToken = true;
                }
                    break;
                case '>': {
                    Operator = TOKEN_OPERATOR_LESSGREATER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_LESSEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '-': {
                    Operator = TOKEN_OPERATOR_UNKNOWN;
                    
                    c = nextWLCharacter();
                    
                    if (c == '>') {
                        
                        Operator = TOKEN_OPERATOR_LESSMINUSGREATER;
                        
                        String.put('-');
                        String.put(c);
                        
                        c = nextWLCharacter();
                        
                    } else {
                        
                        auto Loc = TheSourceManager->getSourceLocation();
                        
                        characterQueue.push_back(std::make_pair(c, Loc));
                        setCurrentWLCharacter('-', Loc);
                        
                        
                        Operator = TOKEN_OPERATOR_LESS;
                    }
                }
                    break;
            }
        }
            break;
        case '>': {
            Operator = TOKEN_OPERATOR_GREATER;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '>': {
                    Operator = TOKEN_OPERATOR_GREATERGREATER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    _stringifyNextToken = true;
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_GREATEREQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '-': {
            Operator = TOKEN_OPERATOR_MINUS;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            //
            // Do not lex as a number here
            // Makes it easier to handle invisible multiplication later
            //
            // Because if we lexed - as a number here, then it is
            // harder to know that b-1 is Plus[b, -1] instead of
            // b<invisiblespace>-1 which is Times[b, -1]
            //
            
            switch (c) {
                case '>': {
                    Operator = TOKEN_OPERATOR_MINUSGREATER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '-': {
                    Operator = TOKEN_OPERATOR_MINUSMINUS;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_MINUSEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '|': {
            Operator = TOKEN_OPERATOR_BAR;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '>': {
                    Operator = TOKEN_OPERATOR_BARGREATER;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '|': {
                    Operator = TOKEN_OPERATOR_BARBAR;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case ';': {
            Operator = TOKEN_OPERATOR_SEMI;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            if (c == ';') {
                
                Operator = TOKEN_OPERATOR_SEMISEMI;
                
                String.put(c);
                
                c = nextWLCharacter();
            }
        }
            break;
        case '!': {
            Operator = TOKEN_OPERATOR_BANG;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '=': {
                    Operator = TOKEN_OPERATOR_BANGEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '!': {
                    Operator = TOKEN_OPERATOR_BANGBANG;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '#': {
            
            // auto Loc = TheSourceManager->getNextLoc();
            
            Operator = TOKEN_OPERATOR_UNKNOWN;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            //
            // A slot that starts with a digit goes down one path
            // And a slot that starts with a letter does down another path
            //
            // Make sure e.g. #1a is not parsd as SlotNode["#1a"]
            //
            
            if (isDigit(c)) {
                
                Operator = TOKEN_OPERATOR_HASH;
                
                handleDigits();
                
                return Operator;
                
            } else if (isAlphaOrDollar(c)) {
                
                Operator = TOKEN_OPERATOR_HASH;
                
                handleDigitsOrAlphaOrDollar();
                
                return Operator;
                
            } else if (c == '#') {
                
                Operator = TOKEN_OPERATOR_HASHHASH;
                
                String.put(c);
                
                c = nextWLCharacter();
                
                if (isDigit(c)) {
                    
                    handleDigits();
                    
                    return Operator;
                }

            } else {
                
                Operator = TOKEN_OPERATOR_HASH;
            }
        }
            break;
        case '%': {
            Operator = TOKEN_OPERATOR_PERCENT;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            if (c == '%') {
                
                while (true) {
                    
                    if (c != '%') {
                        break;
                    }
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                
                return Operator;
                
            } else if (isDigit(c)) {
                
                handleDigits();
                
                return Operator;
            }
        }
            break;
        case '&': {
            Operator = TOKEN_OPERATOR_AMP;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            if (c == '&') {
                
                Operator = TOKEN_OPERATOR_AMPAMP;
                
                String.put(c);
                
                c = nextWLCharacter();
            }
        }
            break;
        case '/': {
            Operator = TOKEN_OPERATOR_SLASH;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '@': {
                    Operator = TOKEN_OPERATOR_SLASHAT;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case ';': {
                    Operator = TOKEN_OPERATOR_SLASHSEMI;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '.': {
                    Operator = TOKEN_OPERATOR_UNKNOWN;
                    
                    auto Loc = TheSourceManager->getSourceLocation();
                    
                    c = nextWLCharacter();
                    
                    if (isDigit(c)) {
                        
                        //
                        // Something like Round[t/.03 + 1]
                        //
                        
                        auto SlashLoc = Loc;
                        SlashLoc.Col--;
                        
                        auto Issue = SyntaxIssue(TAG_SYNTAXAMBIGUITY, "Put a space between / and . to reduce ambiguity", SEVERITY_REMARK, (SourceSpan{SlashLoc,Loc}));
                        
                        Issues.push_back(Issue);

                        
                        Loc = TheSourceManager->getSourceLocation();
                        
                        characterQueue.push_back(std::make_pair(c, Loc));
                        setCurrentWLCharacter('.', Loc);
                        
                        
                        Operator = TOKEN_OPERATOR_SLASH;
                        
                    } else {
                        
                        String.put('.');
                        
                        Operator = TOKEN_OPERATOR_SLASHDOT;
                    }
                }
                    break;
                case '/': {
                    Operator = TOKEN_OPERATOR_SLASHSLASH;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    switch (c) {
                        case '.': {
                            Operator = TOKEN_OPERATOR_SLASHSLASHDOT;
                            
                            String.put(c);
                            
                            c = nextWLCharacter();
                        }
                            break;
                        case '@': {
                            Operator = TOKEN_OPERATOR_SLASHSLASHAT;
                            
                            String.put(c);
                            
                            c = nextWLCharacter();
                        }
                            break;
                    }
                }
                    break;
                case ':': {
                    Operator = TOKEN_OPERATOR_SLASHCOLON;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_SLASHEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '*': {
                    Operator = TOKEN_OPERATOR_SLASHSTAR;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '@': {
            Operator = TOKEN_OPERATOR_AT;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '@': {
                    Operator = TOKEN_OPERATOR_ATAT;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                    
                    if (c == '@') {
                        
                        Operator = TOKEN_OPERATOR_ATATAT;
                        
                        String.put(c);
                        
                        c = nextWLCharacter();
                    }
                }
                    break;
                case '*': {
                    Operator = TOKEN_OPERATOR_ATSTAR;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '+': {

            Operator = TOKEN_OPERATOR_PLUS;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '+': {
                    Operator = TOKEN_OPERATOR_PLUSPLUS;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_PLUSEQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '~': {
            Operator = TOKEN_OPERATOR_TILDE;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            if (c == '~') {
                
                Operator = TOKEN_OPERATOR_TILDETILDE;
                
                String.put(c);
                
                c = nextWLCharacter();
            }
        }
            break;
        case '?': {
            Operator = TOKEN_OPERATOR_QUESTION;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        case '*': {
            Operator = TOKEN_OPERATOR_STAR;
            
            String.put(c);
            
            c = nextWLCharacter();
            
            switch (c) {
                case '=': {
                    Operator = TOKEN_OPERATOR_STAREQUAL;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                case '*': {
                    Operator = TOKEN_OPERATOR_STARSTAR;
                    
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
            }
        }
            break;
        case '^': {
            
            Operator = TOKEN_OPERATOR_UNKNOWN;
            
            c = nextWLCharacter();
            
            switch (c) {
                case ':': {
                    
                    c = nextWLCharacter();
                    
                    if (c == '=') {
                        
                        Operator = TOKEN_OPERATOR_CARETCOLONEQUAL;
                        
                        String.put('^');
                        String.put(':');
                        String.put(c);
                        
                        c = nextWLCharacter();
                        
                    } else {
                        
                        String.put('^');
                        String.put(':');
                        
                        cur = TOKEN_ERROR_UNHANDLEDCHARACTER;
                        return cur;
                    }
                }
                    break;
                case '=': {
                    Operator = TOKEN_OPERATOR_CARETEQUAL;
                    
                    String.put('^');
                    String.put(c);
                    
                    c = nextWLCharacter();
                }
                    break;
                default: {
                    Operator = TOKEN_OPERATOR_CARET;
                    
                    String.put('^');
                }
                    break;
            }
        }
            break;
        case '\'': {
            Operator = TOKEN_OPERATOR_TICK;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
            break;
        default: {
            
            assert(isOperatorCodePoint(c));
            
            String << WLCharacterToString(c);
            
            Operator = LongNameCodePointToOperator(c);
            
            c = nextWLCharacter();
        }
    }
    
    return Operator;
}

//
// Could be . or .. or ... or .0
// Have to lookahead to decide
//
Token Tokenizer::handleDot() {
    
    auto c = currentWLCharacter();

    assert(c == '.');
    
    c = nextWLCharacter();
    
    if (isDigit(c)) {
        
        auto Loc = TheSourceManager->getSourceLocation();
        
        characterQueue.push_back(std::make_pair(c, Loc));
        setCurrentWLCharacter('.', Loc);
        
        cur = handleNumber();
        return cur;
    }
    
    String.put('.');
    
    auto Operator = TOKEN_OPERATOR_DOT;
    
    if (c == '.') {
        
        Operator = TOKEN_OPERATOR_DOTDOT;
        
        String.put(c);
        
        c = nextWLCharacter();
        
        if (c == '.') {
            
            Operator = TOKEN_OPERATOR_DOTDOTDOT;
            
            String.put(c);
            
            c = nextWLCharacter();
        }
    }
    
    cur = Operator;
    return cur;
}

std::string Tokenizer::getString() {
    
    auto StringStr = String.str();
    
    String.str("");
    
    return StringStr;
}

std::vector<SyntaxIssue> Tokenizer::getIssues() {

    auto Tmp = Issues;

    Issues.clear();
    
    auto CharacterIssues = TheCharacterDecoder->getIssues();
    
    std::copy(CharacterIssues.begin(), CharacterIssues.end(), std::back_inserter(Tmp));
    
    return Tmp;
}

Tokenizer *TheTokenizer = nullptr;
