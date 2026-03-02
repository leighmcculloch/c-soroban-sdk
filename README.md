# soroban-sdk-c

> [!CAUTION]
> This is experimental and should not be used for anything other than toy experiments.

A [C99] SDK for writing [Soroban] smart contracts on the [Stellar] network.

The SDK depends on C99 and the following clang/GCC builtins for overflow-checked arithmetic:
- `__builtin_add_overflow`
- `__builtin_sub_overflow`

## Requirements

- [LLVM](https://llvm.org) clang and lld (for `wasm32` target, `wasm-ld`, and overflow builtins)
- [Stellar CLI](https://github.com/stellar/stellar-cli) (for deploying and invoking contracts)

### macOS

```
brew install llvm lld
export PATH="$(brew --prefix llvm)/bin:$PATH"
```

### Ubuntu/Debian

```
sudo apt-get install clang lld
```

## Build

```
make
```

## Examples

| Example | Description |
|---------|-------------|
| [increment](contracts/increment) | Counter stored in contract data |
| [token](contracts/token) | SEP-41 fungible token |

## Deploy

```
stellar network use testnet
stellar keys generate testacc --fund
stellar keys use testacc

# Increment
stellar contract deploy \
  --wasm contracts/increment/increment.wasm \
  --alias increment

# Token (with constructor args)
stellar contract deploy \
  --wasm contracts/token/token.wasm \
  --alias token \
  -- \
  --admin testacc \
  --decimal 2 \
  --name "C Token" \
  --symbol "CT"
```

## Test

```
# Increment
stellar contract invoke --id increment -- increment

# Token
stellar contract invoke --id token -- balance --id testacc
stellar contract invoke --id token -- mint --to testacc --amount 1234
stellar contract invoke --id token -- balance --id testacc
stellar contract invoke --id token -- transfer --from testacc --to CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABSC4 --amount 123
stellar contract invoke --id token -- balance --id testacc
stellar contract invoke --id token -- balance --id CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABSC4
```

## License

Copyright 2026 Stellar Development Foundation (This is not an official project of the Stellar Development Foundation)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[C99]: https://open-std.org/JTC1/SC22/WG14/www/docs/n1256.pdf
[Soroban]: https://soroban.stellar.org
[Stellar]: https://stellar.org

