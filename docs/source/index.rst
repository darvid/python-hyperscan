python-hyperscan
================

Unofficial CPython bindings for `Hyperscan`_.

.. _hyperscan: https://github.com/01org/hyperscan

.. contents::
   :backlinks: none

Quickstart
----------

Building Hyperscan
~~~~~~~~~~~~~~~~~~

.. code-block:: shell

    $ git clone https://github.com/01org/hyperscan.git
    $ cd hyperscan
    $ mkdir -p build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -G "Unix Makefiles" \
    -DBUILD_SHARED_LIBS=ON ../
    $ make
    $ make install

Installing python-hyperscan
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

    $ pip install hyperscan

API
---

.. automodule:: hyperscan
   :members:
   :undoc-members:
