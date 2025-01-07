# Commands

The header file `Commands.h` contains the following to allow access to the LAS Environment's Unix-like command functionality. The `Command` object is a base class that provides freedom for a Module developer to use derived classes to provide the operations they desire.

***

## <span style="font-size:smaller;">LAS : : </span><span style="font-size:larger;">Command</span>

```
class Command (std::string Key, std::string Description)
```

Type `Command` represents a keyword that when entered in the command line is executed, with or without additional arguments, according to the `Command`'s `execute()` function.

## Member Types
| <u>Name</u>       | <u>Definition</u>    |
| ----------------- | ------------- |
| key               | An `std::string`, it is the collection of characters that when found by the parser, will initiate execution of the command  |
| description       | An `std::string`, that contains end-user helpful information about what the command does  |

## Member Functions
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| (constructor)     | Constructs a new `Command`   |
| getKey            | Returns the `key` member variable  |
| getDescription    | Returns the `description` member variable  |
| execute           | Pure virtual function that is called when the `Command` is given to the Environment's shell |

## Non-Member Functions
*(none)*



