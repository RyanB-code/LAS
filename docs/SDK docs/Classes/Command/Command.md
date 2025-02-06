
# <span style="font-size:smaller;">LAS : : </span><span style="font-size:larger;">Command</span>

***

```
class Command (
        std::string Key, 
        std::string Description
    )
```

The class `Command` represents a keyword. When entered into the LAS Environment's shell, with or without additional arguments, the `Command`'s `execute()` function is called.

The `Command` object is a base class that provides freedom for a Module developer to use derived classes to provide the operations they desire.

## Member Variables
| <u>Name</u>       | <u>Definition</u>    |
| ----------------- | ------------- |
| key               | An `std::string`, it is the collection of characters that when found by the parser, will initiate execution of the command  |
| description       | An `std::string`, that contains end-user helpful information about what the command does  |

## Member Functions
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| *(constructor)*     | Constructs a new `Command`   |
| getKey            | Returns the `key` member variable  |
| getDescription    | Returns the `description` member variable  |
| execute           | Pure virtual function that is called when the `Command` is given to the Environment's shell |

## Non-Member Functions
*(none)*

## Related Aliases
```
namespace LAS {
    using CommandPtr    = std::unique_ptr<Command>;
    using CommandList   = std::vector<CommandPtr>;
}
```

## Example
```
CODE HERE
```

