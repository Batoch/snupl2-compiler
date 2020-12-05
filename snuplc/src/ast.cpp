//--------------------------------------------------------------------------------------------------
/// @brief SnuPL abstract syntax tree
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/05/22 Bernhard Egger reimplemented TAC generation
/// 2013/11/04 Bernhard Egger added typechecks for unary '+' operators
/// 2016/03/12 Bernhard Egger adapted to SnuPL/1
/// 2019/09/15 Bernhard Egger added support for constant expressions
/// 2020/08/11 Bernhard Egger adapted to SnuPL/2
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

#include <iostream>
#include <cassert>
#include <cstring>

#include <typeinfo>

#include "ast.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
// CAstNode
//
int CAstNode::_global_id = 0;

CAstNode::CAstNode(CToken token)
	: _token(token), _addr(NULL)
{
	_id = _global_id++;
}

CAstNode::~CAstNode(void)
{
	if (_addr != NULL) delete _addr;
}

int CAstNode::GetID(void) const
{
	return _id;
}

CToken CAstNode::GetToken(void) const
{
	return _token;
}

const CType* CAstNode::GetType(void) const
{
	return CTypeManager::Get()->GetNull();
}

string CAstNode::dotID(void) const
{
	ostringstream out;
	out << "node" << dec << _id;
	return out.str();
}

string CAstNode::dotAttr(void) const
{
	return " [label=\"" + dotID() + "\"]";
}

void CAstNode::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << dotID() << dotAttr() << ";" << endl;
}

CTacAddr* CAstNode::GetTacAddr(void) const
{
	return _addr;
}

ostream& operator<<(ostream &out, const CAstNode &t)
{
	return t.print(out);
}

ostream& operator<<(ostream &out, const CAstNode *t)
{
	return t->print(out);
}

//------------------------------------------------------------------------------
// CAstScope
//
CAstScope::CAstScope(CToken t, const string name, CAstScope *parent)
	: CAstNode(t), _name(name), _symtab(NULL), _parent(parent), _statseq(NULL), _cb(NULL)
{
	if (_parent != NULL) _parent->AddChild(this);
}

CAstScope::~CAstScope(void)
{
	delete _symtab;
	delete _statseq;
	delete _cb;
}

const string CAstScope::GetName(void) const
{
	return _name;
}

CAstScope* CAstScope::GetParent(void) const
{
	return _parent;
}

size_t CAstScope::GetNumChildren(void) const
{
	return _children.size();
}

CAstScope* CAstScope::GetChild(size_t i) const
{
	assert(i < _children.size());
	return _children[i];
}

bool CAstScope::RemoveChild(CAstScope* child)
{
	auto it = _children.begin();
	while ((it != _children.end()) && (*it != child)) it++;

	bool res = it != _children.end();

	if (res) _children.erase(it);
	return res;
}

CSymtab* CAstScope::GetSymbolTable(void) const
{
	assert(_symtab != NULL);
	return _symtab;
}

CSymbol* CAstScope::CreateConst(const string ident, const CType *type, const CDataInitializer *data)
{
	return new CSymConstant(ident, type, data);
}

void CAstScope::SetStatementSequence(CAstStatement *statseq)
{
	_statseq = statseq;
}

CAstStatement* CAstScope::GetStatementSequence(void) const
{
	return _statseq;
}

bool CAstScope::TypeCheck(CToken *t, string *msg) const
{
	bool result = true;
	try {
		CAstStatement* s = _statseq;
		while (result && (s != NULL)) {
			result = s->TypeCheck(t, msg);
			s = s->GetNext();
		}
		auto it = _children.begin();
		while (result && (it != _children.end())) {
			result = (*it)->TypeCheck(t, msg);
			it++;
		}
	}
	catch (...) {
		result = false;
	}
	return result;
}

ostream& CAstScope::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "CAstScope: '" << _name << "'" << endl;
	out << ind << "  symbol table:" << endl;
	_symtab->print(out, indent+4);
	out << ind << "  statement list:" << endl;
	CAstStatement *s = GetStatementSequence();
	if (s != NULL) {
		do {
			s->print(out, indent+4);
			s = s->GetNext();
		} while (s != NULL);
	} else {
		out << ind << "    empty." << endl;
	}

	out << ind << "  nested scopes:" << endl;
	if (_children.size() > 0) {
		for (size_t i=0; i<_children.size(); i++) {
			_children[i]->print(out, indent+4);
		}
	} else {
		out << ind << "    empty." << endl;
	}
	out << ind << endl;

	return out;
}

void CAstScope::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	CAstStatement *s = GetStatementSequence();
	if (s != NULL) {
		string prev = dotID();
		do {
			s->toDot(out, indent);
			out << ind << prev << " -> " << s->dotID() << " [style=dotted];" << endl;
			prev = s->dotID();
			s = s->GetNext();
		} while (s != NULL);
	}

	vector<CAstScope*>::const_iterator it = _children.begin();
	while (it != _children.end()) {
		CAstScope *s = *it++;
		s->toDot(out, indent);
		out << ind << dotID() << " -> " << s->dotID() << ";" << endl;
	}

}

CTacAddr* CAstScope::ToTac(CCodeBlock *cb)
{
	assert(cb != NULL);
	CAstStatement *s = GetStatementSequence();
	while (s != NULL) {
		CTacLabel *next = cb->CreateLabel();
		s->ToTac(cb, next);
		cb->AddInstr(next);
		s = s->GetNext();
	}

	cb->CleanupControlFlow();
	return NULL;
}

CCodeBlock* CAstScope::GetCodeBlock(void) const
{
	return _cb;
}

void CAstScope::SetSymbolTable(CSymtab *st)
{
	if (_symtab != NULL) delete _symtab;
	_symtab = st;
}

void CAstScope::AddChild(CAstScope *child)
{
	_children.push_back(child);
}


//--------------------------------------------------------------------------------------------------
// CAstModule
//
CAstModule::CAstModule(CToken t, const string name)
	: CAstScope(t, name, NULL)
{
	SetSymbolTable(new CSymtab());
}

CSymbol* CAstModule::CreateVar(const string ident, const CType *type)
{
	return new CSymGlobal(ident, type);
}

string CAstModule::dotAttr(void) const
{
	return " [label=\"m " + GetName() + "\",shape=box]";
}



//--------------------------------------------------------------------------------------------------
// CAstProcedure
//
CAstProcedure::CAstProcedure(CToken t, const string name, CAstScope *parent, CSymProc *symbol)
	: CAstScope(t, name, parent), _symbol(symbol)
{
	assert(GetParent() != NULL);
	SetSymbolTable(new CSymtab(GetParent()->GetSymbolTable()));
	assert(_symbol != NULL);
}

CSymProc* CAstProcedure::GetSymbol(void) const
{
	return _symbol;
}

CSymbol* CAstProcedure::CreateVar(const string ident, const CType *type)
{
	return new CSymLocal(ident, type);
}

