.. _extend_support:

Extending the class
===================

.. contents::
   :local:
   :depth: 1

User defined Axes
-----------------

In case we need a histogram with an axes of an arbitrary type not identified by
the standard, you need to provide a overload for the hashing function used by the 
class.

GIL's histogram class uses boost\:\:hash_combine in a sub routine to generate a hash from
the key.

So we need to provide an overload of boost\:\:hash_combine for the purpose.

Example:
Let's consider you need an histogram with an axis over class Test.

.. code-block:: cpp

    // File : test.hpp
    struct Test 
    {
        int a;
    };

Now lets get to the main code.

.. code-block:: cpp 

    #include <test.hpp> 
    #include <boost/gil.hpp>
    // Mind the order of include i.e. test.hpp before boost/gil.hpp

    using namespace boogt::gil;
    namespace boost {
        std::size_t hash_value(Test const& t)
        {
            // Your hashing code
        }
    }

    int main() 
    {
        histogram<Test> h;
        Test t;
        h(t) = 1;
    }

