# FROM ubuntu:22.04


# ENV DEBIAN_FRONTEND=noninteractive
# ENV PG_VERSION=16.0
# ENV PG_PREFIX=/usr/local/pgsql16
# ENV PATH=$PG_PREFIX/bin:$PATH

# # correctly addresses mac checksum issues
# # error on Mac no matter the attempted changes. This 'magically' fixes it
# # https://forums.docker.com/t/hash-sum-mismatch-writing-more-data-as-expected/45940/3
# COPY ./badproxy /etc/apt/apt.conf.d/99fixbadproxy

# RUN apt-get update && apt-get install -y \
#     build-essential \
#     vim \
#     clang \
#     git \
#     curl \
#     wget \
#     flex \
#     bison \
#     zsh \
#     ca-certificates \
#     gnupg2 \
#     && rm -rf /var/lib/apt/lists/*

# WORKDIR /src

# # Build Postgres16 from scratch
# RUN wget https://ftp.postgresql.org/pub/source/v16.0/postgresql-16.0.tar.gz \
#     && tar xzf postgresql-16.0.tar.gz \
#     && cd postgresql-16.0 \
#     && ./configure  \
#     # && make install \
#     # && cd .. \
#     # && rm -rf postgresql-${PG_VERSION} postgresql-${PG_VERSION}.tar.gz

# # init db 
# # RUN adduser postgres \
# #     && mkdir -p /usr/local/pgsql/data \
# #     && chown postgres /usr/local/pgsql/data \
# #     && su -u postgres $PG_PREFIX/bin/initdb -D /usr/local/pgsql/data

# WORKDIR /audb
# COPY . /audb

# # Optional: switch to bash
# CMD ["bash"]


FROM ubuntu:24.04

# # error on Mac no matter the attempted changes. This 'magically' fixes it
# # https://forums.docker.com/t/hash-sum-mismatch-writing-more-data-as-expected/45940/3
COPY ./badproxy /etc/apt/apt.conf.d/99fixbadproxy

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        libreadline-dev \
        zlib1g-dev \
        icu \
        wget \
    && rm -rf /var/lib/apt/lists/*

# Download and build PostgreSQL 16.5 from source
RUN wget https://ftp.postgresql.org/pub/source/v16.5/postgresql-16.5.tar.gz --no-check-certificate \
    && tar -xzf postgresql-16.5.tar.gz \
    && cd postgresql-16.5 \
    && ./configure --prefix=/usr/local/pgsql \
    && make -j$(nproc) \
    && make install \
    && rm -rf /postgresql-16.5*

# Make psql and pg_ctl available
ENV PATH="/usr/local/pgsql/bin:${PATH}"

# Create a postgres user and data directory
RUN useradd postgres \
    && mkdir /pgdata \
    && chown postgres:postgres /pgdata

# Initialize database
USER postgres
RUN initdb -D /pgdata

# Expose port and start PostgreSQL
EXPOSE 5432
CMD ["postgres", "-D", "/pgdata"]