const CType* CAstProcedure::GetType(void) const
{
	return GetSymbol()->GetDataType();
}

string CAstProcedure::dotAttr(void) const
{
	return " [label=\"p/f " + GetName() + "\",shape=box]";
}


//--------------------------------------------------------------------------------------------------
// CAstType
//
CAstType::CAstType(CToken t, const CType *type)
	: CAstNode(t), _type(type)
{
	assert(type != NULL);
}

const CType* CAstType::GetType(void) const
{
	return _type;
}

ostream& CAstType::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "CAstType (" << _type << ")" << endl;
	return out;
}


//--------------------------------------------------------------------------------------------------
// CAstStatement
//
CAstStatement::CAstStatement(CToken token)
	: CAstNode(token), _next(NULL)
{
}

CAstStatement::~CAstStatement(void)
{
	delete _next;
}

void CAstStatement::SetNext(CAstStatement *next)
{
	_next = next;
}

CAstStatement* CAstStatement::GetNext(void) const
{
	return _next;
}

CTacAddr* CAstStatement::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	// generate code for statement (assignment, if-else, etc.)

	// jump to next
	cb->AddInstr(new CTacInstr(opGoto, next));
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstStatAssign
//
CAstStatAssign::CAstStatAssign(CToken t, CAstDesignator *lhs, CAstExpression *rhs)
	: CAstStatement(t), _lhs(lhs), _rhs(rhs)
{
	assert(lhs != NULL);
	assert(rhs != NULL);
}

CAstDesignator* CAstStatAssign::GetLHS(void) const
{
	return _lhs;
}

CAstExpression* CAstStatAssign::GetRHS(void) const
{
	return _rhs;
}

bool CAstStatAssign::TypeCheck(CToken *t, string *msg) const
{
	CAstDesignator *lhs = GetLHS();
	CAstExpression *rhs = GetRHS();

	if (!lhs->TypeCheck(t, msg))
		return false;
	if (!rhs->TypeCheck(t, msg))
		return false;

	if (!lhs->GetType() || !lhs->GetType()->IsScalar()) //tester avec différentes variables pour voir les cas où ça retourne cette erreur
	{
		if (t != NULL)
			*t = lhs->GetToken();
		if (msg != NULL)
			*msg = "lhs type is not accepted."; // voir si on distingue les cas et on dit quel type on a ici
		return false;
	}

	if (!rhs->GetType() || !rhs->GetType()->IsScalar())
	{
		if (t != NULL)
			*t = rhs->GetToken();
		if (msg != NULL)
			*msg = "rhs' type is not accepted."; // voir si on distingue les cas et on dit quel type on a ici
		return false;
	}

	if (!lhs->GetType()->Match(rhs->GetType()))
	{
		if (t != NULL)
			*t = rhs->GetToken();
		if (msg != NULL)
			*msg = "Mismatch between lhs' type and rhs' type.";
		return false;
	}

	//est ce qu'il y a des types non valids ? à traiter !

	return true;
}

const CType* CAstStatAssign::GetType(void) const
{
	return _lhs->GetType();
}

ostream& CAstStatAssign::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << ":=" << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	_lhs->print(out, indent+2);
	_rhs->print(out, indent+2);

	return out;
}

string CAstStatAssign::dotAttr(void) const
{
	return " [label=\":=\",shape=box]";
}

void CAstStatAssign::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_lhs->toDot(out, indent);
	out << ind << dotID() << "->" << _lhs->dotID() << ";" << endl;
	_rhs->toDot(out, indent);
	out << ind << dotID() << "->" << _rhs->dotID() << ";" << endl;
}

CTacAddr* CAstStatAssign::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	cb->AddInstr(new CTacInstr(opAssign, _lhs->ToTac(cb), _rhs->ToTac(cb)));
	cb->AddInstr(new CTacInstr(opGoto, next));
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstStatCall
//
CAstStatCall::CAstStatCall(CToken t, CAstFunctionCall *call)
	: CAstStatement(t), _call(call)
{
	assert(call != NULL);
}

CAstFunctionCall* CAstStatCall::GetCall(void) const
{
	return _call;
}

bool CAstStatCall::TypeCheck(CToken *t, string *msg) const
{
	return GetCall()->TypeCheck(t, msg);
}

ostream& CAstStatCall::print(ostream &out, int indent) const
{
	_call->print(out, indent);

	return out;
}

string CAstStatCall::dotID(void) const
{
	return _call->dotID();
}

string CAstStatCall::dotAttr(void) const
{
	return _call->dotAttr();
}

void CAstStatCall::toDot(ostream &out, int indent) const
{
	_call->toDot(out, indent);
}

CTacAddr* CAstStatCall::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	int nbarg = _call->GetNArgs() - 1;

	// Tac for every arg
	for (int i = nbarg; i>=0; i++){
		CTacAddr *tacArg = _call->GetArg(i)->ToTac(cb);
		cb->AddInstr(new CTacInstr(opParam, new CTacConst(i, GetType()), tacArg, nullptr));
	}

	CTacTemp *tactmp = nullptr;
	if (_call->GetType() != CTypeManager::Get()->GetNull())
		tactmp = cb->CreateTemp(_call->GetType());
	cb->AddInstr(new CTacInstr(opCall, tactmp, new CTacName(_call->GetSymbol()), nullptr));
	cb->AddInstr(new CTacInstr(opGoto, next, nullptr, nullptr));

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstStatReturn
//
CAstStatReturn::CAstStatReturn(CToken t, CAstScope *scope, CAstExpression *expr)
	: CAstStatement(t), _scope(scope), _expr(expr)
{
	assert(scope != NULL);
}

CAstScope* CAstStatReturn::GetScope(void) const
{
	return _scope;
}

CAstExpression* CAstStatReturn::GetExpression(void) const
{
	return _expr;
}

bool CAstStatReturn::TypeCheck(CToken *t, string *msg) const
{
	const CType *st = GetScope()->GetType();
	CAstExpression *e = GetExpression();
	if (st->Match(CTypeManager::Get()->GetNull())) {
		if (e != NULL) {
			if (t != NULL) *t = e->GetToken();
			if (msg != NULL) *msg = "superfluous expression after return.";
			return false;
		}
	} else {
		if (e == NULL) {
			if (t != NULL) *t = GetToken();
			if (msg != NULL) *msg = "expression expected after return.";
			return false;
		}
		if (!e->TypeCheck(t, msg)) return false;
		if (!st->Match(e->GetType())) {
			if (t != NULL) *t = e->GetToken();
			if (msg != NULL) *msg = "return type mismatch.";
			return false;
		}
	}
	return true;
}

const CType* CAstStatReturn::GetType(void) const
{
	const CType *t = NULL;

	if (GetExpression() != NULL) {
		t = GetExpression()->GetType();
	} else {
		t = CTypeManager::Get()->GetNull();
	}

	return t;
}

