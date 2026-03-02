/*
 * Soroban C99 SDK - Single Header
 * Port of stellar/rs-soroban-sdk host function bindings.
 *
 * All Soroban values are 64-bit tagged (Val). Tag in low 8 bits, body in
 * upper 56.  Object handles have tag >= 64.  Host functions are imported
 * via WASM import_module / import_name attributes.
 */
#ifndef SOROBAN_H
#define SOROBAN_H

#ifndef __ASSEMBLER__
/* ---- C-only: types, host imports, inline helpers ---- */

#include <stdint.h>

/* ================================================================
 * Val type
 * ================================================================ */
typedef int64_t val;

/* ---- Tag constants ---- */
#define TAG_FALSE            0
#define TAG_TRUE             1
#define TAG_VOID             2
#define TAG_ERROR            3
#define TAG_U32              4
#define TAG_I32              5
#define TAG_U64_SMALL        6
#define TAG_I64_SMALL        7
#define TAG_TIMEPOINT_SMALL  8
#define TAG_DURATION_SMALL   9
#define TAG_U128_SMALL      10
#define TAG_I128_SMALL      11
#define TAG_U256_SMALL      12
#define TAG_I256_SMALL      13
#define TAG_SYMBOL_SMALL    14

#define TAG_U64_OBJECT      64
#define TAG_I64_OBJECT      65
#define TAG_TIMEPOINT_OBJECT 66
#define TAG_DURATION_OBJECT 67
#define TAG_U128_OBJECT     68
#define TAG_I128_OBJECT     69
#define TAG_U256_OBJECT     70
#define TAG_I256_OBJECT     71
#define TAG_BYTES_OBJECT    72
#define TAG_STRING_OBJECT   73
#define TAG_SYMBOL_OBJECT   74
#define TAG_VEC_OBJECT      75
#define TAG_MAP_OBJECT      76
#define TAG_ADDRESS_OBJECT  77

/* ---- Sentinel values ---- */
#define VAL_VOID   ((val)TAG_VOID)
#define VAL_TRUE   ((val)TAG_TRUE)
#define VAL_FALSE  ((val)TAG_FALSE)

/* ================================================================
 * Val inspection helpers
 * ================================================================ */
static inline int val_tag(val v) { return (int)(v & 0xFF); }
static inline int val_is_void(val v)    { return v == VAL_VOID; }
static inline int val_is_true(val v)    { return v == VAL_TRUE; }
static inline int val_is_false(val v)   { return v == VAL_FALSE; }
static inline int val_is_u32(val v)     { return val_tag(v) == TAG_U32; }
static inline int val_is_i32(val v)     { return val_tag(v) == TAG_I32; }
static inline int val_is_object(val v)  { return val_tag(v) >= 64; }
static inline int val_is_bytes(val v)   { return val_tag(v) == TAG_BYTES_OBJECT; }
static inline int val_is_string(val v)  { return val_tag(v) == TAG_STRING_OBJECT; }
static inline int val_is_symbol(val v)  {
    return val_tag(v) == TAG_SYMBOL_SMALL || val_tag(v) == TAG_SYMBOL_OBJECT;
}
static inline int val_is_vec(val v)     { return val_tag(v) == TAG_VEC_OBJECT; }
static inline int val_is_map(val v)     { return val_tag(v) == TAG_MAP_OBJECT; }
static inline int val_is_address(val v) { return val_tag(v) == TAG_ADDRESS_OBJECT; }
static inline int val_is_i128(val v) {
    return val_tag(v) == TAG_I128_SMALL || val_tag(v) == TAG_I128_OBJECT;
}
static inline int val_is_u128(val v) {
    return val_tag(v) == TAG_U128_SMALL || val_tag(v) == TAG_U128_OBJECT;
}

/* ================================================================
 * Val from/to — inline small types (no host call)
 * ================================================================ */

/* Boolean */
static inline val val_from_bool(int b)  { return b ? VAL_TRUE : VAL_FALSE; }
static inline int val_to_bool(val v)    { return v == VAL_TRUE; }

/* Void */
static inline val val_void(void) { return VAL_VOID; }

