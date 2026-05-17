#! /bin/bash

export CURRENT_UID=$(id -u)
export CURRENT_GID=$(id -g)
docker compose -f ./toolchain-container/compose.yaml run --rm build
