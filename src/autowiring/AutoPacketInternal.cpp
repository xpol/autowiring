// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketInternal.h"
#include "AutoPacketFactory.h"
#include "SatCounter.h"

AutoPacketInternal::AutoPacketInternal(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding) :
  AutoPacket(factory, std::move(outstanding))
{
}

AutoPacketInternal::~AutoPacketInternal(void)
{
}

void AutoPacketInternal::Initialize(void) {
  // Find all subscribers with no required or optional arguments:
  std::list<SatCounter*> callCounters;
  for (auto& satCounter : m_satCounters)
    if (satCounter)
      callCounters.push_back(&satCounter);

  // Call all subscribers with no required or optional arguments:
  // NOTE: This may result in decorations that cause other subscribers to be called.
  for (SatCounter* call : callCounters)
    call->CallAutoFilter(*this);

  // First-call indicated by argumument type AutoPacket&:
  UpdateSatisfaction(typeid(auto_arg<AutoPacket&>::id_type));
}