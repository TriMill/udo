# udo

`udo` is an extremely stripped-down sudo/doas imitation.

## config

the config file is located at `/etc/udo.conf`. it is a space-separated list of UIDs that are allowed to use `udo`. this file should probably contain `0` at some point.

## installation

`sudo make install`

## uninstallation

`sudo make uninstall`, but why would you want to?

## why does it segfault?

it only segfaults if you have done something wrong. do not do wrong things.
