# IngotScript

A simple programming language called IngotScript.

## Features

- Syntax highlighting for `.is` files
- Run IngotScript code directly from VSCode

## Usage

1. Write your IngotScript code in a file with the extension `.is`.
2. Run the IngotScript code using the command palette (`Right Click` -> `Run IngotScript`).

## Example

### [Variable]
```ingotscript
let a = 10
let b = 3.14
let c = "abc"
```

### [Print]
```ingotscript
print a
print b
print c
print "Hello,Ingot World!"
```

### [If]
```ingotscript
if a > 5
    print "a is greater than 5"
end
```

### [While]
```ingotscript
let i = 1
while i <= 5
    print i
    let i = i + 1
end
```

### [For]
```ingotscript
for j = 1 to 5
    print j
end
```

### [Drawer GUI]
```ingotscript
Drawer createWindow
Drawer createLabel "Hello, Drawer!"
Drawer createButton "Click Me!"
```
