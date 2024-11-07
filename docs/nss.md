# NSS

## Library compilation

```sh
mkdir nss-nspr && cd nss-nspr
hg clone https://hg.mozilla.org/projects/nspr
hg clone https://hg.mozilla.org/projects/nss

cd nss
# When building for OSS-Fuzz, use "--fuzz=oss".
./build.sh -c --fuzz --disable-tests

export NSS_NSPR_PATH="$(realpath ..)"
export CXXFLAGS="$CXXFLAGS -I $NSS_NSPR_PATH/dist/public/nss -I $NSS_NSPR_PATH/dist/Debug/include/nspr -DCRYPTOFUZZ_NSS"
# NSS has conflicting symbols with OpenSSL and its derivates and therefore
# can't be built with any of them.
export CXXFLAGS="$CXXFLAGS -DCRYPTOFUZZ_NO_OPENSSL"
# NSS links to sqlite.
export LINK_FLAGS="$LINK_FLAGS -lsqlite3"
```

## Module compilation

```sh
cd cryptofuzz/modules/nss/
make
```
