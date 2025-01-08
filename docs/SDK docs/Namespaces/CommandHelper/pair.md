# <span style="font-size:smaller;">LAS : : CommandHelper : : </span><span style="font-size:larger;">pair</span>

***

```
std::pair<int, std::ostringstream> pair
    (int returnCode, const std::string& msg);
```

Returns a pair of given number and message.

## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **returnCode**    | The return code of type `int` |
| **msg**           | The message to be displayed to user |

## Return value
An `std::pair`, of which are the two given parameters, respectively. A newline will be inserted as the last character if one is not present.

## Exceptions
Does not throw exceptions unless one of the specified operations (e.g. constructor of an element) throws. 

## Example
```
CODE HERE
```