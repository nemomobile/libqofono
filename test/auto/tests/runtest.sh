#!/bin/sh -e

systemctl --user restart phonesim
sleep 10
exec "/usr/lib/libqofono-qt5/tests/${1?Test executable expected}"
