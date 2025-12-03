# # FROM ubuntu:22.04

# # # Install build tools, PostgreSQL dev headers, and zsh
# # RUN apt-get update && apt-get install -y \
# #     build-essential \
# #     clang \
# #     git \
# #     zsh \
# #     && rm -rf /var/lib/apt/lists/*

# # RUN apt-get update && apt-get install -y \
# #     ca-certificates \
# #     /usr/share/postgresql-common/pgdg \ 
    

# # RUN apt-get update && apt-get install -y \
# #     postgresql-client-16 postgresql-server-dev-16 \
# #     && rm -rf /var/lib/apt/lists/*

# # WORKDIR /audb

# # COPY . /audb

# # CMD ["zsh"]


# FROM ubuntu:22.04

# RUN rm -rf /var/lib/apt/lists/* \
#     && apt-get clean \
#     && apt-get update

# RUN apt-get update && apt-get install -y --fix-missing build-essential

#     # clang \
#     # git \
#     # zsh \
#     # && rm -rf /var/lib/apt/lists/*

# WORKDIR /audb

# COPY . /audb

# CMD ["bash"]


FROM ubuntu:22.04

# Avoid apt hash issues
ENV DEBIAN_FRONTEND=noninteractive

# error on Mac no matter the attempted changes. This 'magically' fixes it
# https://forums.docker.com/t/hash-sum-mismatch-writing-more-data-as-expected/45940/3
COPY ./badproxy /etc/apt/apt.conf.d/99fixbadproxy

# Install curl and gnupg (needed to add PostgreSQL repo)
RUN apt-get update && apt-get install -y curl gnupg lsb-release ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# # Add PostgreSQL official repo for version 16
# RUN curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor -o /usr/share/keyrings/pgdg.gpg \
#     && echo "deb [signed-by=/usr/share/keyrings/pgdg.gpg] http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" \
#        > /etc/apt/sources.list.d/pgdg.list

# # Install PostgreSQL 16
# RUN apt-get update && apt-get install -y postgresql-16 postgresql-client-16 \
#     && rm -rf /var/lib/apt/lists/*

# Optional: switch to bash
CMD ["bash"]
