# README #

This is an implementation of a C++ `variant` class. It is based on N4542, but the empty state is
explicit, has an `index()` of -1. You can also check for the empty state with
`std::experimental::has_alternative<std::experimental::empty_t>(v)`.

It has been tested with gcc 4.9 on Ubuntu Linux.

It is provided as a single header file released under the BSD license.

Copyright (c) 2015, Just Software Solutions Ltd
