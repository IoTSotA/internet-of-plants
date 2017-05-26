#!/bin/bash

set -e

: "${GF_PATHS_DATA:=/var/lib/grafana}"
: "${GF_DATABASE_TYPE:=sqlite3}"
: "${GF_DATABASE_PATH:=grafana.db}"

if [ ${GF_DATABASE_TYPE} == "sqlite3" ] && [ ! -f ${GF_PATHS_DATA}/${GF_DATABASE_PATH} ]
then
    echo "Restoring database dump"
    sqlite3 -init /tmp/ioplants.sql ${GF_PATHS_DATA}/${GF_DATABASE_PATH} .quit
fi

# Run grafana
/run.sh
