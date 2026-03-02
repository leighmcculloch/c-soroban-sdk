# soroban.h API Reference

Single-header C99 SDK for writing Soroban smart contracts.

All Soroban values are 64-bit tagged integers (`val`). The tag occupies bits 7-0, and the body occupies the upper bits. Object handles have tags >= 64 and refer to host-managed objects.

## Types

### `val`

```c
typedef int64_t val;
```

The universal value type. All host function parameters and return values are `val`.

## Constants

### Tags

| Constant | Value | Description |
|----------|-------|-------------|
| `TAG_FALSE` | 0 | Boolean false |
| `TAG_TRUE` | 1 | Boolean true |
| `TAG_VOID` | 2 | Void / unit |
| `TAG_ERROR` | 3 | Error value |
| `TAG_U32` | 4 | Unsigned 32-bit integer |
| `TAG_I32` | 5 | Signed 32-bit integer |
| `TAG_U64_SMALL` | 6 | Small u64 (fits in 56 bits) |
| `TAG_I64_SMALL` | 7 | Small i64 (fits in 56 bits) |
| `TAG_TIMEPOINT_SMALL` | 8 | Small timepoint |
| `TAG_DURATION_SMALL` | 9 | Small duration |
| `TAG_U128_SMALL` | 10 | Small u128 (fits in 56 bits) |
| `TAG_I128_SMALL` | 11 | Small i128 (fits in 56 bits) |
| `TAG_U256_SMALL` | 12 | Small u256 |
| `TAG_I256_SMALL` | 13 | Small i256 |
| `TAG_SYMBOL_SMALL` | 14 | Inline symbol (up to 9 chars) |
| `TAG_U64_OBJECT` | 64 | Host object: u64 |
| `TAG_I64_OBJECT` | 65 | Host object: i64 |
| `TAG_TIMEPOINT_OBJECT` | 66 | Host object: timepoint |
| `TAG_DURATION_OBJECT` | 67 | Host object: duration |
| `TAG_U128_OBJECT` | 68 | Host object: u128 |
| `TAG_I128_OBJECT` | 69 | Host object: i128 |
| `TAG_U256_OBJECT` | 70 | Host object: u256 |
| `TAG_I256_OBJECT` | 71 | Host object: i256 |
| `TAG_BYTES_OBJECT` | 72 | Host object: bytes |
| `TAG_STRING_OBJECT` | 73 | Host object: string |
| `TAG_SYMBOL_OBJECT` | 74 | Host object: symbol |
| `TAG_VEC_OBJECT` | 75 | Host object: vec |
| `TAG_MAP_OBJECT` | 76 | Host object: map |
| `TAG_ADDRESS_OBJECT` | 77 | Host object: address |

### Sentinel Values

| Constant | Description |
|----------|-------------|
| `VAL_VOID` | Void value |
| `VAL_TRUE` | Boolean true |
| `VAL_FALSE` | Boolean false |

### Storage Types

| Constant | Value | Description |
|----------|-------|-------------|
| `STORAGE_TEMPORARY` | 0 | Deleted after TTL expires |
| `STORAGE_PERSISTENT` | 1 | Persists, must extend TTL |
| `STORAGE_INSTANCE` | 2 | Bound to contract instance lifetime |

## Val Inspection

```c
int val_tag(val v)
```
Extract the tag (bits 7-0).

```c
int val_is_void(val v)
int val_is_true(val v)
int val_is_false(val v)
int val_is_u32(val v)
int val_is_i32(val v)
int val_is_object(val v)
int val_is_bytes(val v)
int val_is_string(val v)
int val_is_symbol(val v)
int val_is_vec(val v)
int val_is_map(val v)
int val_is_address(val v)
```
Type-check predicates. Return 1 if the value matches the type, 0 otherwise.

## Val Conversion — Inline Types

These operate entirely inline with no host calls.

### Boolean

```c
val val_from_bool(int b)
int val_to_bool(val v)
```

