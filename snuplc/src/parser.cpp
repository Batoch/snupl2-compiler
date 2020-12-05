//--------------------------------------------------------------------------------------------------
/// @brief SnuPL parser
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/11/04 Bernhard Egger maintain unary '+' signs in the AST
/// 2016/04/01 Bernhard Egger adapted to SnuPL/1 (this is not a joke)
/// 2019/09/15 Bernhard Egger added support for constant expressions
/// 2020/07/31 Bernhard Egger adapted to SnuPL/2
/// 2020/09/27 Bernhard Egger assignment 2: parser for SnuPL/-1
///
/// @section license_section License
/// Copyright (c) 2012-2020, Computer Systems and Platforms Laboratory, SNU
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice, this list of condi-
///   tions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
///   tions and the following disclaimer in the documentation and/or other materials provided with
///   the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
/// IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
/// CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/// LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
/// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//--------------------------------------------------------------------------------------------------

#include <limits.h>
#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <exception>

#include "parser.h"
using namespace std;

//--------------------------------------------------------------------------------------------------
// CParser
//
CParser::CParser(CScanner* scanner)
{
	_scanner = scanner;
	_module = NULL;
}

CAstNode* CParser::Parse(void)
{
	_abort = false;

	if (_module != NULL)
	{
		delete _module;
		_module = NULL;
	}

	try
	{
		if (_scanner != NULL) _module = module();
	}
	catch (...)
	{
		_module = NULL;
	}

	return _module;
}

const CToken* CParser::GetErrorToken(void) const
{
	if (_abort) return &_error_token;
	else return NULL;
}

string CParser::GetErrorMessage(void) const
{
	if (_abort) return _message;
	else return "";
}

void CParser::SetError(CToken t, const string message)
{
	_error_token = t;
	_message = message;
	_abort = true;
	throw message;
}

bool CParser::Consume(EToken type, CToken* token)
{
	if (_abort) return false;

	CToken t = _scanner->Get();

	if (t.GetType() != type)
	{
		SetError(t, "expected '" + CToken::Name(type) + "', got '" +
			t.GetName() + "'");
	}

	if (token != NULL) *token = t;

	return t.GetType() == type;
}

void CParser::InitSymbolTable(CSymtab* st)
{
	CTypeManager* tm = CTypeManager::Get();
	CSymProc* f;

	// read and return an integer/longint value from stdin.
	f = new CSymProc("ReadInt", tm->GetInteger(), true);
	st->AddSymbol(f);
	f = new CSymProc("ReadLong", tm->GetLongint(), true);
	st->AddSymbol(f);

	// print integer/longint value ‘v’ to stdout.
	f = new CSymProc("WriteInt", tm->GetNull());
	f->AddParam(new CSymParam(0, "v", tm->GetInteger()));
	st->AddSymbol(f);
	f = new CSymProc("WriteLong", tm->GetNull());
	f->AddParam(new CSymParam(0, "v", tm->GetLongint()));
	st->AddSymbol(f);

	// write a single character to stdout.
	f = new CSymProc("WriteChar", tm->GetNull());
	f->AddParam(new CSymParam(0, "c", tm->GetChar()));
	st->AddSymbol(f);

	// write string ‘string’ to stdout. No newline is added.
	f = new CSymProc("WriteStr", tm->GetNull());
	f->AddParam(new CSymParam(0, "string", tm->GetChar()));
	st->AddSymbol(f);

	// write a newline sequence to stdout.
	f = new CSymProc("WriteLn", tm->GetNull());
	st->AddSymbol(f);

}