/* U32 — value in major (bits 63-32), minor=0, tag in bits 7-0 */
static inline val val_from_u32(uint32_t v) {
    return (val)(((uint64_t)v << 32) | TAG_U32);
}
static inline uint32_t val_to_u32(val v) {
    return (uint32_t)((uint64_t)v >> 32);
}

/* I32 — same layout as U32 but with TAG_I32 */
static inline val val_from_i32(int32_t v) {
    return (val)(((uint64_t)(uint32_t)v << 32) | TAG_I32);
}
static inline int32_t val_to_i32(val v) {
    return (int32_t)((uint64_t)v >> 32);
}

/* ---- Symbol small encoding (compile-time macro) ----
 * Encodes a string literal of up to 9 characters [_0-9A-Za-z] into a
 * TAG_SYMBOL_SMALL val at compile time. For runtime symbols use the
 * host function symbol_new_from_linear_memory via symbol_from_str(). */
#define _SYM_VALID(c) \
    ((c) == '_' || ((c) >= '0' && (c) <= '9') || \
     ((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))

#define _SYM_C(c) \
    ((c) == '_' ? 1ULL : \
     ((c) >= '0' && (c) <= '9') ? (2ULL + (unsigned)(c) - '0') : \
     ((c) >= 'A' && (c) <= 'Z') ? (12ULL + (unsigned)(c) - 'A') : \
     ((c) >= 'a' && (c) <= 'z') ? (38ULL + (unsigned)(c) - 'a') : 0ULL)

/* Compile-time check: negative array size if char is invalid */
#define _SYM_CHECK_CHAR(s, i) \
    ((void)sizeof(char[1 - 2 * ((i) < (int)(sizeof(s) - 1) && !_SYM_VALID((s)[i]))]))

#define _SYM_STEP(body, s, i) \
    ((i) < (int)(sizeof(s) - 1) ? ((body) << 6 | _SYM_C((s)[i])) : (body))

/* Compile-time checks: negative array size on invalid length or chars */
#define _SYM_CHECK_LEN(s) \
    ((void)sizeof(char[1 - 2 * (sizeof(s) - 1 > 9)]))

#define _SYM_CHECK(s) ( \
    _SYM_CHECK_LEN(s), \
    _SYM_CHECK_CHAR(s, 0), _SYM_CHECK_CHAR(s, 1), _SYM_CHECK_CHAR(s, 2), \
    _SYM_CHECK_CHAR(s, 3), _SYM_CHECK_CHAR(s, 4), _SYM_CHECK_CHAR(s, 5), \
    _SYM_CHECK_CHAR(s, 6), _SYM_CHECK_CHAR(s, 7), _SYM_CHECK_CHAR(s, 8))

#define symbol_small(s) (_SYM_CHECK(s), (val)(( \
    _SYM_STEP(_SYM_STEP(_SYM_STEP(_SYM_STEP(_SYM_STEP(_SYM_STEP(_SYM_STEP( \
    _SYM_STEP(_SYM_STEP(0ULL, s, 0), s, 1), s, 2), s, 3), \
    s, 4), s, 5), s, 6), s, 7), s, 8) \
    << 8) | TAG_SYMBOL_SMALL))

/* ---- Error val encoding ---- */
static inline val val_from_contract_error(uint32_t code) {
    return (val)(((uint64_t)code << 32) | TAG_ERROR);
}

/* ---- Storage type constants (raw enum, not Val-encoded) ---- */
#define STORAGE_TEMPORARY   ((val)0)
#define STORAGE_PERSISTENT  ((val)1)
#define STORAGE_INSTANCE    ((val)2)

/* ================================================================
 * Host function imports
 *
 * Module letters: x=context, l=ledger, a=address/auth, i=int,
 *   v=vec, m=map, b=buf, d=call, c=crypto, p=prng
 *
 * All params/returns are i64 (Val) at the WASM ABI level.
 * ================================================================ */

/* ---- Context "x" ---- */
__attribute__((import_module("x"), import_name("_")))
val log_from_linear_memory(val msg_pos, val msg_len, val vals_pos, val vals_len);

__attribute__((import_module("x"), import_name("0")))
val obj_cmp(val a, val b);

__attribute__((import_module("x"), import_name("1")))
val contract_event(val topics, val data);

__attribute__((import_module("x"), import_name("2")))
val get_ledger_version(void);

