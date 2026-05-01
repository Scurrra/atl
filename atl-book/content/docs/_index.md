---
date: '2026-04-30T19:54:05+03:00'
title: 'ATL Docs'
cascade:
  type: docs
---

> Abnormal Template Library *will* provide implementations of some containers and algorithms from the C++ Standard Template Library, and additionally some other data structures and algorithms that are not a part of STL for some reason.

> [!IMPORTANT]
> Whether I'm going to implement everything under the `abl` (like `st`andar`d`, but `ab`norma`l`) namespace, the library will rely on the `std` namespace for things like fixed-width basic types and `std::function`. There will be an additional warning for each header.

> [!NOTE]
> I *already* don't like some things from standard library, so some things *will* be rewritten under `abl` namespace. As sometimes a simple wrapper is more than enough, wider usage of possibly underlying `std::` stuff can be enabled with flag like `-DSTD_ATL`.

> [!WARNING]
> I don't think that anyone would use the library itself. For me, it's just a way to better organize ADS in my mind. <!-- And show potential emploer what I know and can. -->
