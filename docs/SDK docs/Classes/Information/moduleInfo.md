# <span style="font-size:smaller;">LAS : : Information : : </span><span style="font-size:larger;">ModuleInfo</span>

***

```
struct ModuleInfo{
    Version                 sdkVersion          { };
    Version                 moduleVersion       { };
    std::string             title               { };
    std::string             commandGroupName    { };
    WindowPtr    window              { };
    std::vector<CommandPtr> commands            { };
}
```

This simple struct is used to hold versioning information following the sematnic versioning style. See [this website](https://semver.org/) for more information on sematnic versioning.


This class is required for passing a Module's version information to the LAS Environment when attempting to load a Module.

## Parameters
| <u>Name</u>       | <u>Definition</u>         |
| ----------------- | -------------             |
| **major**         | The major revision number |
| **minor**         | The minor revision number |
| **patch**         | The patch revision number |

** NOT FINISHED **
