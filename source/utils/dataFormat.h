#ifndef __DATAFORMAT_H__
#define __DATAFORMAT_H__

#include <mbed.h>
#include <platform/SharedPtr.h>
#include "typedefs.h"

static int convArray[][2] = {   {0xE28099, '\''},
                                {0xC2B0, ' '}};

class DataFormat {
public:
    static void utf8ToAscii(SharedPtr<uint8_t> bufferPtr) {
        char *src = (char*)bufferPtr.get();
        // printf("utf8ToAscii:src=%s\n", src);
        int srcLen = strlen(src);
        for (int i = 0; i < srcLen; i++) {
            int startPos = 0;
            // printf("utf8ToAscii:i=%d\n", i);
            int seqLen = sequence_length(src[i]);
            // printf("utf8ToAscii:seqLen=%d\n", seqLen);
            if (seqLen == 1) { // Nothing to do just leave alone
                continue;
            } else if (seqLen > 1) {
                startPos = i;
                // Replace chars with ASCII equivalent
                // Grab # chars and convert to int32
                int32_t code = src[i];
                for (int j = 1; j < seqLen; j++) {
                    code = (code << 8) | src[i + j];
                }
                // printf("utf8ToAscii:code=%d\n", code);
                // Now look up the code and get the assocated ASCII char
                uint8_t asciiCode = findASCII(code);
                // printf("utf8ToAscii:asciiCode=%c\n", asciiCode);
                // Now replace the chars in the source string with
                // new code and shuffle down all the remaining chars
                src[startPos] = asciiCode;
                // printf("utf8ToAscii:memmove(%d,%d,%d)\n", startPos + 1, startPos + seqLen, srcLen - startPos);
                memmove(&src[startPos + 1], &src[startPos + seqLen], srcLen - startPos);
                i += seqLen;
            }
            // printf("utf8ToAscii:seqLen=%d\n", seqLen);
        }
        // printf("utf8ToAscii:src=%s\n", src);
        // for (int i = 0; i < srcLen; i++) {
        //     printf("0x%X,", src[i]);
        // }
        // printf("\n");
    }

    static SharedPtr<MessageData> parseNotification(SharedPtr<uint8_t> bufferPtr) {
        char* token = strtok((char*)bufferPtr.get(), "|" );
        SharedPtr<MessageData> retMsg = NULL;
        if (NULL != token) {
            retMsg = SharedPtr<MessageData>(new MessageData);
            // if first token then sender
            printf("parseNotification:sender=%s\r\n", token);
            retMsg->sender = SharedPtr<char>(new char[strlen(token) + 1]);
            strcpy(retMsg->sender.get(), token);

            token = strtok(NULL, "|" );
        }
        if (NULL != token) {
            // if second token then from
            printf("parseNotification:from=%s\r\n", token);
            retMsg->from = SharedPtr<char>(new char[strlen(token) + 1]);
            strcpy(retMsg->from.get(), token);

            token = strtok(NULL, "|" );
        }
        if (NULL != token) {
            // if third token then subject
            printf("parseNotification:subject=%s, len=%d\r\n", token, strlen(token));
            retMsg->subject = SharedPtr<char>(new char[strlen(token) + 1]);
            strcpy(retMsg->subject.get(), token);

            token = strtok(NULL, "|" );
        }
        if (NULL != token) {
            // if forth token then body
            printf("parseNotification:body=%s, len=%d\r\n", token, strlen(token));
            retMsg->body = SharedPtr<char>(new char[strlen(token) + 1]);
            strcpy(retMsg->body.get(), token);
        }

        printf("parseNotification:retMsg->sender=%s\r\n", retMsg->sender.get());
        printf("parseNotification:retMsg->from=%s\r\n", retMsg->from.get());
        printf("parseNotification:retMsg->subject=%s\r\n", retMsg->subject.get());
        printf("parseNotification:retMsg->body=%s\r\n", retMsg->body.get());

        return retMsg;
    }

private:
    
    // Find the utf8 code starting position


    // Find and return length
    static int sequence_length(uint8_t lead_it)
    {
        uint8_t lead = mask8(lead_it);
        if (lead < 0x80)
            return 1;
        else if ((lead >> 5) == 0x6)
            return 2;
        else if ((lead >> 4) == 0xe)
            return 3;
        else if ((lead >> 3) == 0x1e)
            return 4;
        else
            return 0;
    }

    static inline uint8_t mask8(uint8_t oc)
    {
        return (0xff & oc);
    }
    
    static uint8_t findASCII(int32_t src) {
        // printf("findASCII:src=%d\n", src);
        for (uint32_t i = 0; i < sizeof convArray / sizeof convArray[0]; i++) {
            if (convArray[i][0] == src) {
                // printf("findASCII:GOT MATCH\n");
                return convArray[i][1];
            }
        }
        
        printf("findASCII:FAILED %d\n", src);
        return '?';
    }

};

#endif // __DATAFORMAT_H__
