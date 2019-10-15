ARG UBUNTU=bionic
FROM ubuntu:bionic
ARG UBUNTU
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pgxnclient

# Setup PostgreSQL apt repository
RUN apt-key adv --fetch-keys https://www.postgresql.org/media/keys/ACCC4CF8.asc
RUN echo "deb http://apt.postgresql.org/pub/repos/apt/ ${UBUNTU}-pgdg main" >> /etc/apt/sources.list.d/pgdg.list

ARG POSTGRESQL=11
ARG POSTGIS=2.5

RUN apt-get update && apt-get install -y \
    postgresql-${POSTGRESQL}-postgis-${POSTGIS}-scripts \
    postgresql-${POSTGRESQL}-postgis-${POSTGIS} \
    postgresql-server-dev-${POSTGRESQL} \
    postgresql-${POSTGRESQL}

# Set workdir
WORKDIR /github/workspace

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
