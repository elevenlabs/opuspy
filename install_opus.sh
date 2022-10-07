# OGG and OpenSSL are required to install.
mkdir -p libs || exit 1
cd libs || exit 1

wget https://archive.mozilla.org/pub/opus/opus-1.3.1.tar.gz || exit 1
tar -xf opus-1.3.1.tar.gz || exit 1
cd opus-1.3.1 || exit 1
./configure || exit 1
make -j 8 || exit 1
make install || exit 1
ldconfig || exit 1
cd .. || exit 1

wget http://downloads.xiph.org/releases/opus/opusfile-0.12.tar.gz || exit 1
tar -xf opusfile-0.12.tar.gz || exit 1
cd opusfile-0.12 || exit 1
./configure || exit 1
make -j 8 || exit 1
make install || exit 1
ldconfig || exit 1
cd .. || exit 1

wget http://downloads.xiph.org/releases/opus//libopusenc-0.2.1.tar.gz || exit 1
tar -xf libopusenc-0.2.1.tar.gz || exit 1
cd libopusenc-0.2.1 || exit 1
./configure || exit 1
make -j 8 || exit 1
make install || exit 1
ldconfig || exit 1
cd .. || exit 1

cd .. || exit 1


