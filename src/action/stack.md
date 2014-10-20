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

Frame semantics
---------------

A user may use for-each-loops in their transactions, in which she/he may also
perform pushes or pops.
Tracking the number of values being pushed or popped may be a tedious task,
dependent on the complexity of the things done within the loop body.
It is also necessary to match expectations when it comes to the position of
`top` after a loop.
We want to satisfy the assumption that, after a loop, `top` is at the position
it was _before_ the loop.

To achieve this, we need the possibility to restore `top` after any number of
values pushed or popped from the stack, while at the same time preventing that
the values up to `top` are destroyed by a pop.

The concept of "frames" enables us to achieve such behavior in an easy way:
a stack has a frame base pointer.
A frame can be started by invoking `ws_processor_stack_start_frame()`.
A handle is returned and one can push values on the stack.
When popping, the top is compared against the frame base pointer rather than
against the bottom pointer.
If one tries to pop more values than there are in the current _frame_, an error
is thrown.

The current frame can be freed by _restoring_ the previous frame.
`ws_processor_stack_restore_frame`, which takes the handle returned by
`ws_processor_stack_start_frame()`, does that by restoring the old `top` and
popping all the values which are in the current frame.

