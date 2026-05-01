---
title: "Flag"
weight: 1
---

> C++ `enum class` lacks option composition, so it can't be used directly as flag. `abl::flag` is an elegant tool to bring flag functionality.

> [!CAUTION]
> Imports `<concepts>` and `<type_traits>`.

> [!TIP]
> To import use `#include "atl/types/flag.hpp"`.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/types/flag.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/8xYcG51hq" title="Godbolt example" icon="globe" >}}
{{< /cards >}}

## `abl::flag<E> : abl::enum_wrapper<E>`

Functionality extension for enum class `E`, where `std::is_enum_v<E>` is satisfied. The base class provides bitwise `&` (`&=`) and `|` (`|=`) operators, `.has()` method for inclusion check, and `.get_value()` to obtain underlying integer value. Flag class provides functions for redefining basic options and composite flags.

> [!IMPORTANT]
> `abl::flag` needs an enum for basic flags. For correct work make sure that underlying values occupy different bits. The simplest way to do so is to define one option equal to `0`, and the following as `1 << n`, starting with n = 0. 

### Usage

Whether the solution is elegant, as a developer you will need to write one weird struct which you will actually use. The struct will have redefined basic flags together with complex combined flags.

> [!TIP]
> An enum is a mandatory requirement, and you might not want to have it in the global scope. The recommended way is to define a nested enum class within the private section of your struct or class.

```cpp
struct Colors {
   private:
    enum class BasicColors {
        NONE = 0,
        RED = 1 << 0,
        GREEN = 1 << 1,
        BLUE = 1 << 2,
    };

   public:
    using value_type = abl::flag<BasicColors>;

    // Reexport
    static constexpr auto RED =
        value_type::template value<BasicColors::RED>();
    static constexpr auto GREEN =
        value_type::template value<BasicColors::GREEN>();
    static constexpr auto BLUE =
        value_type::template value<BasicColors::BLUE>();

    // Reexport and rename
    static constexpr auto BLACK =
        value_type::template value<BasicColors::NONE>();

    // Mix
    static constexpr auto WHITE = value_type::template combine<
        BasicColors::RED, BasicColors::GREEN, BasicColors::BLUE>();
};
```

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/types/flag.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/8xYcG51hq" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