__attribute__((import_module("x"), import_name("3")))
val get_ledger_sequence(void);

__attribute__((import_module("x"), import_name("4")))
val get_ledger_timestamp(void);

__attribute__((import_module("x"), import_name("5")))
val fail_with_error(val error);

/* ---- Type assertion helpers ---- */
#define _require_type(v, check) do { if (!(check)(v)) fail_with_error(val_from_contract_error(0)); } while(0)
#define require_u32(v)     _require_type(v, val_is_u32)
#define require_i32(v)     _require_type(v, val_is_i32)
#define require_address(v) _require_type(v, val_is_address)
#define require_string(v)  _require_type(v, val_is_string)
#define require_symbol(v)  _require_type(v, val_is_symbol)
#define require_bytes(v)   _require_type(v, val_is_bytes)
#define require_vec(v)     _require_type(v, val_is_vec)
#define require_map(v)     _require_type(v, val_is_map)
#define require_i128(v)    _require_type(v, val_is_i128)
#define require_u128(v)    _require_type(v, val_is_u128)

__attribute__((import_module("x"), import_name("6")))
val get_ledger_network_id(void);

__attribute__((import_module("x"), import_name("7")))
val get_current_contract_address(void);

__attribute__((import_module("x"), import_name("8")))
val get_max_live_until_ledger(void);

/* ---- Int "i" ---- */
__attribute__((import_module("i"), import_name("_")))
val obj_from_u64(val v);

__attribute__((import_module("i"), import_name("0")))
val obj_to_u64(val obj);

__attribute__((import_module("i"), import_name("1")))
val obj_from_i64(val v);

__attribute__((import_module("i"), import_name("2")))
val obj_to_i64(val obj);

__attribute__((import_module("i"), import_name("3")))
val obj_from_u128_pieces(val hi, val lo);

__attribute__((import_module("i"), import_name("4")))
val obj_to_u128_lo64(val obj);

__attribute__((import_module("i"), import_name("5")))
val obj_to_u128_hi64(val obj);

__attribute__((import_module("i"), import_name("6")))
val obj_from_i128_pieces(val hi, val lo);

__attribute__((import_module("i"), import_name("7")))
val obj_to_i128_lo64(val obj);

__attribute__((import_module("i"), import_name("8")))
val obj_to_i128_hi64(val obj);

__attribute__((import_module("i"), import_name("9")))
val obj_from_u256_pieces(val hi_hi, val hi_lo, val lo_hi, val lo_lo);

__attribute__((import_module("i"), import_name("a")))
val obj_to_u256_hi_hi(val obj);

__attribute__((import_module("i"), import_name("b")))
val obj_to_u256_hi_lo(val obj);

__attribute__((import_module("i"), import_name("c")))
val obj_to_u256_lo_hi(val obj);

__attribute__((import_module("i"), import_name("d")))
val obj_to_u256_lo_lo(val obj);

__attribute__((import_module("i"), import_name("e")))
val obj_from_i256_pieces(val hi_hi, val hi_lo, val lo_hi, val lo_lo);

__attribute__((import_module("i"), import_name("f")))
val obj_to_i256_hi_hi(val obj);

__attribute__((import_module("i"), import_name("g")))
val obj_to_i256_hi_lo(val obj);

__attribute__((import_module("i"), import_name("h")))
val obj_to_i256_lo_hi(val obj);

__attribute__((import_module("i"), import_name("i")))
val obj_to_i256_lo_lo(val obj);

__attribute__((import_module("i"), import_name("j")))
val u256_add(val a, val b);

__attribute__((import_module("i"), import_name("k")))
val u256_sub(val a, val b);

__attribute__((import_module("i"), import_name("l")))
val u256_mul(val a, val b);

__attribute__((import_module("i"), import_name("m")))
val u256_div(val a, val b);

__attribute__((import_module("i"), import_name("n")))
val u256_rem_euclid(val a, val b);

__attribute__((import_module("i"), import_name("o")))
val u256_pow(val base, val exp);

__attribute__((import_module("i"), import_name("p")))
val u256_shl(val v, val shift);

__attribute__((import_module("i"), import_name("q")))
val u256_shr(val v, val shift);

