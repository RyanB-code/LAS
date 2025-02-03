# <span style="font-size:smaller;">LAS : : CommandHelper : : </span><span style="font-size:larger;">stringValueTrue</span>

***

```
bool stringValueTrue (std::string s);
```

Non case-sensitive way to see if the string spells "true". Internaly the string is converted using `std::tolower( )`.


## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **s**             | The string to compare |

## Return value
A `bool`, true if the string spells "true". Returns false otherwise.

## Exceptions
None.

## Example
```
CODE HERE
```