// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>

class TeardownNotifierTest:
  public testing::Test
{};

class SimpleMember:
  public ContextMember
{
};

TEST_F(TeardownNotifierTest, VerifySingleNotification) {
  bool hit;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    AutoRequired<SimpleMember> member;
    member->AddTeardownListener([&hit] {hit = true;});
  }

  EXPECT_TRUE(hit) << "Teardown listener was not hit as expected during context teardown";
}

TEST_F(TeardownNotifierTest, ReferenceMemberInTeardown) {

  bool hit = false;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    AutoRequired <SimpleMember> member;
    std::weak_ptr<SimpleMember> memberWeak(member);

    ctxt->AddTeardownListener([memberWeak, &hit] {
      try
      {
        if (!memberWeak.expired())
          hit = true;
      }
      catch (autowiring_error) {}
    });
  }

  EXPECT_TRUE(hit) << "Failed to reference a member of a context in it's teardown listener";
}

TEST_F(TeardownNotifierTest, CanAutowireInTeardown) {
  bool foundSimpleMember = false;
  bool calledNotifier = false;
  {
    AutoCreateContext ctxt;
    AutoRequired<SimpleMember> sm(ctxt);
    ctxt->AddTeardownListener([&foundSimpleMember, &calledNotifier](const CoreContext& ctxt) {
      foundSimpleMember = AutowiredFast<SimpleMember>(&ctxt).IsAutowired();
      calledNotifier = true;
    });
  }

  ASSERT_TRUE(foundSimpleMember) << "Failed to find a context member in teardown with AutowiredFast";
  ASSERT_TRUE(calledNotifier) << "Teardown notifier not called as expected";
}