__attribute__((import_module("i"), import_name("r")))
val i256_add(val a, val b);

__attribute__((import_module("i"), import_name("s")))
val i256_sub(val a, val b);

__attribute__((import_module("i"), import_name("t")))
val i256_mul(val a, val b);

__attribute__((import_module("i"), import_name("u")))
val i256_div(val a, val b);

__attribute__((import_module("i"), import_name("v")))
val i256_rem_euclid(val a, val b);

__attribute__((import_module("i"), import_name("w")))
val i256_pow(val base, val exp);

__attribute__((import_module("i"), import_name("x")))
val i256_shl(val v, val shift);

__attribute__((import_module("i"), import_name("y")))
val i256_shr(val v, val shift);

__attribute__((import_module("i"), import_name("z")))
val timepoint_obj_from_u64(val v);

__attribute__((import_module("i"), import_name("A")))
val timepoint_obj_to_u64(val obj);

__attribute__((import_module("i"), import_name("B")))
val duration_obj_from_u64(val v);

__attribute__((import_module("i"), import_name("C")))
val duration_obj_to_u64(val obj);

/* ---- Vec "v" ---- */
__attribute__((import_module("v"), import_name("_")))
val vec_new(void);

__attribute__((import_module("v"), import_name("0")))
val vec_put(val vec, val i, val x);

__attribute__((import_module("v"), import_name("1")))
val vec_get(val vec, val i);

__attribute__((import_module("v"), import_name("2")))
val vec_del(val vec, val i);

__attribute__((import_module("v"), import_name("3")))
val vec_len(val vec);

__attribute__((import_module("v"), import_name("4")))
val vec_push_front(val vec, val x);

__attribute__((import_module("v"), import_name("5")))
val vec_pop_front(val vec);

__attribute__((import_module("v"), import_name("6")))
val vec_push_back(val vec, val x);

__attribute__((import_module("v"), import_name("7")))
val vec_pop_back(val vec);

__attribute__((import_module("v"), import_name("8")))
val vec_front(val vec);

__attribute__((import_module("v"), import_name("9")))
val vec_back(val vec);

__attribute__((import_module("v"), import_name("a")))
val vec_insert(val vec, val i, val x);

__attribute__((import_module("v"), import_name("b")))
val vec_append(val vec_a, val vec_b);

__attribute__((import_module("v"), import_name("c")))
val vec_slice(val vec, val start, val end);

__attribute__((import_module("v"), import_name("g")))
val vec_new_from_linear_memory(val vals_pos, val len);

__attribute__((import_module("v"), import_name("h")))
val vec_unpack_to_linear_memory(val vec, val vals_pos, val len);

/* ---- Map "m" ---- */
__attribute__((import_module("m"), import_name("_")))
val map_new(void);

__attribute__((import_module("m"), import_name("0")))
val map_put(val m, val k, val v);

__attribute__((import_module("m"), import_name("1")))
val map_get(val m, val k);

__attribute__((import_module("m"), import_name("2")))
val map_del(val m, val k);

__attribute__((import_module("m"), import_name("3")))
val map_len(val m);

__attribute__((import_module("m"), import_name("4")))
val map_has(val m, val k);

__attribute__((import_module("m"), import_name("5")))
val map_key_by_pos(val m, val i);

__attribute__((import_module("m"), import_name("6")))
val map_val_by_pos(val m, val i);

__attribute__((import_module("m"), import_name("7")))
val map_keys(val m);

__attribute__((import_module("m"), import_name("8")))
val map_values(val m);

__attribute__((import_module("m"), import_name("9")))
val map_new_from_linear_memory(val keys_pos, val vals_pos, val len);

__attribute__((import_module("m"), import_name("a")))
val map_unpack_to_linear_memory(val m, val keys_pos, val vals_pos, val len);

/* ---- Ledger "l" ---- */
__attribute__((import_module("l"), import_name("_")))
val put_contract_data(val k, val v, val t);

__attribute__((import_module("l"), import_name("0")))
val has_contract_data(val k, val t);

__attribute__((import_module("l"), import_name("1")))
val get_contract_data(val k, val t);

__attribute__((import_module("l"), import_name("2")))
val del_contract_data(val k, val t);

