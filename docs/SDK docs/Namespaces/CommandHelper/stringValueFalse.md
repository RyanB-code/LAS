# <span style="font-size:smaller;">LAS : : CommandHelper : : </span><span style="font-size:larger;">stringValueFalse</span>

***

```
bool stringValueFalse (std::string s);
```

Non case-sensitive way to see if the string spells "false". Internaly the string is converted using `std::tolower( )`.


## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **s**             | The string to compare |

## Return value
A `bool`, true if the string spells "false". Returns false otherwise.

## Exceptions
None.

## Example
```
CODE HERE
```