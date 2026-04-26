# Debugging with `pdbpp`
The usual is using pdb (Python Debugger). The module pdb++ brings colors and allows proper CLI debugging.

Install once:

```bash
python -m pip install pdbpp
```

Run the script in the debugger:

```bash
python -m pdbpp main.py
```

When you see this, you are inside the debugger:

```text
(Pdb++)
```

## Stop inside a function from another file

Use:

```text
b module_name.function_name
```

Example: file `simple_addition.py`, function `add_numbers()`:

```text
b simple_addition.add_numbers
c
```

`c` continues the program until the breakpoint is reached.

## Useful commands

```text
n              # next line, do not enter function
s              # step into function
c              # continue until breakpoint/end
p variable     # print variable
b file.func    # breakpoint in function
until 5        # run until line 5
q              # quit
```

## Notes

- Use `n` on import lines, otherwise you may enter Python internals.
- Use `s` when the line calls your own function.
- `until` uses line numbers, not function names.
- Breakpoints are not saved after quitting `pdbpp`.
