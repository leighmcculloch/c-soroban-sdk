#include "soroban.h"

SOROBAN_ENV_META(25, 0)

SOROBAN_CONTRACT_SPEC(
    SPEC_FN("increment", 9, 0)
    SPEC_OUTPUT(SPEC_TYPE_U32)
)
