# yeetify
Yeetify is a humorous utility meant to turn the entirety of your C or C++ code into permutations of the word 'yeet'.

Inspired by [this reddit post](https://www.reddit.com/r/ProgrammerHumor/comments/bgdxwn/yeet/).

## Installation & Building

### Prerequisites
* [Bazel](https://bazel.build/) (or [Bazelisk](https://github.com/bazelbuild/bazelisk)): `brew install bazelisk`
* LLVM (`libclang`): `brew install llvm`

### Building
To build the `yeetify` binary:
```bash
bazel build //:yeetify
```
The executable will be located at `bazel-bin/yeetify`.

### Testing
Run the complete test suite (unit tests and end-to-end compilation tests with GoogleTest):
```bash
bazel test //...
```

## Usage
Simply invoke:
```bash
./bazel-bin/yeetify <source_file> [-o <output_file>] [--seed <number>]
```
* Supports `.c`, `.cc`, `.cpp`, `.cxx`, `.h`, and `.hpp` files.
* Tokenization is powered by **`libclang`**, so standard C/C++ code is supported without requiring spaces between operators/delimiters, and multi-word string literals or comments work seamlessly out of the box.

## How it works
The program leverages `libclang` to parse and tokenize the source file according to standard C/C++ lexing grammar:
1. **Preprocessor Directives**: Preserves top-level `#include`, `#define`, `#pragma`, and multiline directives as-is.
2. **Tokenization**: Replaces code tokens (identifiers, keywords, literals including strings with spaces, operators, and punctuation) with unique permutations of the word `'yeet'`.
3. **Macro Definitions**: Emits `#define <yeet_ident> <original_token>` directives right after the preserved directives.
4. **Code Body**: Reconstructs the program body replacing each token with its yeet counterpart, preserving line breaks. The resulting code compiles cleanly with `clang++` or `g++`.

For an in-depth breakdown of the component design, lifecycle sequence, and preprocessor expansion invariants, see [ARCHITECTURE.md](file:///Users/zjweiss/Documents/GitHub/yeetify/ARCHITECTURE.md).

## Sample Usage
Just like the inspiration, assume a file called `foo.cpp` that contains the following code:

```
#include <iostream>
int main ( )  
{
	std :: cout << "Yeet!" ;
	return 0 ;
}
```

Running `./yeetify foo`, we generate the file `foo_yeetified.cpp`:

```
#include  <iostream>

#define yE "Yeet!"
#define yEET (
#define yEEe )
#define yEEt 0
#define yET ::
#define yEe ;
#define yEtt <<
#define ye cout
#define yeE main
#define yeEe return
#define yee std
#define yetT {
#define yett }

yeE yEET yEEe 
yetT 
yee yET ye yEtt yE yEe 
yeEe yEEt yEe 
yett 
```
