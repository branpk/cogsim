#!/usr/bin/env bash

gcc \
  -std=c99 \
  -O3 \
  -Wall -Wextra \
  -Wno-missing-braces \
  -lglfw \
  -lGL \
  -fwrapv \
  -fno-strict-aliasing \
  source/*.c \
  -o cogsim