ostream& CAstStatReturn::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "return" << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	if (_expr != NULL) _expr->print(out, indent+2);

	return out;
}

string CAstStatReturn::dotAttr(void) const
{
	return " [label=\"return\",shape=box]";
}

void CAstStatReturn::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	if (_expr != NULL) {
		_expr->toDot(out, indent);
		out << ind << dotID() << "->" << _expr->dotID() << ";" << endl;
	}
}

CTacAddr* CAstStatReturn::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	// can't name it return dam
	CTacAddr *retur = nullptr;
	if (GetExpression() != nullptr){
		retur = GetExpression()->ToTac(cb);
	}

	cb->AddInstr(new CTacInstr(opReturn, nullptr, retur, nullptr));
	cb->AddInstr(new CTacInstr(opGoto, next, nullptr, nullptr));
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstStatIf
//
CAstStatIf::CAstStatIf(CToken t, CAstExpression *cond,
	CAstStatement *ifBody, CAstStatement *elseBody)
	: CAstStatement(t), _cond(cond), _ifBody(ifBody), _elseBody(elseBody)
{
	assert(cond != NULL);
}

CAstExpression* CAstStatIf::GetCondition(void) const
{
	return _cond;
}

CAstStatement* CAstStatIf::GetIfBody(void) const
{
	return _ifBody;
}

CAstStatement* CAstStatIf::GetElseBody(void) const
{
	return _elseBody;
}

bool CAstStatIf::TypeCheck(CToken *t, string *msg) const
{
	CAstExpression* condition = GetCondition();
	CAstStatement* ifBody = GetIfBody();
	CAstStatement* elseBody = GetElseBody();

	if (!condition->TypeCheck(t, msg))
		return false;

	if (!condition->GetType() || !condition->GetType()->Match(CTypeManager::Get()->GetBool()))
	{
		if (t != NULL)
			*t = condition->GetToken();
		if (msg != NULL)
			*msg = "The Condition is not a boolean type.";// voir si on modifie le code pour faire apparaitre le type actuel
		return false;
	}

	while (ifBody)
	{
		if (!ifBody->TypeCheck(t, msg))
			return false;
		ifBody = ifBody->GetNext();
	}

	while (elseBody)
	{
		if (!elseBody->TypeCheck(t, msg))
			return false;
		elseBody = elseBody->GetNext();
	}

	return true;
}

ostream& CAstStatIf::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "if cond" << endl;
	_cond->print(out, indent+2);
	out << ind << "if-body" << endl;
	if (_ifBody != NULL) {
		CAstStatement *s = _ifBody;
		do {
			s->print(out, indent+2);
			s = s->GetNext();
		} while (s != NULL);
	} else out << ind << "  empty." << endl;
	out << ind << "else-body" << endl;
	if (_elseBody != NULL) {
		CAstStatement *s = _elseBody;
		do {
			s->print(out, indent+2);
			s = s->GetNext();
		} while (s != NULL);
	} else out << ind << "  empty." << endl;

	return out;
}

string CAstStatIf::dotAttr(void) const
{
	return " [label=\"if\",shape=box]";
}

void CAstStatIf::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_cond->toDot(out, indent);
	out << ind << dotID() << "->" << _cond->dotID() << ";" << endl;

	if (_ifBody != NULL) {
		CAstStatement *s = _ifBody;
		if (s != NULL) {
			string prev = dotID();
			do {
				s->toDot(out, indent);
				out << ind << prev << " -> " << s->dotID() << " [style=dotted];"
					<< endl;
				prev = s->dotID();
				s = s->GetNext();
			} while (s != NULL);
		}
	}

	if (_elseBody != NULL) {
		CAstStatement *s = _elseBody;
		if (s != NULL) {
			string prev = dotID();
			do {
				s->toDot(out, indent);
				out << ind << prev << " -> " << s->dotID() << " [style=dotted];"
					<< endl;
				prev = s->dotID();
				s = s->GetNext();
			} while (s != NULL);
		}
	}
}

CTacAddr* CAstStatIf::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	CTacLabel *True = cb->CreateLabel("lbl_true");
	CTacLabel *False = cb->CreateLabel("lbl_false");
	GetCondition()->ToTac(cb, True, False);


	cb->AddInstr(True);

	CAstStatement *ifBody = GetIfBody();
	// Parcours du corps du if
	while (ifBody) {
		CTacLabel *body = cb->CreateLabel();
		ifBody->ToTac(cb, body);
		ifBody = ifBody->GetNext();
		cb->AddInstr(body);
	}


	cb->AddInstr(new CTacInstr(opGoto, next, nullptr, nullptr));
	cb->AddInstr(False);


	CAstStatement *elseBody = GetElseBody();
	// Pareil avec le else
	while (elseBody) {
		CTacLabel *body = cb->CreateLabel();
		elseBody->ToTac(cb, body);
		elseBody = elseBody->GetNext();
		cb->AddInstr(body);
	}


	cb->AddInstr(new CTacInstr(opGoto, next, nullptr, nullptr));
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstStatWhile
//
CAstStatWhile::CAstStatWhile(CToken t, CAstExpression *cond, CAstStatement *body)
	: CAstStatement(t), _cond(cond), _body(body)
{
	assert(cond != NULL);
}

CAstExpression* CAstStatWhile::GetCondition(void) const
{
	return _cond;
}

CAstStatement* CAstStatWhile::GetBody(void) const
{
	return _body;
}

bool CAstStatWhile::TypeCheck(CToken *t, string *msg) const
{
	CAstExpression* condition = GetCondition();
	CAstStatement* body = GetBody();

	if (!condition->TypeCheck(t, msg))
		return false;

	if (!condition->GetType() || !condition->GetType()->Match(CTypeManager::Get()->GetBool()))
	{
		if (t != NULL)
			*t = condition->GetToken();
		if (msg != NULL)
			*msg = "The Condition is not a boolean type.";// voir si on modifie le code pour faire apparaitre le type actuel
		return false;
	}

	while (body != NULL)
	{
		if (!body->TypeCheck(t, msg))
			return false;
		body = body->GetNext();
	}
	return true;
}

ostream& CAstStatWhile::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "while cond" << endl;
	_cond->print(out, indent+2);
	out << ind << "while-body" << endl;
	if (_body != NULL) {
		CAstStatement *s = _body;
		do {
			s->print(out, indent+2);
			s = s->GetNext();
		} while (s != NULL);
	}
	else out << ind << "  empty." << endl;

	return out;
}

string CAstStatWhile::dotAttr(void) const
{
	return " [label=\"while\",shape=box]";
}

void CAstStatWhile::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_cond->toDot(out, indent);
	out << ind << dotID() << "->" << _cond->dotID() << ";" << endl;

	if (_body != NULL) {
		CAstStatement *s = _body;
		if (s != NULL) {
			string prev = dotID();
			do {
				s->toDot(out, indent);
				out << ind << prev << " -> " << s->dotID() << " [style=dotted];"
					<< endl;
				prev = s->dotID();
				s = s->GetNext();
			} while (s != NULL);
		}
	}
}

