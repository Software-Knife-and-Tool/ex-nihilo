/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu.h: library functions
 **
 **/
#if !defined(LIBMU_MU_MU_H_)
#define LIBMU_MU_MU_H_

#include "libmu/env.h"

namespace libmu {
namespace mu {

using Frame = core::Env::Frame;

void AcceptSocketStream(Frame*);
void Acos(Frame*);
void Apply(Frame*);
void Asin(Frame*);
void Atan(Frame*);
void Block(Frame*);
void Car(Frame*);
void Cdr(Frame*);
void Clocks(Frame*);
void Close(Frame*);
void Closure(Frame*);
void ConnectSocketStream(Frame*);
void Cosine(Frame*);
void EnvView(Frame*);
void Eq(Frame*);
void Eval(Frame*);
void Exit(Frame*);
void Exp(Frame*);
void FindNamespace(Frame*);
void FindInNamespace(Frame*);
void FindSymbolNamespace(Frame*);
void FixAdd(Frame*);
void FixLessThan(Frame*);
void FixMul(Frame*);
void FixSub(Frame*);
void FloatAdd(Frame*);
void FloatDiv(Frame*);
void FloatLessThan(Frame*);
void FloatMul(Frame*);
void FloatSub(Frame*);
void Floor(Frame*);
void FrameRef(Frame*);
void FunctionStream(Frame*);
void Gc(Frame*);
void GetStringStream(Frame*);
void HeapInfo(Frame*);
void ImportOfNamespace(Frame*);
void ImportOfNamespace(Frame*);
void InFileStream(Frame*);
void InStringStream(Frame*);
void InternNamespace(Frame*);
void Invoke(Frame*);
void IsBound(Frame*);
void IsChar(Frame*);
void IsCode(Frame*);
void IsCons(Frame*);
void IsEof(Frame*);
void IsException(Frame*);
void IsFixnum(Frame*);
void IsFloat(Frame*);
void IsFunction(Frame*);
void IsKeyword(Frame*);
void IsNamespace(Frame*);
void IsSpecOp(Frame*);
void IsStream(Frame*);
void IsStruct(Frame*);
void IsSymbol(Frame*);
void IsThread(Frame*);
void IsVector(Frame*);
void Letq(Frame*);
void ListLength(Frame*);
void Load(Frame*);
void Log(Frame*);
void Log10(Frame*);
void Logand(Frame*);
void Logor(Frame*);
void MacroExpand(Frame*);
void MacroFunction(Frame*);
void MakeCons(Frame*);
void MakeException(Frame*);
void MakeKeyword(Frame*);
void MakeNamespace(Frame*);
void MakeStruct(Frame*);
void MakeView(Frame*);
void MapC(Frame*);
void MapCar(Frame*);
void MapL(Frame*);
void MapList(Frame*);
void NameOfNamespace(Frame*);
void NamespaceSymbols(Frame*);
void Nth(Frame*);
void Nthcdr(Frame*);
void OutFileStream(Frame*);
void OutStringStream(Frame*);
void Pow(Frame*);
void PrintEscape(Frame*);
void Raise(Frame*);
void RaiseException(Frame*);
void Read(Frame*);
void ReadByte(Frame*);
void ReadChar(Frame*);
void Return(Frame*);
void SetMacroChar(Frame*);
void SetNamespace(Frame*);
void Sine(Frame*);
void SocketServerStream(Frame*);
void SocketStream(Frame*);
void Sqrt(Frame*);
void StructType(Frame*);
void StructValues(Frame*);
void SymbolName(Frame*);
void SymbolNamespace(Frame*);
void SymbolValue(Frame*);
void System(Frame*);
void Tangent(Frame*);
void Terpri(Frame*);
void Trampoline(Frame*);
void Truncate(Frame*);
void TypeOf(Frame*);
void UnReadChar(Frame*);
void UninternedSymbol(Frame*);
void VectorCons(Frame*);
void VectorLength(Frame*);
void VectorMap(Frame*);
void VectorMapC(Frame*);
void VectorRef(Frame*);
void VectorType(Frame*);
void WithException(Frame*);
void WriteByte(Frame*);
void WriteChar(Frame*);

} /* namespace mu */
} /* namespace libmu */

#endif /* LIBMU_MU_MU_H_ */
