#include "soroban.h"

SOROBAN_ENV_META(25, 0)

SOROBAN_CONTRACT_SPEC(
    SPEC_ERROR_ENUM("Error", 5, 1)
    SPEC_ERROR_CASE("InsufficientBalance", 19, 1)

    SPEC_FN("__constructor", 13, 3)
    SPEC_INPUT("admin",   5, SPEC_TYPE_ADDRESS)
    SPEC_INPUT("decimal", 7, SPEC_TYPE_U32)
    SPEC_INPUT("symbol",  6, SPEC_TYPE_STRING)
    SPEC_OUTPUT_VOID

    SPEC_FN("mint", 4, 2)
    SPEC_INPUT("to",     2, SPEC_TYPE_ADDRESS)
    SPEC_INPUT("amount", 6, SPEC_TYPE_I128)
    SPEC_OUTPUT_VOID

    SPEC_FN("transfer", 8, 3)
    SPEC_INPUT("from",   4, SPEC_TYPE_ADDRESS)
    SPEC_INPUT("to",     2, SPEC_TYPE_ADDRESS)
    SPEC_INPUT("amount", 6, SPEC_TYPE_I128)
    SPEC_OUTPUT_VOID

    SPEC_FN("balance", 7, 1)
    SPEC_INPUT("id", 2, SPEC_TYPE_ADDRESS)
    SPEC_OUTPUT(SPEC_TYPE_I128)

    SPEC_FN("decimals", 8, 0)
    SPEC_OUTPUT(SPEC_TYPE_U32)

    SPEC_FN("name", 4, 0)
    SPEC_OUTPUT(SPEC_TYPE_STRING)

    SPEC_FN("symbol", 6, 0)
    SPEC_OUTPUT(SPEC_TYPE_STRING)
)