CAstModule* CParser::module()
{
	//
	// module ::= statSequence  ".".
	//
	CToken mt, t;
	string id;

	CAstModule* m;
	CAstStatement* statseq = NULL;

//	while(_scanner->Peek().GetType() == tUndefined){
//		Consume(tUndefined, &mt);
//	}

	Consume(tModule, &mt);
	Consume(tIdent, &mt);
	id = t.GetValue();
	Consume(tSemicolon);

	m = new CAstModule(mt, id);

	InitSymbolTable(m->GetSymbolTable());

	EToken tt = _scanner->Peek().GetType();
	while ((tt == tConstDecl) || (tt == tVarDecl) || (tt == tProcedure) || (tt == tFunction))
	{
		switch (tt)
		{
		case tConstDecl:
			constDeclaration(m);
			break;
		case tVarDecl:
			varDeclaration(m);
			break;
		case tProcedure:
		{
			CAstProcedure* sub = procedureDecl(m);
			if (_scanner->Peek().GetValue() == "Extern")
			{
				Consume(tIdent);
				Consume(tSemicolon);
			}
			else
			{
				subroutineBody(sub);
				Consume(tIdent);
				Consume(tSemicolon);
			}
			break;
		}
		case tFunction:
		{
			CAstProcedure* sub = functionDecl(m);
			if (_scanner->Peek().GetValue() == "Extern")
			{
				Consume(tIdent);
				Consume(tSemicolon);
			}
			else
			{
				subroutineBody(sub);
				Consume(tIdent);
				Consume(tSemicolon);
			}
			break;
		}
		default:
			break;
		}
		tt = _scanner->Peek().GetType();
	}
	if (tt == tBegin)
	{
		Consume(tBegin);
		statseq = statSequence(m);
		m->SetStatementSequence(statseq);
	}

	Consume(tEnd);

	if (id != t.GetValue())
	{
		SetError(t, "Module identifier mismatch (" + id + " and " + t.GetValue() + ")");
	}

	Consume(tIdent, &t);

	Consume(tDot);

	return m;
}

void CParser::subroutineBody(CAstScope* s)
{
	// subroutineBody ::= { constDeclaration | varOeclaration }
	// 						"begin" statSequence "end"
	CAstStatement* statseq = nullptr;

	EToken tt = _scanner->Peek().GetType();
	while ((tt == tConstDecl) || (tt == tVarDecl))
	{
		switch (tt)
		{
		case tConstDecl:
			constDeclaration(s);
			break;
		case tVarDecl:
			varDeclaration(s);
			break;
		default:
			break;
		}
		tt = _scanner->Peek().GetType();
	}
	Consume(tBegin);
	statseq = statSequence(s);
	Consume(tEnd);

	s->SetStatementSequence(statseq);
}

CAstStatement* CParser::statSequence(CAstScope* s)
{
	//
	// statSequence ::= [ statement { ";" statement } ].
	// statement ::= assignment.
	//
	// FIRST(statSequence) = { tLetter }
	// FOLLOW(statSequence) = { tDot }
	//
	// FIRST(statement) = { tLetter }
	// FOLLOW(statement) = { tSemicolon, tDot }
	//

	// The linking of statement sequences is a bit akward here because
	// we implement statSequence as a loop and not recursively.
	// We keep a 'head' that points to the first statement and is finally
	// returned at the end of the function. Head can be NULL if no statement
	// is present.
	// In the loop, we track the end of the linked list using 'tail' and
	// attach new statements to that tail.
	CAstStatement* head = nullptr;

	if (_scanner->Peek().GetType() != tDot)
	{
		CAstStatement* tail = nullptr;

		do
		{
			CAstStatement* st = nullptr;

			switch (_scanner->Peek().GetType())
			{
				// statement ::= assignment
			case tCharConst:
				st = assignment(s);
				break;
			case tIf:
				st = ifstatement(s);
				break;
			case tWhile:
				st = whilestatement(s);
				break;
			case tReturn:
				st = returnstatement(s);
				break;
			case tIdent:
			{
				CToken tt = _scanner->Peek();
				const CSymbol* sym = s->GetSymbolTable()->FindSymbol(tt.GetValue(), sLocal);
				if (sym == NULL){
					sym = s->GetSymbolTable()->FindSymbol(tt.GetValue(), sGlobal);
				}
				ESymbolType stype = sym->GetSymbolType();
				if (stype == stProcedure) st = subroutineCall(s);
				else st = assignment(s);
				break;
			}
			case tEnd:
				break;
			default:
				SetError(_scanner->Peek(), "statement expected.");
				break;
			}
			if (_scanner->Peek().GetType() == tEnd) break;
			assert(st != NULL);
			if (head == NULL) head = st;
			else tail->SetNext(st);
			tail = st;

			if (_scanner->Peek().GetType() == tDot) break;
			if (_scanner->Peek().GetType() == tEnd) break;
			if (_scanner->Peek().GetType() == tElse) break;
			Consume(tSemicolon);
		} while (!_abort);
	}

	return head;
}