CTacAddr* CAstStatWhile::ToTac(CCodeBlock *cb, CTacLabel *next)
{
	// Very close to if statement, i try the same implementation as sawn in the computer architecture class
	CTacLabel *condition = cb->CreateLabel("lbl_condition");
	CTacLabel *corps = cb->CreateLabel("lbl_body");
	CAstStatement *whilebody = GetBody();

	cb->AddInstr(condition);
	GetCondition()->ToTac(cb, corps, next);

	cb->AddInstr(corps);
	while (whilebody != nullptr){
		CTacLabel *body = cb->CreateLabel();
		whilebody->ToTac(cb, body);
		whilebody = whilebody->GetNext();
		cb->AddInstr(body);
	}
	cb->AddInstr(new CTacInstr(opGoto, condition, nullptr, nullptr));
	cb->AddInstr(new CTacInstr(opGoto, next, nullptr, nullptr));

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstExpression
//
CAstExpression::CAstExpression(CToken t)
	: CAstNode(t)
{
}

void CAstExpression::SetParenthesized(bool parenthesized)
{
	_parenthesized = parenthesized;
}

bool CAstExpression::GetParenthesized(void) const
{
	return _parenthesized;
}

const CDataInitializer* CAstExpression::Evaluate(void) const
{
	return NULL;
}

CTacAddr* CAstExpression::ToTac(CCodeBlock *cb)
{
	return NULL;
}

CTacAddr* CAstExpression::ToTac(CCodeBlock *cb,
	CTacLabel *ltrue, CTacLabel *lfalse)
{
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstOperation
//
CAstOperation::CAstOperation(CToken t, EOperation oper)
	: CAstExpression(t), _oper(oper)
{
}

EOperation CAstOperation::GetOperation(void) const
{
	return _oper;
}


//--------------------------------------------------------------------------------------------------
// CAstBinaryOp
//
CAstBinaryOp::CAstBinaryOp(CToken t, EOperation oper, CAstExpression *l,CAstExpression *r)
	: CAstOperation(t, oper), _left(l), _right(r)
{
	// these are the only binary operation we support for now
	assert((oper == opAdd)        || (oper == opSub)         ||
		(oper == opMul)        || (oper == opDiv)         ||
		(oper == opAnd)        || (oper == opOr)          ||
		(oper == opEqual)      || (oper == opNotEqual)    ||
		(oper == opLessThan)   || (oper == opLessEqual)   ||
		(oper == opBiggerThan) || (oper == opBiggerEqual)
	);
	assert(l != NULL);
	assert(r != NULL);
}

CAstExpression* CAstBinaryOp::GetLeft(void) const
{
	return _left;
}

CAstExpression* CAstBinaryOp::GetRight(void) const
{
	return _right;
}

bool CAstBinaryOp::TypeCheck(CToken *t, string *msg) const
{
	CAstExpression* leftTerm = GetLeft();
	CAstExpression* rightTerm = GetRight();
	EOperation operation = GetOperation();

	if (!leftTerm->TypeCheck(t, msg))
		return false;
	if (!rightTerm->TypeCheck(t, msg))
		return false;

	if (!leftTerm->GetType() || !leftTerm->GetType()->IsScalar() || !leftTerm->GetType()->IsPointer())
	{
		if (t != NULL)
			*t = leftTerm->GetToken();
		if (msg != NULL)
			*msg = "The left term is not a scalar type or is a pointer.";// voir si on modifie le code pour faire apparaitre le type actuel
		return false;
	}

	if (!rightTerm->GetType() || !rightTerm->GetType()->IsScalar() || !rightTerm->GetType()->IsPointer())
	{
		if (t != NULL)
			*t = rightTerm->GetToken();
		if (msg != NULL)
			*msg = "The right term is not a scalar type or is a pointer."; // voir si on modifie le code pour faire apparaitre le type actuel
		return false;
	}

	if (!leftTerm->GetType()->Match(rightTerm->GetType()))
	{
		if (t != NULL)
			*t = GetToken();
		if (msg != NULL)
			*msg = "The right term is not a scalar type or is a pointer."; // voir si on modifie le code pour faire apparaitre le type actuel
		return false;
	}

	if (operation == opAdd || operation == opSub || operation == opMul || operation == opDiv)
	{
		if (!leftTerm->GetType()->Match(CTypeManager::Get()->GetInteger()) && !leftTerm->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = leftTerm->GetToken();
			if (msg != NULL)
				*msg = "Left term should be integer.";
			return false;
		}
		else if (!rightTerm->GetType()->Match(CTypeManager::Get()->GetInteger()) && !rightTerm->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = rightTerm->GetToken();
			if (msg != NULL)
				*msg = "Right term should be integer.";
			return false;
		}
	}

	if (operation == opLessThan || operation == opLessEqual || operation == opBiggerEqual || operation == opBiggerThan)
	{
		if (!leftTerm->GetType()->Match(CTypeManager::Get()->GetInteger()) && !leftTerm->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = leftTerm->GetToken();
			if (msg != NULL)
				*msg = "Left term should be integer.";
			return false;
		}
		else if (!rightTerm->GetType()->Match(CTypeManager::Get()->GetInteger()) && !rightTerm->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = rightTerm->GetToken();
			if (msg != NULL)
				*msg = "Right term should be integer.";
			return false;
		}
	}

	if (operation == opAnd || operation == opOr)
	{
		if (!leftTerm->GetType()->Match(CTypeManager::Get()->GetBool()))
		{
			if (t != NULL)
				*t = *t = leftTerm->GetToken();
			if (msg != NULL)
				*msg = "Left term should be boolean.";
			return false;
		}
		else if (!rightTerm->GetType()->Match(CTypeManager::Get()->GetBool()))
		{
			if (t != NULL)
				*t = rightTerm->GetToken();
			if (msg != NULL)
				*msg = "Right term should be boolean.";
			return false;
		}
	}
	return true;
}

const CType* CAstBinaryOp::GetType(void) const
{
//	// binary operators
//	// dst = src1 op src2
//	opAdd=0,                          ///< +  addition
//		opSub,                            ///< -  subtraction
//		opMul,                            ///< *  multiplication
//		opDiv,                            ///< /  division
//		opAnd,                            ///< && binary and
//		opOr,                             ///< || binary or

	const CType* result = NULL;
	EOperation operati = GetOperation();

	switch (operati)
	{
	case opAdd:
	case opSub:
	case opMul:
	case opDiv:
		result = CTypeManager::Get()->GetInteger();
		break;
	case opAnd:
	case opOr:
	case opNot:
	case opEqual:
	case opNotEqual:
	case opLessThan:
	case opLessEqual:
	case opBiggerThan:
	case opBiggerEqual:
		result = CTypeManager::Get()->GetBool();
		break;
	}

	return result;
}

const CDataInitializer* CAstBinaryOp::Evaluate(void) const
{
//	if (GetType()->IsBoolean())	return new CDataInitBoolean((bool) GetData());

	// TODO (phase 3)

	return NULL;
}

ostream& CAstBinaryOp::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << GetOperation() << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	_left->print(out, indent+2);
	_right->print(out, indent+2);

	return out;
}

string CAstBinaryOp::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << GetOperation() << "\",shape=box]";
	return out.str();
}

