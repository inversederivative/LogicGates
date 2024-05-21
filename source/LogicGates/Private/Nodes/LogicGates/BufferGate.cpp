// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/LogicGates/BufferGate.h"

ABufferGate::ABufferGate()
{
	SetNodeName("BufferGate");
}

void ABufferGate::SetInput(AAbstractNode* in)
{
	input = in;
	in->Attach(this);
	ABufferGate::Update(GetState());
}

void ABufferGate::Update(eLogicState state)
{
	outputState_ = state;
	Notify();
}

eLogicState ABufferGate::GetState() const
{
	return outputState_;
}
