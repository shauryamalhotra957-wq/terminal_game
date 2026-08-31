# Contributing

Build the Windows console game with the repository Makefile:

~~~text
make
~~~

Run the manual regression checks in [TESTING.md](TESTING.md) after changing movement, collision, food spawning, map dimensions, or console input. Keep generated binaries out of commits; the ignore file covers the normal build output.

The game is intentionally local and self-contained. Do not add network access or shell execution without a design and security review.
