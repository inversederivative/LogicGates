// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/DisplayOutput.h"

void ADisplayOutput::SetInput(AAbstractNode *input) {
	input_ = input;
	input_->Attach(this);
	ADisplayOutput::Update(input->GetState());
}

eLogicState ADisplayOutput::GetState() const {
	return state_;
}

void ADisplayOutput::Update(eLogicState state) {
	state_ = state;
	Notify();
}

