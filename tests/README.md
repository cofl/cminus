# Test

`test` is a bash script written to test the Cminus compiler for
MTU CS 4121.

## How to use

1. Copy the "tests" directory to the root of your project.
2. Execute `chmod +x tests/test` to give the script execute rights.
3. Run `./tests/test`. The tester only uses absolute paths, so it
   may be run from anywhere. Optionally, you may list specific
   input files or directories (wildcards are supported as in `ls`).

```bash
$ ./tests/test
$ ./tests/test input/1.if.cm
$ ./tests/test input/2*
$ ./tests/test input/1.*.cm input/2*
$ ./tests/test ~/my_test_cases
```

## Adding more tests

New .cm files in the input directory are detected automatically.
To add input to these files, so you don't need to type it in:

1. Create a file or folder with the same name as the source file
   in the "tests" directory.
2. If you have a file, enter your input. Only this file will be       used as input.
3. If you hve a folder, crate more files in it, and enter input
   into each. Each file will serve as a separate test case.

## Skipping tests

If you want to skip a test for any reason, add it to the `SKIP`
variable at the top of the script file. The formats are:

- `file_base_name.cm`
- `file_base_name.cm Reason for skipping the test`