void CAstBinaryOp::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_left->toDot(out, indent);
	out << ind << dotID() << "->" << _left->dotID() << ";" << endl;
	_right->toDot(out, indent);
	out << ind << dotID() << "->" << _right->dotID() << ";" << endl;
}

CTacAddr* CAstBinaryOp::ToTac(CCodeBlock *cb)
{
	EOperation op = GetOperation();
//	CTypeManager *tm = CTypeManager::Get();

	// binary operators
	//		opAdd,                            ///< +  addition
	//		opSub,                            ///< -  subtraction
	//		opMul,                            ///< *  multiplication
	//		opDiv,                            ///< /  division
	//		opAnd,                            ///< && binary and
	//		opOr,                             ///< || binary or

	switch (op)
	{
	case opAdd:
	case opSub:
	case opMul:
	case opDiv:
		CTacTemp *dest = cb->CreateTemp(CTypeManager::Get()->GetInteger());
		cb->AddInstr(new CTacInstr(op, dest, GetLeft()->ToTac(cb), GetRight()->ToTac(cb)));
		return dest;
		break;
	}
	// Bool type

	CTacLabel *lbl_false = cb->CreateLabel();
	CTacLabel *lbl_true = cb->CreateLabel();
	CTacLabel *lbl_end = cb->CreateLabel();

	ToTac(cb, lbl_true, lbl_false);
	CTacTemp *result = cb->CreateTemp(CTypeManager::Get()->GetBool());

	cb->AddInstr(lbl_true);
	// If true
	cb->AddInstr(new CTacInstr(opAssign, result, new CTacConst(1, CTypeManager::Get()->GetBool()), nullptr));
	cb->AddInstr(new CTacInstr(opGoto, lbl_end, nullptr, nullptr));

	cb->AddInstr(lbl_false);
	// If false
	cb->AddInstr(new CTacInstr(opAssign, result, new CTacConst(0, CTypeManager::Get()->GetBool()), nullptr));
	cb->AddInstr(new CTacInstr(opGoto, lbl_end, nullptr, nullptr));			// Useless

	cb->AddInstr(lbl_end);
	return result;
}

CTacAddr* CAstBinaryOp::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	EOperation op = GetOperation();
	CTacLabel *test_b = cb->CreateLabel();

	if (!IsRelOp(op)) {
		if (op == opAnd){
			GetRight()->ToTac(cb, test_b, lfalse);
			cb->AddInstr(test_b);
			GetLeft()->ToTac(cb, ltrue, lfalse);
		}
		else {	//If it is opOr
			GetRight()->ToTac(cb, ltrue, test_b);
			cb->AddInstr(test_b);
			GetLeft()->ToTac(cb, ltrue, lfalse);
		}
	}
	else{
		cb->AddInstr(new CTacInstr(op, ltrue, GetLeft()->ToTac(cb), GetRight()->ToTac(cb)));
		cb->AddInstr(new CTacInstr(opGoto, lfalse));
	}

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstUnaryOp
//
CAstUnaryOp::CAstUnaryOp(CToken t, EOperation oper, CAstExpression *e)
	: CAstOperation(t, oper), _operand(e)
{
	assert((oper == opNeg) || (oper == opPos) || (oper == opNot));
	assert(e != NULL);
}

CAstExpression* CAstUnaryOp::GetOperand(void) const
{
	return _operand;
}

bool CAstUnaryOp::TypeCheck(CToken *t, string *msg) const
{
	CAstExpression* operand = GetOperand();
	EOperation operation = GetOperation();

	if (!operand->TypeCheck(t, msg))
		return false;
	if (operation == opNot)
	{
		if (!operand->GetType()->Match(CTypeManager::Get()->GetBool()))
		{
			if (t != NULL)
				*t = operand->GetToken();
			if (msg != NULL)
				*msg = "The operand should be a boolean.";
			return false;
		}
	}
	else
	{
		if (!operand->GetType()->Match(CTypeManager::Get()->GetInteger()) && !operand->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = operand->GetToken();
			if (msg != NULL)
				*msg = "The operand should be an integer.";
			return false;
		}
	}
	return true;
}

const CType* CAstUnaryOp::GetType(void) const
{
//	// unary operators
//	// dst = op src1
//	opNeg,                            ///< -  negation
//	opPos,                            ///< +  unary +
//	opNot,                            ///< !  binary not
	const CType* result = NULL;
	EOperation operati = GetOperation();

	switch (operati)
	{
	case opNeg:
	case opPos:
		result = CTypeManager::Get()->GetInteger();
		break;
	case opNop:
		result = CTypeManager::Get()->GetBool();
		break;
	}
	return result;
}

const CDataInitializer* CAstUnaryOp::Evaluate(void) const
{
	EOperation operati = GetOperation();
	switch (operati)
	{
	case opNeg:
		// We should use FoldNeg
		return new CDataInitLongint(-reinterpret_cast<long>(GetOperand()));
	case opPos:
		return new CDataInitLongint(reinterpret_cast<long>(GetOperand()));
	case opNop:
		return NULL;
	}
	return NULL;
}

ostream& CAstUnaryOp::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << GetOperation() << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";
	out << endl;

	_operand->print(out, indent+2);

	return out;
}

string CAstUnaryOp::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << GetOperation() << "\",shape=box]";
	return out.str();
}

void CAstUnaryOp::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_operand->toDot(out, indent);
	out << ind << dotID() << "->" << _operand->dotID() << ";" << endl;
}

CTacAddr* CAstUnaryOp::ToTac(CCodeBlock *cb)
{
	EOperation oper = GetOperation();
	CTypeManager* type = CTypeManager::Get();

	CTacTemp *valeur = NULL;

	if (oper == opPos || oper == opNeg) {
		CAstConstant* number = dynamic_cast<CAstConstant*>(GetOperand());

		if (number == NULL) {
			CTacAddr* operandTac = GetOperand()->ToTac(cb);
			valeur = cb->CreateTemp(type->GetInteger());
			cb->AddInstr(new CTacInstr(oper, valeur, operandTac, NULL));
		}
		else {
			long long valeur = number->GetValue();
			if (oper == opNeg)
				valeur = -valeur;
			CTacConst* numTac = new CTacConst(valeur, GetType());
			return numTac;
		}
	}
	else {
		CTacLabel* ltrue = cb->CreateLabel();
		CTacLabel* lfalse = cb->CreateLabel();
		CTacLabel* lend = cb->CreateLabel();
		ToTac(cb, ltrue, lfalse);

		valeur = cb->CreateTemp(type->GetBool());

		cb->AddInstr(ltrue);
		cb->AddInstr(new CTacInstr(opAssign, valeur, new CTacConst(1, GetType()), NULL));
		cb->AddInstr(new CTacInstr(opGoto, lend, NULL, NULL));

		cb->AddInstr(lfalse);
		cb->AddInstr(new CTacInstr(opAssign, valeur, new CTacConst(0, GetType()), NULL));
		cb->AddInstr(lend);
	}
	return valeur;
 }