__attribute__((import_module("l"), import_name("3")))
val create_contract(val deployer, val wasm_hash, val salt);

__attribute__((import_module("l"), import_name("4")))
val update_current_contract_wasm(val hash);

__attribute__((import_module("l"), import_name("7")))
val extend_contract_data_ttl(val k, val t, val threshold, val extend_to);

__attribute__((import_module("l"), import_name("8")))
val extend_current_contract_instance_and_code_ttl(val threshold, val extend_to);

/* ---- Call "d" ---- */
__attribute__((import_module("d"), import_name("_")))
val call(val contract, val func, val args);

__attribute__((import_module("d"), import_name("0")))
val try_call(val contract, val func, val args);

/* ---- Buf "b" ---- */
__attribute__((import_module("b"), import_name("_")))
val serialize_to_bytes(val v);

__attribute__((import_module("b"), import_name("0")))
val deserialize_from_bytes(val b);

__attribute__((import_module("b"), import_name("1")))
val bytes_copy_to_linear_memory(val b, val b_pos, val lm_pos, val len);

__attribute__((import_module("b"), import_name("2")))
val bytes_copy_from_linear_memory(val b, val b_pos, val lm_pos, val len);

__attribute__((import_module("b"), import_name("3")))
val bytes_new_from_linear_memory(val lm_pos, val len);

__attribute__((import_module("b"), import_name("4")))
val bytes_new(void);

__attribute__((import_module("b"), import_name("5")))
val bytes_put(val b, val i, val x);

__attribute__((import_module("b"), import_name("6")))
val bytes_get(val b, val i);

__attribute__((import_module("b"), import_name("7")))
val bytes_del(val b, val i);

__attribute__((import_module("b"), import_name("8")))
val bytes_len(val b);

__attribute__((import_module("b"), import_name("9")))
val bytes_push(val b, val x);

__attribute__((import_module("b"), import_name("a")))
val bytes_pop(val b);

__attribute__((import_module("b"), import_name("b")))
val bytes_front(val b);

__attribute__((import_module("b"), import_name("c")))
val bytes_back(val b);

__attribute__((import_module("b"), import_name("d")))
val bytes_insert(val b, val i, val x);

__attribute__((import_module("b"), import_name("e")))
val bytes_append(val b1, val b2);

__attribute__((import_module("b"), import_name("f")))
val bytes_slice(val b, val start, val end);

__attribute__((import_module("b"), import_name("g")))
val string_copy_to_linear_memory(val s, val s_pos, val lm_pos, val len);

__attribute__((import_module("b"), import_name("h")))
val string_copy_from_linear_memory(val s, val s_pos, val lm_pos, val len);

__attribute__((import_module("b"), import_name("i")))
val string_new_from_linear_memory(val lm_pos, val len);

__attribute__((import_module("b"), import_name("j")))
val symbol_new_from_linear_memory(val lm_pos, val len);

__attribute__((import_module("b"), import_name("k")))
val string_len(val s);

__attribute__((import_module("b"), import_name("l")))
val symbol_len(val s);

__attribute__((import_module("b"), import_name("m")))
val symbol_copy_to_linear_memory(val s, val s_pos, val lm_pos, val len);

__attribute__((import_module("b"), import_name("n")))
val symbol_index_in_linear_memory(val sym, val slices_pos, val len);

/* ---- Address / Auth "a" ---- */
__attribute__((import_module("a"), import_name("_")))
val require_auth_for_args(val address, val args);

__attribute__((import_module("a"), import_name("0")))
val require_auth(val address);

__attribute__((import_module("a"), import_name("3")))
val authorize_as_curr_contract(val auth_entries);

/* ---- Crypto "c" ---- */
__attribute__((import_module("c"), import_name("_")))
val compute_hash_sha256(val msg);

__attribute__((import_module("c"), import_name("0")))
val compute_hash_keccak256(val msg);

__attribute__((import_module("c"), import_name("1")))
val verify_sig_ed25519(val pub_key, val msg, val sig);

__attribute__((import_module("c"), import_name("2")))
val recover_key_ecdsa_secp256k1(val msg_digest, val sig, val rid);

/* ---- PRNG "p" ---- */
__attribute__((import_module("p"), import_name("_")))
val prng_reseed(val seed);