### Void

```c
val val_void(void)
```

### U32

```c
val val_from_u32(uint32_t v)
uint32_t val_to_u32(val v)
```
Value stored in bits 63-32, tag `TAG_U32` in bits 7-0.

### I32

```c
val val_from_i32(int32_t v)
int32_t val_to_i32(val v)
```

### Symbol Small

```c
#define symbol_small(s)
```
Compile-time macro that encodes a string literal of up to 9 characters into a `TAG_SYMBOL_SMALL` val. Valid characters are `[_0-9A-Za-z]`, encoded as 6-bit values: `_`=1, `0-9`=2-11, `A-Z`=12-37, `a-z`=38-63. Strings longer than 9 characters or containing invalid characters produce a compile-time error. For runtime symbol creation use `symbol_from_str()`.

### Contract Error

```c
val val_from_contract_error(uint32_t code)
```
Create an error `val` with tag `TAG_ERROR` and the given contract error code.

## Val Conversion — Host-Backed Types

These use inline encoding for small values and fall back to host object creation for large values.

### U64

```c
val val_from_u64(uint64_t v)
uint64_t val_to_u64(val v)
```
Inline if `v < 2^56`, otherwise creates a host object.

### I64

```c
val val_from_i64(int64_t v)
int64_t val_to_i64(val v)
```
Inline if value fits in signed 56 bits.

### U128

```c
val val_from_u128(uint64_t hi, uint64_t lo)
void val_to_u128(val v, uint64_t *hi, uint64_t *lo)
```
Inline if `hi == 0` and `lo < 2^56`.

### I128

```c
val val_from_i128(int64_t hi, uint64_t lo)
void val_to_i128(val v, int64_t *hi, uint64_t *lo)
```
Inline if value fits in signed 56 bits.

### Timepoint

```c
val val_from_timepoint(uint64_t t)
uint64_t val_to_timepoint(val v)
```

### Duration

```c
val val_from_duration(uint64_t d)
uint64_t val_to_duration(val v)
```

## String / Symbol from Linear Memory

Create host objects from C strings in linear memory.

```c
val symbol_from_str(const char *s, uint32_t len)
val string_from_str(const char *s, uint32_t len)
```

## i128 Arithmetic

Helper functions that decompose i128 host objects, perform arithmetic, and recompose the result.

```c
val i128_add(val a, val b)
val i128_sub(val a, val b)
int i128_lt(val a, val b)
int i128_gt(val a, val b)
int i128_is_zero(val v)
```

## Event Helpers

Emit contract events with 1, 2, or 3 topics.

```c
void emit_event(val topic1, val data)
void emit_event2(val topic1, val topic2, val data)
void emit_event3(val topic1, val topic2, val topic3, val data)
```

## Host Functions

All host functions are imported via WASM `import_module`/`import_name` attributes. Parameters and return values are `val` (i64 at the ABI level).

### Context (`x`)

| Function | Description |
|----------|-------------|
| `val log_from_linear_memory(val msg_pos, val msg_len, val vals_pos, val vals_len)` | Log a message |
| `val obj_cmp(val a, val b)` | Compare two objects |
| `val contract_event(val topics, val data)` | Emit a contract event |
| `val get_ledger_version(void)` | Current ledger protocol version |
| `val get_ledger_sequence(void)` | Current ledger sequence number |
| `val get_ledger_timestamp(void)` | Current ledger timestamp |
| `val fail_with_error(val error)` | Abort with an error |
| `val get_ledger_network_id(void)` | Network ID hash |
| `val get_current_contract_address(void)` | This contract's address |
| `val get_max_live_until_ledger(void)` | Max live-until ledger |

### Integer (`i`)