CTacAddr* CAstUnaryOp::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	EOperation oper = GetOperation();
	CAstExpression* operand = GetOperand();

	if (oper == opNot) {
		operand->ToTac(cb, lfalse, ltrue);
	}

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstSpecialOp
//
CAstSpecialOp::CAstSpecialOp(CToken t, EOperation oper, CAstExpression *e, const CType *type)
	: CAstOperation(t, oper), _operand(e), _type(type)
{
	assert((oper == opAddress) || (oper == opDeref) || (oper = opCast));
	assert(e != NULL);
	assert(((oper != opCast) && (type == NULL)) ||
		((oper == opCast) && (type != NULL)));
}

CAstExpression* CAstSpecialOp::GetOperand(void) const
{
	return _operand;
}

bool CAstSpecialOp::TypeCheck(CToken *t, string *msg) const
{
	EOperation operation = GetOperation();
	CAstExpression *operand = GetOperand();

	if (!operand->TypeCheck(t, msg))
		return false;
	if (!operand->GetType())
	{
		if (t != NULL)
			*t = GetToken();
		if (msg != NULL)
			*msg = "Operand null.";
		return false;
	}
	if (operation == opDeref)
	{
		if (!operand->GetType()->IsPointer())
		{
			if (t != NULL)
				*t = GetToken();
			if (msg != NULL)
				*msg = "The operand should has a pointer type.";
			return false;
		}
	}
	return true;
}

const CType* CAstSpecialOp::GetType(void) const
{
//	// special and pointer operations
//	opAddress,                        ///< reference: dst = &src1
//	opDeref,                          ///< dereference: dst = *src1
//	opCast,                           ///< type cast: dst = (type)src1
	const CType* result = NULL;
	EOperation operati = GetOperation();
	if (GetOperand()->GetType()->IsNull())	return result;
	switch (operati)
	{
	case opAddress:
		result = CTypeManager::Get()->GetPointer(GetOperand()->GetType());
	case opDeref:
		if (!GetOperand()->GetType()->IsPointer())
			break;
		result = dynamic_cast<const CPointerType*>(GetOperand()->GetType())->GetBaseType();
		break;
	case opCast:
		result = _type;
		break;
	}
	return result;
}

ostream& CAstSpecialOp::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << GetOperation() << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";
	out << endl;

	_operand->print(out, indent+2);

	return out;
}

string CAstSpecialOp::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << GetOperation() << "\",shape=box]";
	return out.str();
}

void CAstSpecialOp::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	_operand->toDot(out, indent);
	out << ind << dotID() << "->" << _operand->dotID() << ";" << endl;
}

CTacAddr* CAstSpecialOp::ToTac(CCodeBlock *cb)
{
	CTacAddr *src = GetOperand()->ToTac(cb);
	CTacTemp *dst = cb->CreateTemp(CTypeManager::Get()->GetPointer(GetOperand()->GetType()));

	cb->AddInstr(new CTacInstr(opAddress, dst, src, nullptr));

	return dst;
}


//--------------------------------------------------------------------------------------------------
// CAstFunctionCall
//
CAstFunctionCall::CAstFunctionCall(CToken t, const CSymProc *symbol)
	: CAstExpression(t), _symbol(symbol)
{
	assert(symbol != NULL);
}

const CSymProc* CAstFunctionCall::GetSymbol(void) const
{
	return _symbol;
}

void CAstFunctionCall::AddArg(CAstExpression *arg)
{
	// TODO (phase 3)

	_arg.push_back(arg);
}

unsigned int CAstFunctionCall::GetNArgs(void) const
{
	return _arg.size();
}

CAstExpression* CAstFunctionCall::GetArg(unsigned int index) const
{
	assert((index >= 0) && (index < _arg.size()));
	return _arg[index];
}

bool CAstFunctionCall::TypeCheck(CToken *t, string *msg) const
{
	const CSymProc* proc = GetSymbol();
	int nb_Args = GetNArgs();
	int nb_Params = proc->GetNParams();

	if (nb_Args != nb_Params)
	{
		if (t != NULL)
			*t = GetToken();
		if (msg != NULL)
			*msg = "Not the good number of arguments.";
		return false;
	}

	for (int i = 0; i < nb_Args; i++)
	{
		CAstExpression* expression = GetArg(i);
		const CType* parameterType = proc->GetParam(i)->GetDataType();
		if (expression == NULL)
		{
			if (t != NULL)
				*t = GetToken();
			if (msg != NULL)
				*msg = "The argument is not valid.";
			return false;
		}
		else
		{
			if (!expression->TypeCheck(t, msg))
				return false;
			if (!expression->GetType() || !parameterType || !parameterType->Match(expression->GetType()))
			{
				if (t != NULL)
					*t = GetToken();
				if (msg != NULL)
					*msg = "The parameters don't match.";
				return false;
			}
		}
	}
	return true;
}

const CType* CAstFunctionCall::GetType(void) const
{
	return GetSymbol()->GetDataType();
}

ostream& CAstFunctionCall::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << "call " << _symbol << " ";
	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";
	out << endl;

	for (size_t i=0; i<_arg.size(); i++) {
		_arg[i]->print(out, indent+2);
	}

	return out;
}

string CAstFunctionCall::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"call " << _symbol->GetName() << "\",shape=box]";
	return out.str();
}

void CAstFunctionCall::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	for (size_t i=0; i<_arg.size(); i++) {
		_arg[i]->toDot(out, indent);
		out << ind << dotID() << "->" << _arg[i]->dotID() << ";" << endl;
	}
}

CTacAddr* CAstFunctionCall::ToTac(CCodeBlock *cb)
{
	int n = GetNArgs();

	for (int i = n - 1; i >= 0; i--) {
		CTacAddr *argument = GetArg(i)->ToTac(cb);
		cb->AddInstr(new CTacInstr(opParam, new CTacConst(i, GetType()), argument, NULL));
	}

	CTacTemp* valeurs = cb->CreateTemp(GetType());
	cb->AddInstr(new CTacInstr(opCall, valeurs, new CTacName(GetSymbol()), NULL));

	return valeurs;
}

CTacAddr* CAstFunctionCall::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	CTacAddr* valeur = ToTac(cb);

	cb->AddInstr(new CTacInstr(opEqual, ltrue, valeur, new CTacConst(1, GetType())));
	cb->AddInstr(new CTacInstr(opGoto, lfalse, NULL, NULL));

	return NULL;
}