__attribute__((import_module("p"), import_name("0")))
val prng_bytes_new(val len);

__attribute__((import_module("p"), import_name("1")))
val prng_u64_in_inclusive_range(val lo, val hi);

__attribute__((import_module("p"), import_name("2")))
val prng_vec_shuffle(val vec);

/* ================================================================
 * Val from/to — types that may require host calls
 * ================================================================ */

/* U64 — small if < 2^56 */
static inline val val_from_u64(uint64_t v) {
    if (v < ((uint64_t)1 << 56))
        return (val)((v << 8) | TAG_U64_SMALL);
    return obj_from_u64((val)v);
}
static inline uint64_t val_to_u64(val v) {
    if (val_tag(v) == TAG_U64_SMALL)
        return (uint64_t)v >> 8;
    return (uint64_t)obj_to_u64(v);
}

/* I64 — small if fits in signed 56-bit */
static inline val val_from_i64(int64_t v) {
    if (v >= -((int64_t)1 << 55) && v < ((int64_t)1 << 55))
        return (val)(((uint64_t)v << 8) | TAG_I64_SMALL);
    return obj_from_i64((val)v);
}
static inline int64_t val_to_i64(val v) {
    if (val_tag(v) == TAG_I64_SMALL)
        return (int64_t)v >> 8;
    return (int64_t)obj_to_i64(v);
}

/* U128 */
static inline val val_from_u128(uint64_t hi, uint64_t lo) {
    if (hi == 0 && lo < ((uint64_t)1 << 56))
        return (val)((lo << 8) | TAG_U128_SMALL);
    return obj_from_u128_pieces((val)hi, (val)lo);
}
static inline void val_to_u128(val v, uint64_t *hi, uint64_t *lo) {
    if (val_tag(v) == TAG_U128_SMALL) {
        *hi = 0;
        *lo = (uint64_t)v >> 8;
    } else {
        *lo = (uint64_t)obj_to_u128_lo64(v);
        *hi = (uint64_t)obj_to_u128_hi64(v);
    }
}

/* I128 */
static inline val val_from_i128(int64_t hi, uint64_t lo) {
    if ((hi == 0 && lo < ((uint64_t)1 << 55)) ||
        (hi == -1 && lo >= (uint64_t)(-((int64_t)1 << 55)))) {
        /* Fits in signed 56-bit body */
        uint64_t body = lo & (((uint64_t)1 << 56) - 1);
        return (val)((body << 8) | TAG_I128_SMALL);
    }
    return obj_from_i128_pieces((val)hi, (val)lo);
}
static inline void val_to_i128(val v, int64_t *hi, uint64_t *lo) {
    if (val_tag(v) == TAG_I128_SMALL) {
        int64_t small = (int64_t)v >> 8; /* arithmetic shift sign-extends */
        *hi = small >> 63;               /* -1 if negative, 0 if positive */
        *lo = (uint64_t)small;
    } else {
        *lo = (uint64_t)obj_to_i128_lo64(v);
        *hi = (int64_t)obj_to_i128_hi64(v);
    }
}

/* Timepoint */
static inline val val_from_timepoint(uint64_t t) {
    if (t < ((uint64_t)1 << 56))
        return (val)((t << 8) | TAG_TIMEPOINT_SMALL);
    return timepoint_obj_from_u64((val)t);
}
static inline uint64_t val_to_timepoint(val v) {
    if (val_tag(v) == TAG_TIMEPOINT_SMALL)
        return (uint64_t)v >> 8;
    return (uint64_t)timepoint_obj_to_u64(v);
}

/* Duration */
static inline val val_from_duration(uint64_t d) {
    if (d < ((uint64_t)1 << 56))
        return (val)((d << 8) | TAG_DURATION_SMALL);
    return duration_obj_from_u64((val)d);
}
static inline uint64_t val_to_duration(val v) {
    if (val_tag(v) == TAG_DURATION_SMALL)
        return (uint64_t)v >> 8;
    return (uint64_t)duration_obj_to_u64(v);
}

/* ================================================================
 * String / Symbol from linear memory (create host objects)
 * ================================================================ */
