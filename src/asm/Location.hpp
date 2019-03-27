#ifndef Location_HPP
    #define Location_HPP

namespace Cminus { namespace ASM
{
    enum class LocationType
    {
        Register = 0,
        Literal = 1,
        Memory = 2,
        BoundRegister = 3
    };
}}

#endif
