/*
 * SEP-41 Token contract (simplified)
 *
 * Constructor: __constructor(admin, decimal, name, symbol)
 * Mint:        mint(to, amount)
 * Transfer:    transfer(from, to, amount)
 * Query:       balance(id), decimals(), name(), symbol()
 *
 * Storage layout:
 *   Instance:   ADMIN (Address), DECIMAL (u32), NAME (String), SYMBOL (String)
 *   Persistent: balance keyed by address
 *
 * Env meta and contract spec are in token.S (custom WASM sections).
 */
#include "soroban.h"

/* ---- Storage keys (instance) ---- */
#define KEY_ADMIN   symbol_small("ADMIN")
#define KEY_DECIMAL symbol_small("DECIMAL")
#define KEY_NAME    symbol_small("NAME")
#define KEY_SYMBOL  symbol_small("SYMBOL")

/* ---- Helpers ---- */
static val get_balance(val addr) {
    if (val_to_bool(has_contract_data(addr, STORAGE_PERSISTENT)))
        return get_contract_data(addr, STORAGE_PERSISTENT);
    return val_from_i128(0, 0);
}

static void set_balance(val addr, val amount) {
    put_contract_data(addr, amount, STORAGE_PERSISTENT);
    extend_contract_data_ttl(addr, STORAGE_PERSISTENT,
                             val_from_u32(100), val_from_u32(500));
}

static val get_admin(void) {
    return get_contract_data(KEY_ADMIN, STORAGE_INSTANCE);
}

/* ================================================================
 * Contract entry points
 * ================================================================ */

val __constructor(val admin, val decimal, val name_v, val sym)
    __attribute__((export_name("__constructor")));

val __constructor(val admin, val decimal, val name_v, val sym) {
    put_contract_data(KEY_ADMIN,   admin,   STORAGE_INSTANCE);
    put_contract_data(KEY_DECIMAL, decimal, STORAGE_INSTANCE);
    put_contract_data(KEY_NAME,    name_v,  STORAGE_INSTANCE);
    put_contract_data(KEY_SYMBOL,  sym,     STORAGE_INSTANCE);
    return VAL_VOID;
}

val mint(val to, val amount)
    __attribute__((export_name("mint")));

val mint(val to, val amount) {
    val admin = get_admin();
    require_auth(admin);

    val bal = get_balance(to);
    set_balance(to, i128_add(bal, amount));

    emit_event3(symbol_small("mint"), admin, to, amount);

    extend_current_contract_instance_and_code_ttl(
        val_from_u32(100), val_from_u32(500));

    return VAL_VOID;
}

val transfer(val from, val to, val amount)
    __attribute__((export_name("transfer")));

val transfer(val from, val to, val amount) {
    require_auth(from);

    val from_bal = get_balance(from);
    val to_bal   = get_balance(to);

    if (i128_lt(from_bal, amount))
        fail_with_error(val_from_contract_error(1));

    set_balance(from, i128_sub(from_bal, amount));
    set_balance(to,   i128_add(to_bal, amount));

    emit_event3(symbol_small("transfer"), from, to, amount);

    extend_current_contract_instance_and_code_ttl(
        val_from_u32(100), val_from_u32(500));

    return VAL_VOID;
}

val balance(val id)
    __attribute__((export_name("balance")));

val balance(val id) {
    return get_balance(id);
}

val decimals(void)
    __attribute__((export_name("decimals")));

val decimals(void) {
    return get_contract_data(KEY_DECIMAL, STORAGE_INSTANCE);
}

val name(void)
    __attribute__((export_name("name")));

val name(void) {
    return get_contract_data(KEY_NAME, STORAGE_INSTANCE);
}

val symbol(void)
    __attribute__((export_name("symbol")));

val symbol(void) {
    return get_contract_data(KEY_SYMBOL, STORAGE_INSTANCE);
}
