#include "ststusCode.hpp"

Code::Code()
{
    code[100] = "CONTINUE";
    code[101] = "SWITCHING_PROTOL";
    code[200] = "OK";
    code[201] = "CREATED";
    code[202] = "ACCEPTED";
    code[203] = "NON_AUTHORITATIVE_INFORMATION";
    code[204] = "NO_CONTENT";
    code[205] = "RESET_CONTENT";
    code[206] = "PARTIAL_CONTENT";
    code[300] = "MULTIPLE_CHOICE";
    code[301] = "MOVED_PERMANENTLY";
    code[302] = "FOUND";
    code[303] = "SEE_OTHER";
    code[304] = "NOT_MODIFIED";
    code[307] = "TEMPORARY_REDIRECT";
    code[308] = "PERMANENT_REDIRECT";
    code[400] = "BAD_REQUEST";
    code[401] = "UNAUTHORIZED";
    code[402] = "PAYMENT_REQUIRED";
    code[403] = "FORBIDDEN";
    code[404] = "NOT_FOUND";
    code[405] = "METHOD_NOT_ALLOWED";
    code[406] = "NOT_ACCEPTABLE";
    code[407] = "PROXY_AUTHENTICATION_REQUIRED";
    code[408] = "REQUEST_TIMEOUT";
    code[409] = "CONFLICT";
    code[410] = "GONE";
    code[411] = "LENGTH_REQUIRED";
    code[412] = "PRECONDITION_FAILED";
    code[413] = "PAYLOAD_TOO_LARGE";
    code[414] = "URI_TOO_LONG";
    code[415] = "UNSUPPORTED_MEDIA_TYPE";
    code[416] = "REQUESTED_RANGE_NOT_SATISFIABLE";
    code[417] = "EXPECTATION_FAILED";
    code[418] = "IM_A_TEAPOT";
    code[426] = "UPGRADE_REQUIRED";
    code[428] = "PRECONDITION_REQUIRED";
    code[429] = "TOO_MANY_REQUEST";
    code[431] = "REQUEST_HEADER_FIELDS_TOO_LARGE";
    code[451] = "UNAVAILABLE_FOR_LEGAL_REASONS";
    code[500] = "INTERNAL_SERVER_ERROR";
    code[501] = "NOT_IMPLEMENTED";
    code[502] = "BAD_GATEWAY";
    code[503] = "SERVICE_UNAVAILABLE";
    code[504] = "GATEWAY_TIMEOUT";
    code[505] = "HTTP_VERSION_NOT_SUPPORTED";
    code[506] = "VARIANT_ALSO_NEGOTIATES";
    code[507] = "INSUFFICIENT_STORAGE";
    code[510] = "NOT_EXTENDED";
    code[511] = "NETWORK_AUTHENTICATION_REQUIRED";
};

Code::~Code() {}

std::string Code::getCodeMessage(int code)
{
    return (Code[code]);
}
