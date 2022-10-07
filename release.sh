#!/bin/bash

docker build -f multilinux.env.Dockerfile . -t multilinux-env

rm -R -f dist

python3 setup.py sdist || ! echo "Failed on sdist export." || exit 1

sh build_manylinux_wheels.sh || ! echo "Failed building manylinux wheels." || exit 1

cp wheelhouse/* dist/ || ! echo "No multilinux wheels? Check build_manylinux_wheels.sh" || exit 1
rm -R -f wheelhouse

python3 -m twine upload dist/*