//--------------------------------------------------------------------------------------------------
// CAstOperand
//
CAstOperand::CAstOperand(CToken t)
	: CAstExpression(t)
{
}


//--------------------------------------------------------------------------------------------------
// CAstDesignator
//
CAstDesignator::CAstDesignator(CToken t, const CSymbol *symbol)
	: CAstOperand(t), _symbol(symbol)
{
	assert(symbol != NULL);
}

const CSymbol* CAstDesignator::GetSymbol(void) const
{
	return _symbol;
}

bool CAstDesignator::TypeCheck(CToken *t, string *msg) const
{
	if (GetType() == NULL || GetType()->IsNull())
	{
		if (t != NULL)
			*t = GetToken();
		if (msg != NULL)
			*msg = "Designator is NULL Type.";
		return false;
	}
	return true;
}

const CType* CAstDesignator::GetType(void) const
{
	return GetSymbol()->GetDataType();
}

const CDataInitializer* CAstDesignator::Evaluate(void) const
{
//	if (GetType()->IsInt())
//		return new CDataInitInteger((int)(GetSymbol()->GetData()));

	// TODO (phase 3)

	return NULL;
}

ostream& CAstDesignator::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << _symbol << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	return out;
}

string CAstDesignator::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << _symbol->GetName();
	out << "\",shape=ellipse]";
	return out.str();
}

void CAstDesignator::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);
}

CTacAddr* CAstDesignator::ToTac(CCodeBlock *cb)
{

	return new CTacName(GetSymbol());
}

CTacAddr* CAstDesignator::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	CTacAddr* valeur = ToTac(cb);

	cb->AddInstr(new CTacInstr(opEqual, ltrue, valeur, new CTacConst(1, GetType())));
	cb->AddInstr(new CTacInstr(opGoto, lfalse, NULL, NULL));

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstArrayDesignator
//
CAstArrayDesignator::CAstArrayDesignator(CToken t, const CSymbol *symbol)
	: CAstDesignator(t, symbol), _done(false), _offset(NULL)
{
}

void CAstArrayDesignator::AddIndex(CAstExpression *idx)
{
	assert(!_done);
	_idx.push_back(idx);
}

void CAstArrayDesignator::IndicesComplete(void)
{
	assert(!_done);
	_done = true;
}

unsigned CAstArrayDesignator::GetNIndices(void) const
{
	return _idx.size();
}

CAstExpression* CAstArrayDesignator::GetIndex(unsigned int index) const
{
	assert((index >= 0) && (index < _idx.size()));
	return _idx[index];
}

bool CAstArrayDesignator::TypeCheck(CToken *t, string *msg) const
{
	bool result = true;

	assert(_done);

	int nb_indices = GetNIndices();

	for (int i = 0; i < nb_indices; i++)
	{
		CAstExpression* expression = GetIndex(i);

		if (!expression->TypeCheck(t, msg))
			return false;
		if (!expression->GetType())
		{
			if (t != NULL)
				*t = expression->GetToken();
			if (msg != NULL)
				*msg = "The expression is NULL.";
			return false;
		}
		if (!expression->GetType()->Match(CTypeManager::Get()->GetInteger()) && !expression->GetType()->Match(CTypeManager::Get()->GetLongint()))
		{
			if (t != NULL)
				*t = expression->GetToken();
			if (msg != NULL)
				*msg = "The expression should be an integer or a longint.";
			return false;
		}
	}
	return result;
}

const CType* CAstArrayDesignator::GetType(void) const
{
	const CType *type = GetSymbol()->GetDataType();
	// If it is a pointer, then dereferencing it
	if (type->IsPointer())
		type = dynamic_cast<const CPointerType*>(type)->GetBaseType();

	if (!type->IsArray())
		return NULL;
	// If nbarg > dim array
	if (GetNIndices() > dynamic_cast<const CArrayType*>(type)->GetNDim())
		return NULL;

	// Handle multiple dim arrays
	for (int i = 0; i < GetNIndices(); i++) {
		if (!type->IsArray()) return NULL;
		type = dynamic_cast<const CArrayType*> (type)->GetInnerType();
	}
	return type;
}

ostream& CAstArrayDesignator::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << _symbol << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	for (size_t i=0; i<_idx.size(); i++) {
		_idx[i]->print(out, indent+2);
	}

	return out;
}

string CAstArrayDesignator::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << _symbol->GetName() << "[]\",shape=ellipse]";
	return out.str();
}

void CAstArrayDesignator::toDot(ostream &out, int indent) const
{
	string ind(indent, ' ');

	CAstNode::toDot(out, indent);

	for (size_t i=0; i<_idx.size(); i++) {
		_idx[i]->toDot(out, indent);
		out << ind << dotID() << "-> " << _idx[i]->dotID() << ";" << endl;
	}
}

CTacAddr* CAstArrayDesignator::ToTac(CCodeBlock* cb)
{
	CSymtab* symbolTable = cb->GetOwner()->GetSymbolTable();
	CToken t;
	CTypeManager* typeManager = CTypeManager::Get();

	const CSymbol* DOFS_SYM = symbolTable->FindSymbol("DOFS");

	const CSymbol* DIM_SYM = symbolTable->FindSymbol("DIM");
	CAstConstant* DIM_VAL = new CAstConstant(t, typeManager->GetInteger(), 0);

	CAstExpression* indice_Expr = new CAstDesignator(GetToken(), GetSymbol());
	CTacAddr* id = new CTacName(GetSymbol());
	const CArrayType* dataType;

	if (GetSymbol()->GetDataType()->IsPointer()) {
		dataType = dynamic_cast<const CArrayType*>(dynamic_cast<const CPointerType*> (GetSymbol()->GetDataType())->GetBaseType());
	}
	else {
		CTacTemp* ptr = cb->CreateTemp(typeManager->GetPointer(GetSymbol()->GetDataType()));
		cb->AddInstr(new CTacInstr(opAddress, ptr, id, NULL));
		id = ptr;

		indice_Expr = new CAstSpecialOp(GetToken(), opAddress, indice_Expr, NULL);

		dataType = dynamic_cast<const CArrayType*>(GetSymbol()->GetDataType());
	}

	int dataSize = dataType->GetBaseType()->GetSize();

	CTacAddr* index = NULL;
	int count = dataType->GetNDim();
	for (int i = 0; i < count; i++) {
		if (!index) {
			index = GetIndex(i)->ToTac(cb);
		}
		else
		{
			CTacAddr* precedent = new CTacConst(0, GetType());

			if (i < GetNIndices()) {
				precedent = GetIndex(i)->ToTac(cb);
			}

			CTacAddr* suivant = cb->CreateTemp(typeManager->GetInteger());
			cb->AddInstr(new CTacInstr(opAdd, suivant, index, precedent));
			index = suivant;
		}
		if (i == count - 1)
			break;

		CAstFunctionCall* DIM_FUN = new CAstFunctionCall(t, dynamic_cast<const CSymProc*>(DIM_SYM));

		DIM_FUN->AddArg(indice_Expr);
		DIM_VAL = new CAstConstant(t, typeManager->GetInteger(), i + 2);
		DIM_FUN->AddArg(DIM_VAL);
		CTacAddr* tailleEntrees = DIM_FUN->ToTac(cb);

		CTacAddr* suivant = cb->CreateTemp(typeManager->GetInteger());
		cb->AddInstr(new CTacInstr(opMul, suivant, index, tailleEntrees));
		index = suivant;
	}
	CAstFunctionCall* DOFS_FUN = new CAstFunctionCall(t, dynamic_cast<const CSymProc*>(DOFS_SYM));
	DOFS_FUN->AddArg(indice_Expr);
	CTacAddr* ofs = DOFS_FUN->ToTac(cb);

	CTacTemp* temporaire = cb->CreateTemp(typeManager->GetInteger());
	cb->AddInstr(new CTacInstr(opMul, temporaire, index, new CTacConst(dataSize, GetType())));
	index = temporaire;

	temporaire = cb->CreateTemp(typeManager->GetInteger());
	cb->AddInstr(new CTacInstr(opAdd, temporaire, index, ofs));
	index = temporaire;

	temporaire = cb->CreateTemp(typeManager->GetInteger());
	cb->AddInstr(new CTacInstr(opAdd, temporaire, id, index));

	return new CTacReference(temporaire->GetSymbol(), nullptr);
}