CAstStatAssign* CParser::assignment(CAstScope* s)
{
	//
	// assignment ::= letter ":=" expression.
	//
	CToken t;

	CAstDesignator* lhs = qualident(s);
	Consume(tAssign, &t);

	CAstExpression* rhs = expression(s);

	return new CAstStatAssign(t, lhs, rhs);
}

CAstExpression* CParser::expression(CAstScope* s)
{
	//
	// expression ::= simpleexpr [ relOp simpleexpr ].
	//
	CToken t;
	EOperation relop = opEqual;        //silent compiler warnings
	CAstExpression* left = NULL, * right = NULL;

	left = simpleexpr(s);

	if (_scanner->Peek().GetType() == tRelOp)
	{
		Consume(tRelOp, &t);
		right = simpleexpr(s);

		if (t.GetValue() == "=") relop = opEqual;
		else if (t.GetValue() == "#") relop = opNotEqual;
		else if (t.GetValue() == "<") relop = opLessThan;
		else if (t.GetValue() == "<=") relop = opLessEqual;
		else if (t.GetValue() == ">") relop = opBiggerThan;
		else if (t.GetValue() == ">=") relop = opBiggerEqual;
		else SetError(t, "invalid relation.");

		return new CAstBinaryOp(t, relop, left, right);
	}
	else
	{
		return left;
	}
}

CAstExpression* CParser::simpleexpr(CAstScope* s)
{
	//
	// simpleexpr ::= term { termOp term }.
	//
	CAstExpression* n = NULL;

	bool fUnary = false;
	CToken tpm;
	EOperation opUnary;
	if (tPlusMinus == _scanner->Peek().GetType())
	{
		Consume(tPlusMinus, &tpm);
		fUnary = true;
		if (tpm.GetValue() == "-")
			opUnary = opNeg;
		else if (tpm.GetValue() == "+")
			opUnary = opPos;
		else
		{
			opUnary = opNop;
			fUnary = false;
		}
	}

	n = term(s);

	if (fUnary)
	{
		CAstExpression* u = n;
		n = new CAstUnaryOp(tpm, opUnary, u);
	}

	EOperation termop;

	while (_scanner->Peek().GetType() == tPlusMinus || _scanner->Peek().GetType() == tOr)
	{
		CToken t;
		CAstExpression* l = n, * r;

		if (_scanner->Peek().GetType() == tPlusMinus)
			Consume(tPlusMinus, &t);
		if (_scanner->Peek().GetType() == tOr)
			Consume(tOr, &t);

		r = term(s);
		if (t.GetValue() == "+") termop = opAdd;
		else if (t.GetValue() == "-") termop = opSub;
		else if (t.GetValue() == "||") termop = opOr;
		else SetError(t, "invalid relation.");

		n = new CAstBinaryOp(t, termop, l, r);
	}

	return n;
}

CAstExpression* CParser::term(CAstScope* s)
{
	//
	// term ::= factor { ("*"|"/") factor }.
	//
	CAstExpression* n = NULL;

	n = factor(s);

	EToken tt = _scanner->Peek().GetType();

	while (tt == tMulDiv || tt == tAnd)
	{
		CToken t;
		CAstExpression* l = n, * r;
		EOperation factop = opMul;        // Silent compiler warning

		if (tt == tMulDiv)
			Consume(tMulDiv, &t);
		if (tt == tAnd)
			Consume(tAnd, &t);

		r = factor(s);

		if (t.GetValue() == "*") factop = opMul;
		else if (t.GetValue() == "/") factop = opDiv;
		else if (t.GetValue() == "&&") factop = opAnd;
		else SetError(t, "invalid relation.");

		n = new CAstBinaryOp(t, factop, l, r);

		tt = _scanner->Peek().GetType();
	}

	return n;
}

