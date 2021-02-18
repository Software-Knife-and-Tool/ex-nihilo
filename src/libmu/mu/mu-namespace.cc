/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Cons = core::Cons;
using Env = core::Env;
using Condition = core::Condition;
using Frame = core::Env::Frame;
using Namespace = core::Namespace;
using String = core::String;
using Symbol = core::Symbol;
using Type = core::Type;

/** * (namespacep object) => bool  **/
void IsNamespace(Frame* fp) {
  fp->value = Type::Bool(Namespace::IsType(fp->argv[0]));
}

/** * (find-ns name) => namespace **/
void FindNamespace(Frame* fp) {
  auto name = fp->argv[0];

  if (!String::IsType(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "find-ns",
                     name);

  fp->value = Env::MapNamespace(fp->env, String::StdStringOf(name));
}

/** * (name-ns namespace) => string **/
void NameOfNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "ns-name",
                     ns);

  fp->value = Namespace::name(fp->argv[0]);
}

/** * (ns-import namespace) => list **/
void ImportOfNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "ns-import", ns);

  fp->value = Namespace::imports(fp->argv[0]);
}

/** * (ns name) => namespace **/
void MakeNamespace(Frame* fp) {
  auto name = fp->argv[0];
  auto imports = fp->argv[1];

  if (!String::IsType(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "ns",
                     name);

  if (!Cons::IsList(imports))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "ns",
                     imports);

  Cons::cons_iter<Type::Tag> iter(imports);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    if (!Namespace::IsType(it->car))
      Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "ns",
                       it->car);
  }

  fp->value = Namespace(name, imports).Evict(fp->env);
  Env::AddNamespace(fp->env, fp->value);
}

/** * (ns-symbols namespace) => list **/
void NamespaceSymbols(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "ns-symbols", ns);

  std::vector<Type::Tag> interns;

  for (auto map : Namespace::interns(ns)) interns.push_back(map.second);

  std::vector<Type::Tag> externs;

  for (auto map : Namespace::externs(ns)) externs.push_back(map.second);

  fp->value = core::Cons(core::Cons::List(fp->env, externs),
                         core::Cons::List(fp->env, interns))
                  .Evict(fp->env);
}

/** * (ns->current) => ns **/
void GetNamespace(Frame* fp) { fp->value = fp->env->namespace_; }

/** * (in-ns ns) => ns **/
void SetNamespace(Frame* fp) {
  auto ns = fp->argv[0];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "in-ns",
                     ns);

  auto prev = fp->env->namespace_;

  fp->env->namespace_ = ns;
  fp->value = prev;
}

/** * (find-symbol ns string) => symbol **/
void FindSymbolNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto name = fp->argv[1];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "find-symbol", ns);

  if (!String::IsType(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "find-symbol", name);

  fp->value = Namespace::FindSymbol(fp->env, ns, name);
}

/** * (find-in-ns ns :intern|:extern string) => symbol **/
void FindInNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto type = fp->argv[1];
  auto name = fp->argv[2];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "find-in-ns", ns);

  if (!String::IsType(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "find-in-ns", name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp->value = Namespace::FindInterns(ns, name);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp->value = Namespace::FindExterns(ns, name);
  } else
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "find-in-ns", name);
}

/** * (intern ns keyword symbol value) => symbol **/
void InternNamespace(Frame* fp) {
  auto ns = fp->argv[0];
  auto type = fp->argv[1];
  auto name = fp->argv[2];
  auto value = fp->argv[3];

  if (!Namespace::IsType(ns))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "intern",
                     ns);

  if (!String::IsType(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "intern",
                     name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp->value = Namespace::InternInNs(fp->env, ns, name);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp->value = Namespace::Intern(fp->env, ns, name);
  } else
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "intern",
                     name);

  if (!Symbol::IsBound(fp->value)) Symbol::Bind(fp->value, value);
}

} /* namespace mu */
} /* namespace libmu */
