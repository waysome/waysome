The statemachine in the JSON deserializer {#json_statemachine}
=========================================

This chapter documents the state machine which is used in the JSON deserializer
backend.

States
------

In the following table, the "State" field can be mapped onto the appropriate
enum entry by prefixing it with "STATE_" and making it uppercase.

| State             | Expected next                                            |
| ----------------- | -------------------------------------------------------- |
| Initial           | Message state                                            |
| Message           | Everything which is possible in the JSON object          |
| UID               | String containing the UID                                |
| Type              | String containing the type                               |
| Commands          | Array containing the Commands                            |
| Command Array     | New Command                                              |
| New Command       | Command Name                                             |
| Command Name      | Command Arguments                                        |
| Command Arguments | Commands                                                 |
| Direct Argument   | Direct Arg Key                                           |
| Direct Arg Key    | Direct Arg Value                                         |
| Direct Arg Value  | Command Arguments                                        |
| Indirect Argument | Command Arguments                                        |


State diagrams
--------------

This section contains state diagrams for single paths through the overall state
chart.

### UID state chart 

The following chart describes the states which are passed while parsing an UID
inside the message object. Clearly, other keys can occur in the message
object and of course, the UID key does not need to be the first one. This chart
is only a minimal one.

               "{"                   "uid"*               ...
    Initial ----------> Message ---------------> UID --------------> String
        ^                                                               |
         \-------------------------------------------------------------/

### Command state charts

This section is splitted into several subsections which contain partial state
charts each.

#### From initial to Command arguments

The following state chart describes how the parser goes from the initial state
to the top level key parsing states and (in case of the command key) to the
command array parsing state.

                                Initial
                                    |
                                    |
                                    | "{"
                                    |
                                    v
        +---------------------> Message <-------------------------------+
        |                           |                                   |
        |       +-------------------+-----------------------+           |
        |       |    "UID"*         | "commands"*           |           |
        |       |                   |                       |           |
        |       v                   v                       v           |
        |       UID             Command                     type        |
        |       |                   |                       |           |
        |       |                   |                       |           |
        |       v                   |"["                    v           |
        +-- UID String              |                   Type String-----+
        ^                           v
        |        "]"        +---------------+
        +-------------------| Command Array |
                            +---------------+

#### Command Array parsing

The following chart describes how the command Array is parsed, but excludes the
command argument parsing step, which is described in another state chart.

        Command Array
            |       ^
            |       | "}"
            | "{"   |
            |       |
            v       |
            New Command <-------+
            |                   |
            |                   |
            | <command name>    |
            |                   |
            |                   |
            v                   |
        Command Name            |
            |   |     <number>  |
            |   +-------------->|
        "[" |                   |
            |                   | "]"
            |       +-----------+
            v       |
    +-----------------+
    | Cmd Arg Parsing |
    +-----------------+

#### Command Argument parsing

There are two possible ways to pass something to a command using arguments:
Via the stack and positions on the stack or directly by specifying the arguments
in the array.

Parsing arguments passed via the array is simple, you only have to check if
you are in the argument array or not and push the appropriate argument to the
command. Stack positions have to be passed through an object, though. So do more
complex objects as named values (sets can be passed as arrays).

    Command Arguments ------+-----------------------+
        ^   ^               | <nil>,                |
        |   |               | <boolean>,            |
        |   |               | <integer>,            |
        |   |               | <string>              | "{"
        |   +---------------+                       |
        |                                           v
        |                                   Argument object
        |                                           |
        |                   +-----------------------+
        |                   |                       |
        |                   | "pos"*                | "named"*
        |                   |                       |
        |   <number>        v                       v
        +-------------- Argument: position      Argument: Named val

(*) The used strings are as constants in the code and are defined in a central
place.
