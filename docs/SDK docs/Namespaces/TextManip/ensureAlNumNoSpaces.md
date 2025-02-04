# <span style="font-size:smaller;">LAS : : TextManip : : </span><span style="font-size:larger;">ensureAlNumNoSpaces</span>

***

```
std::string ensureAlNumNoSpaces (const std::string& text);
```

Only alpha numeric characters will be added to the return value. All others, except for spaces, will be skipped over. Every instance of `char` ' ', will be replaced with '-'.

## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **text**          | The `string` of text |

## Return value
An `std::string` of the resultant text.

## Exceptions
None.

## Example
```
CODE HERE
```