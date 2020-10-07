ARG UBUNTU=focal
FROM ubuntu:${UBUNTU}
ARG UBUNTU

# set timezone
RUN ln --symbolic --no-dereference --force \
        /usr/share/zoneinfo/Europe/Copenhagen /etc/localtime 

RUN apt-get update && apt-get install -y \
        build-essential \
        cmake \
        git \
        pgxnclient

# setup postgresql apt repository
RUN apt-key adv --fetch-keys https://www.postgresql.org/media/keys/ACCC4CF8.asc
RUN echo "deb http://apt.postgresql.org/pub/repos/apt/ ${UBUNTU}-pgdg main" >> /etc/apt/sources.list.d/pgdg.list

ARG POSTGRESQL=13
ARG POSTGIS=3

# install postgresql and postgis
RUN apt-get update && apt-get install -y \
        postgresql-${POSTGRESQL}-postgis-${POSTGIS}-scripts \
        postgresql-${POSTGRESQL}-postgis-${POSTGIS} \
        postgresql-server-dev-${POSTGRESQL} \
        postgresql-${POSTGRESQL}

RUN apt-get update && apt-get install -y \
        sudo

WORKDIR /github/workspace

COPY .tmp-tools/entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
