# MODUVO Functions

> **IMPORTANT**  
> This is a collection of functions made for Roblox executors. Most of them are basic.

> **WARNING**  
> Commercial use requires attribution. Removal of MODUVO credits results in license termination and DMCA action.

> **NOTE**  
> Authorized rebrands must include “Based on MODUVO Functions” in documentation or UI.

## Stopwatch

The Stopwatch library provides a simple way to measure elapsed time in Lua.

### Creating a Stopwatch
```lua
local sw = Stopwatch.new()
```

### Methods
- `sw:Start()` — Starts the stopwatch.
- `sw:Stop()` — Stops the stopwatch.
- `sw:Reset()` — Resets stopwatch to 0.
- `sw:ElapsedTime()` — Returns elapsed time in seconds.

### Example
```lua
local sw = Stopwatch.new()
sw:Start()
-- ... work ...
sw:Stop()
print("Elapsed:", sw:ElapsedTime(), "seconds")
```

---

## CSV

The CSV library parses and serializes CSV (Comma-Separated Values) data.

### Functions

#### `csv.parse(csvString[, delimiter])`
```lua
local parsed = csv.parse("A,B,C\n1,2,3")
```

#### `csv.stringify(table[, delimiter])`
```lua
local csvStr = csv.stringify({{"A","B","C"},{"1","2","3"}})
```

#### `csv.getField(table, row, col)`
```lua
local value = csv.getField(parsed, 2, 1)
```

#### `csv.getRow(table, row)`
```lua
local row = csv.getRow(parsed, 2)
```

---

## Bit

Bitwise operations for 32-bit integers.

### Math
- `bit.badd(...)`
- `bit.bsub(...)`
- `bit.bmul(...)`
- `bit.bdiv(...)`

### Logic
- `bit.band(...)`
- `bit.bor(...)`
- `bit.bxor(...)`
- `bit.bnot(n)`

### Shift
- `bit.lshift(value, shift)`
- `bit.rshift(value, shift)`
- `bit.arshift(value, shift)`

### Rotate
- `bit.rol(value, shift)`
- `bit.ror(value, shift)`

### Utility
- `bit.bpopcount(value)`
- `bit.bswap(value)`
- `bit.tohex(value[, nibbles])`
- `bit.tobit(value)`

---

## Regex

Regular expression library for Lua.

### Constructors
- `Regex.new(pattern)`
- `Regex.Escape(str)`

### Methods
- `regex:Status()`
- `regex:Match(str)`
- `regex:Replace(str, replacement)`

### Example
```lua
local regex = Regex.new("\\d+")
local matches = regex:Match("123 and 456")
```