CAstExpression* CParser::factor(CAstScope* s)
{
	//
	// factor ::= qualident | number | boolean | char | string |
	//"(" expression ")" | subroutineCall | "!" factor
	//
	// FIRST(factor) = { tDigit, tLBrak }
	//

	CToken tt = _scanner->Peek();
	CAstExpression* numbr = NULL;

	switch (_scanner->Peek().GetType())
	{
		// factor ::= number
	case tNumber:
		numbr = number();
		break;

		// factor ::= "(" expression ")"
	case tLParens:
		Consume(tLParens);
		numbr = expression(s);
		Consume(tRParens);
		break;

	case tCharConst :
		numbr = character();
		break;

	case tBoolConst:
		numbr = boolean();
		break;

	case tStringConst :
		numbr = stringConst(s);
		break;

	case tIdent:
	{    // necessary for var decl
		const CSymbol* sym = s->GetSymbolTable()->FindSymbol(tt.GetValue());
		if (sym)
		{
			ESymbolType stype = sym->GetSymbolType();
			if (stype == stProcedure)
				numbr = functionCall(s);
			else
				numbr = qualident(s);
		}
		else
			SetError(tt, "undeclared variable \"" + tt.GetValue() + "\"");
		break;
	}

	case tNot:
	{    // necessary for var decl
		Consume(tNot, &tt);
		CAstExpression* e = factor(s);
		numbr = new CAstUnaryOp(tt, opNot, e);
		break;
	}

	default:
		SetError(_scanner->Peek(), "factor expected.");
		break;
	}

	return numbr;
}

CAstConstant* CParser::number()
{
	//
	// number ::= "0".."9".
	//

	CToken t;

	Consume(tNumber, &t);

	errno = 0;
	long long v = strtoll(t.GetValue().c_str(), NULL, 10);
	if (errno != 0) SetError(t, "invalid number.");

	return new CAstConstant(t, CTypeManager::Get()->GetInteger(), v);
}

void CParser::varDeclaration(CAstScope* s)
{
	//
	// varDeclaration = ["var" varDeclSequence ";"].
	// varDeclSequence = varDecl { ";" varDecl }.
	//
	// FOLLOW(varDeclaration) = {tBegin}

	EToken tt = _scanner->Peek().GetType();
	CToken t;
	if (tt == tVarDecl)
	{
		Consume(tVarDecl, &t);

		vector<string> variables;

		do
		{
			vector<string> liste;
			CAstType* ttype;

			varDecl(liste, ttype, variables);

			for (const auto& str : liste)
			{
				CSymbol* var = s->CreateVar(str, ttype->GetType());
				s->GetSymbolTable()->AddSymbol(var);
			}

			Consume(tSemicolon);
			tt = _scanner->Peek().GetType();
		} while (tt != tBegin && tt != tFunction && tt != tProcedure);
	}
}

void CParser::varDecl(vector<string>& variables, CAstType*& ttype, vector<string>& toutesVariables)
{
	//
	// varDecl = ident {"," ident} ":" type.
	//
	while (!_abort)
	{
		CToken t = _scanner->Get();
		EToken tt = t.GetType();
		if (tt != tIdent)
			SetError(t, "invalid identifier");

		for (const auto& var : toutesVariables)
		{
			if (var == t.GetValue())
			{
				SetError(t, "re-decalaration variable \"" + t.GetValue() + "\"");
				return;
			}
		}
		variables.push_back(t.GetValue());
		toutesVariables.push_back(t.GetValue());

		t = _scanner->Peek();
		tt = t.GetType();
		if (tt == tColon)
			break;
		else if (tt != tComma)
			SetError(t, R"(":" or "," expected)");
		Consume(tComma);
		tt = _scanner->Peek().GetType();
	}
	Consume(tColon);
	ttype = type();
}

void CParser::constDeclaration(CAstScope* s)
{
	//
	// constDeclaration = [ "const" constDeclSequence ].
	// constDeclSequence = constDecl ";" { constDecl ";" }
	// FOLLOW(constDeclaration) = {tBegin}
	//
	CAstExpression* express = NULL;
	EToken tt = _scanner->Peek().GetType();
	CToken t;

	if (tt == tConstDecl)
	{
		Consume(tConstDecl, &t);

		do
		{
			if (tt == tVarDecl)
			{
				Consume(tVarDecl, &t);

				vector<string> variables;

				do
				{
					vector<string> liste;
					CAstType* ttype;

					varDecl(liste, ttype, variables);

					for (const auto& str : liste)
					{
						t = _scanner->Get();
						tt = t.GetType();

						express = expression(s);

						if (tt == tRelOp && t.GetValue() == "=")
							Consume(tRelOp);
						else if (tt != tRelOp || t.GetValue() != "=")
							SetError(t, "\"=\" expected");

						CSymbol* constante = s->CreateConst(str, ttype->GetType(),
							reinterpret_cast<const CDataInitializer*>(express));
						s->GetSymbolTable()->AddSymbol(constante);
					}

					Consume(tSemicolon);
					tt = _scanner->Peek().GetType();
				} while (tt != tBegin && tt != tRelOp);
			}

			tt = _scanner->Peek().GetType();
		} while (tt != tBegin);
	}
}

