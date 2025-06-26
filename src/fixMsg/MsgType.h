#pragma once

namespace fix
{
    enum class MsgType
    {
        Heartbeat = '0',
        TestRequest = '1',
        ResendRequest = '2',
        Reject = '3',
        SequenceReset = '4',
        Logout = '5',
        ExecutionReport = '8',
        OrderCancelReject = '9',
        Logon = 'A',
        NewsBulletinMessage = 'B',
        OrderSingle = 'D',
        OrderCancelRequest = 'F',
        OrderCancelReplaceRequest = 'G',
        OrderStatusRequest = 'H',
        Allocation = 'J',
        AllocationAck = 'P',
        NewOrderMultiLeg = 16706, // AB
        MultilegCancelReplaceRequest = 16707, // AC
    };
};

