FROM docker.io/library/postgres:16-alpine AS builder

RUN apk add --no-cache build-base clang19 llvm19-dev;

USER postgres

WORKDIR /workdir

COPY . .

RUN make

USER root

RUN make install

USER postgres

RUN set -ex; \
    initdb /tmp/testdb; \
    pg_ctl -D /tmp/testdb -o "-F -c listen_addresses=''" -w start; \
    make installcheck; \
    pg_ctl -D /tmp/testdb -m fast stop;


FROM docker.io/library/postgres:16-alpine

COPY --from=builder /usr/local/lib/postgresql /usr/local/lib/postgresql