| Function | Description |
|----------|-------------|
| `val obj_from_u64(val v)` | Create u64 object |
| `val obj_to_u64(val obj)` | Extract u64 from object |
| `val obj_from_i64(val v)` | Create i64 object |
| `val obj_to_i64(val obj)` | Extract i64 from object |
| `val obj_from_u128_pieces(val hi, val lo)` | Create u128 object from hi/lo |
| `val obj_to_u128_lo64(val obj)` | Extract u128 low 64 bits |
| `val obj_to_u128_hi64(val obj)` | Extract u128 high 64 bits |
| `val obj_from_i128_pieces(val hi, val lo)` | Create i128 object from hi/lo |
| `val obj_to_i128_lo64(val obj)` | Extract i128 low 64 bits |
| `val obj_to_i128_hi64(val obj)` | Extract i128 high 64 bits |
| `val obj_from_u256_pieces(val hi_hi, val hi_lo, val lo_hi, val lo_lo)` | Create u256 object |
| `val obj_to_u256_hi_hi(val obj)` | Extract u256 bits 255-192 |
| `val obj_to_u256_hi_lo(val obj)` | Extract u256 bits 191-128 |
| `val obj_to_u256_lo_hi(val obj)` | Extract u256 bits 127-64 |
| `val obj_to_u256_lo_lo(val obj)` | Extract u256 bits 63-0 |
| `val obj_from_i256_pieces(val hi_hi, val hi_lo, val lo_hi, val lo_lo)` | Create i256 object |
| `val obj_to_i256_hi_hi(val obj)` | Extract i256 bits 255-192 |
| `val obj_to_i256_hi_lo(val obj)` | Extract i256 bits 191-128 |
| `val obj_to_i256_lo_hi(val obj)` | Extract i256 bits 127-64 |
| `val obj_to_i256_lo_lo(val obj)` | Extract i256 bits 63-0 |
| `val u256_add(val a, val b)` | u256 addition |
| `val u256_sub(val a, val b)` | u256 subtraction |
| `val u256_mul(val a, val b)` | u256 multiplication |
| `val u256_div(val a, val b)` | u256 division |
| `val u256_rem_euclid(val a, val b)` | u256 Euclidean remainder |
| `val u256_pow(val base, val exp)` | u256 exponentiation |
| `val u256_shl(val v, val shift)` | u256 shift left |
| `val u256_shr(val v, val shift)` | u256 shift right |
| `val i256_add(val a, val b)` | i256 addition |
| `val i256_sub(val a, val b)` | i256 subtraction |
| `val i256_mul(val a, val b)` | i256 multiplication |
| `val i256_div(val a, val b)` | i256 division |
| `val i256_rem_euclid(val a, val b)` | i256 Euclidean remainder |
| `val i256_pow(val base, val exp)` | i256 exponentiation |
| `val i256_shl(val v, val shift)` | i256 shift left |
| `val i256_shr(val v, val shift)` | i256 shift right |
| `val timepoint_obj_from_u64(val v)` | Create timepoint object |
| `val timepoint_obj_to_u64(val obj)` | Extract timepoint as u64 |
| `val duration_obj_from_u64(val v)` | Create duration object |
| `val duration_obj_to_u64(val obj)` | Extract duration as u64 |

### Vec (`v`)

| Function | Description |
|----------|-------------|
| `val vec_new(void)` | Create empty vec |
| `val vec_put(val vec, val i, val x)` | Set element at index |
| `val vec_get(val vec, val i)` | Get element at index |
| `val vec_del(val vec, val i)` | Delete element at index |
| `val vec_len(val vec)` | Length of vec |
| `val vec_push_front(val vec, val x)` | Push to front |
| `val vec_pop_front(val vec)` | Pop from front |
| `val vec_push_back(val vec, val x)` | Push to back |
| `val vec_pop_back(val vec)` | Pop from back |
| `val vec_front(val vec)` | First element |
| `val vec_back(val vec)` | Last element |
| `val vec_insert(val vec, val i, val x)` | Insert at index |
| `val vec_append(val vec_a, val vec_b)` | Concatenate two vecs |
| `val vec_slice(val vec, val start, val end)` | Slice a subrange |
| `val vec_new_from_linear_memory(val vals_pos, val len)` | Create from linear memory |
| `val vec_unpack_to_linear_memory(val vec, val vals_pos, val len)` | Copy to linear memory |

