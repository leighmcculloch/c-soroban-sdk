/*
 * Increment contract — stores a counter in instance storage, increments on
 * each call, extends TTL, and returns the new value.
 *
 * Env meta and contract spec are in increment.S (custom WASM sections).
 */
#include "soroban.h"

val increment(void) __attribute__((export_name("increment")));

val increment(void) {
    val key = symbol_small("COUNTER");

    /* Read current value (default 0) */
    uint32_t count = 0;
    if (val_to_bool(has_contract_data(key, STORAGE_INSTANCE)))
        count = val_to_u32(get_contract_data(key, STORAGE_INSTANCE));

    /* Increment with overflow check */
    if (__builtin_add_overflow(count, 1u, &count))
        fail_with_error(val_from_contract_error(1));
    put_contract_data(key, val_from_u32(count), STORAGE_INSTANCE);

    return val_from_u32(count);
}
