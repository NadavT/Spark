#ifndef SPARK_MOUSE_CODES_H
#define SPARK_MOUSE_CODES_H

#include <iostream>

namespace Spark
{
typedef enum class MouseCode : uint16_t
{
    // From glfw3.h
    Button0 = 0,
    Button1 = 1,
    Button2 = 2,
    Button3 = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,

    ButtonLast = Button7,
    ButtonLeft = Button0,
    ButtonRight = Button1,
    ButtonMiddle = Button2
} Mouse;

inline std::ostream &operator<<(std::ostream &stream, MouseCode mouseCode)
{
    stream << static_cast<int32_t>(mouseCode);
    return stream;
}
} // namespace Spark

#endif /* SPARK_MOUSE_CODES_H */
