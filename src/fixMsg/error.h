#pragma once
#include <iostream>

namespace fix
{
    typedef uint64_t ERROR;

    // General

    #define NO_ERROR                     0
    #define SOH_NOT_FOUND_ERROR          1
    #define TAG_ERROR                    2
    #define TAG_POS_ERROR                3

    // Header

    #define BEGIN_STRING_ERROR           4
    #define BODY_LENGHT_ERROR            5
    #define MSG_TYPE_ERROR               6
    #define SENDER_COMP_ID_ERROR         7
    #define TARGET_COMP_ID_ERROR         8
    #define SENDER_SUB_ID_ERROR          9
    #define TARGET_SUB_ID_ERROR          10
    #define SEQ_NUM_ERROR                11
    #define POSS_DUP_FLAG_ERROR          12
    #define POSS_RESEND_ERROR            13
    #define SENDING_TIME_ERROR           14
    #define ORIG_SENDING_TIME_ERROR      15

    #define BEGIN_STRING_MISSING_ERROR   16
    #define BODY_LENGHT_MISSING_ERROR    17
    #define MSG_TYPE_MISSING_ERROR       18
    #define SENDER_COMP_ID_MISSING_ERROR 19
    #define TARGET_COMP_ID_MISSING_ERROR 20
    #define SEQ_NUM_MISSING_ERROR        21
    #define SENDING_TIME_MISSING_ERROR   22


};