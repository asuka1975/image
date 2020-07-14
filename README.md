# Image.h - An reinterpreted container library in C++17

Image.h reinterprets a vector which means memory layout. Images have reference of a vector, they reinterpret it as a vector of other trivially copyable objects and users can read and change them. 

# To start using image.h

Download image.h from this repository and put this file on an include directory of your project. 

# reference

*class template*

`aslib::` <span style="font-size: 200%; ">`image`</span>

```c++
namespace aslib {
    template <TImage>
    class image;
}
```

## Overview

`image` produced in `<image.h>` reinterprets a std::byte vector as other **trivially copyable** objects. Like vector, their size can change dynamically, with their storage being handled automatically by the container. But unlike vector, images use reference of a vector instead of contiguous storage locations for their elements. If images change, reference of their vector also change. In short, **This class is an extension of pointer reinterpretation.**

Operating cost is the same as a vector.

Template parameter's meaning is:
- `TImage`:   this parameter expresses object as which std::bytes is reinterpreted. the object should be a trivially copyable.

## Member functions

### Construction / Destruction
|name|explanation|version|
|:--:|:--:|:--:|
|(constructor)|construct image|C++11|

### Iterator
|name|explanation|version|
|:--:|:--:|:--:|
|begin()|Return iterator to beginning||
|end()|Return iterator to end||

### Capacity
|name|explanation|version|
|:--:|:--:|:--:|
|size()|Return size||
|empty()|Test whether image is empty||

### Element access
|name|explanation|version|
|:--:|:--:|:--:|
|at()|access specified element with bounds chcking||
|operator\[\]()|access specified element||

### Modifiers
|name|explanation|version|
|:--:|:--:|:--:|
|push_back()|Add an element at the end|C++11|
|pop_back()|Remove an element at the end|C++11|
|insert()|insert elements|C++14|
|erase()|erase elements|C++11|

### Deleted functions

This class contains reference of an object so you must avoid life extension of it. You can find wrong life extension in compiling owing to deleting functions involved in construction of pointer.
|name|explanation|version|
|:--:|:--:|:--:|
|operator=()||C++11|
|operator&()||C++11|
|operator new()||C++11|
|operator delete()||C++11|
|operator new\[\]()||C++11|
|operator delete\[\]()||C++11|

## Member type
|name|explanation|version|
|:--:|:--:|:--:|
|iterator||C++11|
|const_iterator||C++11|

## Example

### Basic usage(C++14)

```c++
#include <cstdint>
#include <iostream>
#include <vector>
#include "image.h"

struct object {
    std::uint32_t id;
    float value;
};

int main(void) {
    std::vector<std::byte> v { std::byte{0}, std::byte{0} };
    aslib::image<object> im(v, 2, 0); //if image reinterpret a range larger than referring vector size, vector will be extended.
    
    im.push_back(object { 2, 1.2f });
    std::vector<object> os {
        object { 0, 0.4f },
        object { 1, 1.5f }
    };
    im.insert(im.begin(), os.begin(), os. end());
    im.erase(im.begin() + 1);

    std::cout << "image" << std::endl;

    for(auto&& x : im) {
        std::cout << x.id << " " << x.value << std::endl;
    }

    std::cout << "reinterpret" << std::endl;
    object* o = reinterpret_cast<object*>(v.data() + 2);
    for(auto i = 0; i < im.size(); i++) {
        std::cout << o[i].id << " " << o[i].value << std::endl;
    }

    return 0;
}
```

#### Output
```
image
0 0.4
2 1.2
reinterpret
0 0.4
2 1.2
```