CTacAddr* CAstArrayDesignator::ToTac(CCodeBlock* cb,
	CTacLabel* ltrue, CTacLabel* lfalse)
{
	CTacAddr* valeur = ToTac(cb);

	cb->AddInstr(new CTacInstr(opEqual, ltrue, valeur, new CTacConst(1, GetType())));
	cb->AddInstr(new CTacInstr(opGoto, lfalse, NULL, NULL));

	return NULL;
}


//--------------------------------------------------------------------------------------------------
// CAstConstant
//
CAstConstant::CAstConstant(CToken t, const CType *type, long long value)
	: CAstOperand(t), _type(type), _value(value), _negated(false)
{
}

void CAstConstant::FoldNeg(void)
{
	_value = -_value;

	// The _negated flag indicates that the value has been negated.
	//
	// All constants in SnulPL/2 are positive numbers that can be folded (negated) by the '-' sign
	// of simpleexpr. To allow LLONG_MIN but still detect invalid positive longint constants with
	// the value LLONG_MAX+1, we need to keep track of whether the value has been folded (negated)
	// or not. The value LLONG_MAX+1, when set via the constructor as a long long becomes LLONG_MIN,
	// but will not have the _negated flag set.
	//
	// Of course, this is nitpicking, but exactness comes at a price...
	//
	_negated = !_negated;
}

long long CAstConstant::GetValue(void) const
{
	return _value;
}

string CAstConstant::GetValueStr(void) const
{
	ostringstream out;

	if (GetType() == CTypeManager::Get()->GetBool()) {
		out << (_value == 0 ? "false" : "true");
	} else {
		out << dec << _value;
	}

	return out.str();
}

bool CAstConstant::TypeCheck(CToken *t, string *msg) const
{
	const CType* type = GetType();
	if (type == NULL || type->IsNull())
	{
		if (t != NULL)
			*t = GetToken();
		if (msg != NULL)
			*msg = "The type of the constant is NULL.";
		return false;
	}
	return true;
}

const CType* CAstConstant::GetType(void) const
{
	return _type;
}

const CDataInitializer* CAstConstant::Evaluate(void) const
{
	if (_type->IsLongint()) return new CDataInitLongint((long long)GetValue());
	if (_type->IsInteger()) return new CDataInitInteger((int)GetValue());
	if (_type->IsBoolean()) return new CDataInitBoolean((bool)GetValue());
	if (_type->IsChar()) return new CDataInitChar((char)GetValue());
	return NULL;
}

ostream& CAstConstant::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << GetValueStr() << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	return out;
}

string CAstConstant::dotAttr(void) const
{
	ostringstream out;
	out << " [label=\"" << GetValueStr() << "\",shape=ellipse]";
	return out.str();
}

CTacAddr* CAstConstant::ToTac(CCodeBlock *cb)
{
	return new CTacConst(GetValue(), GetType());
}

CTacAddr* CAstConstant::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	long long condition = GetValue();
	if (condition)
		cb->AddInstr(new CTacInstr(opGoto, ltrue, NULL, NULL));
	else
		cb->AddInstr(new CTacInstr(opGoto, lfalse, NULL, NULL));

  return ToTac(cb);
}


//--------------------------------------------------------------------------------------------------
// CAstStringConstant
//
int CAstStringConstant::_idx = 0;

CAstStringConstant::CAstStringConstant(CToken t, const string value, CAstScope *s)
	: CAstOperand(t)
{
	CTypeManager *tm = CTypeManager::Get();
	CSymtab *st = s->GetSymbolTable();

	_type = tm->GetArray(strlen(CToken::unescape(value).c_str())+1,
		tm->GetChar());
	_value = new CDataInitString(value);

	// in case of name clashes we simply iterate until we find a
	// name that has not yet been used
	_sym = NULL;
	do {
		ostringstream o;
		o << "_str_" << ++_idx;
		if (st->FindSymbol(o.str(), sGlobal) == NULL) {
			_sym = new CSymGlobal(o.str(), _type);
		}
	} while (_sym == NULL);

	_sym->SetData(_value);
	st->AddSymbol(_sym);
}

const string CAstStringConstant::GetValue(void) const
{
	return _value->GetData();
}

const string CAstStringConstant::GetValueStr(void) const
{
	return GetValue();
}

bool CAstStringConstant::TypeCheck(CToken *t, string *msg) const
{
	return true;
}

const CType* CAstStringConstant::GetType(void) const
{
	return _type;
}

const CDataInitializer* CAstStringConstant::Evaluate(void) const
{
	return new CDataInitString(GetValue());
}

ostream& CAstStringConstant::print(ostream &out, int indent) const
{
	string ind(indent, ' ');

	out << ind << '"' << GetValueStr() << '"' << " ";

	const CType *t = GetType();
	if (t != NULL) out << t; else out << "<INVALID>";

	out << endl;

	return out;
}

string CAstStringConstant::dotAttr(void) const
{
	ostringstream out;
	// the string is already escaped, but dot requires double escaping
	out << " [label=\"\\\"" << CToken::escape(tStringConst, GetValueStr())
		<< "\\\"\",shape=ellipse]";
	return out.str();
}

CTacAddr* CAstStringConstant::ToTac(CCodeBlock *cb)
{
	return new CTacName(_sym);
}

CTacAddr* CAstStringConstant::ToTac(CCodeBlock *cb, CTacLabel *ltrue, CTacLabel *lfalse)
{
	return ToTac(cb);
}
