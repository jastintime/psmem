psmem
=====

## Description 

A clone of [ps\_mem](https://github.com/pixelb/ps_mem) written in C. 

![An imagine showing psmem running](example.png)

## Features
* List all your current running processes and their ram usage.
* Runs without root permissions, although running with root permissions provides more accurate results.

## Dependencies

psmem requires POSIX make, an ANSI C compiler, the feature macro `DEFAULT_SOURCE`, `stdio.h`, `stdlib.h`, `string.h`, `unistd.h`, `libgen.h` and `dirent.h`.

## Installing

```
git clone https://github.com/jastintime/psmem.git
cd psmem
sudo make clean install
```

This will install psmem to `/usr/local/bin/` and the manpage to `/usr/local/share/man`, if you'd like it somewhere else modify the makefile.

## Differences from ps\_mem

ps\_mem has some hacks to work with really old versions of the linux kernel(2.6.1 - 2.6.9), these hacks have not been reimplemented. ps\_mem also has the following arguments that have not been reimplemented `--split-args`, `--total`, `--discriminate-by-pid`, `--swap`, `-p`, `-w` these features are either better implemented as scripts that utilize psmem or as completely different tools. 

For example `--total` can be naively implemented as `psmem | tail -n 2 | head -n 1 | xargs`.

FreeBsd support is also not yet added, if you'd like this all you would need to do is change the PROC\_DIRECTORY macro to `/compat/linux/proc` at build time and implement `smaps` support, PR's welcomed.

## Contributing

psmem is written in a plain C style, try to avoid introducing new dependencies and follow the same style. Features and ideas that can be implemented through a script or are better placed in a different tool will most likely be rejected. The Todo section is a good guideline for contributions.

## Todo

- [ ] Fuzzy test
- [ ] Fallback to `smaps` if we cannot find `smaps_rollup`




