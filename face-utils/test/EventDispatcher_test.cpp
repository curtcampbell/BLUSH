#include "EventDispatcher.h"

#include <gtest/gtest.h>
#include <string>
#include <type_traits>
#include <vector>

TEST(EventDispatcherTest, IsNotCopyable) {
    using Dispatcher = EventDispatcher<int>;
    static_assert(!std::is_copy_constructible<Dispatcher>::value,
                  "EventDispatcher must not be copy-constructible");
    static_assert(!std::is_copy_assignable<Dispatcher>::value,
                  "EventDispatcher must not be copy-assignable");
}

TEST(EventDispatcherTest, IsMovable) {
    using Dispatcher = EventDispatcher<int>;
    static_assert(std::is_move_constructible<Dispatcher>::value,
                  "EventDispatcher must be move-constructible");
}

TEST(EventDispatcherTest, InvokesRegisteredCallbackWithArgs) {
    EventDispatcher<int, const std::string&> dispatcher;

    int         receivedCode = 0;
    std::string receivedText;

    dispatcher.Register([&](int code, const std::string& text) {
        receivedCode = code;
        receivedText = text;
    });

    dispatcher(42, "hello");

    EXPECT_EQ(receivedCode, 42);
    EXPECT_EQ(receivedText, "hello");
}

TEST(EventDispatcherTest, InvokesMultipleCallbacksInRegistrationOrder) {
    EventDispatcher<int> dispatcher;
    std::vector<int>     order;

    dispatcher.Register([&](int) { order.push_back(1); });
    dispatcher.Register([&](int) { order.push_back(2); });
    dispatcher.Register([&](int) { order.push_back(3); });

    dispatcher(0);

    EXPECT_EQ(order, (std::vector<int>{1, 2, 3}));
}

TEST(EventDispatcherTest, DispatchWithNoRegistrationsIsNoop) {
    EventDispatcher<int> dispatcher;
    EXPECT_NO_THROW(dispatcher(1));
}

TEST(EventDispatcherTest, CancelStopsFurtherDispatch) {
    EventDispatcher<int> dispatcher;
    int                  callCount = 0;

    auto reg = dispatcher.Register([&](int) { ++callCount; });

    dispatcher(0);
    EXPECT_EQ(callCount, 1);

    reg->Cancel();
    dispatcher(0);
    EXPECT_EQ(callCount, 1);
}

TEST(EventDispatcherTest, CancelIsIdempotent) {
    EventDispatcher<int> dispatcher;
    int                  callCount = 0;

    auto reg = dispatcher.Register([&](int) { ++callCount; });

    reg->Cancel();
    reg->Cancel();
    dispatcher(0);

    EXPECT_EQ(callCount, 0);
}

TEST(EventDispatcherTest, DroppingHandleWithoutCancelKeepsCallbackRegistered) {
    EventDispatcher<int> dispatcher;
    int                  callCount = 0;

    {
        auto reg = dispatcher.Register([&](int) { ++callCount; });
        (void)reg;
    }

    dispatcher(0);
    EXPECT_EQ(callCount, 1);
}

TEST(EventDispatcherTest, CancellingOneHandlerDoesNotAffectOthers) {
    EventDispatcher<int> dispatcher;
    int                  countA = 0;
    int                  countB = 0;

    auto regA = dispatcher.Register([&](int) { ++countA; });
    auto regB = dispatcher.Register([&](int) { ++countB; });

    regA->Cancel();
    dispatcher(0);

    EXPECT_EQ(countA, 0);
    EXPECT_EQ(countB, 1);
}

TEST(EventDispatcherTest, RegistrationDuringDispatchIsNotInvokedUntilNextDispatch) {
    EventDispatcher<int> dispatcher;
    int                  secondCallCount = 0;
    std::shared_ptr<EventRegistration> secondReg;

    dispatcher.Register([&](int) {
        secondReg = dispatcher.Register([&](int) { ++secondCallCount; });
    });

    dispatcher(0);
    EXPECT_EQ(secondCallCount, 0);

    dispatcher(0);
    EXPECT_EQ(secondCallCount, 1);
}

TEST(EventDispatcherTest, CancellationDuringDispatchStillInvokesSnapshotForThatPass) {
    EventDispatcher<int> dispatcher;
    int                  countB = 0;
    std::shared_ptr<EventRegistration> regB;

    auto regA = dispatcher.Register([&](int) { regB->Cancel(); });
    regB = dispatcher.Register([&](int) { ++countB; });

    dispatcher(0);
    EXPECT_EQ(countB, 1);

    dispatcher(0);
    EXPECT_EQ(countB, 1);
}

TEST(EventDispatcherTest, CancelAfterDispatcherDestroyedIsSafeNoop) {
    std::shared_ptr<EventRegistration> reg;

    {
        EventDispatcher<int> dispatcher;
        reg = dispatcher.Register([](int) {});
    }

    EXPECT_NO_THROW(reg->Cancel());
    EXPECT_NO_THROW(reg->Cancel());
}

TEST(EventDispatcherTest, MoveConstructPreservesRegisteredHandlers) {
    EventDispatcher<int> original;
    int                  callCount = 0;
    auto reg = original.Register([&](int) { ++callCount; });

    EventDispatcher<int> moved(std::move(original));
    moved(0);

    EXPECT_EQ(callCount, 1);

    reg->Cancel();
    moved(0);
    EXPECT_EQ(callCount, 1);
}
