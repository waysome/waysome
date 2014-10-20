The action manager processor's stack {#stack_semantics}
====================================

This section describes the features and usage of the internal value stack used
by the action manager's processor.

The API allows users to issue transactions, which are lists of statements
executed on something like a stack machine.
It's not completely a stack machine, since positions within the stack may be
given either absolutely or relative to the stack's top.
More specifically, positive positions and `0` are taken as absolute positions
while negative positions are interpreted as positions relative to the stack's
top, `-1` being the topmost value, `-2` the next lower one and so on.
The following graph shows a stack with four items and the position of each one
of them in both direct and relative addressing:

    +------+
    |      |         <- top
    | FOO  | -1 or 3
    | BAR  | -2 or 2
    | BAZ  | -3 or 1
    | DUCK | -4 or 0 <- bottom
    +------+

This has two main advantages:
 * It eases checking for an empty stack, as `top` and `bottom` are _equal_ in
   this case, rather than `top` being _lower_ than `bottom`.
 * If the position is negative, the corresponding value can easily be reached by
   adding it to `top`.

Of course, values may be pushed to the stack or popped from the stack.
The stack is designed in a way that ensures that the area over the topmost
value, which is returned by `ws_processor_stack_top()`, contains an invalid
value.
It is ensured that checking the type of that value will yield
`ws_VALUE_TYPE_NONE`.


Function invocation
-------------------

Functions usually take parameters.
Regular functions take values as parameters.
To pass these values, the stack is used.
Any regular function invocation takes a pointer to a value union, which is
actually a pointer to a slot of the stack.
A function may then read values from the stack until it encounters an invalid
value, each value read being one input parameter.
Each function should set the first of the parameters to it's output value,
returning it's return value over the stack.

If a function takes custom parameters, the stack is prepared by the processor by
pushing the function's arguments on the stack.
The following graph shows the stack just before a function is called:

    +--------+
    |        | <- top
    | PARAM2 |
    | PARAM1 | <- argument of the function implementation
    | FOO    |
    | BAR    |
    | BAZ    |
    | DUCK   | <- bottom
    +--------+

Function parameters may also be given implicitly, meaning that the parameters
already are on the stack.
In this case, only the number of arguments is given.
The processor will then not push any arguments but calculate the base pointer
by subtracting the number of arguments from top.
The following graph shows a function taking two arguments, which are passed
implicitly:

    +--------+
    |        | <- top
    | FOO    |
    | BAR    | <- argument of the function implementation
    | BAZ    |
    | DUCK   | <- bottom
    +--------+

After a function invocation, all the arguments are popped from the stack except
the first one, which is the return value of the function.

