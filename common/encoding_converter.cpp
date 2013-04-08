#include "encoding_converter.hpp"
#include "tiniconv.h"
#include "logging.hpp"

#include <boost/assign.hpp>


/*
 * Integers are set according to constant values from tiniconv library.
 * See file tiniconv/tiniconv.h
 */
std::map<std::string, int> EncodingConverter::CHARSET_CODES = boost::assign::map_list_of
    ("ascii/unknown",   0)
    ("windows-1250",    1)
    ("windows-1251",    2)
    ("windows-1252",    3)
    ("windows-1253",    4)
    ("windows-1254",    5)  // doesn't exist in uchardet
    ("windows-1255",    6)
    ("windows-1256",    7)
    ("windows-1257",    8)
    ("windows-1258",    9)
    ("CP939",           10)
    ("HZ-GB-2312",      11) // or EUC-CN like in tiniconv/tiniconv_desc.c?
    ("GBK",             12)
    ("ISO-2020-JP",     13)
    ("ISO-8859-1",      14) // doesn't exist in uchardet
    ("ISO-8859-2",      15)
    ("ISO-8859-3",      16) // doesn't exist in uchardet
    ("ISO-8859-4",      17) // doesn't exist in uchardet
    ("ISO-8859-5",      18)
    ("ISO-8859-6",      19) // doesn't exist in uchardet
    ("ISO-8859-7",      20)
    ("ISO-8859-8",      21)
    ("ISO-8859-9",      22) // doesn't exist in uchardet
    ("ISO-8859-10",     23) // doesn't exist in uchardet
    ("ISO-8859-11",     24) // doesn't exist in uchardet
    ("ISO-8859-13",     25) // doesn't exist in uchardet
    ("ISO-8859-14",     26) // doesn't exist in uchardet
    ("ISO-8859-15",     27) // doesn't exist in uchardet
    ("ISO-8859-16",     28) // doesn't exist in uchardet
    ("CP866",           29)
    ("KOI8-R",          30)
    ("KOI8-RU",         31) // doesn't exist in uchardet
    ("KOI8-U",          32) // doesn't exist in uchardet
    ("x-max-cyrillic",  33)
    ("UCS-2",           34) // doesn't exist in uchardet
    ("UTF-7",           35) // doesn't exist in uchardet
    ("UTF-8",           36)
    ("GB-2312",         37) // or EUC-CN like 11?
    ("Big5",            38)
    ;

const int EncodingConverter::TINICONV_OPTION = 0;

EncodingConverter::EncodingConverter() : defaultEncoding_("UTF-8") {
}

EncodingConverter::EncodingConverter(std::string defaultEncoding)
    : defaultEncoding_(defaultEncoding) {
}

std::string EncodingConverter::detect(std::string text) {
    return std::string("");
}

std::string EncodingConverter::convert(std::string from, std::string to, std::string text) {
    if (!CHARSET_CODES.count(from)) {
        WARN("unrecognized source encoding: " << from);
        return text;
    }
    if (!CHARSET_CODES.count(to)) {
        WARN("unrecognized target encoding: " << to);
        return text;
    }

    std::string convertedText("");

    if (!convert_(CHARSET_CODES[from], CHARSET_CODES[to], text, convertedText)) {
        return text;
    }

    return convertedText;
}

bool EncodingConverter::convert_(int inCharsetId, int outCharsetId,
                                 std::string input, std::string& output) {

    struct tiniconv_ctx_s tiniconvStruct;
    int result = tiniconv_init(inCharsetId, outCharsetId, TINICONV_OPTION, &tiniconvStruct);

    if (result != TINICONV_INIT_OK) {
        WARN("tiniconv initialization failed");
        return false;
    }

    int inSizeConsumed, outSizeConsumed;
    unsigned char outputBuffer[400000];

    result = tiniconv_convert(&tiniconvStruct,
                              (const unsigned char*)input.c_str(), input.length(), &inSizeConsumed,
                              outputBuffer, sizeof(outputBuffer) - 1, &outSizeConsumed);

    if (result < TINICONV_CONVERT_OUT_TOO_SMALL) {
        WARN("tiniconv convertion failed");
        return false;
    }

    outputBuffer[outSizeConsumed] = 0;

    output = std::string((const char *)outputBuffer);
    DEBUG("tiniconv convertion output: [" << output << "]");

    return true;
}