void CParser::constDecl(vector<string>& variables, CAstType*& ttype, vector<string>& toutesVariables, CAstScope* s)
{
//
//varDecl = ident {"," ident} ":" type.
//

	CAstExpression* express = nullptr;

	while (!_abort)
	{
		CToken t = _scanner->Get();
		EToken tt = t.GetType();
		if (tt != tIdent)
			SetError(t, "invalid identifier");

		for (const auto& var : toutesVariables)
		{
			if (var == t.GetValue())
			{
				SetError(t, "re-decalaration variable \"" + t.GetValue() + "\"");
				return;
			}
		}
		variables.push_back(t.GetValue());
		toutesVariables.push_back(t.GetValue());

		t = _scanner->Peek();
		tt = t.GetType();
		if (tt == tColon)
			break;
		else if (tt != tComma)
			SetError(t, R"(":" or "," expected)");
		Consume(tComma);
		tt = _scanner->Peek().GetType();
	}
	Consume(tColon);
	ttype = type();

	CToken t = _scanner->Get();
	EToken tt = t.GetType();

	if (tt == tRelOp && t.GetValue() == "=")
		Consume(tRelOp);
	else if (tt != tRelOp || t.GetValue() != "=")
		SetError(t, "\"=\" expected");

	express = expression(s);

	t = _scanner->Get();
	tt = t.GetType();

}

CAstStatIf* CParser::ifstatement(CAstScope* s)
{
	//
	// ifstatement = "if" "(" expression ")" "then" statSequence
	//              [ "else" statSequence ] "end".
	//

	CToken t;

	CAstExpression* condition = NULL;
	CAstStatement* ifBody = NULL;
	CAstStatement* elseBody = NULL;

	Consume(tIf, &t);

	Consume(tLParens);
	condition = expression(s);
	Consume(tRParens);
	Consume(tThen);
	ifBody = statSequence(s);
	if (_scanner->Peek().GetType() == tElse)
	{
		Consume(tElse);
		elseBody = statSequence(s);
	}
	Consume(tEnd);

	return new CAstStatIf(t, condition, ifBody, elseBody);
}

CAstStatWhile* CParser::whilestatement(CAstScope* s)
{
	//
	// whilestatement = "while" "(" expression ")" "do" statSequence "end".
	//

	CToken t;

	CAstExpression* condition = NULL;
	CAstStatement* body = NULL;

	Consume(tWhile, &t);

	Consume(tLParens);
	condition = expression(s);
	Consume(tRParens);
	Consume(tDo);
	body = statSequence(s);
	Consume(tEnd);

	return new CAstStatWhile(t, condition, body);
}

CAstStatReturn* CParser::returnstatement(CAstScope* s)
{
	//
	// returnStatement = "return" [ expression ].
	//
	// FIRST(returnstatement) = {tReturn}
	// FOLLOW(returnstatement) = {tEnd, tSemicolon, tElse}
	//

	CToken t;

	CAstExpression* express = NULL;

	Consume(tReturn, &t);
	if (_isexpressionfirst(_scanner->Peek().GetType()))
		express = expression(s);

	return new CAstStatReturn(t, s, express);
}

bool CParser::_isexpressionfirst(EToken tt)
{
	//FIRST(expression) = {tPlusMinus, tIdent, tNumber, tBoolConst, tCharConst, tStringConst, tLBrak, tNot)
	return (tPlusMinus == tt
		|| tIdent == tt
		|| tNumber == tt
		|| tBoolConst == tt
		|| tCharConst == tt
		|| tStringConst == tt
		|| tLBrak == tt
		|| tNot == tt
	);
}

