# <span style="font-size:smaller;">LAS : : CommandHelper : : </span><span style="font-size:larger;">pairInvalidArgument</span>

***

```
std::pair<int, std::ostringstream> pairInvalidArgument (const std::string& msg);
```

Returns a pair of -1 and a prompted message. The message will contain a description of the circumstance (there was an invalid argument), with the invalid entry in quotes.

Given the message "argument", the full message will read as follows.

```
Invalid argument "argument"
```

## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **msg**           | The argument to be displayed in quotes |

## Return value
An `std::pair`, of which are -1 and then the full message describing the invalid entry. A newline will be inserted at the end.

## Exceptions
None.

## Example
```
CODE HERE
```