/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu.h: library functions
 **
 **/
#if !defined(_LIBMU_MU_MU_H_)
#define _LIBMU_MU_MU_H_

#include "libmu/env.h"

namespace libmu {
namespace mu {

void AcceptSocketStream(Env::Frame*);
void Acos(Env::Frame*);
void Apply(Env::Frame*);
void Asin(Env::Frame*);
void Atan(Env::Frame*);
void Block(Env::Frame*);
void Car(Env::Frame*);
void Cdr(Env::Frame*);
void CharCoerce(Env::Frame*);
void Close(Env::Frame*);
void Closure(Env::Frame*);
void ConnectSocketStream(Env::Frame*);
void Cosine(Env::Frame*);
void EnvStack(Env::Frame*);
void EnvStackDepth(Env::Frame*);
void Eq(Env::Frame*);
void Eval(Env::Frame*);
void Exit(Env::Frame*);
void Exp(Env::Frame*);
void FindInNamespace(Env::Frame*);
void FindNamespace(Env::Frame*);
void FindSymbolNamespace(Env::Frame*);
void FixAdd(Env::Frame*);
void FixDiv(Env::Frame*);
void FixLessThan(Env::Frame*);
void FixMul(Env::Frame*);
void FixSub(Env::Frame*);
void FixnumCoerce(Env::Frame*);
void FloatAdd(Env::Frame*);
void FloatCoerce(Env::Frame*);
void FloatDiv(Env::Frame*);
void FloatLessThan(Env::Frame*);
void FloatMul(Env::Frame*);
void FloatSub(Env::Frame*);
void FrameRef(Env::Frame*);
void FunctionStream(Env::Frame*);
void Gc(Env::Frame*);
void GetNamespace(Env::Frame*);
void GetNamespace(Env::Frame*);
void GetStringStream(Env::Frame*);
void HeapInfo(Env::Frame*);
void HeapLog(Env::Frame*);
void ImportOfNamespace(Env::Frame*);
void ImportOfNamespace(Env::Frame*);
void InFileStream(Env::Frame*);
void InStringStream(Env::Frame*);
void InternNamespace(Env::Frame*);
void Invoke(Env::Frame*);
void IsBound(Env::Frame*);
void IsChar(Env::Frame*);
void IsCode(Env::Frame*);
void IsCons(Env::Frame*);
void IsEof(Env::Frame*);
void IsException(Env::Frame*);
void IsFixnum(Env::Frame*);
void IsFloat(Env::Frame*);
void IsFunction(Env::Frame*);
void IsKeyword(Env::Frame*);
void IsNamespace(Env::Frame*);
void IsSpecOp(Env::Frame*);
void IsStream(Env::Frame*);
void IsStruct(Env::Frame*);
void IsSymbol(Env::Frame*);
void IsThread(Env::Frame*);
void IsVector(Env::Frame*);
void Letq(Env::Frame*);
void ListLength(Env::Frame*);
void Load(Env::Frame*);
void Log(Env::Frame*);
void Log10(Env::Frame*);
void Logand(Env::Frame*);
void Logor(Env::Frame*);
void MacroExpand(Env::Frame*);
void MacroFunction(Env::Frame*);
void MakeCons(Env::Frame*);
void MakeException(Env::Frame*);
void MakeKeyword(Env::Frame*);
void MakeNamespace(Env::Frame*);
void MakeStruct(Env::Frame*);
void MakeVector(Env::Frame*);
void MakeView(Env::Frame*);
void MapC(Env::Frame*);
void MapCar(Env::Frame*);
void MapL(Env::Frame*);
void MapList(Env::Frame*);
void Mod(Env::Frame*);
void NameOfNamespace(Env::Frame*);
void NamespaceSymbols(Env::Frame*);
void Nth(Env::Frame*);
void Nthcdr(Env::Frame*);
void Null(Env::Frame*);
void OutFileStream(Env::Frame*);
void OutStringStream(Env::Frame*);
void Pow(Env::Frame*);
void PrintEscape(Env::Frame*);
void Raise(Env::Frame*);
void RaiseException(Env::Frame*);
void Read(Env::Frame*);
void ReadByte(Env::Frame*);
void ReadChar(Env::Frame*);
void Return(Env::Frame*);
void RunTime(Env::Frame*);
void SetMacroChar(Env::Frame*);
void SetNamespace(Env::Frame*);
void Sine(Env::Frame*);
void SocketServerStream(Env::Frame*);
void SocketStream(Env::Frame*);
void Sqrt(Env::Frame*);
void StackInspect(Env::Frame*);
void StackTrace(Env::Frame*);
void StructType(Env::Frame*);
void StructValues(Env::Frame*);
void SymbolName(Env::Frame*);
void SymbolNamespace(Env::Frame*);
void SymbolValue(Env::Frame*);
void System(Env::Frame*);
void SystemEnv(Env::Frame*);
void SystemTime(Env::Frame*);
void Tangent(Env::Frame*);
void Terpri(Env::Frame*);
void TestIf(Env::Frame*);
void Trampoline(Env::Frame*);
void TypeOf(Env::Frame*);
void UnReadChar(Env::Frame*);
void UninternedSymbol(Env::Frame*);
void VectorLength(Env::Frame*);
void VectorMap(Env::Frame*);
void VectorMapC(Env::Frame*);
void VectorRef(Env::Frame*);
void VectorType(Env::Frame*);
void WithException(Env::Frame*);
void WriteByte(Env::Frame*);
void WriteChar(Env::Frame*);

} /* namespace mu */
} /* namespace libmu */

#endif /* _LIBMU_MU_MU_H_ */