### Map (`m`)

| Function | Description |
|----------|-------------|
| `val map_new(void)` | Create empty map |
| `val map_put(val m, val k, val v)` | Insert or update key |
| `val map_get(val m, val k)` | Get value by key |
| `val map_del(val m, val k)` | Delete key |
| `val map_len(val m)` | Number of entries |
| `val map_has(val m, val k)` | Check if key exists |
| `val map_key_by_pos(val m, val i)` | Key at position |
| `val map_val_by_pos(val m, val i)` | Value at position |
| `val map_keys(val m)` | Vec of all keys |
| `val map_values(val m)` | Vec of all values |
| `val map_new_from_linear_memory(val keys_pos, val vals_pos, val len)` | Create from linear memory |
| `val map_unpack_to_linear_memory(val m, val keys_pos, val vals_pos, val len)` | Copy to linear memory |

### Ledger (`l`)

| Function | Description |
|----------|-------------|
| `val put_contract_data(val k, val v, val t)` | Store contract data |
| `val has_contract_data(val k, val t)` | Check if key exists |
| `val get_contract_data(val k, val t)` | Read contract data |
| `val del_contract_data(val k, val t)` | Delete contract data |
| `val create_contract(val deployer, val wasm_hash, val salt)` | Deploy a new contract |
| `val update_current_contract_wasm(val hash)` | Upgrade contract code |
| `val extend_contract_data_ttl(val k, val t, val threshold, val extend_to)` | Extend data TTL |
| `val extend_current_contract_instance_and_code_ttl(val threshold, val extend_to)` | Extend instance TTL |

### Call (`d`)

| Function | Description |
|----------|-------------|
| `val call(val contract, val func, val args)` | Cross-contract call |
| `val try_call(val contract, val func, val args)` | Cross-contract call (returns error instead of trapping) |

### Buf (`b`)

| Function | Description |
|----------|-------------|
| `val serialize_to_bytes(val v)` | Serialize value to XDR bytes |
| `val deserialize_from_bytes(val b)` | Deserialize XDR bytes to value |
| `val bytes_copy_to_linear_memory(val b, val b_pos, val lm_pos, val len)` | Copy bytes to linear memory |
| `val bytes_copy_from_linear_memory(val b, val b_pos, val lm_pos, val len)` | Copy linear memory to bytes |
| `val bytes_new_from_linear_memory(val lm_pos, val len)` | Create bytes from linear memory |
| `val bytes_new(void)` | Create empty bytes |
| `val bytes_put(val b, val i, val x)` | Set byte at index |
| `val bytes_get(val b, val i)` | Get byte at index |
| `val bytes_del(val b, val i)` | Delete byte at index |
| `val bytes_len(val b)` | Length of bytes |
| `val bytes_push(val b, val x)` | Append byte |
| `val bytes_pop(val b)` | Remove last byte |
| `val bytes_front(val b)` | First byte |
| `val bytes_back(val b)` | Last byte |
| `val bytes_insert(val b, val i, val x)` | Insert byte at index |
| `val bytes_append(val b1, val b2)` | Concatenate two bytes |
| `val bytes_slice(val b, val start, val end)` | Slice a subrange |
| `val string_copy_to_linear_memory(val s, val s_pos, val lm_pos, val len)` | Copy string to linear memory |
| `val string_copy_from_linear_memory(val s, val s_pos, val lm_pos, val len)` | Copy linear memory to string |
| `val string_new_from_linear_memory(val lm_pos, val len)` | Create string from linear memory |
| `val symbol_new_from_linear_memory(val lm_pos, val len)` | Create symbol from linear memory |
| `val string_len(val s)` | Length of string |
| `val symbol_len(val s)` | Length of symbol |
| `val symbol_copy_to_linear_memory(val s, val s_pos, val lm_pos, val len)` | Copy symbol to linear memory |
| `val symbol_index_in_linear_memory(val sym, val slices_pos, val len)` | Find symbol in linear memory |

### Address / Auth (`a`)

| Function | Description |
|----------|-------------|
| `val require_auth_for_args(val address, val args)` | Require auth with specific args |
| `val require_auth(val address)` | Require auth for current call args |
| `val authorize_as_curr_contract(val auth_entries)` | Authorize as current contract |

### Crypto (`c`)

| Function | Description |
|----------|-------------|
| `val compute_hash_sha256(val msg)` | SHA-256 hash |
| `val compute_hash_keccak256(val msg)` | Keccak-256 hash |
| `val verify_sig_ed25519(val pub_key, val msg, val sig)` | Verify Ed25519 signature |
| `val recover_key_ecdsa_secp256k1(val msg_digest, val sig, val rid)` | Recover secp256k1 public key |

### PRNG (`p`)

| Function | Description |
|----------|-------------|
| `val prng_reseed(val seed)` | Reseed the PRNG |
| `val prng_bytes_new(val len)` | Generate random bytes |
| `val prng_u64_in_inclusive_range(val lo, val hi)` | Random u64 in range |
| `val prng_vec_shuffle(val vec)` | Shuffle a vec |

## Assembly Macros

These macros are for use in `.S` files (preprocessed assembly) to embed contract metadata in WASM custom sections.

### `SOROBAN_ENV_META(proto, pre)`

Emit a `contractenvmetav0` custom section declaring the protocol version.

```asm
SOROBAN_ENV_META(25, 0)
```

### `SOROBAN_CONTRACT_SPEC(...)`

Emit a `contractspecv0` custom section containing function specs.

### `SPEC_FN(name, nlen, nin)`

Begin a function spec entry. `name` is the function name string, `nlen` is its byte length, `nin` is the number of inputs.

### `SPEC_INPUT(name, nlen, type)`

Describe one function input. `type` is a `SPEC_TYPE_*` constant.

### `SPEC_OUTPUT(type)`

Declare a single output type.

### `SPEC_OUTPUT_VOID`

Declare void return (no outputs).

### Spec Type Constants

| Constant | Value |
|----------|-------|
| `SPEC_TYPE_VAL` | 0 |
| `SPEC_TYPE_BOOL` | 1 |
| `SPEC_TYPE_VOID` | 2 |
| `SPEC_TYPE_ERROR` | 3 |
| `SPEC_TYPE_U32` | 4 |
| `SPEC_TYPE_I32` | 5 |
| `SPEC_TYPE_U64` | 6 |
| `SPEC_TYPE_I64` | 7 |
| `SPEC_TYPE_TIMEPOINT` | 8 |
| `SPEC_TYPE_DURATION` | 9 |
| `SPEC_TYPE_U128` | 10 |
| `SPEC_TYPE_I128` | 11 |
| `SPEC_TYPE_U256` | 12 |
| `SPEC_TYPE_I256` | 13 |
| `SPEC_TYPE_BYTES` | 14 |
| `SPEC_TYPE_STRING` | 16 |
| `SPEC_TYPE_SYMBOL` | 17 |
| `SPEC_TYPE_ADDRESS` | 19 |

### Example `.S` File

```asm
#include "soroban.h"

SOROBAN_ENV_META(25, 0)

SOROBAN_CONTRACT_SPEC(
    SPEC_FN("mint", 4, 2)
    SPEC_INPUT("to",     2, SPEC_TYPE_ADDRESS)
    SPEC_INPUT("amount", 6, SPEC_TYPE_I128)
    SPEC_OUTPUT_VOID

    SPEC_FN("balance", 7, 1)
    SPEC_INPUT("id", 2, SPEC_TYPE_ADDRESS)
    SPEC_OUTPUT(SPEC_TYPE_I128)
)
```
