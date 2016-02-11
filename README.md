# README #

This is an implementation of a C++ `variant` class, based on N4542, and then
P0088R1 following the WG21 meeting in Kona.

The default constructor default-constructs the first type. If the first type
isn't default-constructible then there is no default constructor.

If a copy or move or emplace assignment operation throws then the variant is put
in a special "valueless by exception" state, which can be queried with
`valueless_by_exception()`.

It has been tested with gcc 4.9 on Ubuntu Linux.

It is provided as a single header file released under the BSD license.

Copyright (c) 2015, Just Software Solutions Ltd