static inline val symbol_from_str(const char *s, uint32_t len) {
    return symbol_new_from_linear_memory(val_from_u32((uint32_t)(uintptr_t)s),
                                         val_from_u32(len));
}
static inline val string_from_str(const char *s, uint32_t len) {
    return string_new_from_linear_memory(val_from_u32((uint32_t)(uintptr_t)s),
                                          val_from_u32(len));
}

/* ================================================================
 * i128 arithmetic helpers (decompose → operate → recompose)
 * ================================================================ */

__attribute__((noinline))
static val i128_add(val a, val b) {
    int64_t a_hi, b_hi;
    uint64_t a_lo, b_lo;
    val_to_i128(a, &a_hi, &a_lo);
    val_to_i128(b, &b_hi, &b_lo);
    uint64_t lo = a_lo + b_lo;
    int64_t carry = (lo < a_lo) ? 1 : 0;
    int64_t hi_tmp, hi;
    if (__builtin_add_overflow(a_hi, b_hi, &hi_tmp) ||
        __builtin_add_overflow(hi_tmp, carry, &hi))
        fail_with_error(val_from_contract_error(1));
    return val_from_i128(hi, lo);
}

__attribute__((noinline))
static val i128_sub(val a, val b) {
    int64_t a_hi, b_hi;
    uint64_t a_lo, b_lo;
    val_to_i128(a, &a_hi, &a_lo);
    val_to_i128(b, &b_hi, &b_lo);
    uint64_t lo = a_lo - b_lo;
    int64_t borrow = (a_lo < b_lo) ? 1 : 0;
    int64_t hi_tmp, hi;
    if (__builtin_sub_overflow(a_hi, b_hi, &hi_tmp) ||
        __builtin_sub_overflow(hi_tmp, borrow, &hi))
        fail_with_error(val_from_contract_error(1));
    return val_from_i128(hi, lo);
}

__attribute__((noinline))
static int i128_lt(val a, val b) {
    int64_t a_hi, b_hi;
    uint64_t a_lo, b_lo;
    val_to_i128(a, &a_hi, &a_lo);
    val_to_i128(b, &b_hi, &b_lo);
    if (a_hi != b_hi) return a_hi < b_hi;
    return a_lo < b_lo;
}

static inline int i128_gt(val a, val b) {
    return i128_lt(b, a);
}

__attribute__((noinline))
static int i128_is_zero(val v) {
    int64_t hi; uint64_t lo;
    val_to_i128(v, &hi, &lo);
    return hi == 0 && lo == 0;
}

/* ================================================================
 * Event helper
 * ================================================================ */
static inline void emit_event(val topic1, val data) {
    val topics = vec_new();
    topics = vec_push_back(topics, topic1);
    contract_event(topics, data);
}

static inline void emit_event2(val topic1, val topic2, val data) {
    val topics = vec_new();
    topics = vec_push_back(topics, topic1);
    topics = vec_push_back(topics, topic2);
    contract_event(topics, data);
}

static inline void emit_event3(val topic1, val topic2, val topic3, val data) {
    val topics = vec_new();
    topics = vec_push_back(topics, topic1);
    topics = vec_push_back(topics, topic2);
    topics = vec_push_back(topics, topic3);
    contract_event(topics, data);
}

#endif /* __ASSEMBLER__ */

/* ================================================================
 * WASM custom section embedding macros
 *
 * LLVM's WASM backend requires .s (assembly) files to create custom
 * sections — C inline asm puts data into linear memory instead.
 * These macros are designed to be used in .S files (preprocessed asm)
 * that are compiled alongside contract .c files.
 *
 * XDR is big-endian, WASM is little-endian, so we emit raw .byte
 * directives rather than .4byte (which would use target endianness).
 *
 * Example .S file:
 *   #include "soroban.h"
 *   SOROBAN_ENV_META(25, 0)
 *   SOROBAN_CONTRACT_SPEC(
 *       SPEC_FN("increment", 9, 0)
 *       SPEC_OUTPUT(SPEC_TYPE_U32)
 *   )
 * ================================================================ */
#define _SOROBAN_STRINGIFY(x) #x
#define _SOROBAN_TOSTR(x) _SOROBAN_STRINGIFY(x)

/* Emit a big-endian u32 via .byte (values must be 0-255) */
#define _XDR_U32(v) .byte 0,0,0,v;

/* ---- Env meta ---- */
#define SOROBAN_ENV_META(proto, pre)                                       \
    .section .custom_section.contractenvmetav0,"",@;                       \
    _XDR_U32(0)     /* SC_ENV_META_KIND_INTERFACE_VERSION */               \
    _XDR_U32(proto) /* protocol version */                                 \
    _XDR_U32(pre)   /* pre-release */

/* ---- ScSpec type constants (for SPEC_INPUT / SPEC_OUTPUT) ---- */
#define SPEC_TYPE_VAL        0
#define SPEC_TYPE_BOOL       1
#define SPEC_TYPE_VOID       2
#define SPEC_TYPE_ERROR      3
#define SPEC_TYPE_U32        4
#define SPEC_TYPE_I32        5
#define SPEC_TYPE_U64        6
#define SPEC_TYPE_I64        7
#define SPEC_TYPE_TIMEPOINT  8
#define SPEC_TYPE_DURATION   9
#define SPEC_TYPE_U128      10
#define SPEC_TYPE_I128      11
#define SPEC_TYPE_U256      12
#define SPEC_TYPE_I256      13
#define SPEC_TYPE_BYTES     14
#define SPEC_TYPE_STRING    16
#define SPEC_TYPE_SYMBOL    17
#define SPEC_TYPE_ADDRESS   19

/* ---- Contract spec building blocks ---- */

/* Begin a function spec entry.
   name:  function name as a C string literal (e.g. "mint")
   nlen:  byte-length of the name (e.g. 4)
   nin:   number of inputs (e.g. 2) */
#define SPEC_FN(name, nlen, nin)                                           \
    _XDR_U32(0)     /* SC_SPEC_ENTRY_FUNCTION_V0 */                        \
    _XDR_U32(0)     /* doc: empty string */                                \
    _XDR_U32(nlen)  /* name length */                                      \
    .ascii name;                                                           \
    .balign 4, 0;                                                          \
    _XDR_U32(nin)   /* inputs count */

/* Describe one function input.
   name:  parameter name as a C string literal
   nlen:  byte-length of the name
   type:  SPEC_TYPE_* constant */
#define SPEC_INPUT(name, nlen, type)                                       \
    _XDR_U32(0)     /* doc: empty string */                                \
    _XDR_U32(nlen)  /* name length */                                      \
    .ascii name;                                                           \
    .balign 4, 0;                                                          \
    _XDR_U32(type)  /* ScSpecTypeDef */

/* Declare one output type */
#define SPEC_OUTPUT(type)                                                  \
    _XDR_U32(1)     /* outputs count = 1 */                                \
    _XDR_U32(type)  /* ScSpecTypeDef */

/* Declare void return (0 outputs) */
#define SPEC_OUTPUT_VOID                                                   \
    _XDR_U32(0)     /* outputs count = 0 */

/* Begin an error enum spec entry.
   name:    enum name as a C string literal (e.g. "Error")
   nlen:    byte-length of the name
   ncases:  number of error cases */
#define SPEC_ERROR_ENUM(name, nlen, ncases)                                \
    _XDR_U32(4)     /* SC_SPEC_ENTRY_UDT_ERROR_ENUM_V0 */                  \
    _XDR_U32(0)     /* doc: empty string */                                \
    _XDR_U32(0)     /* lib: empty string */                                \
    _XDR_U32(nlen)  /* name length */                                      \
    .ascii name;                                                           \
    .balign 4, 0;                                                          \
    _XDR_U32(ncases) /* cases count */

/* Describe one error case.
   name:   case name as a C string literal (e.g. "Overflow")
   nlen:   byte-length of the name
   value:  error code (u32) */
#define SPEC_ERROR_CASE(name, nlen, value)                                 \
    _XDR_U32(0)     /* doc: empty string */                                \
    _XDR_U32(nlen)  /* name length */                                      \
    .ascii name;                                                           \
    .balign 4, 0;                                                          \
    _XDR_U32(value) /* error value */

/* Wrap all spec entries in a contractspecv0 custom section */
#define SOROBAN_CONTRACT_SPEC(...)                                         \
    .section .custom_section.contractspecv0,"",@;                          \
    __VA_ARGS__

#endif /* SOROBAN_H */
