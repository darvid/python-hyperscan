# Usage

## API Support

``python-hyperscan`` currently exposes *most* of the C API, with the
following caveats or exceptions:

* No [Chimera][1] support yet.
* No [stream compression][2] support.
* No [custom allocator][3] support.
* ``hs_expression_info``, ``hs_expression_ext_info``,
  ``hs_populate_platform``, and ``hs_serialized_database_info`` not
  exposed yet.

!!! tip

    Refer to the [Hyperscan documentation][4] to gain an understanding of
    how the Hyperscan compiler C API works, including supported pattern
    constructs and matching modes.

``python-hyperscan`` has been built and tested against Hyperscan
``v4.7.0`` and upwards, including the latest as of April 2019,
``v5.1.1``.

Please [create an issue][5] to request prioritization of certain C API
features, report inconsistencies between the C API and this Python
wrapper, and of course, report any bugs.

## Building a Database

The only required parameter to ``hyperscan.Database`` is
**expressions**, which should be a sequence of regular expressions. The
rest of the parameters, including **ids**, **elements**, and **flags**
are optional.

```python
import hyperscan

db = hyperscan.Database()
patterns = (
    # expression,  id, flags
    (br'fo+',      0,  0),
    (br'^foobar$', 1,  hyperscan.HS_FLAG_CASELESS),
    (br'BAR',      2,  hyperscan.HS_FLAG_CASELESS
                       | hyperscan.HS_FLAG_SOM_LEFTMOST),
)
expressions, ids, flags = zip(*patterns)
db.compile(
    expressions=expressions, ids=ids, elements=len(patterns), flags=flags
)
print(db.info().decode())
# Version: 5.1.1 Features: AVX2 Mode: BLOCK
```

## Match Event Handling

Match handler callbacks will be invoked with exactly the same parameters
as their analog from the Hyperscan C API:

```python
# Type annotated Hyperscan match handler signature
def on_match(
    id: int,
    from: int,
    to: int,
    flags: int,
    context: Optional[Any] = None
) -> Optional[bool]:
    ...
```

Refer to the [Hyperscan documentation][6] for ``match_event_handler``
for details about each parameter. Note that **context** in this case is
any Python object passed to a ``scan`` method.

The return value determines whether or not Hyperscan should halt
scanning. If the match handler returns anything other than ``None``
that is *truthy*, scanning will be halted and any subsequent calls to
``Database.scan`` or ``Stream.scan`` will throw a ``hyperscan.error``.

## Pattern Scanning

``python-hyperscan`` manages Hyperscan's [scratch spaces][7] behind the
scenes, so performing the actual scanning is extremely trivial.

!!! note

    Mirroring the behavior of the Hyperscan C API, both block and
    stream mode ``scan`` methods do not require a
    **match_event_handler** callback function to be provided. Not
    passing a match callback will suppress match production entirely.

    One possible use case for this behavior is error checking or
    performing a dry run before performing a scan with a registered
    match handler.

### Block Mode

```python
db.scan(b'foobar', match_event_handler=on_match)
# Or, to provide a context object:
db.scan(b'foobar', match_event_handler=on_match, context='foo')
```

### Streaming Mode

First, ensure the ``Database`` object was created with streaming mode
enabled.

```python
db = hyperscan.Database(mode=hyperscan.HS_MODE_STREAM)
```

Next, simply use the ``Database.stream`` method, which provides the
``Stream`` context manager. The ``Database.stream`` can be passed a
**match_event_handler** and **context** object which will be used for
all invocations of ``Stream.scan``, unless overridden.

```python
with db.stream(match_event_handler=on_match, context=2345) as stream:
    stream.scan(b'foobar')
    # Override context only for one chunk
    stream.scan(b'barfoofoobarbarfoobar', context=1234)
    # Override match handler only for one chunk
    stream.scan(b'qux', match_event_handler=on_qux_match)
```

## Serialization

Refer to the [Hyperscan documentation][8] for more information on
serialization, its use cases, and caveats. Usage is simple:

```python
# Serializing (dumping to bytes)
serialized = hyperscan.dumpb(db)
with open('hs.db', 'wb') as f:
    f.write(serialized)

# Deserializing (loading from bytes):
db = hyperscan.loadb(serialized)
```

[1]: http://intel.github.io/hyperscan/dev-reference/chimera.html
[2]: http://intel.github.io/hyperscan/dev-reference/runtime.html#stream-compression
[3]: http://intel.github.io/hyperscan/dev-reference/runtime.html#custom-allocators
[4]: http://intel.github.io/hyperscan/dev-reference/compilation.html
[5]: https://github.com/darvid/python-hyperscan/issues
[6]: http://intel.github.io/hyperscan/dev-reference/api_files.html#c.match_event_handler
[7]: http://intel.github.io/hyperscan/dev-reference/runtime.html#scratch-space
[8]: https://intel.github.io/hyperscan/dev-reference/serialization.html