CAstType* CParser::type()
{
	//
	// type = basetype | type "[" [simpleexpr] "]".
	// basetype = "boolean" | "char" | "integer" | "longint".
	//

	const CType* ttype = NULL;
	vector<long long> insideBrackets;

	CToken t1 = _scanner->Peek();
	EToken tt1 = t1.GetType();

	if (tt1 == tBoolean)
	{
		ttype = CTypeManager::Get()->GetBool();
		Consume(tBoolean);
	}
	else if (tt1 == tChar)
	{
		ttype = CTypeManager::Get()->GetChar();
		Consume(tChar);
	}
	else if (tt1 == tInteger)
	{
		ttype = CTypeManager::Get()->GetInteger();
		Consume(tInteger);
	}
//	else if (tt1 == tLong)
//	{
//		ttype = CTypeManager::Get()->GetLongint();
//		Consume(tLong);
//	}

	CToken t2 = _scanner->Peek();
	while (t2.GetType() == tLBrak)
	{
		Consume(tLBrak);
		if (_scanner->Peek().GetType() != tRBrak)
		{
			if (_scanner->Peek().GetType() == tPlusMinus)
				Consume(tPlusMinus);
			insideBrackets.push_back(number()->GetValue());
		}
		else
			insideBrackets.push_back(CArrayType::OPEN);

		Consume(tRBrak);
		if (_scanner->Peek().GetType() != tLBrak)
			break;
	}
	if (!insideBrackets.empty())
	{
		const CType* innertype = ttype;
		for (int i = (int)insideBrackets.size() - 1; i >= 0; i--)
			innertype = CTypeManager::Get()->GetArray(insideBrackets[i], innertype);
		ttype = innertype;
	}
	return new CAstType(t1, ttype);
}

CAstDesignator* CParser::qualident(CAstScope* s)
{
	//
	// qualident ::= ident { "[" expression "]" }
	//

	CAstDesignator* id = ident(s);
	EToken tt = _scanner->Peek().GetType();
	if (tt == tLBrak)
	{
		const CToken saveToken = id->GetToken();
		const CSymbol* saveSymbol = id->GetSymbol();

		free(id);
		auto* arrayId = new CAstArrayDesignator(saveToken, saveSymbol);
		while (tt == tLBrak)
		{
			Consume(tLBrak);

			arrayId->AddIndex(simpleexpr(s));

			Consume(tRBrak);
			tt = _scanner->Peek().GetType();
		}
		arrayId->IndicesComplete();
		return arrayId;
	}

	return id;
}

CAstDesignator* CParser::ident(CAstScope* s)
{
	CToken t;

	Consume(tIdent, &t);

	CSymtab* symtab = s->GetSymbolTable();
	const CSymbol* symbol = symtab->FindSymbol(t.GetValue(), sLocal);
	if (symbol == NULL)
		symbol = symtab->FindSymbol(t.GetValue(), sGlobal);

	return new CAstDesignator(t, symbol);
}

CAstStatCall* CParser::subroutineCall(CAstScope* s)
{
	//
	// subroutineCall = ident "(" [ expression { "," expression } ] ")".
	//

	CToken t = _scanner->Peek();
	if (t.GetType() != tIdent)
		SetError(t, "invalid subroutine call");

	CAstFunctionCall* func = functionCall(s);

	return new CAstStatCall(t, func);
}

CAstFunctionCall* CParser::functionCall(CAstScope* s)
{
	//
	// subroutineCall = ident "(" [ expression { "," expression } ] ")".
	//
	CToken t;

	Consume(tIdent, &t);
	const CSymbol* symbol = s->GetSymbolTable()->FindSymbol(t.GetValue(), sGlobal);
	if (!symbol)
		SetError(t, "undeclared subroutine name");

	auto* func = new CAstFunctionCall(t, dynamic_cast<const CSymProc*>(symbol));

	Consume(tLParens);

	while (_scanner->Peek().GetType() != tRParens)
	{
		func->AddArg(expression(s));

		if (_scanner->Peek().GetType() == tComma)
			Consume(tComma);
	}

	Consume(tRParens);

	return func;
}

