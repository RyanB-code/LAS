# <span style="font-size:smaller;">LAS : : CommandHelper : : </span><span style="font-size:larger;">pairErrorWithMessage</span>

***

```
std::pair<int, std::ostringstream> pairErrorWithMessage (const std::string& msg);
```

Returns a pair of -1 with the given message.

## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **msg**           | The message to be displayed to user |

## Return value
An `std::pair`, of which are -1 and then the message. A newline will be inserted as the last character if one is not present.

## Exceptions
None.

## Example
```
CODE HERE
```