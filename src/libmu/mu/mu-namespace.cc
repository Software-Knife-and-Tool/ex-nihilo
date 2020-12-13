/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** mu-namespace.cc: library namespace functions
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Exception = core::Exception;
using Frame = core::Env::Frame;
using Namespace = core::Namespace;
using String = core::String;
using Symbol = core::Symbol;
using Type = core::Type;

/** * (namespacep object) => bool  **/
void IsNamespace(Frame* fp) {
  fp->value = Type::Bool(Namespace::IsType(fp->argv[0]));
}

/** * (name-ns namespace) => string **/
void NameOfNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "ns-name",
                     ns);

  fp->value = Namespace::name(fp->argv[0]);
}

/** * (name-ns namespace) => string **/
void ImportOfNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "ns-import",
                     ns);

  fp->value = Namespace::import(fp->argv[0]);
}

/** * (ns name) => namespace **/
void MakeNamespace(Frame* fp) {
  auto name = fp->argv[0];
  auto import = fp->argv[1];

  if (!String::IsType(name))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "ns", name);

  if (!Namespace::IsType(import))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "ns", name);

  fp->value = Namespace(name, import).Evict(fp->env, "mu-namespace:make");
  core::Env::AddNamespace(fp->env, fp->value);
}

/** * (ns-symbols namespace) => list **/
void NamespaceSymbols(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "ns-symbols",
                     ns);

  std::vector<Type::TagPtr> interns;

  for (auto map : Namespace::interns(ns)) interns.push_back(map.second);

  std::vector<Type::TagPtr> externs;

  for (auto map : Namespace::externs(ns)) externs.push_back(map.second);

  fp->value = core::Cons(core::Cons::List(fp->env, externs),
                         core::Cons::List(fp->env, interns))
                  .Evict(fp->env, "mu-namespace:symbols");
}

/** * (in-ns ns) => ns **/
void SetNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "in-ns", ns);

  fp->env->namespace_ = ns;
  fp->value = ns;
}

/** * (ns-current ns) => ns **/
void GetNamespace(Frame* fp) { fp->value = fp->env->namespace_; }

/** * (find-symbol ns symbol) => symbol **/
void FindSymbolNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto name = fp->argv[1];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "find-symbol", ns);

  if (!String::IsType(name))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "find-symbol", name);

  fp->value = Namespace::FindSymbol(fp->env, ns, name);
}

/** * (find-in-ns ns :intern|:extern string) => symbol **/
void FindInNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto type = fp->argv[1];
  auto name = fp->argv[2];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "find-in-ns",
                     ns);

  if (!String::IsType(name))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "find-in-ns",
                     name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp->value = Namespace::FindInInterns(fp->env, ns, name);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp->value = Namespace::FindInExterns(fp->env, ns, name);
  } else
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "find-in-ns",
                     name);
}

/** * (intern ns keyword symbol value) => symbol **/
void InternNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto type = fp->argv[1];
  auto name = fp->argv[2];
  auto value = fp->argv[3];

  if (!Namespace::IsType(ns))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "intern",
                     ns);

  if (!String::IsType(name))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "intern",
                     name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp->value = Namespace::InternInNs(fp->env, ns, name);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp->value = Namespace::Intern(fp->env, ns, name);
  } else
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "intern",
                     name);

  if (!Symbol::IsBound(fp->value)) Symbol::Bind(fp->value, value);
}

} /* namespace mu */
} /* namespace libmu */
