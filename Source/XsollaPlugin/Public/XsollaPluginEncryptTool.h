#pragma once

#include "AES.h"
#include "CoreMinimal.h"

class XsollaPluginEncryptTool
{
public:
    XsollaPluginEncryptTool()
    {

    }

    /**
    * Encrypt string with AES.
    *
    * @param str - String to encrypt.
    *
    * @return Encrypted string
    */
    static FString EncryptString(FString& str)
    {
        if (str.IsEmpty())
            return str;

        uint32 size = str.Len();
        size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));

        uint8* blob = new uint8[size];
        blob = (uint8*)TCHAR_TO_UTF8(str.GetCharArray().GetData());

        FAES::EncryptData(blob, size, "GffdH6446FVdfcbhFDFdIHNgD556GCdf");

        TArray<uint8> encryptedArray;
        encryptedArray.Append(blob, size);

        FString encryptedStr = BinaryArrayToFString(encryptedArray, encryptedArray.Num());
        return encryptedStr;
    }

    /**
    * Decrypt string with AES.
    *
    * @param str - String to decrypt.
    *
    * @return Decrypted string
    */
    static FString DecryptString(FString& str)
    {
        if (str.IsEmpty())
            return str;

        uint32 size = str.Len();
        size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));

        TArray<uint8> encryptedArray = FStringToBinaryArray(str);

        uint8* blob = new uint8[size];
        blob = encryptedArray.GetData();

        FAES::DecryptData(blob, size, "GffdH6446FVdfcbhFDFdIHNgD556GCdf");

        FString decryptedStr = UTF8_TO_TCHAR(blob);

        return decryptedStr;
    }

    /**
    * Convert wchar string to array of chars.
    *
    * @param Input - String to convert.
    *
    * @return Array of chars
    */
    static TArray<uint8> FStringToBinaryArray(const FString& Input)
    {
        // We found that FSting char has 4 bytes on ios and 2 bytes on Windows
        int32 len = Input.Len();

        TArray<uint8> Output;
        Output.AddZeroed(2 * len);

        for (int32 i = 0; i < len; ++i)
        {
            Output[2 * i + 0] = Input[i] & 0xFF; // Save first byte of char
            Output[2 * i + 1] = (Input[i] >> 8) & 0xFF; // Save second byte of char
        }

        return Output;
    }

    /**
    * Convert charr array to wchar string.
    *
    * @param Input - Array of uint8
    * @param Len - Size of array
    *
    * @return Converted string
    */
    static FString BinaryArrayToFString(const TArray<uint8>& Input, int32 Len /*= -1*/)
    {
        if (Len < 0)
        {
            Len = Input.Num();
        }

        // We found that FSting char has 4 bytes on ios and 2 bytes on Windows
        Len /= 2;

        FString Output;
        auto& TCharArray = Output.GetCharArray();
        TCharArray.AddZeroed(Len + 1);

        for (int32 i = 0; i < Len; ++i)
        {
            TCharArray[i] = Input[2 * i + 0] | (Input[2 * i + 1] << 8);
        }

        return Output;
    }
};