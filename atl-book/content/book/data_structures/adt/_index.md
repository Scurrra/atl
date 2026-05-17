---
title: "Algebraic Data Types"
weight: 0
---

> [Algebraic Data Type (ADT)](https://en.wikipedia.org/wiki/Algebraic_data_type) is a composite data type, i.e. a type that is composed of other types.

ADTs are built of two constructions: a sum and a product. 

A sum type is a type that can be one of several different types. The examples of this are Rust's enums, Python's `Union` types, C's unions, and C++'s safe union --- `std::variant`. Of these four, the best work was done by Rust's enums.

A product type is a type that is composed of multiple types. You can think of it as a Cartesian product of sets, where each set corresponds to all possible values of a type. Or just think of it as a tuple, struct, class, whatever. Once again, the best work was done by Rust's structs, no inheritance as intended by God.

I will show how construct your own sum and product types on options, variant, and tuples.