CAstProcedure* CParser::functionDecl(CAstScope* s)
{
	//
	// funtionDecl = "function" ident [ formalParam ] ":" type ";".
	//

	CToken t1;

	Consume(tFunction, &t1);

	CToken t2 = _scanner->Get();
	if (t2.GetType() != tIdent)
		SetError(t2, "expected function identifier");

	const string& functionName = t2.GetValue();
	if (s->GetSymbolTable()->FindSymbol(functionName, sGlobal))
		SetError(t2, "re-declaration function \"" + functionName + "\"");

	vector<string> paramNames;
	vector<CAstType*> paramTypes;
	CAstType* returnType;

	t2 = _scanner->Peek();
	switch (t2.GetType())
	{
	case tLParens:
		formalParam(paramNames, paramTypes);
		break;
	case tColon:
		break;
	default:
		SetError(t2, R"("(" or ":" expected)");
		break;
	}

	Consume(tColon);
	returnType = type();
	Consume(tSemicolon);

	auto* symbol = new CSymProc(functionName, returnType->GetType());
	s->GetSymbolTable()->AddSymbol(symbol);

	auto* ret = new CAstProcedure(t1, functionName, s, symbol);
	AddParameters(ret, symbol, paramNames, paramTypes);

	return ret;
}

CAstProcedure* CParser::procedureDecl(CAstScope* s)
{
	//
	// procedureDecl ::= "procedure" ident [ formalParam ] ";".
	//
	CToken t1;

	Consume(tProcedure, &t1);

	CToken t2 = _scanner->Get();
	if (t2.GetType() != tIdent)
		SetError(t2, "expected procedure identifier");

	const string& procedureName = t2.GetValue();
	if (s->GetSymbolTable()->FindSymbol(procedureName, sGlobal))
		SetError(t2, "re-declaration procedure \"" + procedureName + "\"");

	vector<string> noms;
	vector<CAstType*> parametresTypes;

	t2 = _scanner->Peek();
	switch (t2.GetType())
	{
	case tLParens:
		formalParam(noms, parametresTypes);
		break;
	case tSemicolon:
		break;
	default:
		SetError(t2, R"("(" or ";" expected)");
		break;
	}
	Consume(tSemicolon);

	auto* symbol =
		new CSymProc(procedureName, CTypeManager::Get()->GetNull());
	s->GetSymbolTable()->AddSymbol(symbol);

	auto* ret = new CAstProcedure(t1, procedureName, s, symbol);
	AddParameters(ret, symbol, noms, parametresTypes);

	return ret;
}

void CParser::formalParam(vector<string>& noms, vector<CAstType*>& types)
{
	//
	// formalParam = "(" [ varDeclSequence ] ")".
	//
	Consume(tLParens);

	CToken t = _scanner->Peek();
	if (t.GetType() == tIdent)
	{
		do
		{
			vector<string> liste;
			CAstType* ttype;
			varDecl(liste, ttype, noms);

			for (int i = 0; i < (int)liste.size(); i++)
				types.push_back(ttype);
			t = _scanner->Peek();
			if (t.GetType() == tRParens)
				break;
			else Consume(tSemicolon);
		} while (!_abort);
	}
	Consume(tRParens);
}

void CParser::AddParameters(CAstScope* s, CSymProc* symbol, vector<string>& noms, vector<CAstType*>& types)
{
	int cnt = 0;

	for (int i = 0; i < (int)noms.size(); i++)
	{
		string& nom = noms[i];
		CAstType* type = types[i];

		auto* param = new CSymParam(cnt++, nom, type->GetType());
		symbol->AddParam(param);
		s->GetSymbolTable()->AddSymbol(param);
	}
}

CAstConstant* CParser::boolean()
{
	//
	// boolean ::= "true" | "false".
	//
	CToken t;

	Consume(tBoolConst, &t);
	long long b = (t.GetValue() == "false") ? 0 : 1;

	return new CAstConstant(t, CTypeManager::Get()->GetBool(), b);
}

CAstConstant* CParser::character()
{
	//
	// char ::= "'" character "'".
	//

	CToken t;

	Consume(tCharConst, &t);
	long long ch = (long long)CToken::unescape(t.GetValue())[0];

	return new CAstConstant(t, CTypeManager::Get()->GetChar(), ch);
}

CAstStringConstant* CParser::stringConst(CAstScope* s)
{
	//
	// string := '"' { character } '"'.
	//
	CToken t;

	Consume(tStringConst, &t);

	return new CAstStringConstant(t, t.GetValue(), s);
}