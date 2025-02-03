# <span style="font-size:smaller;">LAS : : Command : : </span><span style="font-size:larger;">execute</span>

***

```
virtual std::pair<int, std::ostringstream> 
    execute ( const std::vector<std::string>& args) = 0;
```

Pure virtual function that allows the devleoper to implement what they see fit. 

This function is called when the LAS Environment's shell encounters the `Command`'s **key**. The shell passes quoted arguments as **args**.

## Parameters
| <u>Name</u>       | <u>Definition</u> |
| ----------------- | ------------- |
| **args**          | A constant reference to a `std::vector<std::string>` that contains any quoted arguments entered after the **key** |

## Return value
An `std::pair` of which the first type is an `int` designed to be a return code, and the second of type `std::ostringstream` meant to provide user-facing information about the event.

## Exceptions
*Any a developer choose to throw.*

## Example
```
CODE HERE
```
